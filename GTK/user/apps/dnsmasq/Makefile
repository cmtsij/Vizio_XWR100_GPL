ifneq ($(wildcard $(SRCBASE)/cy_conf.mak),)
  include $(SRCBASE)/cy_conf.mak
endif

OBJS = cache.o rfc1035.o dhcp.o util.o option.o forward.o network.o dnsmasq.o
BINDIR = /usr/local/sbin
MANDIR = /usr/local/man
CFLAGS += -I../nvram/include
LIBS += -L../nvram/bin -lnvram

#ifeq ($(MPPPOE_SUPPORT),1)
#OBJS += mpppoe.o
#CFLAGS  += -I. -I$(SRCBASE) -I$(SRCBASE)/include -I$(SRCBASE)/router/shared -DMPPPOE_SUPPORT=1
#LIBS += -L$(SRCBASE)/router/nvram -lnvram  -L$(SRCBASE)/router/shared -lshared
#endif

#ifeq ($(DNS_SUPPORT),1)
#OBJS	+= ../libutility/shutils.o
#CFLAGS  += -I. -I../shared -I../../include -Wall -DDNS_SUPPORT=1
#LIBS += -L../nvram/nvram -L$(INSTALLDIR)/nvram/usr/lib -lnvram
#LIBS	+= $(LIBNVRAM) 
#-L../shared -L$(INSTALLDIR)/shared/usr/lib -lshared
#endif

#ifeq ($(SYMC_OUTBREAK_SUPPORT),1)
#OBJS += oba.o
#CFLAGS  += -I. -I../shared -I../iproute2/include -Wall -I$(SRCBASE)/include -DSYMC_OUTBREAK_SUPPORT=1
#LIBS += -L../nvram -L$(INSTALLDIR)/nvram/usr/lib -lnvram 
#-L../shared -L$(INSTALLDIR)/shared/usr/lib -lshared
#endif

all : dnsmasq

clean :
	rm -f *~ $(OBJS) dnsmasq

.c.o: dnsmasq.h
	$(CC) $(CFLAGS) -O2 -Wall -W -c $*.c 


dnsmasq : $(OBJS) dnsmasq.h config.h
	$(CC) -o $@  $(OBJS) $(LIBS)

install : dnsmasq
	install -d $(BINDIR) -d $(MANDIR)
	install -m 644 dnsmasq.8 $(MANDIR)/man8 
	install -m 755 dnsmasq $(BINDIR)

*.o: $(CY_DEPS)


