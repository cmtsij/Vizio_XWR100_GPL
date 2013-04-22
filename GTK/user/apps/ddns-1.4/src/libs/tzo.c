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

#define DYNDNSHOST	"members.dyndns.org"
#define CHECKIPHOST	"ClusterLookup1.tzo.com"
//#define CHECKIPHOST     "140.239.225.164"
#define PORT		21340
#define PORT2		21347

#define CACHEFILE	"/tmp/updatedd-dyndns.ip"
#define MAXDATA 	512

#ifdef LONGOPT
#define USAGE	"\n                                TZO - \033[0;31;1mTZO\033[0m\n\n"				\
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
		"	-y	--with-syslog		print error messages to syslog\n"				\
		"       -k      --key			register key value\n"						\
		"	-e	--email			email address\n\n"	
#endif
#ifdef NOLONGOPT
#define USAGE	"\n                       TZO - \033[0;31;1mTZO\033[0m\n\n"			\
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
	char *key;
	char *email;
};

int tzo_i_syslog = 0;

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

static struct tzo_return_codes {
	char *code;
	char *message;
	int  error;
} return_codes[] = {
	{ "40",	"DDNS is updated successfully",		0 },
	{ "41",	"Invalid Domain Name",		1 },
	{ "42",	"Invalid E-mail", 				1 },
	{ "43",	"Invalid Action",		0 },
	{ "44",	"Invalid Key",		0 },
	{ "45",	"Invalid IP Address",	1 },
	{ "46",	"Duplicate Domain Name.",		1 },
	{ "47",	"Domain Name has already been created for this domain name", 					1 },
	{ "48",	"The account has expire",			1 },
	{ "49",	"An unexpected server error",		1 },
	{ NULL,		NULL,							0 }
};

int tzo_get_flags(struct arguments *args, int argc, char *argv[]);
//int Get_Update_Server_IP(int s, char *serverip);
void update_tzo(int s, struct arguments *args);
void tzo_update_cache(struct ip_addresses *ip);
void tzo_get_ip(int s, char *serverip, struct arguments *args);
int tzo_get_local_cached_ip(struct ip_addresses *ip);
int tzo_check_server_msg(int s);

int Register_DDNS(int s, struct arguments *args);
extern int tzo_getip(unsigned int IPaddr);

void
tzo_print_error(int priority, int instruction, int error_type, const char *message, ...) {

	va_list az;

	va_start(az, message);
	if(tzo_i_syslog == 1)
		
	    log_error(priority, instruction, error_type, message, az);
	else
	    error_f(instruction, error_type, message, az);
	va_end(az);

	return;

}

int
tzo(int argc, char *argv[]) {

	struct arguments args = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
	struct ip_addresses server_ip;
	char serverip[20];
	struct error_s err_s;
	int s = 0;
	unsigned long ip_record1 = 0, ip_record2 = 0;

	nvram_set("TZODDNSok", "Connecting to TZO server...");
	while(1)
	{

		(void)memset(&serverip, 0, sizeof(serverip));
		init_error(&err_s, LOG_USER, argv[0], EMAIL);

		if(tzo_get_flags(&args, argc, argv)) {
			(void)fprintf(stderr, USAGE,  argv[0]);
			nvram_set("DDNSok", "0");//ddns status
			exit(EXIT_FAILURE);
		}
			

		ip_record2 = inet_addr(nvram_get("wan_ipaddr"));
				
		//if(ip_record1 == ip_record2)
		if (tzo_getip(ip_record2)) // Gemtek_SW_arthur_2006_8_15
		{
			nvram_set("DDNSok", "1");//ddns status
			printf("\n\rThe same IP");
			fflush(stdout);		
			goto last_l;
		}

//	Phase I : get TZO DDNS server IP
	

		if((s = get_connection(CHECKIPHOST, PORT)) < 0)
		{
			//tzo_print_error(LOG_ERR, EXIT, STANDARD, "get_connection() failed");
			printf("\n\rBad connection");
			fflush(stdout);
			goto last_s;
		}

		printf("Message: %s", serverip);
		tzo_get_ip(s, serverip, &args);
		(void)close(s);
		


	
//	Phase II : register DDNS	
		if((s = get_connection(serverip, PORT2)) < 0)
			//tzo_print_error(LOG_ERR, EXIT, STANDARD, "get_connection() failed");
			goto last_s;

// ARTHUR_LIN 2006.7.26 marked  , test new tzo_dns command
    printf(" TZO_Register_DDNS in\n");
		Register_DDNS(s, &args);

		ip_record1 = ip_record2;
		printf("\n\rI record IP!");
		fflush(stdout);
	

		(void)close(s);
last_l:
		printf("\n TZO long sleep \n");
		sleep(3540);
				
last_s:
    printf("\n TZO short sleep \n");
		sleep(10);
		sleep(10);
		sleep(10);
		sleep(10);
		sleep(10);
		sleep(10);
		//printf("Updatedd wake up!\n\r");
	}

	return 0;

}

int
tzo_get_flags(struct arguments *args, int argc, char *argv[]) {

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
			{ "key",		1, 0, 'k' },
			{ "email",		1, 0, 'e' },
			{ NULL,			0, 0, 0   }
		};

		c = getopt_long(argc, argv, "byohw:n:S:s:m:vu:k:e:",
				long_options, &option_index);
#endif

#ifdef NOLONGOPT
		c = getopt(argc, argv, "byohw:n:S:s:m:vu:k:e:");
#endif
		if(c == -1)
		    break;

		switch(c) {
			case 'b':
			    args->backmx = yn.yes;
			    break;
			case 'y':
			    tzo_i_syslog = 1;
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
				tzo_print_error(LOG_ERR, EXIT, PERROR, "malloc() failed");
			    strcpy(args->user, optarg);
			    args->user[strlen(optarg)] = '\0';
			    bzero(optarg, strlen(optarg));	// prevent that anybody can see your pass with ps
			    break;
			case 'e':
			    args->email = optarg;
			    break;
			case 'k':
			    args->key   = optarg;
			    break;
			    
		}
	}

	if(args->backmx == NULL)
	    args->backmx = yn.no;
	if(args->wildcard == NULL)
	    args->wildcard = "NOCHG";
	if(args->mx == NULL)
	    args->mx = &yn.nothing;
	if(args->hostnames == NULL || args->email == NULL || args->key == NULL)
	    return 1;

	return 0;

}

//int
//Get_Update_Server_IP(int s, char *serverip) {
//
//	get_ip(s, serverip);
//	if(get_local_cached_ip(host_ip))
//	    return 1;
//	if(ip->real_ip.s_addr != ip->cached.s_addr)
//	    return 1;
//	return 0;
//
//}

void
tzo_get_ip(int s, char *serverip, struct arguments *args) {

	char server_msg[MAXDATA];
	char *ptr1, *ptr2;


//	(void)dprintf(s,	"GET / HTTP/1.1\r\n"
//				"Host: %s\r\n"
//				"User-Agent: %s %s - %s\r\n"
//				"Connection: close\r\n"
//				"Pragma: no-cache\r\n\n",
//				CHECKIPHOST, PNAME, VERSION, HOMEPAGE);

	(void)memset(server_msg, 0, sizeof(server_msg));
	if(read(s, server_msg, sizeof(server_msg) - 1) <= 0)
	  tzo_print_error(LOG_ERR, EXIT, PERROR, "read() failed");


	//printf("\n\rServer Message: %s", server_msg);
	//fflush(stdout);


	(void)memset(server_msg, 0, sizeof(server_msg));
	strcat(server_msg, "I ");
	strcat(server_msg, args->key);
	strcat(server_msg, "\r\n");
        (void)dprintf(s, server_msg);


	(void)memset(server_msg, 0, sizeof(server_msg));
        if(read(s, server_msg, sizeof(server_msg) -1) <= 0)
	  tzo_print_error(LOG_ERR, EXIT, PERROR, "read() failed");

	//printf("\n\rServer Message: %s", server_msg);

	ptr1 = server_msg;
	ptr2 = strstr(ptr1, ",");
	ptr2++;
	ptr1 = strstr(ptr2, ",");
	strncpy(serverip, ptr2, ptr1-ptr2);

	//printf("\n\rServer IP: %s", serverip);
	//fflush(stdout);
	
	
//	if(strstr(server_msg, "HTTP/1.1 200 OK") ||
//	   strstr(server_msg, "HTTP/1.0 200 OK")) {
//		if( !((ptr = strstr(server_msg, "Address: ")) &&
//		      (ptr = strtok(ptr + sizeof("Address: ") - 1, "\t\n ")) &&
//		      (0 != inet_aton(ptr, &ip->real_ip))))
//		    tzo_print_error(LOG_ERR, EXIT, STANDARD, "received invalid ip address");
//	}
//
//	else
//	    tzo_print_error(LOG_ERR, EXIT, STANDARD, "Internal Server Error");

	return;

}

int
tzo_get_local_cached_ip(struct ip_addresses *ip) {

	FILE *fp;
	char ip_buffer[18] = { '\0' };

	if((fp = fopen(CACHEFILE, "r")) == NULL)
	    return 1;
	if(!(fgets(ip_buffer, sizeof(ip_buffer) - 1, fp) &&
	     inet_aton(ip_buffer, &ip->cached) != 0)) {
		(void)fclose(fp);
		tzo_print_error(LOG_ERR, EXIT, STANDARD, "%s: invalid ip address", CACHEFILE);
	}
	(void)fclose(fp);

	return 0;

}

void
update_tzo(int s, struct arguments *args) {
	

	char *b64user;

	if(strlen(args->user) > 128)
	    tzo_print_error(LOG_ERR, EXIT, STANDARD, "username is too long");
	
	if((b64user = (char *)malloc((2 * strlen(args->user) + 1) * sizeof(char))) == NULL)
	    tzo_print_error(LOG_ERR, EXIT, PERROR, "malloc() failed");
	(void)memset(b64user, 0, 2 * strlen(args->user) + 1);

	base64encode(args->user, b64user);
	
	(void)dprintf(s,	"GET /nic/update?system=%s&hostname=%s&wildcard=%s"
				"&backmx=%s&offline=%s&mx=%s HTTP/1.1\r\n"
				"Host: %s\r\n"
				"Authorization: basic %s\r\n"
				"User-Agent: %s-%s-%s\r\n"
				"Connection: close\r\n"
				"Pragma: no-cache\r\n\n",
				args->system, args->hostnames, args->wildcard,
				args->backmx, args->offline, args->mx,
				DYNDNSHOST, b64user, COMPANY, MODEL_NAME, FW_VERSION);
				
	free(b64user);
	return;

}

void
tzo_update_cache(struct ip_addresses *ip) {

	FILE *fp;

	(void)unlink(CACHEFILE);
	if((fp = fopen(CACHEFILE, "a")) == NULL)
	    tzo_print_error(LOG_ERR, EXIT, PERROR, "fopen() failed");
	(void)fprintf(fp, "%s", inet_ntoa(ip->real_ip));
	fclose(fp);

	return;

}

int
tzo_check_server_msg(int s) {

	int n;
	char server_msg[MAXDATA];

	(void)memset(server_msg, 0, sizeof(server_msg));
	if(read(s, server_msg, sizeof(server_msg) - 1) < 0)
	  tzo_print_error(LOG_ERR, EXIT, PERROR, "read() failed");
	else if(strstr(server_msg, "HTTP/1.1 200 OK") ||
	        strstr(server_msg, "HTTP/1.0 200 OK")) {
		for(n = 0; return_codes[n].code != NULL; n++) {
			if(strstr(server_msg, return_codes[n].code)) {
				if(return_codes[n].error == 1)
				    tzo_print_error(LOG_ERR, EXIT, STANDARD, return_codes[n].message);
				else {
					tzo_print_error(LOG_NOTICE, CONTINUE, STANDARD, return_codes[n].message);
					
					return 0;
				}
			}
		}
	} else if(strstr(server_msg, "401 Authorization Required"))
	    tzo_print_error(LOG_ERR, EXIT, STANDARD, "wrong username or password");
	
	else
	    tzo_print_error(LOG_ERR, EXIT, STANDARD, "Internal Server Error");

	return 1;
}

int
Register_DDNS(int s, struct arguments *args)
{
	int n;
	char server_msg[MAXDATA];
	char *ptr1, *ptr2;
	char host_ip[20];
	char comm[3];
	char head[3];
	char tail[3];

	sprintf(head, "R ");
	sprintf(comm, ",");
	sprintf(tail, "\r\n");
	//sprintf(host_ip, "");
	(void)memset(host_ip, 0, sizeof(host_ip));
	
	nvram_set("DDNSok", "0");//ddns status

	
//	(void)dprintf(s,	"GET / HTTP/1.1\r\n"
//				"Host: %s\r\n"
//				"User-Agent: %s %s - %s\r\n"
//				"Connection: close\r\n"
//				"Pragma: no-cache\r\n\n",
//				CHECKIPHOST, PNAME, VERSION, HOMEPAGE);

	(void)memset(server_msg, 0, sizeof(server_msg));
	if(read(s, server_msg, sizeof(server_msg) - 1) <= 0)
	  tzo_print_error(LOG_ERR, EXIT, PERROR, "read() failed");

	ptr1 = strstr(server_msg, "[");
	ptr2 = strstr(server_msg, "]");
	ptr1++;
	strncpy(host_ip, ptr1, ptr2-ptr1);

	//fprintf(stderr, "host_ip: %s\n", host_ip);

	(void)memset(server_msg, 0, sizeof(server_msg));
	
  strcat(server_msg, head);
	strcat(server_msg, args->hostnames);
	strcat(server_msg, comm);
	strcat(server_msg, args->email);
	strcat(server_msg, comm);
	strcat(server_msg, args->key);
	strcat(server_msg, comm);
	strcat(server_msg, host_ip);
	strcat(server_msg, tail);
	//sprintf(server_msg, "%s\r\n", server_msg);

	//fprintf(stderr, "\n\rServer Message: %s", server_msg);
	//fflush(stdout);



	(void)dprintf(s, server_msg);
	
	(void)memset(server_msg, 0, sizeof(server_msg));
	
  if(read(s, server_msg, sizeof(server_msg) -1) <= 0)
		tzo_print_error(LOG_ERR, EXIT, PERROR, "read() failed");

	printf("\n\rServer Message: %s", server_msg);
	fflush(stdout);
	nvram_set("TZODDNSok", "Unknown error!");
	for(n = 0; return_codes[n].code != NULL; n++) 
	{
		if(strstr(server_msg, return_codes[n].code)) 
		{
			nvram_set("DDNS_updated_status", return_codes[n].code);
			nvram_set("TZODDNSok", return_codes[n].message);
		}
	}
	
	return 1;
}
