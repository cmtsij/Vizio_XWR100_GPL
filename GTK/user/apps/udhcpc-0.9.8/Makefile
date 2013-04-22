# udhcp makefile

#prefix=/usr
#SBINDIR=/sbin
#USRSBINDIR=${prefix}/sbin
#USRBINDIR=${prefix}/bin
#USRSHAREDIR=${prefix}/share

CFLAGS	+= -I. -Wall $(PATHNVRAM)
CFLAGS	+= -s -O2 -DOSCAR
LDFLAGS	+= $(LIBNVRAM)

# Uncomment this to get a shared binary. Call as udhcpd for the server,
# and udhcpc for the client
#COMBINED_BINARY=1

# Uncomment this for extra output and to compile with debugging symbols
#DEBUG=1

# Uncomment this to output messages to syslog, otherwise, messages go to stdout
#CFLAGS += -DSYSLOG

# Uncomment this, dhcpc will calculate lease time by using uptime,
# otherwise dhcpc will times to calculate and sometimes it makes dhcpc state goes wrong state
CFLAGS += -DSYS_UPTIME

VER := 0.9.8


OBJS_SHARED = options.o socket.o packet.o pidfile.o
DHCPD_OBJS = dhcpd.o arpping.o files.o leases.o serverpacket.o
DHCPC_OBJS = dhcpc.o clientpacket.o script.o

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

ifdef DEBUG
CFLAGS += -g -DDEBUG
STRIP=true
else
CFLAGS += -O2 -fomit-frame-pointer
STRIP=$(CROSS_COMPILE)strip
endif

CFLAGS	+= -I. -I../shared -I../shared/nvram
LDFLAGS	+= -L../nvram/bin
LDFLAGS += -lnvram

all: $(EXEC2)

$(OBJS1) $(OBJS2) $(OBJS3): *.h Makefile
$(EXEC1) $(EXEC2) $(EXEC3): Makefile

.c.o:
	$(CC) -c $(CFLAGS) $<

$(EXEC1): $(OBJS1)
	$(CC) $(LDFLAGS) $(OBJS1) -o $(EXEC1)
	$(STRIP) $(EXEC1)
	
$(EXEC2): $(OBJS2)
	$(CC) $(LDFLAGS) $(OBJS2) -o $(EXEC2)

$(EXEC3): $(OBJS3)
	$(CC) $(LDFLAGS) $(OBJS3) -o $(EXEC3)
	$(STRIP) $(EXEC3)

install:
	install -D udhcpc $(INSTALLDIR)/bin/udhcpc

clean:
	-rm -f udhcpd udhcpc dumpleases *.o core

#romfs:
#	cp  udhcpc $(ROMFSDIR)/usr/sbin/udhcpc

