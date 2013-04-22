#
# udhcpd Makefile
#
# Copyright (C) 2002 Gemtek Corporation
#
# $Id: Makefile,v 0.1 2002/12/13 AM 09:47:55 , Made by PeterChen
#

prefix=/usr
SBINDIR=/sbin
USRSBINDIR=${prefix}/sbin
USRBINDIR=${prefix}/bin
USRSHAREDIR=${prefix}/share

CFLAGS	+= $(PATHNVRAM)
CFLAGS	+= $(PATHUSER)
CFLAGS  += $(PATHGEM)
CFLAGS  += $(PATHSHARED)
LDFLAGS	+= $(LIBNVRAM)
LDFLAGS += $(LIBUTILILTY)
LDFLAGS += $(LIBSHARED)
CFLAGS	+= -I. -I../shared -I$(SRCBASE)/include -Wall
CFLAGS	+= -s -O2 -DOSCAR
LDFLAGS	+= $(LIBNVRAM)

# Comment this to involve some features ( related with LED control ).
# Uncomment this to remove some features ( related with LED control ).
# This item is for some project that they needn't the WAN side LED control.
# The WAN Led control was added by Gemtek
# This item is added by Gemtek
CFLAGS += -DRemoveWANLedControl

# Uncomment this to get a shared binary. Call as udhcpd for the server,
# and udhcpc for the client
# COMBINED_BINARY=1

# Uncomment this for extra output and to compile with debugging symbols
# Added by PeterChen , 2002/12/12 PM 02:38 , For debugging
# PeterChen_DBG=1
# PeterChen_DEBUG=1

# Uncomment this for extra output and to compile with debugging symbols
#DEBUG=1

# Uncomment this to output messages to syslog, otherwise, messages go to stdout
#CFLAGS += -DSYSLOG

VER := 0.9.8


OBJS_SHARED = options.o socket.o packet.o pidfile.o
DHCPD_OBJS = dhcpd.o arpping.o files.o leases.o serverpacket.o
DHCPC_OBJS = dhcpc.o clientpacket.o script.o ../shared/shutils.o

ifdef COMBINED_BINARY
EXEC1 = udhcpd
OBJS1 = $(DHCPD_OBJS) $(DHCPC_OBJS) $(OBJS_SHARED) frontend.o
CFLAGS += -DCOMBINED_BINARY
else
EXEC1 = udhcpd
OBJS1 = $(DHCPD_OBJS) $(OBJS_SHARED)

EXEC2 = udhcpc
OBJS2 = $(DHCPC_OBJS) $(OBJS_SHARED)
endif

EXEC3 = dumpleases
OBJS3 = dumpleases.o

BOOT_PROGRAMS = udhcpc
DAEMONS = udhcpd
COMMANDS = dumpleases

ifdef SYSLOG
CFLAGS += -DSYSLOG
endif

CFLAGS += -W -Wall -Wstrict-prototypes -DVERSION='"$(VER)"'

ifdef PeterChen_DBG		# Added by PeterChen , 2002/12/12 PM 02:38 , For debugging
CFLAGS += -DPeterChen_DBG
endif

ifdef	PeterChen_DEBUG		# Added by PeterChen , 2002/12/12 PM 02:38 , For debugging
CFLAGS += -DPeterChen_DEBUG
endif

ifdef DEBUG
CFLAGS += -g -DDEBUG
#STRIP=true
else
CFLAGS += -O2 -fomit-frame-pointer
#STRIP=$(CROSS_COMPILE)strip
endif

all: $(EXEC1) $(EXEC3)

$(OBJS1) $(OBJS2) $(OBJS3): *.h Makefile
$(EXEC1) $(EXEC2) $(EXEC3): Makefile

.c.o:
	$(CC) -c $(CFLAGS) $<

$(EXEC1): $(OBJS1)
	$(CC) $(LDFLAGS) $(OBJS1) -o $(EXEC1) -lnvram
	$(STRIP) $(EXEC1)

$(EXEC2): $(OBJS2)
	$(CC) $(LDFLAGS) $(OBJS2) -o $(EXEC2) -lnvram
	$(STRIP) $(EXEC2)

$(EXEC3): $(OBJS3)
	$(CC) $(LDFLAGS) $(OBJS3) -o $(EXEC3)
	$(STRIP) $(EXEC3)

install:
	install -D udhcpd $(INSTALLDIR)/bin/udhcpd
	install -D $(EXEC3) $(INSTALLDIR)/bin/$(EXEC3)

clean:
	-rm -f udhcpd udhcpc dumpleases *.o core