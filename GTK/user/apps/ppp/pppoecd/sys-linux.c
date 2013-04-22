/*
 * sys-linux.c - System-dependent procedures for setting up
 * PPP interfaces on Linux systems
 *
 * Copyright (c) 1989 Carnegie Mellon University.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by Carnegie Mellon University.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/errno.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <sys/sysmacros.h>

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <time.h>
#include <memory.h>
#include <utmp.h>
#include <mntent.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <unistd.h>

/* This is in netdevice.h. However, this compile will fail miserably if
   you attempt to include netdevice.h because it has so many references
   to __memcpy functions which it should not attempt to do. So, since I
   really don't use it, but it must be defined, define it now. */

#ifndef MAX_ADDR_LEN
#define MAX_ADDR_LEN 7
#endif

#if __GLIBC__ >= 2
#include <asm/types.h>		/* glibc 2 conflicts with linux/types.h */
#include <net/if.h>
#include <net/if_arp.h>
#include <net/route.h>
#include <netinet/if_ether.h>
#else
#include <linux/types.h>
#include <linux/if.h>
#include <linux/if_arp.h>
#include <linux/route.h>
#include <linux/if_ether.h>
#endif
#include <netinet/in.h>
#include <arpa/inet.h>

#include <linux/ppp_defs.h>
#include <linux/if_ppp.h>

#include "pppd.h"
#include "fsm.h"
#include "ipcp.h"

/* We can get an EIO error on an ioctl if the modem has hung up */
#define ok_error(num) ((num)==EIO)

static int tty_disc = N_TTY;	/* The TTY discipline */
static int ppp_disc = N_PPP;	/* The PPP discpline */
static int initfdflags = -1;	/* Initial file descriptor flags for fd */
static int ppp_fd = -1;		/* fd which is set to PPP discipline */
static int sock_fd = -1;	/* socket for doing interface ioctls */
static int slave_fd = -1;
static int master_fd = -1;
static int ppp_dev_fd = -1;	/* fd for /dev/ppp (new style driver) */
static int chindex;		/* channel index (new style driver) */

static fd_set in_fds;		/* set of fds that wait_input waits for */
static int max_in_fd;		/* highest fd set in in_fds */

static int driver_version      = 0;
static int driver_modification = 0;
static int driver_patch        = 0;

static char loop_name[20];
static unsigned char inbuf[512]; /* buffer for chars read from loopback */

static int	if_is_up;	/* Interface has been marked up */
static u_int32_t our_old_addr;		/* for detecting address changes */
static int	dynaddr_set;		/* 1 if ip_dynaddr set */
static int	looped;			/* 1 if using loop */

static int kernel_version;
#define KVERSION(j,n,p)	((j)*1000000 + (n)*1000 + (p))

#define MAX_IFS		100

#define FLAGS_GOOD (IFF_UP          | IFF_BROADCAST)
#define FLAGS_MASK (IFF_UP          | IFF_BROADCAST | \
		    IFF_POINTOPOINT | IFF_LOOPBACK  | IFF_NOARP)

#define SIN_ADDR(x)	(((struct sockaddr_in *) (&(x)))->sin_addr.s_addr)

/* Prototypes for procedures local to this file. */
static int get_flags (int fd);
static void set_flags (int fd, int flags);
static int make_ppp_unit(void);
static void restore_loop(void);	/* Transfer ppp unit back to loopback */

extern u_char	inpacket_buf[];	/* borrowed from main.c */

/*
 * SET_SA_FAMILY - set the sa_family field of a struct sockaddr,
 * if it exists.
 */

#define SET_SA_FAMILY(addr, family)			\
    memset ((char *) &(addr), '\0', sizeof(addr));	\
    addr.sa_family = (family);

/*
 * Determine if the PPP connection should still be present.
 */

extern int hungup;

/* new_fd is the fd of a tty */
static void set_ppp_fd (int new_fd)
{
	SYSDEBUG ((LOG_DEBUG, "setting ppp_fd to %d\n", new_fd));
	ppp_fd = new_fd;
	if (!new_style_driver)
		ppp_dev_fd = new_fd;
}

static int still_ppp(void)
{
	if (new_style_driver)
		return !hungup && ppp_fd >= 0;
	if (!hungup || ppp_fd == slave_fd)
		return 1;
	if (slave_fd >= 0) {
		set_ppp_fd(slave_fd);
		return 1;
	}
	return 0;
}

/********************************************************************
 *
 * Functions to read and set the flags value in the device driver
 */

static int get_flags (int fd)
{
    int flags;

    if (ioctl(fd, PPPIOCGFLAGS, (caddr_t) &flags) < 0) {
	if ( ok_error (errno) )
	    flags = 0;
	else
	    fatal("ioctl(PPPIOCGFLAGS): %m");
    }

    SYSDEBUG ((LOG_DEBUG, "get flags = %x\n", flags));
    return flags;
}

/********************************************************************/

static void set_flags (int fd, int flags)
{
    SYSDEBUG ((LOG_DEBUG, "set flags = %x\n", flags));

    if (ioctl(fd, PPPIOCSFLAGS, (caddr_t) &flags) < 0) {
	if (! ok_error (errno) )
	    fatal("ioctl(PPPIOCSFLAGS, %x): %m", flags, errno);
    }
}

/********************************************************************
 *
 * sys_init - System-dependent initialization.
 */

void sys_init(void)
{
    int flags;

    if (new_style_driver) {
	ppp_dev_fd = open("/dev/ppp", O_RDWR);
	if (ppp_dev_fd < 0)
	    fatal("Couldn't open /dev/ppp: %m");
	flags = fcntl(ppp_dev_fd, F_GETFL);
	if (flags == -1
	    || fcntl(ppp_dev_fd, F_SETFL, flags | O_NONBLOCK) == -1)
	    warn("Couldn't set /dev/ppp to nonblock: %m");
    }

    /* Get an internet socket for doing socket ioctls. */
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0)
	fatal("Couldn't create IP socket: %m(%d)", errno);

    FD_ZERO(&in_fds);
    max_in_fd = 0;
}

/********************************************************************
 *
 * sys_cleanup - restore any system state we modified before exiting:
 * mark the interface down, delete default route and/or proxy arp entry.
 * This shouldn't call die() because it's called from die().
 */

void sys_cleanup(void)
{
/*
 * Take down the device
 */
    if (if_is_up) {
	if_is_up = 0;
	sifdown(0);
    }
}

/********************************************************************
 *
 * sys_close - Clean up in a child process before execing.
 */
void
sys_close(void)
{
	close(ppp_dev_fd);
    if (sock_fd >= 0)
	close(sock_fd);
    if (slave_fd >= 0)
	close(slave_fd);
    if (master_fd >= 0)
	close(master_fd);
    closelog();
}

/********************************************************************
 *
 * set_kdebugflag - Define the debugging level for the kernel
 */

static int set_kdebugflag (int requested_level)
{
    if (new_style_driver && ifunit < 0)
	return 1;
    if (ioctl(ppp_dev_fd, PPPIOCSDEBUG, &requested_level) < 0) {
	if ( ! ok_error (errno) )
	    error("ioctl(PPPIOCSDEBUG): %m");
	return (0);
    }
    SYSDEBUG ((LOG_INFO, "set kernel debugging level to %d",
		requested_level));
    return (1);
}


/********************************************************************
 *
 * generic_establish_ppp - Turn the fd into a ppp interface.
 */
int generic_establish_ppp (int fd)
{
    int x;
/*
 * Demand mode - prime the old ppp device to relinquish the unit.
 */
    if (!new_style_driver && looped
	&& ioctl(slave_fd, PPPIOCXFERUNIT, 0) < 0) {
	error("ioctl(transfer ppp unit): %m");
	return -1;
    }


    if (new_style_driver) {
	/* Open another instance of /dev/ppp and connect the channel to it */
	int flags;

	if (ioctl(fd, PPPIOCGCHAN, &chindex) == -1) {
	    error("Couldn't get channel number: %m");
	    goto err;
	}
	dbglog("using channel %d", chindex);
	fd = open("/dev/ppp", O_RDWR);
	if (fd < 0) {
	    error("Couldn't reopen /dev/ppp: %m");
	    goto err;
	}
	if (ioctl(fd, PPPIOCATTCHAN, &chindex) < 0) {
	    error("Couldn't attach to channel %d: %m", chindex);
	    goto err_close;
	}
	flags = fcntl(fd, F_GETFL);
	if (flags == -1 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
	    warn("Couldn't set /dev/ppp (channel) to nonblock: %m");
	set_ppp_fd(fd);

	if (!looped)
	    ifunit = -1;
	if (!looped && !multilink) {
	    /*
	     * Create a new PPP unit.
	     */
	    if (make_ppp_unit() < 0)
		goto err_close;
	}

	if (looped)
	    set_flags(ppp_dev_fd, get_flags(ppp_dev_fd) & ~SC_LOOP_TRAFFIC);

	if (!multilink) {
	    add_fd(ppp_dev_fd);
	    if (ioctl(fd, PPPIOCCONNECT, &ifunit) < 0) {
		error("Couldn't attach to PPP unit %d: %m", ifunit);
		goto err_close;
	    }
	}

    } else {

	/*
	 * Old-style driver: find out which interface we were given.
	 */
	set_ppp_fd (fd);
	if (ioctl(fd, PPPIOCGUNIT, &x) < 0) {
	    if (ok_error (errno))
		goto err;
	    fatal("ioctl(PPPIOCGUNIT): %m(%d)", errno);
	}
	/* Check that we got the same unit again. */
	if (looped && x != ifunit)
	    fatal("transfer_ppp failed: wanted unit %d, got %d", ifunit, x);
	ifunit = x;

	/*
	 * Fetch the initial file flags and reset blocking mode on the file.
	 */
	initfdflags = fcntl(fd, F_GETFL);
	if (initfdflags == -1 ||
	    fcntl(fd, F_SETFL, initfdflags | O_NONBLOCK) == -1) {
	    if ( ! ok_error (errno))
		warn("Couldn't set device to non-blocking mode: %m");
	}
    }


    /*
     * Enable debug in the driver if requested.
     */
    if (!looped)
	set_kdebugflag (kdebugflag);

    SYSDEBUG ((LOG_NOTICE, "Using version %d.%d.%d of PPP driver",
	    driver_version, driver_modification, driver_patch));

    return ppp_fd;

 err_close:
    close(fd);
 err:
    if (ioctl(fd, TIOCSETD, &tty_disc) < 0 && !ok_error(errno))
	warn("Couldn't reset tty to normal line discipline: %m");
    return -1;
}

/********************************************************************
 *
 * generic_disestablish_ppp - Restore device components to normal
 * operation, and reconnect the ppp unit to the loopback if in demand
 * mode.  This shouldn't call die() because it's called from die().
*/
void generic_disestablish_ppp(int dev_fd){
    /* Restore loop if needed */
    if(demand)
	restore_loop();

    /* Finally detach the device */
    initfdflags = -1;

    if (new_style_driver) {
	close(ppp_fd);
	ppp_fd = -1;
	if (!looped && ifunit >= 0 && ioctl(ppp_dev_fd, PPPIOCDETACH) < 0)
	    error("Couldn't release PPP unit: %m");
	if (!multilink)
	    remove_fd(ppp_dev_fd);
    }
}

/*
 * make_ppp_unit - make a new ppp unit for ppp_dev_fd.
 * Assumes new_style_driver.
 */
static int make_ppp_unit()
{
	int x;

	ifunit = req_unit;
	x = ioctl(ppp_dev_fd, PPPIOCNEWUNIT, &ifunit);
	if (x < 0 && req_unit >= 0 && errno == EEXIST) {
		warn("Couldn't allocate PPP unit %d as it is already in use");
		ifunit = -1;
		x = ioctl(ppp_dev_fd, PPPIOCNEWUNIT, &ifunit);
	}
	if (x < 0)
		error("Couldn't create new ppp unit: %m");
	return x;
}

/********************************************************************
 *
 * clean_check - Fetch the flags for the device and generate
 * appropriate error messages.
 */
void clean_check(void)
{
    int x;
    char *s;

    if (still_ppp()) {
	if (ioctl(ppp_fd, PPPIOCGFLAGS, (caddr_t) &x) == 0) {
	    s = NULL;
	    switch (~x & (SC_RCV_B7_0|SC_RCV_B7_1|SC_RCV_EVNP|SC_RCV_ODDP)) {
	    case SC_RCV_B7_0:
		s = "all had bit 7 set to 1";
		break;

	    case SC_RCV_B7_1:
		s = "all had bit 7 set to 0";
		break;

	    case SC_RCV_EVNP:
		s = "all had odd parity";
		break;

	    case SC_RCV_ODDP:
		s = "all had even parity";
		break;
	    }

	    if (s != NULL) {
		warn("Receive serial link is not 8-bit clean:");
		warn("Problem: %s", s);
	    }
	}
    }
}

/********************************************************************
 *
 * output - Output PPP packet.
 */

void output (int unit, unsigned char *p, int len)
{
    int fd = ppp_fd;
    int proto;

    if (debug)
	dbglog("sent %P", p, len);

    if (len < PPP_HDRLEN)
	return;
    if (new_style_driver) {
	p += 2;
	len -= 2;
	proto = (p[0] << 8) + p[1];
	if (ifunit >= 0 && !(proto >= 0xc000 || proto == PPP_CCPFRAG))
	    fd = ppp_dev_fd;
    }
{
    int x;
    unsigned char *c;
    c = p;
//syslog(LOG_NOTICE, "===============================================\n");
    for (x = 0; x < len; x++)
    {
//        syslog(LOG_NOTICE, "%02x, ", *c);
        c++;
    };
//syslog(LOG_NOTICE, "\n===============================================\n");
}
    if (write(fd, p, len) < 0) {
	if (errno == EWOULDBLOCK || errno == ENOBUFS
	    || errno == ENXIO || errno == EIO || errno == EINTR)
	    warn("write: warning: %m (%d)", errno);
	else
	    error("write: %m (%d)", errno);
    }
}

/********************************************************************
 *
 * wait_input - wait until there is data available,
 * for the length of time specified by *timo (indefinite
 * if timo is NULL).
 */

void wait_input(struct timeval *timo)
{
    fd_set ready, exc;
    int n;

    ready = in_fds;
    exc = in_fds;
    n = select(max_in_fd + 1, &ready, NULL, &exc, timo);
    if (n < 0 && errno != EINTR)
	fatal("select: %m(%d)", errno);
}

/*
 * add_fd - add an fd to the set that wait_input waits for.
 */
void add_fd(int fd)
{
    FD_SET(fd, &in_fds);
    if (fd > max_in_fd)
	max_in_fd = fd;
}

/*
 * remove_fd - remove an fd from the set that wait_input waits for.
 */
void remove_fd(int fd)
{
    FD_CLR(fd, &in_fds);
}


/********************************************************************
 *
 * read_packet - get a PPP packet from the serial device.
 */

int read_packet (unsigned char *buf)
{
    int len, nr;

    len = PPP_MRU + PPP_HDRLEN;
    if (new_style_driver) {
	*buf++ = PPP_ALLSTATIONS;
	*buf++ = PPP_UI;
	len -= 2;
    }
    nr = -1;
    if (ppp_fd >= 0) {
	nr = read(ppp_fd, buf, len);
	if (nr < 0 && errno != EWOULDBLOCK && errno != EIO && errno != EINTR)
	    error("read: %m");
	if (nr < 0 && errno == ENXIO)
	    return 0;
    }
    if (nr < 0 && new_style_driver && ifunit >= 0) {
	/* N.B. we read ppp_fd first since LCP packets come in there. */
	nr = read(ppp_dev_fd, buf, len);
	if (nr < 0 && errno != EWOULDBLOCK && errno != EIO && errno != EINTR)
	    error("read /dev/ppp: %m");
	if (nr < 0 && errno == ENXIO)
	    return 0;
    }
    return (new_style_driver && nr > 0)? nr+2: nr;
}

/********************************************************************
 *
 * get_loop_output - get outgoing packets from the ppp device,
 * and detect when we want to bring the real link up.
 * Return value is 1 if we need to bring up the link, 0 otherwise.
 */
int
get_loop_output(void)
{
    int rv = 0;
    int n;

    if (new_style_driver) {
	while ((n = read_packet(inpacket_buf)) > 0)
	    if (loop_frame(inpacket_buf, n))
		rv = 1;
	return rv;
    }

    while ((n = read(master_fd, inbuf, sizeof(inbuf))) > 0)
	if (loop_chars(inbuf, n))
	    rv = 1;

    if (n == 0)
	fatal("eof on loopback");

    if (errno != EWOULDBLOCK)
	fatal("read from loopback: %m(%d)", errno);

    return rv;
}

/*
 * netif_set_mtu - set the MTU on the PPP network interface.
 */
void
netif_set_mtu(int unit, int mtu)
{
    struct ifreq ifr;

    SYSDEBUG ((LOG_DEBUG, "netif_set_mtu: mtu = %d\n", mtu));

    memset (&ifr, '\0', sizeof (ifr));
    strlcpy(ifr.ifr_name, ifname, sizeof (ifr.ifr_name));
    ifr.ifr_mtu = mtu;

    if (ifunit >= 0 && ioctl(sock_fd, SIOCSIFMTU, (caddr_t) &ifr) < 0)
	fatal("ioctl(SIOCSIFMTU): %m");
}

/********************************************************************
 *
 * ccp_test - ask kernel whether a given compression method
 * is acceptable for use.
 */

int ccp_test (int unit, u_char *opt_ptr, int opt_len, int for_transmit)
{
    struct ppp_option_data data;

    memset (&data, '\0', sizeof (data));
    data.ptr      = opt_ptr;
    data.length   = opt_len;
    data.transmit = for_transmit;

    if (ioctl(ppp_dev_fd, PPPIOCSCOMPRESS, (caddr_t) &data) >= 0)
	return 1;

    return (errno == ENOBUFS)? 0: -1;
}

/********************************************************************
 *
 * ccp_flags_set - inform kernel about the current state of CCP.
 */

void ccp_flags_set (int unit, int isopen, int isup)
{
    if (still_ppp()) {
	int x = get_flags(ppp_dev_fd);
	x = isopen? x | SC_CCP_OPEN : x &~ SC_CCP_OPEN;
	x = isup?   x | SC_CCP_UP   : x &~ SC_CCP_UP;
	set_flags (ppp_dev_fd, x);
    }
}

/********************************************************************
 *
 * get_idle_time - return how long the link has been idle.
 */
int
get_idle_time(u, ip)
    int u;
    struct ppp_idle *ip;
{
    return ioctl(ppp_dev_fd, PPPIOCGIDLE, ip) >= 0;
}

/********************************************************************
 *
 * get_ppp_stats - return statistics for the link.
 */
int
get_ppp_stats(u, stats)
    int u;
    struct pppd_stats *stats;
{
    struct ifpppstatsreq req;

    memset (&req, 0, sizeof (req));

    req.stats_ptr = (caddr_t) &req.stats;
    strlcpy(req.ifr__name, ifname, sizeof(req.ifr__name));
    if (ioctl(sock_fd, SIOCGPPPSTATS, &req) < 0) {
	error("Couldn't get PPP statistics: %m");
	return 0;
    }
    stats->bytes_in = req.stats.p.ppp_ibytes;
    stats->bytes_out = req.stats.p.ppp_obytes;
    return 1;
}

/********************************************************************
 *
 * ccp_fatal_error - returns 1 if decompression was disabled as a
 * result of an error detected after decompression of a packet,
 * 0 otherwise.  This is necessary because of patent nonsense.
 */

int ccp_fatal_error (int unit)
{
    int x = get_flags(ppp_dev_fd);

    return x & SC_DC_FERROR;
}

/********************************************************************
 *
 * Return user specified netmask, modified by any mask we might determine
 * for address `addr' (in network byte order).
 * Here we scan through the system's list of interfaces, looking for
 * any non-point-to-point interfaces which might appear to be on the same
 * network as `addr'.  If we find any, we OR in their netmask to the
 * user-specified netmask.
 */

u_int32_t GetMask (u_int32_t addr)
{
    u_int32_t mask, nmask, ina;
    struct ifreq *ifr, *ifend, ifreq;
    struct ifconf ifc;
    struct ifreq ifs[MAX_IFS];

    addr = ntohl(addr);

    if (IN_CLASSA(addr))	/* determine network mask for address class */
	nmask = IN_CLASSA_NET;
    else if (IN_CLASSB(addr))
	    nmask = IN_CLASSB_NET;
    else
	    nmask = IN_CLASSC_NET;

    /* class D nets are disallowed by bad_ip_adrs */
    mask = netmask | htonl(nmask);
/*
 * Scan through the system's network interfaces.
 */
    ifc.ifc_len = sizeof(ifs);
    ifc.ifc_req = ifs;
    if (ioctl(sock_fd, SIOCGIFCONF, &ifc) < 0) {
	if ( ! ok_error ( errno ))
	    warn("ioctl(SIOCGIFCONF): %m(%d)", errno);
	return mask;
    }

    ifend = (struct ifreq *) (ifc.ifc_buf + ifc.ifc_len);
    for (ifr = ifc.ifc_req; ifr < ifend; ifr++) {
/*
 * Check the interface's internet address.
 */
	if (ifr->ifr_addr.sa_family != AF_INET)
	    continue;
	ina = SIN_ADDR(ifr->ifr_addr);
	if (((ntohl(ina) ^ addr) & nmask) != 0)
	    continue;
/*
 * Check that the interface is up, and not point-to-point nor loopback.
 */
	strlcpy(ifreq.ifr_name, ifr->ifr_name, sizeof(ifreq.ifr_name));
	if (ioctl(sock_fd, SIOCGIFFLAGS, &ifreq) < 0)
	    continue;

	if (((ifreq.ifr_flags ^ FLAGS_GOOD) & FLAGS_MASK) != 0)
	    continue;
/*
 * Get its netmask and OR it into our mask.
 */
	if (ioctl(sock_fd, SIOCGIFNETMASK, &ifreq) < 0)
	    continue;
	mask |= SIN_ADDR(ifreq.ifr_addr);
	break;
    }
    return mask;
}

/********************************************************************
 *
 * ppp_available - check whether the system has any ppp interfaces
 * (in fact we check whether we can do an ioctl on ppp0).
 */

int ppp_available(void)
{
    struct utsname utsname;	/* for the kernel version */
    int osmaj, osmin, ospatch;

    /* get the kernel version now, since we are called before sys_init */
    uname(&utsname);
    osmaj = osmin = ospatch = 0;
    sscanf(utsname.release, "%d.%d.%d", &osmaj, &osmin, &ospatch);
    kernel_version = KVERSION(osmaj, osmin, ospatch);

    driver_version = 2;
    driver_modification = 4;
    driver_patch = 0;

    return 1;
}

/********************************************************************
 *
 * sifvjcomp - config tcp header compression
 */

int sifvjcomp (int u, int vjcomp, int cidcomp, int maxcid)
{
    u_int x = get_flags(ppp_dev_fd);

    if (vjcomp) {
        if (ioctl (ppp_dev_fd, PPPIOCSMAXCID, (caddr_t) &maxcid) < 0) {
	    if (! ok_error (errno))
		error("ioctl(PPPIOCSMAXCID): %m(%d)", errno);
	    vjcomp = 0;
	}
    }

    x = vjcomp  ? x | SC_COMP_TCP     : x &~ SC_COMP_TCP;
    x = cidcomp ? x & ~SC_NO_TCP_CCID : x | SC_NO_TCP_CCID;
    set_flags (ppp_dev_fd, x);

    return 1;
}

/********************************************************************
 *
 * sifup - Config the interface up and enable IP packets to pass.
 */

int sifup(int u)
{
    struct ifreq ifr;

    memset (&ifr, '\0', sizeof (ifr));
    strlcpy(ifr.ifr_name, ifname, sizeof (ifr.ifr_name));
    if (ioctl(sock_fd, SIOCGIFFLAGS, (caddr_t) &ifr) < 0) {
	if (! ok_error (errno))
	    error("ioctl (SIOCGIFFLAGS): %m(%d)", errno);
	return 0;
    }

    ifr.ifr_flags |= (IFF_UP | IFF_POINTOPOINT);
    if (ioctl(sock_fd, SIOCSIFFLAGS, (caddr_t) &ifr) < 0) {
	if (! ok_error (errno))
	    error("ioctl(SIOCSIFFLAGS): %m(%d)", errno);
	return 0;
    }
//sam add +++ for add pseudo default route
	system("route add default ppp0");
//sam add ---
    if_is_up++;

    return 1;
}

/********************************************************************
 *
 * sifdown - Disable the indicated protocol and config the interface
 *	     down if there are no remaining protocols.
 */

int sifdown (int u)
{
    struct ifreq ifr;

    if (if_is_up && --if_is_up > 0)
	return 1;

    memset (&ifr, '\0', sizeof (ifr));
    strlcpy(ifr.ifr_name, ifname, sizeof (ifr.ifr_name));
    if (ioctl(sock_fd, SIOCGIFFLAGS, (caddr_t) &ifr) < 0) {
	if (! ok_error (errno))
	    error("ioctl (SIOCGIFFLAGS): %m(%d)", errno);
	return 0;
    }

    ifr.ifr_flags &= ~IFF_UP;
    ifr.ifr_flags |= IFF_POINTOPOINT;
    if (ioctl(sock_fd, SIOCSIFFLAGS, (caddr_t) &ifr) < 0) {
	if (! ok_error (errno))
	    error("ioctl(SIOCSIFFLAGS): %m(%d)", errno);
	return 0;
    }
    return 1;
}

/********************************************************************
 *
 * sifaddr - Config the interface IP addresses and netmask.
 */

int sifaddr (int unit, u_int32_t our_adr, u_int32_t his_adr,
	     u_int32_t net_mask)
{
    struct ifreq   ifr;
    struct rtentry rt;

    memset (&ifr, '\0', sizeof (ifr));
    memset (&rt,  '\0', sizeof (rt));

    SET_SA_FAMILY (ifr.ifr_addr,    AF_INET);
    SET_SA_FAMILY (ifr.ifr_dstaddr, AF_INET);
    SET_SA_FAMILY (ifr.ifr_netmask, AF_INET);

    strlcpy (ifr.ifr_name, ifname, sizeof (ifr.ifr_name));
/*
 *  Set our IP address
 */
    SIN_ADDR(ifr.ifr_addr) = our_adr;
    if (ioctl(sock_fd, SIOCSIFADDR, (caddr_t) &ifr) < 0) {
	if (errno != EEXIST) {
	    if (! ok_error (errno))
		error("ioctl(SIOCSIFADDR): %m(%d)", errno);
	}
        else {
	    warn("ioctl(SIOCSIFADDR): Address already exists");
	}
        return (0);
    }
/*
 *  Set the gateway address
 */
    SIN_ADDR(ifr.ifr_dstaddr) = his_adr;
    if (ioctl(sock_fd, SIOCSIFDSTADDR, (caddr_t) &ifr) < 0) {
	if (! ok_error (errno))
	    error("ioctl(SIOCSIFDSTADDR): %m(%d)", errno);
	return (0);
    }
/*
 *  Set the netmask.
 *  For recent kernels, force the netmask to 255.255.255.255.
 */
    if (kernel_version >= KVERSION(2,1,16))
	net_mask = ~0L;
    if (net_mask != 0) {
	SIN_ADDR(ifr.ifr_netmask) = net_mask;
	if (ioctl(sock_fd, SIOCSIFNETMASK, (caddr_t) &ifr) < 0) {
	    if (! ok_error (errno))
		error("ioctl(SIOCSIFNETMASK): %m(%d)", errno);
	    return (0);
	}
    }
/*
 *  Add the device route
 */
    if (kernel_version < KVERSION(2,1,16)) {
	SET_SA_FAMILY (rt.rt_dst,     AF_INET);
	SET_SA_FAMILY (rt.rt_gateway, AF_INET);
	rt.rt_dev = ifname;

	SIN_ADDR(rt.rt_gateway) = 0L;
	SIN_ADDR(rt.rt_dst)     = his_adr;
	rt.rt_flags = RTF_UP | RTF_HOST;

	if (kernel_version > KVERSION(2,1,0)) {
	    SET_SA_FAMILY (rt.rt_genmask, AF_INET);
	    SIN_ADDR(rt.rt_genmask) = -1L;
	}

	if (ioctl(sock_fd, SIOCADDRT, &rt) < 0) {
	    if (! ok_error (errno))
		error("ioctl(SIOCADDRT) device route: %m(%d)", errno);
	    return (0);
	}
    }

    /* set ip_dynaddr in demand mode if address changes */
    if (demand && tune_kernel && !dynaddr_set
	&& our_old_addr && our_old_addr != our_adr) {
	/* set ip_dynaddr if possible */
	char *path;
	int fd;

	path = "/proc/sys/net/ipv4/ip_dynaddr";
	if (path != 0 && (fd = open(path, O_WRONLY)) >= 0) {
	    if (write(fd, "1", 1) != 1)
		error("Couldn't enable dynamic IP addressing: %m");
	    close(fd);
	}
	dynaddr_set = 1;	/* only 1 attempt */
    }
    our_old_addr = 0;

    return 1;
}

/********************************************************************
 *
 * cifaddr - Clear the interface IP addresses, and delete routes
 * through the interface if possible.
 */

int cifaddr (int unit, u_int32_t our_adr, u_int32_t his_adr)
{
    struct ifreq ifr;

    if (kernel_version < KVERSION(2,1,16)) {
/*
 *  Delete the route through the device
 */
	struct rtentry rt;
	memset (&rt, '\0', sizeof (rt));

	SET_SA_FAMILY (rt.rt_dst,     AF_INET);
	SET_SA_FAMILY (rt.rt_gateway, AF_INET);
	rt.rt_dev = ifname;

	SIN_ADDR(rt.rt_gateway) = 0;
	SIN_ADDR(rt.rt_dst)     = his_adr;
	rt.rt_flags = RTF_UP | RTF_HOST;

	if (kernel_version > KVERSION(2,1,0)) {
	    SET_SA_FAMILY (rt.rt_genmask, AF_INET);
	    SIN_ADDR(rt.rt_genmask) = -1L;
	}

	if (ioctl(sock_fd, SIOCDELRT, &rt) < 0 && errno != ESRCH) {
	    if (still_ppp() && ! ok_error (errno))
		error("ioctl(SIOCDELRT) device route: %m(%d)", errno);
	    return (0);
	}
    }

    /* This way it is possible to have an IPX-only or IPv6-only interface */
    memset(&ifr, 0, sizeof(ifr));
    SET_SA_FAMILY(ifr.ifr_addr, AF_INET);
    strlcpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));

    if (ioctl(sock_fd, SIOCSIFADDR, (caddr_t) &ifr) < 0) {
	if (! ok_error (errno)) {
	    error("ioctl(SIOCSIFADDR): %m(%d)", errno);
	    return 0;
	}
    }

    our_old_addr = our_adr;

    return 1;
}

/********************************************************************
 *
 * open_loopback - open the device we use for getting packets
 * in demand mode.  Under Linux, we use a pty master/slave pair.
 */
int
open_ppp_loopback(void)
{
    int flags;

    looped = 1;
    if (new_style_driver) {
	/* allocate ourselves a ppp unit */
	if (make_ppp_unit() < 0)
	    die(1);
	set_flags(ppp_dev_fd, SC_LOOP_TRAFFIC);
	set_kdebugflag(kdebugflag);
	ppp_fd = -1;
	return ppp_dev_fd;
    }

    if (!get_pty(&master_fd, &slave_fd, loop_name, 0))
	fatal("No free pty for loopback");
    SYSDEBUG(("using %s for loopback", loop_name));

    set_ppp_fd(slave_fd);

    flags = fcntl(master_fd, F_GETFL);
    if (flags == -1 ||
	fcntl(master_fd, F_SETFL, flags | O_NONBLOCK) == -1)
	warn("couldn't set master loopback to nonblock: %m(%d)", errno);

    flags = fcntl(ppp_fd, F_GETFL);
    if (flags == -1 ||
	fcntl(ppp_fd, F_SETFL, flags | O_NONBLOCK) == -1)
	warn("couldn't set slave loopback to nonblock: %m(%d)", errno);

    if (ioctl(ppp_fd, TIOCSETD, &ppp_disc) < 0)
	fatal("ioctl(TIOCSETD): %m(%d)", errno);
/*
 * Find out which interface we were given.
 */
    if (ioctl(ppp_fd, PPPIOCGUNIT, &ifunit) < 0)
	fatal("ioctl(PPPIOCGUNIT): %m(%d)", errno);
/*
 * Enable debug in the driver if requested.
 */
    set_kdebugflag (kdebugflag);

    return master_fd;
}

/********************************************************************
 *
 * restore_loop - reattach the ppp unit to the loopback.
 *
 * The kernel ppp driver automatically reattaches the ppp unit to
 * the loopback if the serial port is set to a line discipline other
 * than ppp, or if it detects a modem hangup.  The former will happen
 * in disestablish_ppp if the latter hasn't already happened, so we
 * shouldn't need to do anything.
 *
 * Just to be sure, set the real serial port to the normal discipline.
 */

void
restore_loop(void)
{
    looped = 1;
    if (new_style_driver) {
	set_flags(ppp_dev_fd, get_flags(ppp_dev_fd) | SC_LOOP_TRAFFIC);
	return;
    }
    if (ppp_fd != slave_fd) {
	(void) ioctl(ppp_fd, TIOCSETD, &tty_disc);
	set_ppp_fd(slave_fd);
    }
}

/********************************************************************
 *
 * sifnpmode - Set the mode for handling packets for a given NP.
 */

int
sifnpmode(u, proto, mode)
    int u;
    int proto;
    enum NPmode mode;
{
    struct npioctl npi;

    npi.protocol = proto;
    npi.mode     = mode;
    if (ioctl(ppp_dev_fd, PPPIOCSNPMODE, (caddr_t) &npi) < 0) {
	if (! ok_error (errno))
	    error("ioctl(PPPIOCSNPMODE, %d, %d): %m (%d)",
		   proto, mode, errno);
	return 0;
    }
    return 1;
}

/*
 * Use the hostname as part of the random number seed.
 */
int
get_host_seed()
{
    int h;
    char *p = hostname;

    h = 407;
    for (p = hostname; *p != 0; ++p)
	h = h * 37 + *p;
    return h;
}


//sam add +++









/********************************************************************
 *
 * Translate from a speed_t to bits/second.
 */

/*
 * List of valid speeds.
 */

struct speed {
    int speed_int, speed_val;
} speeds[] = {
#ifdef B50
    { 50, B50 },
#endif
#ifdef B75
    { 75, B75 },
#endif
#ifdef B110
    { 110, B110 },
#endif
#ifdef B134
    { 134, B134 },
#endif
#ifdef B150
    { 150, B150 },
#endif
#ifdef B200
    { 200, B200 },
#endif
#ifdef B300
    { 300, B300 },
#endif
#ifdef B600
    { 600, B600 },
#endif
#ifdef B1200
    { 1200, B1200 },
#endif
#ifdef B1800
    { 1800, B1800 },
#endif
#ifdef B2000
    { 2000, B2000 },
#endif
#ifdef B2400
    { 2400, B2400 },
#endif
#ifdef B3600
    { 3600, B3600 },
#endif
#ifdef B4800
    { 4800, B4800 },
#endif
#ifdef B7200
    { 7200, B7200 },
#endif
#ifdef B9600
    { 9600, B9600 },
#endif
#ifdef B19200
    { 19200, B19200 },
#endif
#ifdef B38400
    { 38400, B38400 },
#endif
#ifdef B57600
    { 57600, B57600 },
#endif
#ifdef B76800
    { 76800, B76800 },
#endif
#ifdef B115200
    { 115200, B115200 },
#endif
#ifdef EXTA
    { 19200, EXTA },
#endif
#ifdef EXTB
    { 38400, EXTB },
#endif
#ifdef B230400
    { 230400, B230400 },
#endif
#ifdef B460800
    { 460800, B460800 },
#endif
#ifdef B921600
    { 921600, B921600 },
#endif
    { 0, 0 }
};

static int baud_rate_of (int speed)
{
    struct speed *speedp;

    if (speed != 0) {
	for (speedp = speeds; speedp->speed_int; speedp++) {
	    if (speed == speedp->speed_val)
		return speedp->speed_int;
	}
    }
    return 0;
}

/********************************************************************
 *
 * Translate from bits/second to a speed_t.
 */

static int translate_speed (int bps)
{
    struct speed *speedp;

    if (bps != 0) {
	for (speedp = speeds; speedp->speed_int; speedp++) {
	    if (bps == speedp->speed_int)
		return speedp->speed_val;
	}
	warn("speed %d not supported", bps);
    }
    return 0;
}

/********************************************************************
 *
 * setdtr - control the DTR line on the serial port.
 * This is called from die(), so it shouldn't call die().
 */

void setdtr (int tty_fd, int on)
{
    int modembits = TIOCM_DTR;

    ioctl(tty_fd, (on ? TIOCMBIS : TIOCMBIC), &modembits);
}
static int restore_term        = 0;	/* 1 => we've munged the terminal */
static struct termios inittermios;	/* Initial TTY termios */

/********************************************************************
 *
 * set_up_tty: Set up the serial port on `fd' for 8 bits, no parity,
 * at the requested speed, etc.  If `local' is true, set CLOCAL
 * regardless of whether the modem option was specified.
 */
extern int	inspeed;	/* Input/Output speed requested */

void set_up_tty(int tty_fd, int local)
{
    int speed;
    struct termios tios;

    setdtr(tty_fd, 1);
    if (tcgetattr(tty_fd, &tios) < 0) {
	if (!ok_error(errno))
	    fatal("tcgetattr: %m (line %d)", __LINE__);
	return;
    }

    if (!restore_term)
	inittermios = tios;

    tios.c_cflag     &= ~(CSIZE | CSTOPB | PARENB | CLOCAL);
    tios.c_cflag     |= CS8 | CREAD | HUPCL;

    tios.c_iflag      = IGNBRK | IGNPAR;
    tios.c_oflag      = 0;
    tios.c_lflag      = 0;
    tios.c_cc[VMIN]   = 1;
    tios.c_cc[VTIME]  = 0;

    if (local || !modem)
	tios.c_cflag ^= (CLOCAL | HUPCL);

    switch (crtscts) {
    case 1:
	tios.c_cflag |= CRTSCTS;
	break;

    case -2:
	tios.c_iflag     |= IXON | IXOFF;
	tios.c_cc[VSTOP]  = 0x13;	/* DC3 = XOFF = ^S */
	tios.c_cc[VSTART] = 0x11;	/* DC1 = XON  = ^Q */
	break;

    case -1:
	tios.c_cflag &= ~CRTSCTS;
	break;

    default:
	break;
    }

    speed = translate_speed(inspeed);
    if (speed) {
	cfsetospeed (&tios, speed);
	cfsetispeed (&tios, speed);
    }
/*
 * We can't proceed if the serial port speed is B0,
 * since that implies that the serial port is disabled.
 */
    else {
	speed = cfgetospeed(&tios);
	if (speed == B0)
	    fatal("Baud rate for %s is 0; need explicit baud rate", devnam);
    }
/*
    if (tcsetattr(tty_fd, TCSAFLUSH, &tios) < 0)
{
	if (!ok_error(errno))
{
	    fatal("tcsetattr: %m (line %d)", __LINE__);
}
}
*/
    baud_rate    = baud_rate_of(speed);
    restore_term = 1;
}


/********************************************************************
 *
 * tty_set_xaccm - set the extended transmit ACCM for the interface.
 */

void tty_set_xaccm (ext_accm accm)
{
    SYSDEBUG ((LOG_DEBUG, "set_xaccm: %08lx %08lx %08lx %08lx\n",
		accm[0], accm[1], accm[2], accm[3]));
    if (!still_ppp())
	return;
    if (ioctl(ppp_fd, PPPIOCSXASYNCMAP, accm) < 0 && errno != ENOTTY) {
	if ( ! ok_error (errno))
	    warn("ioctl(set extended ACCM): %m(%d)", errno);
    }
}

/********************************************************************
 *
 * tty_send_config - configure the transmit characteristics of
 * the ppp interface.
 */
static int	link_mtu;		/* mtu for the link (not bundle) */

void tty_send_config (int mtu,u_int32_t asyncmap,int pcomp,int accomp)
{
    u_int x;

/*
 * Set the asyncmap and other parameters for the ppp device
 */
    if (!still_ppp())
	return;
    link_mtu = mtu;
    SYSDEBUG ((LOG_DEBUG, "send_config: asyncmap = %lx\n", asyncmap));
    if (ioctl(ppp_fd, PPPIOCSASYNCMAP, (caddr_t) &asyncmap) < 0) {
	if (!ok_error(errno))
	    fatal("ioctl(PPPIOCSASYNCMAP): %m(%d)", errno);
	return;
    }

    x = get_flags(ppp_fd);
    x = pcomp  ? x | SC_COMP_PROT : x & ~SC_COMP_PROT;
    x = accomp ? x | SC_COMP_AC   : x & ~SC_COMP_AC;
    x = sync_serial ? x | SC_SYNC : x & ~SC_SYNC;
    set_flags(ppp_fd, x);
}

/********************************************************************
 *
 * restore_tty - restore the terminal to the saved settings.
 */

void restore_tty (int tty_fd)
{
    if (restore_term) {
	restore_term = 0;
/*
 * Turn off echoing, because otherwise we can get into
 * a loop with the tty and the modem echoing to each other.
 * We presume we are the sole user of this tty device, so
 * when we close it, it will revert to its defaults anyway.
 */
	if (!default_device)
	    inittermios.c_lflag &= ~(ECHO | ECHONL);

	if (tcsetattr(tty_fd, TCSAFLUSH, &inittermios) < 0) {
	    if (! ok_error (errno))
		warn("tcsetattr: %m");
	}
    }
}

/********************************************************************
 *
 * tty_establish_ppp - Turn the serial port into a ppp interface.
 */

int tty_establish_ppp (int tty_fd)
{
    int ret_fd;

/*
 * Ensure that the tty device is in exclusive mode.
 */
//sam modify +++ for integrate l2tpd, pptpd and pppoecd	
    if(!strcmp(parent_mode,"pptpd"))
    {
    if (ioctl(tty_fd, TIOCEXCL, 0) < 0) {
	if ( ! ok_error ( errno ))
	    warn("Couldn't make tty exclusive: %m");
  }
    }
//sam modify ---
/*
 * Set the current tty to the PPP discpline
 */

#ifndef N_SYNC_PPP
#define N_SYNC_PPP 14
#endif
    ppp_disc = (new_style_driver && sync_serial)? N_SYNC_PPP: N_PPP;
    if (ioctl(tty_fd, TIOCSETD, &ppp_disc) < 0) {
	if ( ! ok_error (errno) ) {
	    error("Couldn't set tty to PPP discipline: %m");
	    return -1;
	}
    }

    ret_fd = generic_establish_ppp(tty_fd);
#define SC_RCVB	(SC_RCV_B7_0 | SC_RCV_B7_1 | SC_RCV_EVNP | SC_RCV_ODDP)
#define SC_LOGB	(SC_DEBUG | SC_LOG_INPKT | SC_LOG_OUTPKT | SC_LOG_RAWIN \
		 | SC_LOG_FLUSH)

    set_flags(ppp_fd, ((get_flags(ppp_fd) & ~(SC_RCVB | SC_LOGB))
		       | ((kdebugflag * SC_DEBUG) & SC_LOGB)));
    return ret_fd;
}

/********************************************************************
 *
 * tty_disestablish_ppp - Restore the serial port to normal operation.
 * This shouldn't call die() because it's called from die().
 */

void tty_disestablish_ppp(int tty_fd)
{
    generic_disestablish_ppp(tty_fd);

    if (!hungup) {
/*
 * Flush the tty output buffer so that the TIOCSETD doesn't hang.
 */
	if (tcflush(tty_fd, TCIOFLUSH) < 0)
	    warn("tcflush failed: %m");
/*
 * Restore the previous line discipline
 */
	if (ioctl(tty_fd, TIOCSETD, &tty_disc) < 0) {
	    if ( ! ok_error (errno))
		error("ioctl(TIOCSETD, N_TTY): %m");
	}

	if (ioctl(tty_fd, TIOCNXCL, 0) < 0) {
	    if ( ! ok_error (errno))
		warn("ioctl(TIOCNXCL): %m(%d)", errno);
	}

	/* Reset non-blocking mode on fd. */
	if (initfdflags != -1 && fcntl(tty_fd, F_SETFL, initfdflags) < 0) {
	    if ( ! ok_error (errno))
		warn("Couldn't restore device fd flags: %m");
	}
    }
}

/********************************************************************
 *
 * tty_recv_config - configure the receive-side characteristics of
 * the ppp interface.
 */

void tty_recv_config (int mru,u_int32_t asyncmap,int pcomp,int accomp)
{
    SYSDEBUG ((LOG_DEBUG, "recv_config: mru = %d\n", mru));
/*
 * If we were called because the link has gone down then there is nothing
 * which may be done. Just return without incident.
 */
    if (!still_ppp())
	return;
/*
 * Set the receiver parameters
 */
    if (ioctl(ppp_fd, PPPIOCSMRU, (caddr_t) &mru) < 0) {
	if ( ! ok_error (errno))
	    error("ioctl(PPPIOCSMRU): %m(%d)", errno);
    }
    if (new_style_driver && ifunit >= 0
	&& ioctl(ppp_dev_fd, PPPIOCSMRU, (caddr_t) &mru) < 0)
	error("Couldn't set MRU in generic PPP layer: %m");

    SYSDEBUG ((LOG_DEBUG, "recv_config: asyncmap = %lx\n", asyncmap));
    if (ioctl(ppp_fd, PPPIOCSRASYNCMAP, (caddr_t) &asyncmap) < 0) {
	if (!ok_error(errno))
	    error("ioctl(PPPIOCSRASYNCMAP): %m(%d)", errno);
    }
}


/*
 * get_pty - get a pty master/slave pair and chown the slave side
 * to the uid given.  Assumes slave_name points to >= 16 bytes of space.
 */
int
get_pty(master_fdp, slave_fdp, slave_name, uid)
    int *master_fdp;
    int *slave_fdp;
    char *slave_name;
    int uid;
{
    int i, mfd, sfd = -1;
    char pty_name[16]="\0";
    char tty_name[16]="\0";
    struct termios tios;

mknod("/dev/ttyp0",S_IFCHR | S_IRUSR | S_IWUSR | S_IRGRP |S_IWGRP | S_IROTH | S_IWOTH,makedev(3, 0));
    if (sfd < 0) {
	/* the old way - scan through the pty name space */
//	for (i = 0; i < 64; ++i) {
//	    slprintf(pty_name, sizeof(pty_name), "/dev/pty%c%x", 'p' + i / 16, i % 16);
//sam modify +++ 2008.04.16 for create tty & pty device.
//		strcat(pty_name, "/dev/pty/m0" );
		strcat(pty_name, "/dev/ptyp0" );
//
	    mfd = open(pty_name, O_RDWR, 0);
	    if (mfd >= 0) {
//		pty_name[5] = 't';
		strcat(tty_name, "/dev/ttyp0" );
		sfd = open(tty_name, O_RDWR | O_NOCTTY, 0);
		if (sfd >= 0) {
		    fchown(sfd, uid, -1);
		    fchmod(sfd, S_IRUSR | S_IWUSR);
		}
//sam modify +++ I don't know why there need to close mfd, but close it isatty become 0
//		close(mfd);
//sam modify ---
	    }
//	}
    }

    if (sfd < 0)
	return 0;

    strlcpy(slave_name, tty_name, 16);
    *master_fdp = mfd;
    *slave_fdp = sfd;
    if (tcgetattr(sfd, &tios) == 0) {
	tios.c_cflag &= ~(CSIZE | CSTOPB | PARENB);
	tios.c_cflag |= CS8 | CREAD | CLOCAL;
	tios.c_iflag  = IGNPAR;
	tios.c_oflag  = 0;
	tios.c_lflag  = 0;
	if (tcsetattr(sfd, TCSAFLUSH, &tios) < 0)
	    warn("couldn't set attributes on pty: %m");
    } else 
	warn("couldn't get attributes on pty: %m");

    return 1;
}


//sam add ---

