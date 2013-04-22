/* -- updatedd: hn.c --
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
 #error This software has not been tested on this operating system.
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
#include "../modules/get_ifaddr.h"
#include "../modules/error.h"
#include "../version.h"
#ifdef NODPRINTF
 #include "../modules/dprintf.c"
#endif

#include <bcmnvram.h>

#define DYNDNSHOST	"dup.hn.org"
#define DYNDNSHOST_IP	"24.97.1.168"
#define PORT		80

#define CACHEFILE	"/tmp/updatedd-hn.ip"
#define MAXDATA 	512

#ifdef LONGOPT
#define USAGE	"\n                                Updatedd - \033[0;34;1mhn\033[0m\n\n"				\
		"Usage: %s [--with-syslog] [--service <service>] <--interface <interface>> <--user <user:pass>>\n"	\
		"	   <-n <hostname>>\n\n"										\
		"	-h	--help			print this help\n"						\
		"	-i	--interface <interface> interface where to grab the ip from\n"				\
		"	-L	--list-services		list supported services\n"					\
		"	-S	--service <service>	select the type of service you are using\n"			\
		"	-u	--user <user:pass>	user and password to login with\n"				\
		"	-v	--version		print the version\n"						\
		"	-y	--with-syslog		print error messages to syslog\n\n"
#endif
#ifdef NOLONGOPT
#define USAGE	"\n                       Updatedd - \033[0;34;1mhn\033[0m\n\n"			\
		"Usage: %s [-iy] [-S <service>] <-i <interface>> <-u <user:pass>> <-n <hostname>>\n\n"	\
		"	-h		print this help\n"						\
		"	-i <interface>	interface where to grad the ip from\n"				\
		"	-L		list supported services\n"					\
		"	-S <service>	select the type of service you are using\n"			\
		"	-u <user:pass>	user and password to login with\n"				\
		"	-v		print the version\n"						\
		"	-y		print error messages to syslog\n\n"
#endif

struct arguments {
	char *interface;
	char *user;
};

int i_syslog_hn = 0;

struct ip_addresses {
	struct in_addr cached;
	struct in_addr real_ip;
};

static struct dyndns_return_codes {
	char *code;
	char *message;
	int  error;
} return_codes[] = {
	{ "101",	"Successfully Updated",							0 },
	{ "201",	"Failure because previous update occured less than 300 seconds ago",	1 },
	{ "202",	"Failure because of server error",					1 },
	{ "203",	"Failure because account is frozen (by admin)",				1 },
	{ "204",	"Failure because account is locked (by user)",				1 },
	{ NULL,		NULL,									0 }
};

int get_flags_hn(struct arguments *args, int argc, char *argv[]);
int check_hn(char *interface, struct ip_addresses *ip);
void update_hn(int s, struct ip_addresses *ip, struct arguments *args);
void update_cache_hn(struct ip_addresses *ip);
int get_local_cached_ip_hn(struct ip_addresses *ip);
int check_server_msg_hn(int s);

void
print_error_hn(int priority, int instruction, int error_type, const char *message, ...) {

	va_list az;

	va_start(az, message);
	if(i_syslog_hn == 1)
	    log_error(priority, instruction, error_type, message, az);
	else
	    error_f(instruction, error_type, message, az);
	va_end(az);

	return;

}

void
hn(int argc, char *argv[]) {

	struct arguments args = { NULL, NULL };
	struct ip_addresses ip;
	struct error_s err_s;
	int s = 0;

	(void)memset(&ip, 0, sizeof(ip));
	init_error(&err_s, LOG_USER, argv[0], EMAIL);

	if(get_flags_hn(&args, argc, argv)) {
		(void)fprintf(stderr, USAGE,  argv[0]);
		exit(EXIT_FAILURE);
	}

	if(check_hn(args.interface, &ip)) {
		if((s = get_connection(DYNDNSHOST_IP, PORT)) < 0)
		    print_error_hn(LOG_ERR, EXIT, STANDARD, "get_connection() failed");
		update_hn(s, &ip, &args);
		if(check_server_msg_hn(s))
		    print_error_hn(LOG_ERR, EXIT, STANDARD, "unknown server message");
		else update_cache_hn(&ip);
		(void)close(s);
	} else print_error_hn(LOG_INFO, CONTINUE, STANDARD, "update is not necessary");

	return;

}

int
get_flags_hn(struct arguments *args, int argc, char *argv[]) {

	int c;

	while(1) {
#ifdef LONGOPT
		int option_index = 0;
		static struct option long_options[] = {
			{ "with-syslog",	0, 0, 'y' },
			{ "help",		0, 0, 'h' },
			{ "interface",		1, 0, 'i' },
			{ "service",		1, 0, 'S' },
			{ "version",		0, 0, 'v' },
			{ "user",		1, 0, 'u' },
			{ NULL,			0, 0, 0   }
		};

		c = getopt_long(argc, argv, "yhi:S:vu:",
				long_options, &option_index);
#endif

#ifdef NOLONGOPT
		c = getopt(argc, argv, "yhi:S:vu:");
#endif
		if(c == -1)
		    break;

		switch(c) {
			case 'y':
			    i_syslog_hn = 1;
			    break;
			case 'h':
			    (void)printf(USAGE, argv[0]);
			    exit(EXIT_SUCCESS);
			case 'i':
			    args->interface = optarg;
			    break;
			case 'S':
			    break;
			case 'v':
			    (void)printf("\n%s %s - © by %s\n\t\t %s\n\n",
					 PNAME, VERSION, AUTHOR, HOMEPAGE);
			    exit(EXIT_SUCCESS);
			case 'u':
			    if((args->user = (char *)malloc(strlen(optarg) + 1)) == NULL)
				print_error_hn(LOG_ERR, EXIT, PERROR, "malloc() failed");
			    strcpy(args->user, optarg);
			    args->user[strlen(optarg)] = '\0';
			    bzero(optarg, strlen(optarg));	// prevent that anybody can see your pass with ps
		}
	}

	if(args->user == NULL || args->interface == NULL)
	    return 1;

	return 0;

}

int
check_hn(char *interface, struct ip_addresses *ip) {

	if((ip->real_ip.s_addr = get_ifaddr(interface)) == 0)
	    print_error_hn(LOG_ERR, EXIT, STANDARD, "invalid interface: %s", interface);
	if(get_local_cached_ip_hn(ip))
	    return 1;
	if(ip->real_ip.s_addr != ip->cached.s_addr)
	    return 1;
	return 0;

}

int
get_local_cached_ip_hn(struct ip_addresses *ip) {

	FILE *fp;
	char ip_buffer[18] = { '\0' };

	if((fp = fopen(CACHEFILE, "r")) == NULL)
	    return 1;
	if(!(fgets(ip_buffer, sizeof(ip_buffer) - 1, fp) &&
	     inet_aton(ip_buffer, &ip->cached) != 0)) {
		(void)fclose(fp);
		print_error_hn(LOG_ERR, EXIT, STANDARD, "%s: invalid ip address", CACHEFILE);
	}
	(void)fclose(fp);

	return 0;

}

void
update_hn(int s, struct ip_addresses *ip, struct arguments *args) {

	char *b64user;

	if(strlen(args->user) > 128)
	    print_error_hn(LOG_ERR, EXIT, STANDARD, "username is too long");
	if((b64user = (char *)malloc((2 * strlen(args->user) + 1) * sizeof(char))) == NULL)
	    print_error_hn(LOG_ERR, EXIT, PERROR, "malloc() failed");
	(void)memset(b64user, 0, 2 * strlen(args->user) + 1);

	base64encode(args->user, b64user);
	(void)dprintf(s,	"GET /vanity/update/?VER=1&IP=%s HTTP/1.1\r\n"
				"Host: %s\r\n"
				"Authorization: basic %s\r\n"
				"User-Agent: %s %s - %s\r\n"
				"Connection: close\r\n"
				"Pragma: no-cache\r\n\n",
				inet_ntoa(ip->real_ip), DYNDNSHOST,
				b64user, PNAME, VERSION, HOMEPAGE);
	free(b64user);
	return;

}

void
update_cache_hn(struct ip_addresses *ip) {

	FILE *fp;

	(void)unlink(CACHEFILE);
	if((fp = fopen(CACHEFILE, "a")) == NULL)
	    print_error_hn(LOG_ERR, EXIT, PERROR, "fopen() failed");
	(void)fprintf(fp, "%s", inet_ntoa(ip->real_ip));
	fclose(fp);

	return;

}

int
check_server_msg_hn(int s) {

	int n;
	char server_msg[MAXDATA];

	(void)memset(server_msg, 0, sizeof(server_msg));
	if(read(s, server_msg, sizeof(server_msg) - 1) < 0)
	    print_error_hn(LOG_ERR, EXIT, PERROR, "read() failed");

	else if(strstr(server_msg, "HTTP/1.1 200 OK") ||
	        strstr(server_msg, "HTTP/1.0 200 OK")) {
		for(n = 0; return_codes[n].code != NULL; n++) {
			if(strstr(server_msg, return_codes[n].code)) {
				if(return_codes[n].error == 1)
				{
				    nvram_set("DDNSok","0");
				    print_error_hn(LOG_ERR, EXIT, STANDARD, return_codes[n].message);
				}
				else {
					nvram_set("DDNSok","1");
					print_error_hn(LOG_NOTICE, CONTINUE, STANDARD, return_codes[n].message);
					return 0;
				}
			}
		}
	} else if(strstr(server_msg, "HTTP/1.1 401 Authorization Required")) {
		print_error_hn(LOG_ERR, EXIT, STANDARD, "bad username or passwd");
	} else print_error_hn(LOG_ERR, EXIT, STANDARD, "Internal Server Error");

	return 1;
}
