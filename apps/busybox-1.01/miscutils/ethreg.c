/*
 * Copyright (c) 2002-2005 Atheros Communications, Inc.
 * All rights reserved.
 *
 * $Id: //depot/sw/releases/7.3_AP/apps/busybox-1.01/miscutils/ethreg.c#1 $
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <getopt.h>
#include <errno.h>
#include <err.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <net/if.h>

#define SIOCDEVPRIVATE  0x89F0  /* to 89FF */
#define S26_RD_PHY  (SIOCDEVPRIVATE | 0x1)
#define S26_WR_PHY  (SIOCDEVPRIVATE | 0x2)
#define S26_FORCE_PHY  (SIOCDEVPRIVATE | 0x3)

struct eth_diag {
    char    ad_name[IFNAMSIZ];      /* if name, e.g. "eth0" */
    union {
        u_int16_t portnum;           /* pack to fit, yech */
        u_int8_t duplex;
    }ed_u;
    u_int32_t phy_reg;
    u_int   val;
    caddr_t ad_in_data;
    caddr_t ad_out_data;
};

struct eth_diag etd;
int s,opt_force = 0,duplex = 1;
const char *progname;

u_int32_t
regread(u_int32_t phy_reg,u_int16_t portno)
{
	etd.phy_reg = phy_reg;
        etd.ed_u.portnum = portno;

	if (ioctl(s,S26_RD_PHY, &etd) < 0)
		err(1, etd.ad_name);

	return etd.val;
}

static void
regwrite(u_int32_t phy_reg,u_int32_t val,u_int16_t portno)
{

	etd.val = val;
	etd.phy_reg = phy_reg;

        if(opt_force)  {
            etd.ed_u.duplex = duplex;
	    if (ioctl(s,S26_FORCE_PHY, &etd) < 0)
		err(1, etd.ad_name);
            opt_force = 0;
        }
        else {
            etd.ed_u.portnum = portno;
	    if (ioctl(s,S26_WR_PHY, &etd) < 0)
		err(1, etd.ad_name);
        }
}

static void
usage()
{
	fprintf(stderr, "usage: %s [-i ifname] [-p portnum] offset[=value]\n", progname);
	fprintf(stderr, "usage: %s [-f]  portnum=10/100/0 [-d duplex]\n", progname);
	exit(-1);
}

int
ethreg_main(int argc, char *argv[])
{
	const char *ifname = "eth0";
	int c,portnum = 0xf;

	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0)
		err(1, "socket");

        opt_force = 0;
	progname = argv[0];

	while ((c = getopt(argc, argv, "fi:p:d:")) != -1) { 
	    switch (c) {
		case 'i':
			ifname = optarg;
			break;
		case 'p':
			portnum = strtoul(optarg,0,0);
			break;
		case 'f':
			opt_force = 1;
			break;
                case 'd':
			duplex = strtoul(optarg,0,0);
			break;
		default:
			usage();
			/*NOTREACHED*/
		}

        }

	argc -= optind;
	argv += optind;
        
	strncpy(etd.ad_name, ifname, sizeof (etd.ad_name));

        for (; argc > 0; argc--, argv++) {
                u_int32_t   off;
                u_int32_t  val, oval;
                char *cp;

                cp = strchr(argv[0], '=');

                if (cp != NULL)
                        *cp = '\0';

                off = (u_int) strtoul(argv[0], 0, 0);

                if (off == 0 && errno == EINVAL)
                        errx(1, "%s: invalid reg offset %s",
                             progname, argv[0]);

                if (cp == NULL) {
                    val = regread(off,portnum);
                    printf("Read Reg: 0x%08x = 0x%08x\n",off, val);
                    return 0;
                } else {
                    val = (u_int32_t) strtoul(cp+1, 0, 0);
                    if (val == 0 && errno == EINVAL) {
                            errx(1, "%s: invalid reg value %s",
                                 progname, cp+1);
                     }
                     else {
			
                        oval = regread(off,portnum);
			if(opt_force == 0)
                            printf("Write Reg: 0x%08x: Oldval = 0x%08x Newval = 0x%08x\n", off, oval, val);
                        regwrite(off,val,portnum);
                     }
                }
        }
        return 0;
}

