CFLAGS	+= -s -O2
CFLAGS	+= $(PATHNVRAM)
CFLAGS	+= $(PATHUSER)
CFLAGS	+= $(PATHGEM)
CFLAGS	+= $(PATHSHARED)
CFLAGS	+= $(PATHWIRELESS)

LDFLAGS += $(LIBUTILILTY)
LDFLAGS	+= $(LIBNVRAM)
LDFLAGS += $(LIBSHARED)

OBJS := rc.o init.o interface.o dhcpc.o ppp.o optimization.o wan_connect.o 
#gpio_control.o convert.o 
# dhcpc.o ppp.o convert.o optimization.o gpio_control.o wan_connect.o
WANOBJS	= wan_connect.o 

UDHCPD_DUMPLEASE_OBJS	:= udhcpd_dumplease.o

vpath %.h $(SRCBASE)/user/include

all: rc udhcpd_dumplease

clean:
	rm -f rc .*.depend wan_connect udhcpd_dumplease

install:
	install -d $(INSTALLDIR)/sbin
	install rc $(INSTALLDIR)/sbin
	$(STRIP) $(INSTALLDIR)/sbin/rc
	cd $(INSTALLDIR)/sbin && ln -sf rc init
	cd $(INSTALLDIR)/sbin && ln -sf rc erase
	cd $(INSTALLDIR)/sbin && ln -sf rc write
	cd $(INSTALLDIR)/sbin && ln -sf rc fw_crc
	cd $(INSTALLDIR)/sbin && ln -sf rc cfe
	cd $(INSTALLDIR)/sbin && ln -sf rc stats
	cd $(INSTALLDIR)/sbin && ln -sf rc wanup.sh
	cd $(INSTALLDIR)/sbin && ln -sf rc subwan_up.sh	
	cd $(INSTALLDIR)/sbin && ln -sf rc udhcpc.sh
	install -D udhcpd_dumplease $(INSTALLDIR)/bin/udhcpd_dumplease	

rc: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

psmon: $(PSOBJS) 
	$(CC) $(CFLAGS) -o $@ $(PSOBJS) $(LDFLAGS)
	$(STRIP) psmon	

udhcpd_dumplease: $(UDHCPD_DUMPLEASE_OBJS)
	$(CC) $(CFLAGS) -o $@ $(UDHCPD_DUMPLEASE_OBJS) $(LDFLAGS)

rc.o : vendor_default.h rc_action.h

.PHONY: all
