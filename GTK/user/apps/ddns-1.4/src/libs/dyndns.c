/* -- updatedd: dyndns.c --
 *
 * Copyright (c) 2002 Philipp Benner <philipp@philippb.tk>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * The GNU C Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the GNU C Library; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA.
 *
 */

#if defined(__linux__)
 #define LONGOPT
 #define _GNU_SOURCE
#elif defined(__OpenBSD__) || defined(__NetBSD__) || defined(__FreeBSD__)
 #define NOLONGOPT
 #define NODPRINTF
#else
 #error Updatedd has not been tested on this operating system.
 #define NOLONGOPT
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#ifdef LONGOPT
 #include <getopt.h>
#endif
#include <sys/socket.h>		// |
#include <netinet/in.h>		// { inet_ntoa()
#include <arpa/inet.h>		// |
#include "../modules/base64encode.h"
#include "../modules/get_connection.h"
#include "../modules/error.h"
#include "../version.h"
#ifdef NODPRINTF
 #include "../modules/dprintf.c"
#endif

#include	<bcmnvram.h>
#include  <vendor_default.h>

#include <sys/sysinfo.h>

#define DYNDNSHOST	"members.dyndns.org"
#define CHECKIPHOST	"checkip.dyndns.org"

#define DYNDNSHOST_IP "66.151.188.47"
#define CHECKIPHOST_IP "209.69.32.139"



#define PORT		80

#define CACHEFILE	"/tmp/updatedd-dyndns.ip"
#define MAXDATA 	512

#ifdef LONGOPT
#define USAGE	"\n                                Updatedd - \033[0;31;1mdyndns\033[0m\n\n"				\
		"Usage: %s [--with-backmx] [--with-wildcard <on|off>] [--with-syslog] [--offline] [--mx <mxhost>]\n"	\
		"	[--service <service>] <--hostnames <h1,h2,h3...>> <--system <system>> <--user <user:pass>>\n\n"	\
		"	-b	--with-backmx		enable backmx\n"						\
		"	-h	--help			print this help\n"						\
		"	-L	--list-services		list supported services\n"					\
		"	-m	--mx <mxhost>		hostname of your mail exchange\n"				\
		"	-n	--hostnames <h1,h2...>	your hostnames, seperate each with a comma\n"			\
		"	-o	--offline		set host to offline mode\n"					\
		"	-S	--service <service>	select the type of service you are using\n"			\
		"	-s	--system <system>	specify the system (dyndns, statdns, custom)\n"			\
		"	-u	--user <user:pass>	user and password to login with\n"				\
		"	-v	--version		print the version\n"						\
		"	-w	--wildcard <on|off>	enable wildcard\n"						\
		"	-y	--with-syslog		print error messages to syslog\n\n"
#endif
#ifdef NOLONGOPT
#define USAGE	"\n                       Updatedd - \033[0;31;1mdyndns\033[0m\n\n"			\
		"Usage: %s [-boy] [-w <on|off> [-S <service>] [-m <mxhost>] <-n <h1,h2...>>\n"		\
		"	   <-s <system>> <-u <user:pass>>\n\n"						\
		"	-b		enable backmx\n"						\
		"	-h		print this help\n"						\
		"	-L		list supported services\n"					\
		"	-m <mxhost>	hostname of your mail exchange\n"				\
		"	-n <h1,h2...>	your hostnames, seperate each with a comma\n"			\
		"	-o		set host to offline mode\n"					\
		"	-S <service>	select the type of service you are using\n"			\
		"	-s <system>	specify the system (dyndns, statdns, custom)\n"			\
		"	-u <user:pass>	user and password to login with\n"				\
		"	-w <on|off>	enable wildcard\n"						\
		"	-v		print the version of updatedd\n"				\
		"	-y		print error messages to syslog\n\n"
#endif

struct arguments {
	char *backmx;
	char *hostnames;
	char *mx;
	char *offline;
	char *system;
	char *user;
	char *wildcard;
};

int i_syslog = 0;

static struct yesno {
	char *yes;
	char *no;
	char nothing;
} yn = { "yes", "no", '\0' };

static struct systems {
	char *dyndns;
	char *statdns;
	char *custom;
} s_system = { "dyndns", "statdns", "custom" };

struct ip_addresses {
	struct in_addr real_ip;
	struct in_addr cached;
};

static struct dyndns_return_codes {
	char *code;
	char *message;
	int  error;
} return_codes[] = {
	{ "badauth",	"Authorization fails (username or passwords)",		1 },
	{ "badsys",	"The system parameters are invalid",		1 },
	{ "badagent",	"This useragent has been blocked", 				1 },
	{ "good",	"DDNS is updated successfully",		0 },
	{ "nochg",	"Update Success - No Change",		0 },
	{ "notfqdn",	"A Fully-Qualified Domain Name was not provided.",	1 },
	{ "nohost",	"Invalid Domain Name",		1 },
	{ "!donator",	"A feature requested is only available to donators, please donate", 					1 },
	{ "!yours",	"Duplicate Domain Name",			1 },
	{ "!active",	"The hostname specified is in a Custom DNS "
			"domain which has not yet been activated.",		1 },
	{ "abuse",	"The hostname is blocked by the DDNS server",		1 },
	{ "notfqdn",	"Domain Name is not correct",					1 },
	{ "numhost",	"Too many or too few hosts found",			1 },
	{ "dnserr",	"DNS error encountered",				1 },
	{ "911",	"An unexpected error (1)",				1 },
	{ NULL,		NULL,							0 }
};

int get_flags(struct arguments *args, int argc, char *argv[]);
int check_dyndns(int s, struct ip_addresses *ip);
void update_fakeIp(int s, struct arguments *args);
void update_realIp(int s, struct arguments *args);
void update_cache(struct ip_addresses *ip);
void get_ip(int s, struct ip_addresses * ip);
int get_local_cached_ip(struct ip_addresses *ip);
int check_server_msg(int s);

void
print_error(int priority, int instruction, int error_type, const char *message, ...) {

	va_list az;

	va_start(az, message);
	if(i_syslog == 1)
	    log_error(priority, instruction, error_type, message, az);
	else
	    error_f(instruction, error_type, message, az);
	va_end(az);

	return;

}

void
dyndns(int argc, char *argv[]) {

#define DDNS_UPDATED_TIMER	28*24*60*60
#define	DAY_TIME						24*60*60

	struct arguments args = { NULL, NULL, NULL, NULL, NULL, NULL, NULL };
	struct ip_addresses ip;
	struct error_s err_s;
	int s = 0;

	struct sysinfo info;
	long	last_checktime=0;
	long	counter=0;

		init_error(&err_s, LOG_USER, argv[0], EMAIL);

		if(get_flags(&args, argc, argv)) {
			(void)fprintf(stderr, USAGE,  argv[0]);
		nvram_set("DDNSok", "0");//ddns status
			exit(EXIT_FAILURE);
		}
	
		nvram_set("DYNDNSDDNSok", "Connecting to DynDNS server...");
		while(1)
		{
		counter ++;		
		sysinfo(&info);
		
		if (strlen(nvram_safe_get("DDNS_updated_ipaddr")) == 0 || 
			  strlen(nvram_safe_get("DDNS_updated_time")) == 0   ||
			  strcmp(nvram_safe_get("DDNS_updated_ipaddr"), nvram_safe_get("wan_ipaddr")) != 0 ||
			  (info.uptime - atoi(nvram_safe_get("DDNS_updated_time"))) >= DDNS_UPDATED_TIMER)
		{
			while( 1 )
			{	
				if((s = get_connection(DYNDNSHOST, PORT)) < 0)
				{
						sleep(30);
						continue;
				}
				update_realIp(s, &args);
				check_server_msg(s);
				(void)close(s);
				break;
			}	
		}
		else if (strcmp(nvram_safe_get("DDNS_updated_status"), "nochg") == 0 && 
			      (info.uptime - atoi(nvram_safe_get("DDNS_updated_time"))) >=  DAY_TIME)
		{
				while( 1 )
				{	
					if((s = get_connection(DYNDNSHOST, PORT)) < 0)
					{
							sleep(30);
							continue;
					}
					update_fakeIp(s, &args);
					check_server_msg(s);
					(void)close(s);
					break;
				}	
			
				sleep(10);
			
				while( 1 )
				{	
					if((s = get_connection(DYNDNSHOST, PORT)) < 0)
					{
							sleep(30);
							continue;
					}
					update_realIp(s, &args);
					check_server_msg(s);
					(void)close(s);
					break;
				}	
		}  
		else
			nvram_set("DYNDNSDDNSok", "No changes, update considered abusive.");
		sleep(DAY_TIME-info.uptime);
	}
	

#if 0
{
		while(1)
		{
				while( 1 )
				{	
					if((s = get_connection(DYNDNSHOST, PORT)) < 0)
	{
							sleep(30);
							continue;
					}
					update_fakeIp(s, &args);
					check_server_msg(s);
					(void)close(s);
					break;
				}	
				sleep(10);
			while( 1 )
			{	
				if((s = get_connection(DYNDNSHOST, PORT)) < 0)
				{
							sleep(30);
							continue;
				}
				update_realIp(s, &args);
				check_server_msg(s);
		(void)close(s);
				break;
			}	
				sleep(36000);
		}
}		
#endif		  

	return;
}

int
get_flags(struct arguments *args, int argc, char *argv[]) {

	int c;

	while(1) {
#ifdef LONGOPT
		int option_index = 0;
		static struct option long_options[] = {
			{ "with-backmx",	0, 0, 'b' },
			{ "with-syslog",	0, 0, 'y' },
			{ "with-wildcard",	1, 0, 'w' },
			{ "offline",		0, 0, 'o' },
			{ "help",		0, 0, 'h' },
			{ "hostnames",		1, 0, 'n' },
			{ "service",		1, 0, 'S' },
			{ "system",		1, 0, 's' },
			{ "mx",			1, 0, 'm' },
			{ "version",		0, 0, 'v' },
			{ "user",		1, 0, 'u' },
			{ NULL,			0, 0, 0   }
		};

		c = getopt_long(argc, argv, "byohw:n:S:s:m:vu:",
				long_options, &option_index);
#endif

#ifdef NOLONGOPT
		c = getopt(argc, argv, "byohw:n:S:s:m:vu:");
#endif
		if(c == -1)
		    break;

		switch(c) {
			case 'b':
			    args->backmx = yn.yes;
			    break;
			case 'y':
			    i_syslog = 1;
			    break;
			case 'h':
			    (void)printf(USAGE, argv[0]);
			    exit(EXIT_SUCCESS);
			case 'm':
			    args->mx = optarg;
			    break;
			case 'n':
			    args->hostnames = optarg;
			    break;
			case 'o':
			    args->offline = yn.yes;
			    break;
			case 'S':
			    break;
			case 's':
			    if(strstr(optarg, "custom"))
				args->system = s_system.custom;
			    else if(strstr(optarg, "statdns"))
				args->system = s_system.statdns;
			    else
				args->system = s_system.dyndns;
			    break;
			case 'w':
			    if(strstr(optarg, "ON") || strstr(optarg, "on"))
				args->wildcard = "ON";
			    else if(strstr(optarg, "OFF") || strstr(optarg, "off"))
				args->wildcard = "OFF";
			    else
				return 1;
			    break;
			case 'v':
			    (void)printf("\n%s %s - ?by %s\n\t\t %s\n\n",
					 PNAME, VERSION, AUTHOR, HOMEPAGE);
			    exit(EXIT_SUCCESS);
			case 'u':
			    if((args->user = (char *)malloc(strlen(optarg) + 1)) == NULL)
				print_error(LOG_ERR, EXIT, PERROR, "malloc() failed");
			    strcpy(args->user, optarg);
			    args->user[strlen(optarg)] = '\0';
			    bzero(optarg, strlen(optarg));	// prevent that anybody can see your pass with ps
		}
	}

	if(args->backmx == NULL)
	    args->backmx = yn.no;
	if(args->wildcard == NULL)
	    args->wildcard = "NOCHG";
	if(args->mx == NULL)
	    args->mx = &yn.nothing;
	if(args->hostnames == NULL || args->system == NULL || args->user == NULL)
	    return 1;

	return 0;

}

int
check_dyndns(int s, struct ip_addresses *ip) {

	get_ip(s, ip);
	if(get_local_cached_ip(ip))
	    return 1;
	if(ip->real_ip.s_addr != ip->cached.s_addr)
	    return 1;
	return 0;

}

void
get_ip(int s, struct ip_addresses *ip) {

	char server_msg[MAXDATA];
	char *ptr;
	//"User-Agent: %s %s - %s\r\n" //PNAME, VERSION, HOMEPAGE
	(void)dprintf(s,	"GET / HTTP/1.1\r\n"
				"Host: %s\r\n"
				"User-Agent: %s-%s-%s\r\n"
				"Connection: close\r\n"
				"Pragma: no-cache\r\n\n",
				CHECKIPHOST, COMPANY, MODEL_NAME, FW_VERSION);

	(void)memset(server_msg, 0, sizeof(server_msg));
	if(read(s, server_msg, sizeof(server_msg) - 1) <= 0)
	    print_error(LOG_ERR, EXIT, PERROR, "read() failed");
	if(strstr(server_msg, "HTTP/1.1 200 OK") ||
	   strstr(server_msg, "HTTP/1.0 200 OK")) {
		if( !((ptr = strstr(server_msg, "Address: ")) &&
		      (ptr = strtok(ptr + sizeof("Address: ") - 1, "\t\n ")) &&
		      (0 != inet_aton(ptr, &ip->real_ip))))
		    print_error(LOG_ERR, EXIT, STANDARD, "received invalid ip address");
	}

	else
	    print_error(LOG_ERR, EXIT, STANDARD, "Internal Server Error");

	return;

}

int
get_local_cached_ip(struct ip_addresses *ip) {

	FILE *fp;
	char ip_buffer[18] = { '\0' };

	if((fp = fopen(CACHEFILE, "r")) == NULL)
	    return 1;
	if(!(fgets(ip_buffer, sizeof(ip_buffer) - 1, fp) &&
	     inet_aton(ip_buffer, &ip->cached) != 0)) {
		(void)fclose(fp);
		print_error(LOG_ERR, EXIT, STANDARD, "%s: invalid ip address", CACHEFILE);
	}
	(void)fclose(fp);

	return 0;

}

void
update_fakeIp(int s, struct arguments *args) {

	char *b64user;

	if(strlen(args->user) > 128)
	    print_error(LOG_ERR, EXIT, STANDARD, "username is too long");
	if((b64user = (char *)malloc((2 * strlen(args->user) + 1) * sizeof(char))) == NULL)
	    print_error(LOG_ERR, EXIT, PERROR, "malloc() failed");
	(void)memset(b64user, 0, 2 * strlen(args->user) + 1);

	base64encode(args->user, b64user);

/*	
	(void)dprintf(s,	"GET /nic/update?system=%s&hostname=%s&wildcard=%s"
				"&backmx=%s&offline=%s&mx=%s HTTP/1.1\r\n"
				"Host: %s\r\n"
				"Authorization: basic %s\r\n"
				"User-Agent: %s %s - %s\r\n"
				"Connection: close\r\n"
				"Pragma: no-cache\r\n\n",
				args->system, args->hostnames, args->wildcard,
				args->backmx, args->offline, args->mx,
				DYNDNSHOST, b64user, PNAME, VERSION, HOMEPAGE);
*/

	dprintf ( s , "GET /nic/update?"
				"system=%s&"
				"hostname=%s&"
				"myip=%s&"
				"wildcard=%s&"
				"backmx=%s&"
				"offline=%s&"
				"mx=%s HTTP/1.0\r\n"
				"Host: %s\r\n"
				"Authorization: basic %s\r\n"
				"User-Agent: %s-%s-%s\r\n"
				"Connection: close\r\n"
				"Pragma: no-cache\r\n\r\n\n",
				args->system , args->hostnames , nvram_safe_get ( "lan_ipaddr" ) , args->wildcard ,
				args->backmx , ( args->offline == NULL ) ? "NO" : args->offline , args->mx ,
				DYNDNSHOST , b64user , COMPANY , MODEL_NAME, FW_VERSION );

				
	free(b64user);
	return;

}

void
update_realIp(int s, struct arguments *args) {

	char *b64user;

	if(strlen(args->user) > 128)
	    print_error(LOG_ERR, EXIT, STANDARD, "username is too long");
	if((b64user = (char *)malloc((2 * strlen(args->user) + 1) * sizeof(char))) == NULL)
	    print_error(LOG_ERR, EXIT, PERROR, "malloc() failed");
	(void)memset(b64user, 0, 2 * strlen(args->user) + 1);

	base64encode(args->user, b64user);

/*	
	(void)dprintf(s,	"GET /nic/update?system=%s&hostname=%s&wildcard=%s"
				"&backmx=%s&offline=%s&mx=%s HTTP/1.1\r\n"
				"Host: %s\r\n"
				"Authorization: basic %s\r\n"
				"User-Agent: %s %s - %s\r\n"
				"Connection: close\r\n"
				"Pragma: no-cache\r\n\n",
				args->system, args->hostnames, args->wildcard,
				args->backmx, args->offline, args->mx,
				DYNDNSHOST, b64user, PNAME, VERSION, HOMEPAGE);
*/

	dprintf ( s , "GET /nic/update?"
				"system=%s&"
				"hostname=%s&"
				"myip=%s&"
				"wildcard=%s&"
				"backmx=%s&"
				"offline=%s&"
				"mx=%s HTTP/1.0\r\n"
				"Host: %s\r\n"
				"Authorization: basic %s\r\n"
				"User-Agent: %s-%s-%s\r\n"
				"Connection: close\r\n"
				"Pragma: no-cache\r\n\r\n\n",
				args->system , args->hostnames , nvram_safe_get ( "wan_ipaddr" ) , args->wildcard ,
				args->backmx , ( args->offline == NULL ) ? "NO" : args->offline , args->mx ,
				DYNDNSHOST , b64user , COMPANY , MODEL_NAME, FW_VERSION );

				
	free(b64user);
	return;

}

void
update_cache(struct ip_addresses *ip) {

	FILE *fp;

	(void)unlink(CACHEFILE);
	if((fp = fopen(CACHEFILE, "a")) == NULL)
	    print_error(LOG_ERR, EXIT, PERROR, "fopen() failed");
	(void)fprintf(fp, "%s", inet_ntoa(ip->real_ip));
	fclose(fp);

	return;

}

int
check_server_msg(int s) {

	int n;
	char server_msg[MAXDATA];

	struct sysinfo info;
	char updated_time[128];
	
	(void)memset(updated_time, 0, sizeof(updated_time));
	(void)memset(server_msg, 0, sizeof(server_msg));
	nvram_set("DDNSok", "0");//ddns status
	if(read(s, server_msg, sizeof(server_msg) - 1) < 0) {
	    print_error(LOG_ERR, EXIT, PERROR, "read() failed");
				nvram_set("DYNDNSDDNSok", "Open socket error!");
	}
	else if(strstr(server_msg, "HTTP/1.1 200 OK") ||
	        strstr(server_msg, "HTTP/1.0 200 OK")) 
	{
		for(n = 0; return_codes[n].code != NULL; n++) 
		{
			if(strstr(server_msg, return_codes[n].code)) 
			{
				nvram_set("DDNS_updated_status", return_codes[n].code);
				nvram_set("DYNDNSDDNSok", return_codes[n].message);
				if(return_codes[n].error == 1)
				{
				    //nvram_set("DDNSok","0");	
				    print_error(LOG_ERR, EXIT, STANDARD, return_codes[n].message);
				} 
				else 
				{
					nvram_set("DDNSok","1");
					if ( strcmp(return_codes[n].code, "good") == 0 ||
						  (strcmp(return_codes[n].code, "nochg") == 0 && atoi(nvram_safe_get("DDNS_updated_forced"))==1))
					{
						sysinfo(&info);
						sprintf(updated_time, "%ld", info.uptime);
						nvram_set("DDNS_updated_time", updated_time);
						nvram_set("DDNS_updated_ipaddr", nvram_safe_get("wan_ipaddr"));
						nvram_set("DDNS_updated_forced", "0");
						nvram_set("DDNS_updated_status", "good");
						nvram_commit();
						gpio_monitor("1", "/proc/GPIO01"); //Pull high to disable the blink
						gpio_monitor("0", "/proc/GPIO02"); //Pull low to enable the watchdog
						if(atoi(nvram_safe_get("wl0_wlmode"))!=0 || atoi(nvram_safe_get("wl1_wlmode"))!=0)
						{
							gpio_monitor("0", "/proc/GPIO06"); //Pull low to enable the wireless led
							if(!strcmp(nvram_safe_get( "wsc_config_state" ) , "1"))
							{
								system("cat /proc/wsc_led_ctrl/WPS_CONFIGED");
								//system("echo 0 > /proc/GPIO04");
							}
						}
					}		
					print_error(LOG_NOTICE, CONTINUE, STANDARD, return_codes[n].message);
					return 0;
				}
			}
		}
	} 
	else if(strstr(server_msg, "401 Authorization Required")) {
	    print_error(LOG_ERR, EXIT, STANDARD, "wrong username or password");
				nvram_set("DYNDNSDDNSok", "wrong username or password");
	}
	else {
	    print_error(LOG_ERR, EXIT, STANDARD, "Internal Server Error");
				nvram_set("DYNDNSDDNSok", "Internal Server Error");
	}

	return 1;
}
