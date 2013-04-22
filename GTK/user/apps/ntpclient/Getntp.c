/*
 * ntpclient.c - NTP client
 *
 * Copyright 1997, 1999, 2000  Larry Doolittle  <larry@doolittle.boa.org>
 * Last hack: 2 December, 2000
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License (Version 2,
 *  June 1991) as published by the Free Software Foundation.  At the
 *  time of writing, that license was published by the FSF with the URL
 *  http://www.gnu.org/copyleft/gpl.html, and is incorporated herein by
 *  reference.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  Possible future improvements:
 *      - Double check that the originate timestamp in the received packet
 *        corresponds to what we sent.
 *      - Verify that the return packet came from the host we think
 *        we're talking to.  Not necessarily useful since UDP packets
 *        are so easy to forge.
 *      - Complete phase locking code.
 *      - Write more documentation  :-(
 *
 *  Compile with -D_PRECISION_SIOCGSTAMP if your machine really has it.
 *  There are patches floating around to add this to Linux, but
 *  usually you only get an answer to the nearest jiffy.
 *  Hint for Linux hacker wannabes: look at the usage of get_fast_time()
 *  in net/core/dev.c, and its definition in kernel/time.c .
 *
 *  If the compile gives you any flak, check below in the section
 *  labelled "XXXX fixme - non-automatic build configuration".
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/utsname.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#ifdef _PRECISION_SIOCGSTAMP
#include <linux/ioctl.h>
#endif
#ifdef TIMEZONE
#include <bcmnvram.h>
#endif



#define ENABLE_DEBUG

extern char *optarg;

/* XXXX fixme - non-automatic build configuration */
#ifdef linux
typedef u_int32_t __u32;
#include <sys/timex.h>
#else
extern int h_errno;
#define herror(hostname) \
	fprintf(stderr,"Error %d looking up hostname %s\n", h_errno,hostname)
typedef uint32_t __u32;
#endif

#define JAN_1970        0x83aa7e80      /* 2208988800 1970 - 1900 in seconds */
#define NTP_PORT (123)

/* How to multiply by 4294.967296 quickly (and not quite exactly)
 * without using floating point or greater than 32-bit integers.
 * If you want to fix the last 12 microseconds of error, add in
 * (2911*(x))>>28)
 */
#define NTPFRAC(x) ( 4294*(x) + ( (1981*(x))>>11 ) )

/* The reverse of the above, needed if we want to set our microsecond
 * clock (via settimeofday) based on the incoming time in NTP format.
 * Basically exact.
 */
#define USEC(x) ( ( (x) >> 12 ) - 759 * ( ( ( (x) >> 10 ) + 32768 ) >> 16 ) )

/* Converts NTP delay and dispersion, apparently in seconds scaled
 * by 65536, to microseconds.  RFC1305 states this time is in seconds,
 * doesn't mention the scaling.
 * Should somehow be the same as 1000000 * x / 65536
 */
#define sec2u(x) ( (x) * 15.2587890625 )

struct ntptime {
	unsigned int coarse;
	unsigned int fine;
};

void send_packet(int usd);
void rfc1305print(char *data, struct ntptime *arrival);
void udp_handle(int usd, char *data, int data_len, struct sockaddr *sa_source, int sa_len);

/* global variables (I know, bad form, but this is a short program) */
char incoming[1500];
struct timeval time_of_send;
int live=0;
int set_clock=0;   /* non-zero presumably needs root privs */

#ifdef ENABLE_DEBUG
int debug=0;
#define DEBUG_OPTION "d"
#else
#define debug 0
#define DEBUG_OPTION
#endif

int contemplate_data(unsigned int absolute, double skew, double errorbar, int freq);

int get_current_freq()
{
	/* OS dependent routine to get the current value of clock frequency.
	 */
#if 0
	struct timex txc;
	txc.modes=0;
	if (__adjtimex(&txc) < 0) {
		perror("adjtimex"); exit(1);
	}
	return txc.freq;
#else
	return 0;
#endif
}

int set_freq(int new_freq)
{
	/* OS dependent routine to set a new value of clock frequency.
	 */
#if 0
	struct timex txc;
	txc.modes = ADJ_FREQUENCY;
	txc.freq = new_freq;
	if (__adjtimex(&txc) < 0) {
		perror("adjtimex"); exit(1);
	}
	return txc.freq;
#else
	return 0;
#endif
}

void send_packet(int usd)
{
	__u32 data[12];
	struct timeval now;
#define LI 0
#define VN 3
#define MODE 3
#define STRATUM 0
#define POLL 4 
#define PREC -6

	if (debug) fprintf(stderr,"Sending ...\n");
	if (sizeof(data) != 48) {
		fprintf(stderr,"size error\n");
		return;
	}
	bzero(data,sizeof(data));
	data[0] = htonl (
		( LI << 30 ) | ( VN << 27 ) | ( MODE << 24 ) |
		( STRATUM << 16) | ( POLL << 8 ) | ( PREC & 0xff ) );
	data[1] = htonl(1<<16);  /* Root Delay (seconds) */
	data[2] = htonl(1<<16);  /* Root Dispersion (seconds) */
	gettimeofday(&now,NULL);
	data[10] = htonl(now.tv_sec + JAN_1970); /* Transmit Timestamp coarse */
	data[11] = htonl(NTPFRAC(now.tv_usec));  /* Transmit Timestamp fine   */
	send(usd,data,48,0);
	time_of_send=now;
}


void udp_handle(int usd, char *data, int data_len, struct sockaddr *sa_source, int sa_len)
{
	struct timeval udp_arrival;
	struct ntptime udp_arrival_ntp;

#ifdef _PRECISION_SIOCGSTAMP
	if ( ioctl(usd, SIOCGSTAMP, &udp_arrival) < 0 ) {
		perror("ioctl-SIOCGSTAMP");
		gettimeofday(&udp_arrival,NULL);
	}
#else
	gettimeofday(&udp_arrival,NULL);
#endif
	udp_arrival_ntp.coarse = udp_arrival.tv_sec + JAN_1970;
	udp_arrival_ntp.fine   = NTPFRAC(udp_arrival.tv_usec);

	if (debug) {
		struct sockaddr_in *sa_in=(struct sockaddr_in *)sa_source;
		//printf("packet of length %d received\n",data_len);
		if (sa_source->sa_family==AF_INET) {
			//printf("Source: INET Port %d host %s\n",
			//	ntohs(sa_in->sin_port),inet_ntoa(sa_in->sin_addr));
		} else {
			//printf("Source: Address family %d\n",sa_source->sa_family);
		}
	}
	rfc1305print(data,&udp_arrival_ntp);
}

double ntpdiff( struct ntptime *start, struct ntptime *stop)
{
	int a;
	unsigned int b;
	a = stop->coarse - start->coarse;
	if (stop->fine >= start->fine) {
		b = stop->fine - start->fine;
	} else {
		b = start->fine - stop->fine;
		b = ~b;
		a -= 1;
	}

	return a*1.e6 + b * (1.e6/4294967296.0);
}

void rfc1305print(char *data, struct ntptime *arrival)
{
/* straight out of RFC-1305 Appendix A */
	int li, vn, mode, stratum, poll, prec;
	int delay, disp, refid;
	struct ntptime reftime, orgtime, rectime, xmttime;
	double etime,stime,skew1,skew2;
	int freq;

#define Data(i) ntohl(((unsigned int *)data)[i])
	li      = Data(0) >> 30 & 0x03;
	vn      = Data(0) >> 27 & 0x07;
	mode    = Data(0) >> 24 & 0x07;
	stratum = Data(0) >> 16 & 0xff;
	poll    = Data(0) >>  8 & 0xff;
	prec    = Data(0)       & 0xff;
	if (prec & 0x80) prec|=0xffffff00;
	delay   = Data(1);
	disp    = Data(2);
	refid   = Data(3);
	reftime.coarse = Data(4);
	reftime.fine   = Data(5);
	orgtime.coarse = Data(6);
	orgtime.fine   = Data(7);
	rectime.coarse = Data(8);
	rectime.fine   = Data(9);
	xmttime.coarse = Data(10);
	xmttime.fine   = Data(11);
#undef Data

#ifdef TIMEZONE				// andy add 20021127 start
	float timezone;
	int autodayli;
	char strrcv[64],*cp , *cp2;
/*
	strrcv= (char *) nvram_safe_get("TimeZone");
	if (strrcv != NULL)
		timezone = atof(strrcv);
	else
		timezone = 0;			//default time zone is GreenWich
*/
  strcpy(strrcv, nvram_safe_get("timezone_diff"));
  /* + and - are inversed */
  if( (cp=strchr( strrcv , '+')) != NULL )
  {
  	*cp = 0x0; cp++; 
  	if( (cp2 = strchr( cp , ':')) != NULL )
  	{
  			*cp2 = 0x0; 
  			cp2++;
  			printf("cp = %f\n" , atof( cp ));
  		  timezone = atof( cp )*-1.0 + atof( cp2 )/60.0*-1.0;
  	}
  	else
  		  timezone = atof( cp )*-1.0;	
  }	
  else if ( (cp=strchr( strrcv , '-')) != NULL )		
  {
    *cp = 0x0; cp++; 
  	if( (cp2 = strchr( cp , ':')) != NULL )
  	{
  			*cp2 = 0x0; cp2++;
  		  timezone = (atof( cp ) + atof( cp2 )/60.0);
  	}
  	else
  		  timezone = atof( cp );	
  }  
  else
  		timezone = 0;
  
	strcpy( strrcv, nvram_safe_get("timezone_daylightsaving"));
	if (strrcv != NULL)
		autodayli= atoi(strrcv);
	else
		autodayli = 0;			//default setting is disable autodaylight
#endif 						// andy add 20021127 end

	if (set_clock) {   /* you'd better be root, or ntpclient will crash! */
		struct timeval tv_set;
		struct timezone	tzone;
		/* it would be even better to subtract half the slop */
#ifdef TIMEZONE 		   // andy add 20021127 start
		tv_set.tv_sec  = xmttime.coarse + (int)(3600*timezone) - JAN_1970;
		//tv_set.tv_sec  = xmttime.coarse - JAN_1970;
		tzone.tz_minuteswest = (int)(60 * timezone);
#else
		tv_set.tv_sec  = xmttime.coarse - JAN_1970;
#endif					  // andy add 20021127 end
  		/* divide xmttime.fine by 4294.967296 */
		tv_set.tv_usec = USEC(xmttime.fine);

#ifdef DAYLIGHTSAVE
		if (autodayli == 1){
			if ( autodaylightsave(&tv_set) ){
				tv_set.tv_sec  = tv_set.tv_sec + 3600;
				tzone.tz_dsttime = 1;
				printf(" In time period of Daylight Saving\n");
			}
			else{
				printf(" Not in time period of daylight saving\n");
				tzone.tz_dsttime = 0;
			}
			//settimeofday(&tv_set,NULL);
			settimeofday(&tv_set, &tzone);
		}
	    else {
			tzone.tz_dsttime = 0;
			//settimeofday(&tv_set,NULL);
			settimeofday(&tv_set, &tzone);
		}
#else
		if (settimeofday(&tv_set,NULL)<0) {
			perror("settimeofday");
			exit(1);
		}
#endif
		if (debug) {
			//printf("set time to %lu.%.6lu\n", tv_set.tv_sec, tv_set.tv_usec);
		}
	}

	if (debug) {
	printf("LI=%d  VN=%d  Mode=%d  Stratum=%d  Poll=%d  Precision=%d\n",
		li, vn, mode, stratum, poll, prec);
	printf("Delay=%.1f  Dispersion=%.1f  Refid=%u.%u.%u.%u\n",
		sec2u(delay),sec2u(disp),
		refid>>24&0xff, refid>>16&0xff, refid>>8&0xff, refid&0xff);
	printf("Reference %u.%.10u\n", reftime.coarse, reftime.fine);
	printf("Originate %u.%.10u\n", orgtime.coarse, orgtime.fine);
	printf("Receive   %u.%.10u\n", rectime.coarse, rectime.fine);
	printf("Transmit  %u.%.10u\n", xmttime.coarse, xmttime.fine);
	printf("Our recv  %u.%.10u\n", arrival->coarse, arrival->fine);
	}
	etime=ntpdiff(&orgtime,arrival);
	stime=ntpdiff(&rectime,&xmttime);
	skew1=ntpdiff(&orgtime,&rectime);
	skew2=ntpdiff(&xmttime,arrival);
	freq=get_current_freq();
	if (debug) {
	printf("Total elapsed: %9.2f\n"
	       "Server stall:  %9.2f\n"
	       "Slop:          %9.2f\n",
		etime, stime, etime-stime);
	printf("Skew:          %9.2f\n"
	       "Frequency:     %9d\n"
	       " day   second     elapsed    stall     skew  dispersion  freq\n",
		(skew1-skew2)/2, freq);
	}
	printf("%d %5d.%.3d  %8.1f %8.1f  %8.1f %8.1f %9d\n",
		arrival->coarse/86400+15020, arrival->coarse%86400,
		arrival->fine/4294967, etime, stime,
		(skew1-skew2)/2, sec2u(disp), freq);
	fflush(stdout);
	if (live) {
		int new_freq;
		new_freq = contemplate_data(arrival->coarse, (skew1-skew2)/2,
			etime+sec2u(disp), freq);
		if (!debug && new_freq != freq) set_freq(new_freq);
	}
}

void stuff_net_addr(struct in_addr *p, char *hostname)
{
	struct hostent *ntpserver;
	ntpserver=gethostbyname(hostname);
	if (ntpserver == NULL) {
		herror(hostname);
		exit(1);
	}
	if (ntpserver->h_length != 4) {
		fprintf(stderr,"oops %d\n",ntpserver->h_length);
		exit(1);
	}
	memcpy(&(p->s_addr),ntpserver->h_addr_list[0],4);
}

void setup_receive(int usd, unsigned int interface, short port)
{
	struct sockaddr_in sa_rcvr;
	bzero((char *) &sa_rcvr, sizeof(sa_rcvr));
	sa_rcvr.sin_family=AF_INET;
	sa_rcvr.sin_addr.s_addr=htonl(interface);
	sa_rcvr.sin_port=htons(port);
	if(bind(usd,(struct sockaddr *) &sa_rcvr,sizeof(sa_rcvr)) == -1) {
		fprintf(stderr,"could not bind to udp port %d\n",port);
		perror("bind");
		exit(1);
	}
	listen(usd,3);
}

void setup_transmit(int usd, char *host, short port)
{
	struct sockaddr_in sa_dest;
	bzero((char *) &sa_dest, sizeof(sa_dest));
	sa_dest.sin_family=AF_INET;
	stuff_net_addr(&(sa_dest.sin_addr),host);
	sa_dest.sin_port=htons(port);
	if (connect(usd,(struct sockaddr *)&sa_dest,sizeof(sa_dest))==-1)
		//{perror("connect");exit(1);}
                	{perror("connect");return ;}
}

void primary_loop(int usd, int num_probes, int interval)
{
	fd_set fds;
	struct sockaddr sa_xmit;
	int i, pack_len, sa_xmit_len, probes_sent;
	struct timeval to;

	if (debug) printf("Listening...\n");

	probes_sent=0;
	sa_xmit_len=sizeof(sa_xmit);
	to.tv_sec=0;
	to.tv_usec=0;
	for (;;) {
		FD_ZERO(&fds);
		FD_SET(usd,&fds);
		i=select(usd+1,&fds,NULL,NULL,&to);  /* Wait on read or error */
		if ((i!=1)||(!FD_ISSET(usd,&fds))) {
			if (i==EINTR) continue;
			if (i<0) perror("select");
			if (to.tv_sec == 0) {
				if (probes_sent >= num_probes &&
					num_probes != 0) break;
				send_packet(usd);
				++probes_sent;
				to.tv_sec=interval;
				to.tv_usec=0;
			}	
			continue;
		}
		pack_len=recvfrom(usd,incoming,sizeof(incoming),0,
		                  &sa_xmit,&sa_xmit_len);
		if (pack_len<0) {
			perror("recvfrom");
		} else if (pack_len>0 && pack_len<sizeof(incoming)){
			udp_handle(usd,incoming,pack_len,&sa_xmit,sa_xmit_len);
		} else {
			printf("Ooops.  pack_len=%d\n",pack_len);
			fflush(stdout);
		}
		if (probes_sent >= num_probes && num_probes != 0) break;
	}
}

void do_replay(void)
{
	char line[100];
	int n, day, freq, absolute;
	float sec, etime, stime, disp;
	double skew, errorbar;
	int simulated_freq = 0;
	unsigned int last_fake_time = 0;
	double fake_delta_time = 0.0;

	while (fgets(line,sizeof(line),stdin)) {
		n=sscanf(line,"%d %f %f %f %lf %f %d",
			&day, &sec, &etime, &stime, &skew, &disp, &freq);
		if (n==7) {
			fputs(line,stdout);
			absolute=(day-15020)*86400+(int)sec;
			errorbar=etime+disp;
			if (debug) printf("contemplate %u %.1f %.1f %d\n",
				absolute,skew,errorbar,freq);
			if (last_fake_time==0) simulated_freq=freq;
			fake_delta_time += (absolute-last_fake_time)*((double)(freq-simulated_freq))/65536;
			if (debug) printf("fake %f %d \n", fake_delta_time, simulated_freq);
			skew += fake_delta_time;
			freq = simulated_freq;
			last_fake_time=absolute;
			simulated_freq = contemplate_data(absolute, skew, errorbar, freq);
		} else {
			fprintf(stderr,"Replay input error\n");
			exit(2);
		}
	}
}

void usage(char *argv0)
{
	fprintf(stderr,
	"Usage: %s [-c count] [-d] -h hostname [-i interval] [-l]\n"
	"\t[-p port] [-r] [-s] \n",
	argv0);
}

#if 1
int getntp_main(int argc, char *argv[]) {
#else	
int main(int argc, char *argv[]) {
#endif	
	int usd;  /* socket */
	int c;
	/* These parameters are settable from the command line
	   the initializations here provide default behavior */
	short int udp_local_port=0;   /* default of 0 means kernel chooses */
	int cycle_time=600;           /* seconds */
	int probe_count=0;            /* default of 0 means loop forever */
	/* int debug=0; is a global above */
	char *hostname=NULL;          /* must be set */
	int replay=0;                 /* replay mode overrides everything */
	pid_t 	ntppid;
	char 	pidname[]="/var/ntp.pid";
	FILE	*pidscript;

	ntppid = getpid();
	//printf("The pid no is %d \n", ntppid);

	pidscript = fopen(pidname, "w+");
	fprintf(pidscript, "%d",ntppid);
	fclose(pidscript);

	for (;;) {
		c = getopt( argc, argv, "c:" DEBUG_OPTION "h:i:lp:rs");
		if (c == EOF) break;
		switch (c) {
			case 'c':
				probe_count = atoi(optarg);
				break;
#ifdef ENABLE_DEBUG
			case 'd':
				++debug;
				break;
#endif
			case 'h':
				hostname = optarg;
				break;
			case 'i':
				cycle_time = atoi(optarg);
				break;
			case 'l':
				live++;
				break;
			case 'p':
				udp_local_port = atoi(optarg);
				break;
			case 'r':
				replay++;
				break;
			case 's':
				set_clock++;
				probe_count = 1;
				break;
			default:
				usage(argv[0]);
				return 0;
		}
	}
	
	optind = 0;//Set index to 0 for next use
	if (replay) {
		
		do_replay();
		exit(0);
	}
	if (hostname == NULL) {
		fprintf( stderr , "hostname == NULL\n");
		usage(argv[0]);
		return 0;
	}
	if (debug) {
		printf("Configuration:\n"
		"  -c probe_count %d\n"
		"  -d (debug)     %d\n"
		"  -h hostname    %s\n"
		"  -i interval    %d\n"
		"  -l live        %d\n"
		"  -p local_port  %d\n"
		"  -s set_clock   %d\n",
		probe_count, debug, hostname, cycle_time,
		live, udp_local_port, set_clock );
	}

	/* Startup sequence */
	if ((usd=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))==-1)
		{perror ("socket");exit(1);}

	setup_receive(usd, INADDR_ANY, udp_local_port);

	setup_transmit(usd, hostname, NTP_PORT);

	primary_loop(usd, probe_count, cycle_time);

	close(usd);
	return 0;
}

int nationselect(void)
{
	//char *nation;

	//nation = (char *) nvram_safe_get("Country");
#ifdef ANDYDEBUG
	//printf("nation is %s \n", nation);
#endif

	int nation = 0;
	nation = atoi(nvram_safe_get("timezone_index"));
	printf("ntpclient : nation is %d\n", nation);
	
	if(nation == 1)
			return 0;	
	if(nation == 2)
			return 0;	
	if(nation == 3)
			return 0;	
	if(nation == 4)
			return 1;	
	if(nation == 5)
			return 1;	
	if(nation == 6)
			return 0;	
	if(nation == 7)
			return 1;	
	if(nation == 8)
			return 1;	
	if(nation == 9)
			return 1;	
	if(nation == 10)
			return 0;	
	if(nation == 11)
			return 1;	
	if(nation == 12)
			return 0;	
	if(nation == 13)
			return 1;	
	if(nation == 14)
			return 0;	
	if(nation == 15)
			return 3;	
	if(nation == 16)
			return 0;	
	if(nation == 17)
			return 0;	
	if(nation == 18)
			return 0;	
	if(nation == 19)
			return 2;	
	if(nation == 20)
			return 0;	
	if(nation == 21)
			return 2;			
	if(nation == 22)
			return 0;
	if(nation == 23)
			return 2;
	if(nation == 24)
			return 2;
	/*		
	if(nation == 25)
			return 0;
	*/		
	if(nation == 26)
			return 2;
	if(nation == 27)
			return 2;
	if(nation == 28)
			return 2;
	/*if(nation == 29)
			return 2;
	
	if(nation == 30)
			return 0;
	*/
	if(nation == 31)
			return 0;
	if(nation == 32)
			return 1;
	if(nation == 33)
			return 3;
	if(nation == 34)
			return 0;
	if(nation == 35)
			return 0;
	if(nation == 36)
			return 3;
																																								
	return 0;																
	
//	if(nation == 259)
//		return 0;
//	if(nation == 402)
//		return 0;
//	if(nation == 450)
//		return 0;	
//	if(nation == 471)
//		return 0;	
//	if(nation == 467)
//		return 0;	
//	if(nation == 469)
//		return 0;	
//	if(nation == 284)
//		return 0;	
//	if(nation == 276)
//		return 0;	
//	if(nation == 225)
//		return 0;			
//	if(nation == 465)
//		return 1;
//	if(nation == 170)
//		return 1;
//	if(nation == 474)
//		return 1;
//	if(nation == 468)
//		return 1;								
//	if(nation == 167)
//		return 1;
//	if(nation == 470)
//		return 1;
//	if(nation == 314)
//		return 1;
//	if(nation == 272)
//		return 1;
//	if(nation == 156)
//		return 4;		
//	if(nation == 293)
//		return 7;						
//				
//	return 0;
/*
	if (!strcmp(nation,"Kwajalein"))
		return 0;
	if (!strcmp(nation,"MidwayIsland"))
		return 0;
	if (!strcmp(nation,"Hawaii"))
		return 0;
	if (!strcmp(nation,"Arizona"))
		return 0;
	if (!strcmp(nation,"IndianaEast"))
		return 0;
	if (!strcmp(nation,"Bolivia"))
		return 0;
	if (!strcmp(nation,"Guyana"))
		return 0;
	if (!strcmp(nation,"Mid-Atlantic"))
		return 0;
	if (!strcmp(nation,"Azores"))
		return 0;
	if (!strcmp(nation,"Gambia"))
		return 0;
	if (!strcmp(nation,"Tunisia"))
		return 0;
	if (!strcmp(nation,"SouthAfrica"))
		return 0;
	if (!strcmp(nation,"Solomon"))
		return 0;
	if (!strcmp(nation,"Fiji"))
		return 0;
	if (!strcmp(nation,"Japan"))
		return 0;	
*/
//	if (!strcmp(nation,"Guam"))
//		return 1;
//	if (!strcmp(nation,"Alaska"))
//		return 1;
//	if (!strcmp(nation,"PacificTime"))
//		return 1;
//	if (!strcmp(nation,"MountainTime"))
//		return 1;
//	if (!strcmp(nation,"Mexico"))
//		return 1;
//	if (!strcmp(nation,"CentralTime"))
//		return 1;
//	if (!strcmp(nation,"EasternTime"))
//		return 1;
//	if (!strcmp(nation,"AtlanticTime"))
//		return 1;
//	if (!strcmp(nation,"England"))
//		return 2;
//	if (!strcmp(nation,"France"))
//		return 2;
//	if (!strcmp(nation,"Greece"))
//		return 2;
//	if (!strcmp(nation,"Iraq"))
//		return 2;
//	if (!strcmp(nation,"Pakistan"))
//		return 2;
//	if (!strcmp(nation,"Bangladesh"))
//		return 2;
//	if (!strcmp(nation,"Thailand"))
//		return 2;
//	if (!strcmp(nation,"China"))
//		return 2;
///*	if (!strcmp(nation,"Singapore"))
//		return 2;*/
//	if (!strcmp(nation,"BrazilEast"))
//		return 3;
//	if (!strcmp(nation,"NewZealand"))
//		return 3;
//	if (!strcmp(nation,"Australia"))
//		return 3;
//	if (!strcmp(nation,"Santiago"))
//		return 4;
//	if (!strcmp(nation,"Brazilia"))
//		return 5;
//	if (!strcmp(nation,"Aukland"))
//		return 6;
//	if (!strcmp(nation,"Hobart"))
//		return 7;
//	else
//		return 0;

}

int firstSunChk(int mday, int wday)
{
	while(1)
	{
		if(mday == 0)
			return 0;
		if(wday == 0)
		 	return 1;
		mday--;
		wday--;
	}
}
int lastSunChk(int mday, int wday)
{
	while(1)
	{
		if(mday == 24)
			return 0;
		if(wday == 0)
		 	return 1;
		mday--;
		wday--;
	}
}
/****************************************************************/
/* autodaylightsave												*/
/* 																*/
/* Input  : struct timeval *tv_now								*/
/* Output : 1 = Enable Daylightsaving							*/
/*			0 = Disable Daylightsaving							*/
/****************************************************************/
int autodaylightsave(struct timeval *tv_now)
{
	static struct 	tm		*rv_tm;
	time_t			timesec;
	int			retval;

	timesec = tv_now->tv_sec ;
	rv_tm = gmtime(&timesec);
	printf("time is month:%d day:%d hour:%d min:%d\n", rv_tm->tm_mon
	,rv_tm->tm_mday, rv_tm->tm_hour, rv_tm->tm_min);


	retval = nationselect();
	printf("retval is %d\n", retval);
	switch (retval) {
	case 0:	
		return 0;
		//Daylight Saving range is from the first Sunday of April to the last Sunday of October
	case 1:								/*tm_mon range is 0~11 */
		if ( (rv_tm->tm_mon == 3))     /* Present Month is April */
		{
			if ((rv_tm->tm_mday >= 1)&&(rv_tm->tm_mday <= 7))	/*tm_day range is 1~31 */
			{
				if (rv_tm->tm_wday == 0)	/*tm_wday range is 0~6*/
					return 1;          // first sunday of April to start daylight saving
				if (firstSunChk(rv_tm->tm_mday, rv_tm->tm_wday))
					return 1;
				else
					return 0;
			}
			else return 1;
		}
		else if (rv_tm->tm_mon > 3)
		{
			if (rv_tm->tm_mon == 9)		/* when month is October */
			{
				if ((rv_tm->tm_mday >= 25)&&(rv_tm->tm_mday <= 31))
				{
					if (rv_tm->tm_wday == 0)
						return 1;          // last sunday of October to stop daylight saving
				 	if (!lastSunChk(rv_tm->tm_mday, rv_tm->tm_wday))
						return 1;
					else
						return 0;
				}
				else return 1;
			}
			else if (rv_tm->tm_mon >9)
				return 0;
			else
				return 1;
		}
		else
			return 0;
	break; // end of if retval ==1;
		//Daylight Saving range is from the last Sunday of March to the last Sunday of October
	case 2:
		if (rv_tm->tm_mon == 2){	/* when month is March*/
			if ((rv_tm->tm_mday >= 25)&&(rv_tm->tm_mday <= 31)){
				if (rv_tm->tm_wday == 0)
					return 1;          // last sunday of March to start daylight saving
				if (lastSunChk(rv_tm->tm_mday, rv_tm->tm_wday))
					return 1;
				else
					return 0;
			}
			else return 0;
		}
		else if (rv_tm->tm_mon > 2)
		{
			if (rv_tm->tm_mon == 9)
			{
				if ((rv_tm->tm_mday >= 25)&&(rv_tm->tm_mday <= 31))
				{
					if (rv_tm->tm_wday == 0)
						return 1;          // last sunday of October to stop daylight saving
					else if (!lastSunChk(rv_tm->tm_mday, rv_tm->tm_wday))
						return 1;
					else
						return 0;
				}
				else return 1;

			}
			else if (rv_tm->tm_mon >9)
					return 0;
			else
					return 1;
		}
		else
			return 0;
	break;
		//Daylight Saving range is from the last Sunday of October to the last Sunday of March
	case 3:
		if (rv_tm->tm_mon < 2 )
					return 1;
		else if (rv_tm->tm_mon == 2){
			if ((rv_tm->tm_mday >= 25)&&(rv_tm->tm_mday <= 31)){
				if (rv_tm->tm_wday == 0)
					return 0;		// last sunday of March to stop daylight saving
				if (lastSunChk(rv_tm->tm_mday, rv_tm->tm_wday))
					return 0;
				else
					return 1;
			}
			else
				return 1;
		}
		else if (rv_tm->tm_mon == 9){
			if ((rv_tm->tm_mday >= 25)&&(rv_tm->tm_mday <= 31)){
				if (rv_tm->tm_wday == 0)
					return 1;		// last sunday of October to start daylight saving
				if (lastSunChk(rv_tm->tm_mday, rv_tm->tm_wday))
					return 1;
				else
					return 0;
			}
			else
				return 0;
		}
		else if (rv_tm->tm_mon > 9)
				return 1;
		else
				return 0;
	break;		
		//Daylight Saving range is from the Second Sunday of October to the Second Sunday of March
	case 4:
		if (rv_tm->tm_mon < 2 )
					return 1;
		else if (rv_tm->tm_mon == 2){
			if ((rv_tm->tm_mday >= 8)&&(rv_tm->tm_mday <= 14)){
				if (rv_tm->tm_wday == 0)
					return 0;		
			}
			else
				return 1;
		}
		else if (rv_tm->tm_mon == 9){
			if ((rv_tm->tm_mday >= 8)&&(rv_tm->tm_mday <= 14)){
				if (rv_tm->tm_wday == 0)
					return 1;		
			}
			else
				return 0;
		}
		else if (rv_tm->tm_mon > 9)
				return 1;
		else
				return 0;
	break;
		//Daylight Saving range is from the third Sunday of October to the third Sunday of February
	case 5:
		if (rv_tm->tm_mon < 1 )
					return 1;
		else if (rv_tm->tm_mon == 1){
			if ((rv_tm->tm_mday >= 15)&&(rv_tm->tm_mday <= 21)){
				if (rv_tm->tm_wday == 0)
					return 0;		
			}
			else
				return 1;
		}
		else if (rv_tm->tm_mon == 9){
			if ((rv_tm->tm_mday >= 15)&&(rv_tm->tm_mday <= 21)){
				if (rv_tm->tm_wday == 0)
					return 1;		
			}
			else
				return 0;
		}
		else if (rv_tm->tm_mon > 9)
				return 1;
		else
				return 0;
	break;
		//Daylight Saving range is from the first Sunday of October to the third Sunday of March
	case 6:
		if (rv_tm->tm_mon < 2 )
					return 1;
		else if (rv_tm->tm_mon == 2){
			if ((rv_tm->tm_mday >= 15)&&(rv_tm->tm_mday <= 21)){
				if (rv_tm->tm_wday == 0)
					return 0;		
			}
			else
				return 1;
		}
		else if (rv_tm->tm_mon == 9){
			if ((rv_tm->tm_mday >= 1)&&(rv_tm->tm_mday <= 7)){
				if (rv_tm->tm_wday == 0)
					return 1;		
			}
			else
				return 0;
		}
		else if (rv_tm->tm_mon > 9)
				return 1;
		else
				return 0;
	break;
		//Daylight Saving range is from the first Sunday of October to the last Sunday of March
	case 7:
		if (rv_tm->tm_mon < 2 )
					return 1;
		else if (rv_tm->tm_mon == 2){
			if ((rv_tm->tm_mday >= 25)&&(rv_tm->tm_mday <= 31)){
				if (rv_tm->tm_wday == 0)
					return 0;		
			}
			else
				return 1;
		}
		else if (rv_tm->tm_mon == 9){
			if ((rv_tm->tm_mday >= 1)&&(rv_tm->tm_mday <= 7)){
				if (rv_tm->tm_wday == 0)
					return 1;		
			}
			else
				return 0;
		}
		else if (rv_tm->tm_mon > 9)
				return 1;
		else
				return 0;
	break;
	default:
	{
		printf("Default nation\n");
		return 0;
	}
	}	// end of switch
}

