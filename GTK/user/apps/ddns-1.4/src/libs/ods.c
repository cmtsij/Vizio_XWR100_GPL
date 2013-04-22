/* -- updatedd: ods.c --
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
#include "../modules/get_ifaddr.h"
#include "../modules/error.h"
#include "../version.h"
#ifdef NODPRINTF
 #include "../modules/dprintf.c"
#endif


#include <bcmnvram.h>

#define DYNDNSHOST	"ods.org"
#define DYNDNSHOST_IP	"66.28.41.162"
#define PORT		7070

#define CACHEFILE	"/tmp/updatedd-ods.ip"
#define MAXDATA 	128

#ifdef LONGOPT
#define USAGE	"\n                                Updatedd - \033[0;32;1mods\033[0m\n\n"							\
		"Usage: %s [--with-syslog] [--mx <mxhost>] [--service <service>] [--interface <interface>]\n"		\
		"	   <--hostnames <h1,h2,h3...>> <--user <user:pass>>\n\n"					\
		"	-h	--help			print this help\n"						\
		"	-i	--interface <interface>	iterface where to grab the ip from\n"				\
		"	-L	--list-services		list supported services\n"					\
		"	-m	--mx <mxhost>		hostname of your mail exchange\n"				\
		"	-n	--hostname <hostname>	your hostname\n"						\
		"	-S	--service <service>	select the type of service you are using\n"			\
		"	-u	--user <user:pass>	user and password to login with\n"				\
		"	-v	--version		print the version\n"						\
		"	-y	--with-syslog		print error messages to syslog\n\n"
#endif
#ifdef NOLONGOPT
#define USAGE	"\n                       Updatedd - \033[0;32;1mods\033[0m\n\n"			\
		"Usage: %s [-iy] [-S <service>] [-m <mxhost>] [-i <interface>] <-n <h1,h2...>>\n"	\
		"	   <-u <user:pass>>\n\n"							\
		"	-b		enable backmx\n"						\
		"	-h		print this help\n"						\
		"	-i <interface>	interface where to grad the ip from\n"				\
		"	-L		list supported services\n"					\
		"	-m <mxhost>	hostname of your mail exchange\n"				\
		"	-n <hostname>	your hostname\n"						\
		"	-o		set host to offline mode\n"					\
		"	-S <service>	select the type of service you are using\n"			\
		"	-u <user:pass>	user and password to login with\n"				\
		"	-w		enable wildcard\n"						\
		"	-v		print the version of updatedd\n"				\
		"	-y		print error messages to syslog\n\n"
#endif

struct arguments {
	char *hostname;
	char *interface;
	char *mx;
	char *user;
};

int i_syslog_ods = 0;

struct ip_addresses {
	struct in_addr cached;
	struct in_addr real_ip;
};

int get_flags_ods(struct arguments *args, int argc, char *argv[]);
int check_ods(char *interface, struct ip_addresses *ip);
void update_ods(int s, struct ip_addresses *ip, struct arguments *args);
int get_local_cached_ip_ods(struct ip_addresses *ip);
int check_server_msg_ods(int s);
char *server_msg_ods(int s, char *msg, int err_code);

void
print_error_ods(int priority, int instruction, int error_type, const char *message, ...) {

	va_list az;

	va_start(az, message);
	if(i_syslog_ods == 1)
	    log_error(priority, instruction, error_type, message, az);
	else
	    error_f(instruction, error_type, message, az);
	va_end(az);

	return;

}

void
ods(int argc, char *argv[]) {

	struct arguments args = { NULL, NULL, NULL, NULL };
	struct ip_addresses ip;
	struct error_s err_s;
	int s = 0;

	(void)memset(&ip, 0, sizeof(ip));
	init_error(&err_s, LOG_USER, argv[0], EMAIL);

	if(get_flags_ods(&args, argc, argv)) {
		(void)fprintf(stderr, USAGE, argv[0]);
		exit(EXIT_FAILURE);
	}

	if((args.interface != NULL && check_ods(args.interface, &ip))
	    || args.interface == NULL) {
		if((s = get_connection(DYNDNSHOST_IP, PORT)) < 0)
		    print_error_ods(LOG_ERR, EXIT, STANDARD, "get_connection() failed");
		update_ods(s, &ip, &args);
		(void)close(s);
	}

	return;

}


int
get_flags_ods(struct arguments *args, int argc, char *argv[]) {

	int c;

	while(1) {
#ifdef LONGOPT
		int option_index = 0;
		static struct option long_options[] = {
			{ "with-syslog",	0, 0, 'y' },
			{ "help",		0, 0, 'h' },
			{ "hostnames",		1, 0, 'n' },
			{ "interface",		1, 0, 'i' },
			{ "service",		1, 0, 'S' },
			{ "mx",			1, 0, 'm' },
			{ "version",		0, 0, 'v' },
			{ "user",		1, 0, 'u' },
			{ NULL,			0, 0, 0   }
		};

		c = getopt_long(argc, argv, "yhi:n:S:m:vu:",
				long_options, &option_index);
#endif

#ifdef NOLONGOPT
		c = getopt(argc, argv, "yhi:n:S:m:vu:");
#endif
		if(c == -1)
		    break;

		switch(c) {
			case 'y':
			    i_syslog_ods = 1;
			    break;
			case 'h':
			    (void)printf(USAGE, argv[0]);
			    exit(EXIT_SUCCESS);
			case 'i':
			    args->interface = optarg;
			case 'm':
			    args->mx = optarg;
			    break;
			case 'n':
			    args->hostname = optarg;
			    break;
			case 'S':
			    break;
			case 'v':
			    (void)printf("\n%s %s - © by %s\n\t\t %s\n\n",
					 PNAME, VERSION, AUTHOR, HOMEPAGE);
			    exit(EXIT_SUCCESS);
			case 'u':
			    if((args->user = (char *)malloc(strlen(optarg) + 1)) == NULL)
				print_error_ods(LOG_ERR, EXIT, PERROR, "malloc() failed");
			    strcpy(args->user, optarg);
			    args->user[strlen(optarg)] = '\0';
			    bzero(optarg, strlen(optarg));	// prevent that anybody can see your pass with ps
		}
	}

	if(args->hostname == NULL || args->user == NULL)
	    return 1;

	return 0;

}

int
check_ods(char *interface, struct ip_addresses *ip) {

	if((ip->real_ip.s_addr = get_ifaddr(interface)) == 0)
	    print_error_ods(LOG_ERR, EXIT, STANDARD, "%s: invalid interface", interface);
	if(get_local_cached_ip_ods(ip))
	    return 1;
	if(ip->real_ip.s_addr != ip->cached.s_addr)
	    return 1;
	return 0;

}

int
get_local_cached_ip_ods(struct ip_addresses *ip) {

	FILE *fp;
	char ip_buffer[18] = { '\0' };

	if((fp = fopen(CACHEFILE, "r")) == NULL)
	    return 1;
	if(!(fgets(ip_buffer, sizeof(ip_buffer) - 1, fp) &&
	     inet_aton(ip_buffer, &ip->cached) != 0)) {
		(void)fclose(fp);
		print_error_ods(LOG_ERR, EXIT, STANDARD, "%s: invalid ip address", CACHEFILE); 
	}
	(void)fclose(fp);

	return 0;

}

void
update_ods(int s, struct ip_addresses *ip, struct arguments *args) {

	char *ip_addr, msg[MAXDATA], *ptr;
	char *user, *pass;
	FILE *fp;

	user = strtok(args->user, ":");
	pass = strtok(NULL, ":");

	if(args->interface != NULL) {
		ip_addr = inet_ntoa(ip->real_ip);

		(void)unlink(CACHEFILE);
		if((fp = fopen(CACHEFILE, "a")) == NULL)
		    print_error_ods(LOG_ERR, EXIT, PERROR, "fopen() failed");
		(void)fprintf(fp, "%s", ip_addr);
		fclose(fp);
	} else ip_addr = "CONNIP";

	nvram_set("DDNSok", "0");

	if((ptr = server_msg_ods(s, msg, 100)) != NULL)
	    print_error_ods(LOG_ERR, EXIT, STANDARD, "%s", ptr);
	else {
		(void)dprintf(s, "LOGIN %s %s\n", user, pass);
		if((ptr = server_msg_ods(s, msg, 225)) != NULL)
		    print_error_ods(LOG_ERR, EXIT, STANDARD, "%s", ptr);
		else {
			dprintf(s, "DELRR %s A\n", args->hostname);
			if((ptr = server_msg_ods(s, msg, 901)) != NULL)
			    print_error_ods(LOG_ERR, EXIT, STANDARD, "%s", ptr);
			else {
				dprintf(s, "ADDRR %s A %s\n", args->hostname, ip_addr);
				if((ptr = server_msg_ods(s, msg, 795)) != NULL)
				    print_error_ods(LOG_ERR, EXIT, STANDARD, "%s", ptr);
				else { 
					nvram_set("DDNSok", "1");
					print_error_ods(LOG_NOTICE, CONTINUE, STANDARD, "update successful");
				}
			}
		}
	}

	return;

}

char *
server_msg_ods(int s, char *msg, int err_code) {

	(void)memset(msg, 0, MAXDATA);
	(void)read(s, msg, MAXDATA-1);

	*(msg+3) = '\0';
	if(err_code != atoi(msg))
	    return (msg+4);
	return NULL;

}
