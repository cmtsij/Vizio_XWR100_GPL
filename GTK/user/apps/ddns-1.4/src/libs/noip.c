/* -- updatedd: noip.c --
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
#include "../modules/get_connection.h"
#include "../modules/get_ifaddr.h"
#include "../modules/error.h"
#include "../version.h"
#ifdef NODPRINTF
 #include "../modules/dprintf.c"
#endif



#include <bcmnvram.h>

#define DYNDNSHOST	"dynupdate.no-ip.com"
#define DYNDNSHOST_IP	"66.185.162.101"
#define PORT		80

#define CACHEFILE	"/tmp/updatedd-noip.ip"
#define MAXDATA 	512

#ifdef LONGOPT
#define USAGE	"\n                                Updatedd - \033[0;35;1mnoip\033[0m\n\n"				\
		"Usage: %s [--with-syslog] [--service <service>] <--email <email>> <--pass <password>>\n"		\
		"	   <--group <groupname> || --hostname <hostname>> <--interface <interface>>\n\n"		\
		"	-e	--email <email>		your e-mail address\n"						\
		"	-h	--help			print this help\n"						\
		"	-L	--list-services		list supported services\n"					\
		"	-i	--interface <interface>	interface connected to the internet\n"				\
		"	-g	--group <groupname>	name of the group which you want to update\n"			\
		"	-n	--hostname <hostname>	your hostname\n"						\
		"	-S	--service <service>	select the type of service you are using\n"			\
		"	-p	--pass <pass>		your password\n"						\
		"	-v	--version		print the version\n"						\
		"	-y	--with-syslog		print error messages to syslog\n\n"
#endif
#ifdef NOLONGOPT
#define USAGE	"\n                       Updatedd - \033[0;35;1mnoip\033[0m\n\n"			\
		"Usage: %s [-y] [-S <service>] [-m <mxhost>] <-e <email>> <-p <password>>\n"		\
		"	   <-n <hostname> || -g <groupname>> <-i <interface>>\n\n"			\
		"	-e <email>	your email address\n"						\
		"	-h		print this help\n"						\
		"	-L		list supported services\n"					\
		"	-i <interface>	interface connected to the internet\n"				\
		"	-n <hostname>	your hostname\n"						\
		"	-g <groupname>	name of the group which you want to update\n"			\
		"	-S <service>	select the type of service you are using\n"			\
		"	-p <password>	password to login with\n"					\
		"	-v		print the version of updatedd\n"				\
		"	-y		print error messages to syslog\n\n"
#endif

struct arguments {
	char *hostname;
	char *email;
	char *group;
	char *interface;
	char *pass;
};

int i_syslog_noip = 0;

struct ip_addresses {
	struct in_addr real_ip;
	struct in_addr cached;
};

static struct dyndns_return_codes {
	int code;
	char *message;
	int  error;
} return_codes[] = {
	{ 0,	"no update needed",				0 },
	{ 1,	"successfully updated",				0 },
	{ 2,	"bad hostname",					1 },
	{ 3,	"bad password",					1 },
	{ 4,	"bad user",					1 },
	{ 6,	"account has been banned",			1 },
	{ 7,	"invalid ip",					1 },
	{ 8,	"host has been disabled",			1 },
	{ 9,	"invalid host (web redirect)",			1 },
	{ 10,	"bad group",					1 },
	{ 11,	"group has been updated",			0 },
	{ 12,	"no update needed",				0 },
	{ 13,	"this client software has been disabled",	1 },
	{ 0,		NULL,					0 }
};

int get_flags_noip(struct arguments *args, int argc, char *argv[]);
int check_noip(char *interface, struct ip_addresses *ip);
void update_noip(int s, struct arguments *args, struct ip_addresses *ip);
void update_cache_noip(struct ip_addresses *ip);
int get_local_cached_ip_noip(struct ip_addresses *ip);
int check_server_msg_noip(int s);

void
print_error_noip(int priority, int instruction, int error_type, const char *message, ...) {

	va_list az;

	va_start(az, message);
	if(i_syslog_noip == 1)
	    log_error(priority, instruction, error_type, message, az);
	else
	    error_f(instruction, error_type, message, az);
	va_end(az);

	return;

}

void
noip(int argc, char *argv[]) {

	struct arguments args = { NULL, NULL, NULL, NULL, NULL };
	struct ip_addresses ip;
	struct error_s err_s;
	int s = 0;

	(void)memset(&ip, 0, sizeof(ip));
	init_error(&err_s, LOG_USER, argv[0], EMAIL);

	if(get_flags_noip(&args, argc, argv)) {
		(void)fprintf(stderr, USAGE,  argv[0]);
		exit(EXIT_FAILURE);
	}

	if(check_noip(args.interface, &ip)) {
		(void)close(s);
		if((s = get_connection(DYNDNSHOST_IP, PORT)) < 0)
		    print_error_noip(LOG_ERR, EXIT, STANDARD, "get_connection() failed");
		update_noip(s, &args, &ip);
		if(check_server_msg_noip(s))
		    print_error_noip(LOG_ERR, EXIT, STANDARD, "unknown server message");
		else update_cache_noip(&ip);
		(void)close(s);
	} else print_error_noip(LOG_INFO, CONTINUE, STANDARD, "update is not necessary");

	return;

}

int
get_flags_noip(struct arguments *args, int argc, char *argv[]) {

	int c;

	while(1) {
#ifdef LONGOPT
		int option_index = 0;
		static struct option long_options[] = {
			{ "with-syslog",	0, 0, 'y' },
			{ "help",		0, 0, 'h' },
			{ "group",		1, 0, 'g' },
			{ "hostname",		1, 0, 'n' },
			{ "interface",		1, 0, 'i' },
			{ "service",		1, 0, 'S' },
			{ "version",		0, 0, 'v' },
			{ "email",		1, 0, 'e' },
			{ "pass",		1, 0, 'p' },
			{ NULL,			0, 0, 0   }
		};

		c = getopt_long(argc, argv, "yhg:n:i:S:ve:p:",
				long_options, &option_index);
#endif

#ifdef NOLONGOPT
		c = getopt(argc, argv, "yhg:n:i:S:ve:p:");
#endif
		if(c == -1)
		    break;

		switch(c) {
			case 'y':
			    i_syslog_noip = 1;
			    break;
			case 'h':
			    (void)printf(USAGE, argv[0]);
			    exit(EXIT_SUCCESS);
			case 'g':
			    args->group = optarg;
			    break;
			case 'n':
			    args->hostname = optarg;
			    break;
			case 'i':
			    args->interface = optarg;
			    break;
			case 'S':
			    break;
			case 'v':
			    (void)printf("\n%s %s - © by %s\n\t\t %s\n\n",
					 PNAME, VERSION, AUTHOR, HOMEPAGE);
			    exit(EXIT_SUCCESS);
			case 'e':
			    args->email = optarg;
			    break;
			case 'p':
			    if((args->pass = (char *)malloc(strlen(optarg) + 1)) == NULL)
				print_error_noip(LOG_ERR, EXIT, PERROR, "malloc() failed");
			    strcpy(args->pass, optarg);
			    args->pass[strlen(optarg)] = '\0';
			    bzero(optarg, strlen(optarg));	// prevent that anybody can see your pass with ps
		}
	}

	if(args->email == NULL || args->pass == NULL || args->interface == NULL)
	    return 1;
	if((args->hostname == NULL && args->group == NULL) ||
	   (args->hostname != NULL && args->group != NULL))
	    return 1;

	return 0;

}

int
check_noip(char *interface,  struct ip_addresses *ip) {

	if((ip->real_ip.s_addr = get_ifaddr(interface)) == 0)
	    print_error_noip(LOG_ERR, EXIT, STANDARD, "invalid interface: %s", interface);
	if(get_local_cached_ip_noip(ip))
	    return 1;
	if(ip->real_ip.s_addr != ip->cached.s_addr)
	    return 1;
	return 0;

}

int
get_local_cached_ip_noip(struct ip_addresses *ip) {

	FILE *fp;
	char ip_buffer[18] = { '\0' };

	if((fp = fopen(CACHEFILE, "r")) == NULL)
	    return 1;
	if(!(fgets(ip_buffer, sizeof(ip_buffer) - 1, fp) &&
	     inet_aton(ip_buffer, &ip->cached) != 0)) {
		(void)fclose(fp);
		print_error_noip(LOG_ERR, EXIT, STANDARD, "%s: invalid ip address", CACHEFILE);
	}
	(void)fclose(fp);

	return 0;

}

void
update_noip(int s, struct arguments *args, struct ip_addresses *ip) {

	char *ptr, *arg;

	if(args->hostname != NULL) {
		ptr = "host=";
		arg = args->hostname;
	} else {
		ptr = "groupname=";
		arg = args->group;
	}

	(void)dprintf(s,	"GET /update.php?%s%s&username=%s&pass=%s&ip=%s HTTP/1.1\r\n"
				"Host: %s\r\n"
				"User-Agent: %s %s - %s\r\n"
				"Connection: close\r\n"
				"Pragma: no-cache\r\n\n",
				ptr, arg, args->email, args->pass, inet_ntoa(ip->real_ip),
				DYNDNSHOST, PNAME, VERSION, HOMEPAGE);

	return;

}

void
update_cache_noip(struct ip_addresses *ip) {

	FILE *fp;

	(void)unlink(CACHEFILE);
	if((fp = fopen(CACHEFILE, "a")) == NULL)
	    print_error_noip(LOG_ERR, EXIT, PERROR, "fopen() failed");
	(void)fprintf(fp, "%s", inet_ntoa(ip->real_ip));
	fclose(fp);

	return;

}

int
check_server_msg_noip(int s) {

	int n, ret_code;
	char server_msg[MAXDATA], *ptr;

	(void)memset(server_msg, 0, sizeof(server_msg));
	if(read(s, server_msg, sizeof(server_msg) - 1) < 0)
	    print_error_noip(LOG_ERR, EXIT, PERROR, "read() failed");
	else if(strstr(server_msg, "HTTP/1.1 200 OK") ||
	        strstr(server_msg, "HTTP/1.0 200 OK")) {
		ptr = strstr(server_msg, "status=");
		ret_code = atoi((ptr+7));
		for(n = 0; return_codes[n].message != NULL; n++) {
			if(ret_code == return_codes[n].code) {
				if(return_codes[n].error == 1)
				{
				    nvram_set("DDNSok","0");
				    print_error_noip(LOG_ERR, EXIT, STANDARD, return_codes[n].message);
				}
				else {
					nvram_set("DDNSok","1");
					print_error_noip(LOG_NOTICE, CONTINUE, STANDARD, return_codes[n].message);
					return 0;
				}
			}
		}
	}
	else
	    print_error_noip(LOG_ERR, EXIT, STANDARD, "Internal Server Error");

	return 1;
}
