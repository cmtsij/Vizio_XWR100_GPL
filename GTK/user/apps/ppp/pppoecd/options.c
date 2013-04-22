/*
 * options.c - handles option processing for PPP.
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

#define RCSID	"$Id$"

#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include "pppd.h"
#include "fsm.h"
#include "lcp.h"

int	debug = 0;		/* Debug flag */
int	kdebugflag = 0;		/* Tell kernel to print debug messages */
int	default_device = 1;	/* Using /dev/tty or equivalent */
char	devnam[MAXPATHLEN];	/* Device name */
bool	nodetach = 0;		/* Don't detach from controlling tty */
bool	updetach = 0;		/* Detach once link is up */
int	maxconnect = 0;		/* Maximum connect time */
char	user[MAXNAMELEN];	/* Username for PAP */
char	passwd[MAXSECRETLEN];	/* Password for PAP */
bool	persist = 0;		/* Reopen link after it goes down */
char	our_name[MAXNAMELEN];	/* Our name for authentication purposes */
char	parent_mode[MAXNAMELEN];/* parent_mode for caller's mode */
char	server_ip[MAXNAMELEN];	/* pptp server ip*/
bool	demand = 0;		/* do dial-on-demand */
char	*ipparam = NULL;	/* Extra parameter for ip up/down scripts */
int	idle_time_limit = 0;	/* Disconnect if idle for this many seconds */
int	holdoff = 30;		/* # seconds to pause before reconnecting */
bool	holdoff_specified;	/* true if a holdoff value has been given */
int	log_to_fd = 1;		/* send log messages to this fd too */
bool	log_default = 1;	/* log_to_fd is default (stdout) */
int	maxfail = 10;		/* max # of unsuccessful connection attempts */
char	linkname[MAXPATHLEN];	/* logical name for link */
bool	tune_kernel = 1;	/* may alter kernel settings */
int	connect_delay = 1000;	/* wait this many ms after connect script */
int	req_unit = -1;		/* requested interface unit */
char	*bundle_name = NULL;	/* bundle name for multilink */
bool	dump_options;		/* print out option values */
bool	dryrun;			/* print out option values and exit */
char	*domain;		/* domain name set by domain option */
int	baud_rate;		/* Actual bits/second for serial device */

char *current_option;		/* the name of the option being parsed */
int  privileged_option;		/* set iff the current option came from root */
char *option_source;		/* string saying where the option came from */
int  option_priority = OPRIO_CFGFILE; /* priority of the current options */
bool devnam_fixed;		/* can no longer change device name */

extern char *pppoe_ac_name;
extern char *pppoe_srv_name;
extern int setdevname_pppoe(const char *cp);

static char *usage_string = "usage: %s interface -d -k [-i idle] [-u username] [-m modename] [-p passwd] [-a acname] [-s srvname] [-r mru] [-t mtu]\n";

/*
 * parse_args - parse a string of arguments from the command line.
 */
int
parse_args(argc, argv)
    int argc;
    char **argv;
{
    int opt, j;
//	for(j=0;j<argc;j++)
//		syslog(LOG_NOTICE, "j=%d,value=%s\n", j, argv[j]);
    while ((opt = getopt(argc, argv, "dki:u:p:m:I:a:s:r:t:U:")) != -1) {
//syslog(LOG_NOTICE, "opt=%d,optarg=%s\n", opt, optarg );
	    switch (opt) {
	    case 'd':
		    debug = nodetach = 1;
		    break;
	    case 'k':
		    persist = 1;
		    break;
	    case 'i':
		    idle_time_limit = atoi(optarg);
		    if (idle_time_limit > 0)
			    demand = 1;
		    break;
	    case 'u':
		    strncpy(user, optarg, MAXNAMELEN);
		    strncpy(our_name, optarg, MAXNAMELEN);
		    break;
//sam add +++ for integrate pppoecd and l2tpd
	    case 'm':
		    strncpy(parent_mode, optarg, MAXNAMELEN);
		    break;
	    case 'I':
		    strncpy(server_ip, optarg, MAXNAMELEN);
		    break;
//sam add ---
	    case 'p':
		    strncpy(passwd, optarg, MAXSECRETLEN);
		    break;
	    case 'a':
		    pppoe_ac_name = optarg;
		    break;
	    case 's':
		    pppoe_srv_name = optarg;
		    break;
	    case 'r':
		    lcp_wantoptions[0].neg_mru = 1;
		    lcp_wantoptions[0].mru = atoi(optarg);
		    break;
	    case 't':
		    lcp_allowoptions[0].mru = atoi(optarg);
		    break;
	    case 'U':
		    req_unit = atoi(optarg);
		    break;
	    default:
		    fprintf(stderr, usage_string, argv[0]);
		    return 0;
	    }
    }
//sam modify +++ for integrate l2tpd, pptpd and pppoecd
if(!strcmp(parent_mode,"l2tpd"))
{
	return 1;
}
else if(!strcmp(parent_mode,"pptpd"))
{
	return 1;
}
else
{
    if (optind < argc)
	    setdevname_pppoe(argv[optind]);
    else {
	    fprintf(stderr, usage_string, argv[0]);
	    return 0;
    }

    return 1;
}
//sam modify ---
}



//sam add +++
#ifndef _ROOT_PATH
#define _ROOT_PATH
#endif
#define _PATH_TTYOPT	 _ROOT_PATH "/tmp/ppp/options."
/*
 * options_for_tty - See if an options file exists for the serial
 * device, and if so, interpret options from it.
 * We only allow the per-tty options file to override anything from
 * the command line if it is something that the user can't override
 * once it has been set by root; this is done by giving configuration
 * files a lower priority than the command line.
 */

int
options_for_tty()
{
    char *dev, *path, *p;
    int ret;
    size_t pl;

    dev = devnam;
    if (strncmp(dev, "/dev/", 5) == 0)
	dev += 5;
    if (dev[0] == 0 || strcmp(dev, "tty") == 0)
	return 1;		/* don't look for /etc/ppp/options.tty */
    pl = strlen(_PATH_TTYOPT) + strlen(dev) + 1;
    path = malloc(pl);
    if (path == NULL)
	novm("tty init file name");
    slprintf(path, pl, "%s%s", _PATH_TTYOPT, dev);
    /* Turn slashes into dots, for Solaris case (e.g. /dev/term/a) */
    for (p = path + strlen(_PATH_TTYOPT); *p != 0; ++p)
	if (*p == '/')
	    *p = '.';
    option_priority = OPRIO_CFGFILE;
    ret = options_from_file(path, 0, 0, 1);
    free(path);
    return ret;
}


//sam add ---


