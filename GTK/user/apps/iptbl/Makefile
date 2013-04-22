#
# filter and port forwarding control Makefile
#

vpath %.h $(SRCBASE)/user/include

CFLAGS	+= -s -O2 -Wall

CFLAGS	+= $(PATHNVRAM)
CFLAGS	+= $(PATHUSER)
CFLAGS  += $(PATHGEM)
CFLAGS  += $(PATHSHARED)
LDFLAGS	+= $(LIBNVRAM)
LDFLAGS += $(LIBUTILILTY)
LDFLAGS += $(LIBSHARED)

IPTBL_OBJ += iptbl.o
IPTBL_OBJ += firewall.o
IPTBL_OBJ += nat.o
IPTBL_OBJ += gemtek.o
IPTBL_OBJ += port_trigger.o
IPTBL_OBJ += port_fw.o
IPTBL_OBJ += single_port_fw.o
IPTBL_OBJ += normal_dmz.o
IPTBL_OBJ += dmz.o
IPTBL_OBJ += remote_management.o
#IPTBL_OBJ += port_filter.o
IPTBL_OBJ += schedule.o
#IPTBL_OBJ += filter_iprange_with_port.o
#IPTBL_OBJ += access_restriction.o
IPTBL_OBJ += filter_ip_mac.o
IPTBL_OBJ += filter_idnt.o
IPTBL_OBJ += lan_rule.o
IPTBL_OBJ += vpn_passthrough.o
IPTBL_OBJ += mac_filter.o
IPTBL_OBJ += ppp_ondemand.o
#IPTBL_OBJ += ftp.o

all: iptbl

clean:
	rm -f iptbl

install: all
	install -d $(INSTALLDIR)/bin
	install iptbl $(INSTALLDIR)/bin/iptbl
	$(STRIP) $(INSTALLDIR)/bin/iptbl
#	$(SSTRIP) $(INSTALLDIR)/bin/iptbl
	
romfs: all
	cp iptbl $(ROMFSDIR)/usr/sbin
	
iptbl: $(IPTBL_OBJ) Makefile
	$(CC) -o $@ $(IPTBL_OBJ) $(LDFLAGS)
	$(STRIP) iptbl
	
$(IPTBL_OBJ): gmtk_config.h