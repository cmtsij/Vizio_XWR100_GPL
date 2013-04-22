/* -- updatedd: updatedd.c --
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

#if defined (__linux__)
 #define LONGOPT
#elif defined (__OpenBSD__) || defined (__FreeBSD__) || defined (__NetBSD__)
 #define NOLONGOPT
#else
 #error This Program has not been tested on this operating system.
 #define NOLONGOPT
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
//#include <dlfcn.h>
#include <sys/types.h>
#include <dirent.h>
#include "modules/error.h"
#include "version.h"
#include <netinet/in.h>
#include <netdb.h>
#include <bcmnvram.h>
#include <sys/signal.h>

#define LIBPATH		"/usr/local/lib/updatedd/"
#define MAXLIBS		10
#define MAXDATA		256

char hostIp[20];
void* load_library(char *modul);
char* get_service(int argc, char *argv[], char *service[]);

void
error(int instruction, int error_type, char *message, ...) {

	va_list az;

	va_start(az, message);
	error_f(instruction, error_type, message, az);
	va_end(az);
	return;

}

char *
savestring(char *old_string) {
	char *string;
	int len = strlen(old_string) + 1;

	if(strlen(old_string) >= MAXDATA)
	    error(EXIT, STANDARD, "%s: service name is too long",
				old_string);

	if((string = (char *)malloc(len * sizeof(char))) == NULL)
	    error(EXIT, PERROR, "malloc() failed");
	(void)memset(string, 0, len);
	(void)strcpy(string, old_string);

	return string;
}

void
list_services(char *service[]) {

	struct dirent *dir_info;
	DIR *dir;
	int c;

	for(c = 0; c < MAXLIBS; c++)
	    service[c] = NULL;

//	if((dir = opendir(LIBPATH)) == NULL)
//	    error(EXIT, STANDARD, "no such file or directory: %s",
//				LIBPATH);
//	for(c = 0; (dir_info = readdir(dir)) != NULL;) {
//		for(; strstr(dir_info->d_name, ".so") && c < MAXLIBS; c++) {
//			/* cut ".so" and save the modul name */
//			dir_info->d_name[strlen(dir_info->d_name) - 3] = '\0';
//			service[c] = savestring(dir_info->d_name);
//		}
//	}



	service[0] = savestring("default");
	service[1] = savestring("dyndns");
	service[2] = savestring("hn");
	service[3] = savestring("ovh");
	service[4] = savestring("ods");
	service[5] = savestring("noip");
	service[6] = savestring("tzo");
	service[7] = savestring("changeip");
	
	
}

void add_iptable_rule(int ddns_type)
{
	struct	sockaddr_in addr;
	struct	hostent *host;
	char tmpbuf[512];
	
	if(ddns_type == 0 || ddns_type == 1) {
		if((host = MyGetHostByName("members.dyndns.org")) == NULL)
		   return -1;
		
		strcpy( hostIp , inet_ntoa(*(struct in_addr*)host->h_addr) );
		sprintf(tmpbuf , "/bin/iptables -t nat -A PORT_FW -p tcp -s %s --sport 80 -d %s -j ACCEPT"
			               , hostIp , nvram_safe_get("wan_ipaddr"));
		system( tmpbuf );
	}/*
	else if(ddns_type == 6) {
		sprintf(tmpbuf , "/bin/iptables -t nat -A PORT_FW -p tcp --sport 21340 -d %s -j ACCEPT"
		               , nvram_safe_get("wan_ipaddr"));
		system( tmpbuf );
		sprintf(tmpbuf , "/bin/iptables -t nat -A PORT_FW -p tcp --sport 21347 -d %s -j ACCEPT"
		               , nvram_safe_get("wan_ipaddr"));
		system( tmpbuf );	
	}*/
	/* GTK Marmot : add the ddns type check or the update packet will send to the members.dyndns.org every time 20080219*/
	/*
	if((host = MyGetHostByName("members.dyndns.org")) == NULL)
	    return -1;

	strcpy( hostIp , inet_ntoa(*(struct in_addr*)host->h_addr) );
	sprintf(tmpbuf , "/bin/iptables -t nat -A PORT_FW -p tcp -s %s --sport 80 -d %s -j ACCEPT"
	               , hostIp , nvram_safe_get("wan_ipaddr"));
	system( tmpbuf );
	sprintf(tmpbuf , "/bin/iptables -t nat -A PORT_FW -p tcp --sport 21340 -d %s -j ACCEPT"
		               , nvram_safe_get("wan_ipaddr"));
	system( tmpbuf );
	sprintf(tmpbuf , "/bin/iptables -t nat -A PORT_FW -p tcp --sport 21347 -d %s -j ACCEPT"
		               , nvram_safe_get("wan_ipaddr"));
	system( tmpbuf );	
	*/
}

void del_iptable_rule()
{
	struct	sockaddr_in addr;
	struct	hostent *host;
	char tmpbuf[512];
	/* GTK Marmot : add the ddns type check or the update packet will send to the members.dyndns.org every time 20080219*/
	
	if(strcmp(hostIp, "")) {	
		sprintf(tmpbuf , "/bin/iptables -t nat -D PORT_FW -p tcp -s %s --sport 80 -d %s -j ACCEPT"
	        	       , hostIp , nvram_safe_get("wan_ipaddr"));
		system( tmpbuf );
	}/*
	sprintf(tmpbuf , "/bin/iptables -t nat -D PORT_FW -p tcp --sport 21340 -d %s -j ACCEPT"
	               , nvram_safe_get("wan_ipaddr"));
	system( tmpbuf );
	sprintf(tmpbuf , "/bin/iptables -t nat -D PORT_FW -p tcp --sport 21347 -d %s -j ACCEPT"
	               , nvram_safe_get("wan_ipaddr"));
	system( tmpbuf );	*/
	exit(0);
}


int
main(int argc, char *argv[]) {

	char *lib, *service[MAXLIBS], *default_lib = NULL;
	struct error_s err_s;
	int c, cc;

	signal(SIGUSR1, del_iptable_rule);
	
	init_error(&err_s, LOG_USER, argv[0], EMAIL);
	list_services(service);

	if((lib = get_service(argc, argv, service)) == NULL)
		cc = 0;
	else
		for(c = 0; service[c] !=NULL; c++)
			if(strcmp(service[c], lib) == 0)
			{
				cc = c;
				break;
			}		

	sleep(3);
	add_iptable_rule(cc);

	switch(cc) {
		case 0:
			dyndns(argc, argv);
			break;
		case 1:
			dyndns(argc, argv);
			break;
/*		case 2:
			hn(argc, argv);
			break;
		case 3:
			ovh(argc, argv);			
			break;
		case 4:
			ods(argc, argv);
			break;
		case 5:
			noip(argc, argv);
			break;*/
		case 6:
			tzo(argc, argv);
			break;
/*		case 7:
			changeip(argc, argv);
			break;*/
		default:
			dyndns(argc, argv);
			break;
	}
	del_iptable_rule();
	for(c = 0; service[c]; c++)
	    free(service[c]);
	return 0;

}

char *
get_service(int argc, char *argv[], char *service[]) {

	int c, i;

	for(c = 1; c < argc; c++) {
#ifdef LONGOPT
		if(strcmp(argv[c], "-L") == 0 ||
		    strcmp(argv[c], "--list-services") == 0) {
#endif
#ifdef NOLONGOPT
		if(strcmp(argv[c], "-L") == 0) {
#endif
			(void)printf("\n\t \033[0;32;1m  Services\033[0m\n\t|¯¯¯¯¯¯¯¯¯¯¯¯\n");
			for(c = 0; service[c]; c++)
			    (void)printf("\t|  %s\n", service[c]);
			(void)printf("\n");
			exit(EXIT_SUCCESS);
		}
	}

	for(c = 1; c < argc; c++) {
#ifdef LONGOPT
		if(strcmp(argv[c], "-S") == 0 ||
		    strcmp(argv[c], "--service") == 0) {
#endif
#ifdef NOLONOPT
		if(strcmp(argv[c], "-S") == 0) {
#endif
			if((c+=1) >= argc)
			    return NULL;
			for(i = 0; service[i] != NULL; i++) {
				if(strcmp(argv[c], service[i]) == 0)
				    return service[i];
			}
		}
	}

	return NULL;

}

void *
load_library(char *lib) {

//	char *c_error;
//	void *handle;
//	void (*function)(void);
//	char path[256];
//
//	(void)memset(path, 0, sizeof(path));
//	(void)snprintf(path, sizeof(path)-1, "%s%s.so", LIBPATH, lib);
//
//	if((handle = dlopen(path, RTLD_LAZY)) == NULL)
//	    error(EXIT, STANDARD, "%s: could not open library", path);
//
//	function = dlsym(handle, "dyndns");
//	if((c_error = dlerror()) != NULL)
//	    error(EXIT, STANDARD, "error in library '%s': %s", c_error, path);
//
//	return function;

}
