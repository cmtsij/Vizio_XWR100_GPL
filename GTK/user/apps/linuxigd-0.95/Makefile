# +++ Gemtek
#PREFIX=/usr
#LIBUPNP_PREFIX=/usr
#LIBIPTC_PREFIX=/usr
#CC=gcc
LIBUPNP_PREFIX = ../libupnp-1.6.0
INCLUDES= -I$(LIBUPNP_PREFIX)/upnp/inc -I$(LIBUPNP_PREFIX)/ixml/inc -I$(LIBUPNP_PREFIX)/threadutil/inc -I../include
LIBS=  -L$(LIBUPNP_PREFIX)/upnp/.libs -L$(LIBUPNP_PREFIX)/ixml/.libs -L$(LIBUPNP_PREFIX)/threadutil/.libs -lpthread -lupnp -lixml -lthreadutil
# --- Gemtek
FILES= main.o gatedevice.o pmlist.o util.o config.o

CFLAGS += -Wall -g -O2

# +++ Gemtek, uncommnet this to enable Getmek Debug message
#CFLAGS += -DGMTK_DEBUG

ifdef HAVE_LIBIPTC
ifdef LIBIPTC_PREFIX
LIBS += -L$(LIBIPTC_PREFIX)/lib
INCLUDES += -I$(LIBIPTC_PREFIX)/include
endif

LIBS += -liptc
INCLUDES += -DHAVE_LIBIPTC
FILES += iptc.o
endif

all: upnpd

upnpd: $(FILES) Makefile
	$(CC) $(CFLAGS) $(FILES) $(LIBS) -o $@
	$(STRIP) upnpd
	@echo "make $@ finished on `date`"

%.o:	%.c Makefile
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

clean:
	rm -f *.o upnpd

install: upnpd
	install -d $(INSTALLDIR)/etc/linuxigd
	install -d $(INSTALLDIR)/bin
#	install -d $(INSTALLDIR)/etc
#	install etc/gatedesc.xml $(INSTALLDIR)/etc/linuxigd/gatedesc.xml
	install etc/gateconnSCPD.xml  $(INSTALLDIR)/etc/linuxigd/gateconnSCPD.xml
	install etc/gateicfgSCPD.xml $(INSTALLDIR)/etc/linuxigd/gateicfgSCPD.xml
# +++ Gemtek, UPnP Certification	
	install etc/Layer3ForwardingSCPD.xml $(INSTALLDIR)/etc/linuxigd/Layer3ForwardingSCPD.xml
	install etc/WANCommonInterfaceConfigSCPD.xml $(INSTALLDIR)/etc/linuxigd/WANCommonInterfaceConfigSCPD.xml
	install etc/WANIPConnectionSCPD.xml $(INSTALLDIR)/etc/linuxigd/WANIPConnectionSCPD.xml
	install etc/WANPPPConnectionSCPD.xml $(INSTALLDIR)/etc/linuxigd/WANPPPConnectionSCPD.xml
# --- Gemtek, UPnP Certification
	install etc/dummy.xml $(INSTALLDIR)/etc/linuxigd/dummy.xml
	install upnpd $(INSTALLDIR)/bin/upnpd
#	install etc/upnpd.conf $(INSTALLDIR)/etc/upnpd.conf
