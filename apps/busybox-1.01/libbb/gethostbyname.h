/* gethostbyname.h
 *
 * Copyright 2008, Gemtek Corporation
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
/*
 * gethostbyname.h
 *
 * Copyright (C) many different people.  If you wrote this, please
 * acknowledge your work.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 */
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <syslog.h>
#include <errno.h>


#ifndef DPROXY_H
#define DPROXY_H

#ifndef PORT
#define PORT 53
#endif

#define BUF_SIZE 2048

#ifndef CONFIG_FILE_DEFAULT 
#define CONFIG_FILE_DEFAULT "/etc/dproxy.conf"
#endif
#ifndef DENY_FILE_DEFAULT 
#define DENY_FILE_DEFAULT "/etc/dproxy.deny"
#endif
#ifndef CACHE_FILE_DEFAULT 
#define CACHE_FILE_DEFAULT "/tmp/dproxy.cache"
#endif
#ifndef HOSTS_FILE_DEFAULT 
#define HOSTS_FILE_DEFAULT "/etc/hosts"
#endif
#ifndef PURGE_TIME_DEFAULT 
#define PURGE_TIME_DEFAULT 48 * 60 * 60
#endif
#ifndef PPP_DEV_DEFAULT 
#define PPP_DEV_DEFAULT "/var/run/ppp0.pid"
#endif
#ifndef DHCP_LEASES_DEFAULT 
#define DHCP_LEASES_DEFAULT "/var/state/dhcp/dhcpd.leases"
#endif
#ifndef PPP_DETECT_DEFAULT 
#define PPP_DETECT_DEFAULT 1
#endif
#ifndef DEBUG_FILE_DEFAULT 
#define DEBUG_FILE_DEFAULT "/tmp/var/log/dproxy.debug.log"
#endif
 

struct udp_packet{
  struct dns_header dns_hdr;
  char buf[4096];
  char len;
  struct in_addr src_ip;
  int src_port;
};

void debug(char *fmt, ...);
void debug_perror(char * msg);

#endif

extern struct resolv_header;
static char *ws ( char ** );
void open_nameservers ();
void dns_catch_signal ( int );
struct hostent *GetHostByName ( const char * );
int length_question ( unsigned char * , int );
int length_dotted ( const unsigned char * , int );
char **cfgfind ( FILE * , char * );
char **cfgread ( FILE * );
int encode_dotted ( const char * , unsigned char * , int );
int encode_header ( struct resolv_header * , unsigned char * , int );
int decode_header ( unsigned char *, struct resolv_header * );
int encode_header ( struct resolv_header * , unsigned char * , int );

typedef unsigned long	ip_address_t;	/* Added by PeterChen , 2002-1126-1414 */

