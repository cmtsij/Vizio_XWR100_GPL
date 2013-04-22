#
# milli_httpd Makefile
#
# Copyright 2005, Broadcom Corporation
# All Rights Reserved.
# 
# THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
# KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
# SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
#
# $Id$
#

# Usage : httpd serverip port
# serverip : httpd socket will bound on this ipaddress
# port : httpd socket will bound on this port
# example : httpd -i br0 -p 80 -s 1(-s https enabled/disabled(1/0) )
# example : httpd -i eth0 -p 443


# Uncomment this to support HTTPS (with OpenSSL)
# Bug : Wait for fix
# find out the current https solution and working flow, try to optimize the source code
#+++ GTK July 11, 2007.
#USE_SSL = y
#--- GTK


# Uncomment this to keep HTTP alive by forking a child process to handle normal HTTP,
# the parent process will watch the status of child process. If child process die, parent will
# revoke http by create new process
#+++ GTK July 10, 2007.
#KEEP_ALIVE = y
#--- GTK

# Uncomment this to display httpd debug message
# HTTPD_DEBUG = y

# Uncomment this to display CGI debug message
CGI_DEBUG = n

# Uncomment this to make httpd deny access from wireless client
WL_ACL = y

CFLAGS	+=  -Wall -s -Os

ifdef KEEP_ALIVE
	CFLAGS += -DKEEP_ALIVE
endif

ifdef HTTPD_DEBUG
	CFLAGS += -DHTTPD_DEBUG
endif

ifdef CGI_DEBUG
	CFLAGS += -DCGI_DEBUG
endif

ifdef WL_ACL
	CFLAGS += -DWL_ACL
endif

ifdef USE_SSL
	CFLAGS	+= -DUSE_SSL  -DLINUX
endif

CFLAGS	+= $(PATHNVRAM)
CFLAGS	+= $(PATHUSER)
CFLAGS	+= $(PATHGEM)
CFLAGS	+= $(PATHSHARED)
CFLAGS	+= -I$(TOPDIR)/apps/wireless_tools.28/
CFLAGS	+= -I$(TOPDIR)/wlan/linux

LDFLAGS += $(LIBUTILILTY)
LDFLAGS	+= $(LIBNVRAM)
LDFLAGS += $(LIBSHARED)
LDFLAGS	+= -L$(TOPDIR)/apps/wireless_tools.28 -liw


ifdef USE_SSL
	OBJS = httpd.o cgi.o ej.o convert.o gemtek.o webs_control.o gemtek_wep_passphrase.o ssl_util.o options.o sslSocket.o 
else
	OBJS = httpd.o cgi.o ej.o convert.o gemtek.o webs_control.o gemtek_wep_passphrase.o options.o
endif

# for HNAP +++++++++++++++++++++
#OBJS += hnap/hnap_implementation.o hnap/hnap_host.o hnap/xml_tree.o hnap/linked_list.o hnap/stack.o hnap/xmlparse.o hnap/xmlrole.o hnap/xmltok.o
# ------------------------------

all: clean httpd

clean:
	rm -f httpd rmt_httpd COPYRIGHT	
#	rm -f ./hnap/*.o

install:
	$(STRIP) httpd
	install -D httpd $(INSTALLDIR)/bin/httpd
	cd $(INSTALLDIR)/bin && ln -sf httpd rmt_httpd
	
romfs:
	$(STRIP) httpd
	cp httpd $(ROMFSDIR)/bin/httpd
	cd $(ROMFSDIR)/bin && ln -sf httpd rmt_httpd

httpd: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	$(STRIP) $@
	
%.o: %.c %.h Makefile
	 $(CC) $(CFLAGS) -c $< -o $@
