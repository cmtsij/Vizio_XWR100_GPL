#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/mroute.h>

#include "util.h"

int log_level;
extern char upstream_interface[10][8];

int
upstream_interface_lookup(char *s)
{
	int i;
	
	for (i=0; i<10; i++) 
	   if (strcmp(s,upstream_interface[i]) == 0)
		return 1;

	return 0;
}
char *
myif_indextoname(int sockfd,unsigned int ifindex,char *ifname)
{
	struct ifreq ifr;
	int status;

	memset(&ifr,0,sizeof(struct ifreq));
 	ifr.ifr_ifindex = ifindex;
	
	status = ioctl(sockfd,SIOCGIFNAME,&ifr);
	
	if (status < 0) {
		//dbg_printf("ifindex %d has no device \n",ifindex);
		return NULL;
	}
	else
	{
		//dbg_printf("interface name is %s\n", ifr.ifr_name);
		return strncpy(ifname,ifr.ifr_name,IFNAMSIZ);
	}
}

/*
 * Set/reset the IP_MULTICAST_LOOP. Set/reset is specified by "flag".
 */
void k_set_loop(socket, flag)
    int socket;
    int flag;
{
    u_char loop;

    loop = flag;
    if (setsockopt(socket, IPPROTO_IP, IP_MULTICAST_LOOP,
		   (char *)&loop, sizeof(loop)) < 0)
	printf("setsockopt IP_MULTICAST_LOOP %u\n", loop);
}

/*
 * Set the IP_MULTICAST_IF option on local interface ifa.
 */
void k_set_if(socket, ifa)
    int socket;
    u_long ifa;
{
    struct in_addr adr;

    adr.s_addr = ifa;
    if (setsockopt(socket, IPPROTO_IP, IP_MULTICAST_IF,
		   (char *)&adr, sizeof(adr)) < 0)
	printf("ERROR in setsockopt IP_MULTICAST_IF \n");
}


/*
 * void debug(int level)
 *
 * Write to stdout
 */
void debug(int level, const char* fmt, ...)
{
	va_list args;

	if (level < log_level)
		return;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
}

/*
 * u_short in_cksum(u_short *addr, int len)
 *
 * Compute the inet checksum
 */
unsigned short in_cksum(unsigned short *addr, int len)
{
    int nleft = len;
    int sum = 0;
    unsigned short *w = addr;
    unsigned short answer = 0;

    while (nleft > 1) {
        sum += *w++;
        nleft -= 2;
    }
    if (nleft == 1) {
        *(unsigned char*)(&answer) = *(unsigned char*)w;
        sum += answer;
    }
    sum = (sum >> 16) + (sum & 0xffff);
    answer = ~sum;
    return (answer);
}

/*
 * interface_list_t* get_interface_list(short af, short flags, short unflags)
 *
 * Get the list of interfaces with an address from family af, and whose flags 
 * match 'flags' and don't match 'unflags'. 
 */
interface_list_t* get_interface_list(short af, short flags, short unflags)
{
    char *p, buf[IFNAMSIZ];
    interface_list_t *ifp, *ifprev, *list;
    struct sockaddr *psa;
    struct ifreq ifr;
    int sockfd;
    int i, err;

    sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (sockfd <= 0)
    {
        dbg_printf("sockfd <= 0\n");
        return NULL;
    }
    list = ifp = ifprev = NULL;
    for (i=2; i<= 32; i++) {
    	p = myif_indextoname(sockfd,i, buf);
        if (p == NULL)
	   		continue;

/*
	FixME : 
		The interface name shoudl get from nvram or should be prefined,
		Otherwise, there will have many porting issue.
 */
        //if (strncmp(p ,"eth",3) ==0) /* skip eth bridge */
//	Gemtek	Gamble, Start        
//        if (strncmp(p ,"eth1",4) ==0) /* Joshua - skip eth1 (LAN)  */
//		  if (strncmp(p ,"eth0",4) ==0) /* Gamble - skip eth0 (LAN)  */	
//		  	continue;
//	Gemtek 	Gamble, End

//	Gemtek	Gamble, Start
//        if (strncmp(p ,"ra0",3) ==0) /* Joshua - skip Wireless */
		  if (strncmp(p ,"eth2",4) ==0) /* Gamble - skip Wireless */	
		  	continue;
//	Gemtek 	Gamble, End
	   		
        if (strncmp(p ,"usb",3) ==0) /* skip usb */
	   		continue;

		/* skip the one user did not enable from webUI */
		if ( upstream_interface_lookup(p) == 0 && strncmp(p,"br",2) != 0)
	   		continue;

        strncpy(ifr.ifr_name, p , IFNAMSIZ);
		dbg_printf("(%s->%s, line %d), get interface: %s\n",__FILE__,__FUNCTION__,__LINE__,p);
        err = ioctl(sockfd, SIOCGIFADDR, (void*)&ifr);
        psa = &ifr.ifr_ifru.ifru_addr;
//eddie
#if 0
        if (err == -1 || psa->sa_family != af)
            continue;
        err = ioctl(sockfd, SIOCGIFFLAGS, (void*)&ifr);
        if (err == -1)
            continue;
        if (((ifr.ifr_flags & flags) != flags) ||
            ((ifr.ifr_flags & unflags) != 0))
            continue;
#endif
        ifp = (interface_list_t*) malloc(sizeof(*ifp));
        if (ifp) {
	    	ifp->ifl_index =i;
	     dbg_printf("ifp->ifl_index=%d name=%s\n",i,ifr.ifr_name);
            strncpy(ifp->ifl_name, ifr.ifr_name, IFNAMSIZ);
            memcpy(&ifp->ifl_addr, psa, sizeof(*psa));
            ifp->ifl_next = NULL;
            if (list == NULL)
                list = ifp;
            if (ifprev != NULL)
                ifprev->ifl_next = ifp;
            ifprev = ifp;
        }
    }
    close(sockfd);
    return list;
}

/*
 * void free_interface_list(interface_list_t *ifl)
 *
 * Free a list of interfaces
 */
void free_interface_list(interface_list_t *ifl)
{
    interface_list_t *ifp = ifl;

    while (ifp) {
        ifl = ifp;
        ifp = ifp->ifl_next;
        free(ifl);
    }
}

/*
 * short get_interface_flags(char *ifname)
 *
 * Get the value of the flags for a certain interface 
 */
short get_interface_flags(char *ifname)
{
    struct ifreq ifr;
    int sockfd, err;

    sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (sockfd <= 0)
        return -1;
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	err = ioctl(sockfd, SIOCGIFFLAGS, (void*)&ifr);
	close(sockfd);
	if (err == -1)
		return -1;
	return ifr.ifr_flags; 
}

/*
 * short set_interface_flags(char *ifname, short flags)
 *
 * Set the value of the flags for a certain interface 
 */
short set_interface_flags(char *ifname, short flags)
{
    struct ifreq ifr;
    int sockfd, err;

    sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (sockfd <= 0)
        return -1;
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	ifr.ifr_flags = flags; 
	err = ioctl(sockfd, SIOCSIFFLAGS, (void*)&ifr);
	close(sockfd);
	if (err == -1)
		return -1;
	return 0; 
}

/*
 * short get_interface_flags(char *ifname)
 *
 * Get the value of the flags for a certain interface 
 */
int get_interface_mtu(char *ifname)
{
    struct ifreq ifr;
    int sockfd, err;

    sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (sockfd <= 0)
        return -1;
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
    err = ioctl(sockfd, SIOCGIFMTU, (void*)&ifr);
    close(sockfd);
    if (err == -1)
	return -1;
    return ifr.ifr_mtu; 
}

/*
 * int mrouter_onoff(int sockfd, int onoff)
 *
 * Tell the kernel if a multicast router is on or off 
 */
int mrouter_onoff(int sockfd, int onoff)
{
    int err, cmd, i;

	cmd = (onoff) ? MRT_INIT : MRT_DONE; 
	i = 1;
	//err = setsockopt(sockfd, IPPROTO_IP, cmd, (void*)&i, sizeof(i));
	err = setsockopt(sockfd, IPPROTO_IP, cmd, (char*)&i, sizeof(i)); //Joshua
	return err; 
}

