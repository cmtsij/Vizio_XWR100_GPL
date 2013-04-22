#
# (C) Copyright 2008 Gemtek, Inc.
# All rights reserved.
#
#CFLAGS	+=	-DCWA
GTK_ROOT = $(INSTALL_ROOT)
#GTK_ROOT = $(TOPDIR)/GTK/gtk_rootfs
export MODEL = XWR100
export SRCBASE := $(TOPDIR)/GTK
export KERNEL_DIR := $(KERNELPATH)
export CFLAGS += -I$(KERNELPATH)/include
export PATHUSER := -I. -I$(shell (pwd -P))/user
export PATHNVRAM := -I$(shell (pwd -P))/user/apps/nvram/include
export PATHSHARED		:= 
export PATHGEM		:= -I$(shell (pwd -P))/user/include
export PATHWIRELESS := -I$(shell (pwd -P))/../wlan/linux
export LIBNVRAM := -L$(shell (pwd -P))/user/apps/nvram/bin -lnvram
export LIBUTILILTY	:= -L$(shell (pwd -P))/user/apps/libutility -lutility
export CC = $(TOOLPREFIX)gcc
export LD = $(TOOLPREFIX)ld
export STRIP=$(TOOLPREFIX)strip
export AR = $(TOOLPREFIX)ar
export RANLIB = $(TOOLPREFIX)ranlib

gpio: 
	cd ./user/apps/GPIO
	$(MAKEARCH) -C $(KERNELPATH) SUBDIRS=$(SRCBASE)/user/apps/GPIO modules
	cp ./user/apps/GPIO/gpio_mod.ko $(MODULEPATH)
	cp ./user/apps/GPIO/led_wsc_mod.ko $(MODULEPATH)

nvram:
	cd ./user/apps/nvram;make; \
	cp -f bin/nvram $(GTK_ROOT)/sbin;\
	cp -f bin/nvserv $(GTK_ROOT)/sbin;\
	cp -f bin/libnvram.so $(GTK_ROOT)/lib/libnvram.so;\
	cd $(GTK_ROOT)/dev/ && ln -sf /tmp/dev_log log;
	#$(STRIP) bin/nvram;\
	#$(STRIP) bin/nvserv;\
	#$(STRIP) bin/libnvram.so;\

libutility:
	cd ./user/apps/libutility;make; \
	cp -f libutility.so $(GTK_ROOT)/lib/libutility.so;
	#$(STRIP) libutility.so;\
	
rc:
	cd ./user/apps/rc;make; \
	cp -f rc $(GTK_ROOT)/sbin/sysinit;\
	cp -f udhcpd_dumplease $(GTK_ROOT)/sbin;\
#	cp -f $(KERNELPATH)/net/ipv4/netfilter/*.ko $(MODULEPATH)/../
	cp -f $(KERNELPATH)/drivers/usb/host/ehci-hcd.ko $(MODULEPATH)/../
	cp -f $(KERNELPATH)/drivers/usb/host/ohci-hcd.ko $(MODULEPATH)/../
	cd $(GTK_ROOT)/sbin/;\
	ln -sf sysinit stats;\
	ln -sf sysinit wanup.sh;\
	ln -sf sysinit subwan_up.sh;\
	ln -sf sysinit udhcpc.sh;\
	ln -sf sysinit init;\
	ln -sf sysinit GenerateConfigForHostapd_ForVISTA; 
	cd $(GTK_ROOT); rm -rf var;\
	ln -sf /tmp/var var;
	#$(STRIP) rc;\
	#$(STRIP) udhcpd_dumplease;\

ntpclient:
	cd ./user/apps/ntpclient; make; \
	cp -f ./exeNtp $(GTK_ROOT)/bin/;
	#$(STRIP) exeNtp;\ 
	
ddns_final:
	cd ./user/apps/ddns-1.4; make; \
	cp -f ./updatedd $(GTK_ROOT)/bin/;
	#$(STRIP) updatedd;\
	
tzoupdate:
	cd ./user/apps/tzoupdate-1.11; make; \
	cp -f ./tzoupdate $(GTK_ROOT)/bin/;
	#$(STRIP) tzoupdate;\
	
inadyn:
	cd ./user/apps/inadyn-1.96.2; make; \
	cp -f ./inadyn $(GTK_ROOT)/bin/;
	#$(STRIP) inadyn;\
	
dnrd:
	cd ./user/apps/dnrd-2.20.3; make; \
	cp -f src/dnrd $(GTK_ROOT)/bin/;
	#$(STRIP) src/dnrd;\
	
iptables:
	cd ./user/apps/iptables-1.4.0rc1; make dep ;make ;\
	cp -f ./iptables $(GTK_ROOT)/bin/;	\
	cd $(GTK_ROOT)/bin/; \
	ln -sf iptables gtk_iptables_batch;
	#$(STRIP) iptables;\
	
zebra:
	cd ./user/apps/zebra; make; \
	cp -f ./zebra/zebra $(GTK_ROOT)/bin/; \
	cp -f ./ripd/ripd $(GTK_ROOT)/bin/;
	#$(STRIP) ./zebra/zebra;\
	#$(STRIP) ./ripd/ripd;\

zebrip:
	cd ./user/apps/zebrip; make; \
	cp -f ./zebrip $(GTK_ROOT)/bin/;
	#$(STRIP) zebrip;\

igmp:
	cd ./user/apps/igmp; make; \
	cp -f ./igmp $(GTK_ROOT)/bin/;
	#$(STRIP) igmp;\

iproute2:
	cd ./user/apps/iproute2; make; \
	cp -f ./tc/tc $(GTK_ROOT)/bin/;
	cp -f $(TOOLPATH)/lib/libresolv-0.9.28.so $(GTK_ROOT)/lib/libresolv.so.0;
	#$(STRIP) tc/tc;\

iptbl:
	cd ./user/apps/iptbl; make; \
	cp -f ./iptbl $(GTK_ROOT)/sbin;
	#$(STRIP) iptbl;\
	
udhcpc:
	cd ./user/apps/udhcpc-0.9.8;make;\
	cp -f udhcpc $(GTK_ROOT)/bin;
	#$(STRIP) udhcpc;\

hbeat:
	cd ./user/apps/bpalogin-2.0.2;make;\
	cp -f bpalogin $(GTK_ROOT)/bin;
	#$(STRIP) bpalogin;\

udhcpd:
	cd ./user/apps/udhcpd-0.9.8;make;\
	cp -f udhcpd $(GTK_ROOT)/bin;\
	cp -f dumpleases $(GTK_ROOT)/bin;
	#$(STRIP) udhcpd; \

udhcpdset:
	cd ./user/apps/udhcpdset;make;\
	cp -f udhcpdset $(GTK_ROOT)/sbin;
	#$(STRIP) udhcpdset;\

ppp:
	cd ./user/apps/ppp/pppoecd;make;\
	cp -f pppoecd $(GTK_ROOT)/sbin;
	#$(STRIP) pppoecd;\

l2tpd:
	cd ./user/apps/l2tpd;make; \
	cp -f ./l2tpd $(GTK_ROOT)/sbin;
	#$(STRIP) l2tpd;\

pptp:
	cd ./user/apps/pptp;make;\
	cp -f pptp $(GTK_ROOT)/sbin;\
	cp -f $(TOOLPATH)/lib/libutil-0.9.28.so $(GTK_ROOT)/lib/libutil.so.0;\
	#$(STRIP) pptp;\
	#$(STRIP) $(GTK_ROOT)/lib/libutil.so.0;

httpd:
	cd ./user/apps/httpd; make; \
	cp -f ./httpd $(GTK_ROOT)/bin/; \
	cd $(GTK_ROOT)/bin/; \
	ln -sf httpd rmt_httpd; \
	cp -f $(TOOLPATH)/lib/libm-0.9.28.so $(GTK_ROOT)/lib/libm.so.0; \
	cp -f $(TOOLPATH)/lib/libpthread-0.9.28.so $(GTK_ROOT)/lib/libpthread.so.0;
	#$(STRIP) httpd;\

matrixtunnel:
	cd ./user/apps/matrixtunnel; make; \
	cp ./matrixtunnel $(GTK_ROOT)/bin; \
	cp ./matrixssl/src/libmatrixssl.so $(GTK_ROOT)/lib; \
	cp ./matrixssl/certificate-Gemtek/privkeySrv.pem $(GTK_ROOT)/etc/privkeySrv.pem; \
	cp ./matrixssl/certificate-Gemtek/certSrv.pem $(GTK_ROOT)/etc/certSrv.pem;
	#$(STRIP) matrixtunnel;\
	#$(STRIP) ./matrixssl/src/libmatrixssl.so;\

md5:
	cd ./user/apps/md5 ; make ;\
	cp -f ./md5 $(GTK_ROOT)/bin;
	#$(STRIP) md5;\

conn_log:
	cd ./user/apps/conn_log; make; \
	cp -f ./conn_log $(GTK_ROOT)/bin;
	#$(STRIP) conn_log;\

device_check:
	cd ./user/apps/device_check; make; \
	cp -f ./device_check $(GTK_ROOT)/bin;
	
vizio_qos:
	cd ./user/apps/vizio_qos; make; \
	cp -f ./vizio_qos $(GTK_ROOT)/bin;

www:
	rm -rf $(GTK_ROOT)/www; \
	cp -r ./user/apps/www/VIZIO/ $(GTK_ROOT)/www;\
	cd $(GTK_ROOT)/www; touch /tmp/config.bin;ln -sf /tmp/config.bin config.bin; \
	cd $(GTK_ROOT)/www; touch /var/log/dhcpc.log;ln -sf /var/log/dhcpc.log dhcpc.log;\
	cd $(GTK_ROOT)/www; touch /var/log/security.log;ln -sf /var/log/security.log security.log;\
	cd $(GTK_ROOT)/www; touch /var/log/outgoing.log;ln -sf /var/log/outgoing.log outgoing.log;\
	cd $(GTK_ROOT)/www; touch /var/log/incoming.log;ln -sf /var/log/incoming.log incoming.log;
	cd $(GTK_ROOT)/www/language; ln -sf /language newlang;

tftpd:
	cd ./user/apps/tftpd-fw/; make;\
	cp -f ./tftpd $(GTK_ROOT)/bin;
	#$(STRIP) tftpd;\

dslmode:
	cd ./user/apps/dslmode/;make; \
	cp -f ./dslmode $(GTK_ROOT)/sbin/;

easytest:
	cd ./user/apps/easytest/;make; \
	cp -f ./easytest $(GTK_ROOT)/sbin/;

#hnapssdp:
#	cd ./user/apps/hnapssdp/;make; \
#	cp -f ./hnapssdp $(GTK_ROOT)/sbin/;
	
fwupgrade:
	cd ./user/apps/fwupgrade;make; \
	cp -f fwupgrade $(GTK_ROOT)/bin;
	#$(STRIP) fwupgrade;\
	
memdbg:
	cd ./user/apps/memdbg;make; \
	cp -f get_proc_mem $(GTK_ROOT)/bin;
	#$(STRIP) get_proc_mem;\
	
busybox:
	cd ./user/apps/busybox;make; \
	make STRIPTOOL=$(STRIP) PREFIX=$(GTK_ROOT) install;
	#$(STRIP) busybox;\
	
birdge:
	cd ./user/apps/birdge;make; \
	cp -f brctl/brctl $(GTK_ROOT)/bin;
	#$(STRIP) brctl/brctl;\
	
vlan:
	cd ./user/apps/vlan;make; \
	cp -f vconfig $(GTK_ROOT)/bin;
	#$(STRIP) vconfig;\

httptools:
	cd ./user/apps/httptools;make; \
	cd ./user/apps/httptools; cp -f ./downsize $(GTK_ROOT)/www/;
	cd  $(GTK_ROOT)/www/ &&find .  >list; \
	cd  $(GTK_ROOT)/www/ &&./downsize -f list; \
	rm -rf $(GTK_ROOT)/www/downsize;\
	rm -rf $(GTK_ROOT)/www/Makefile;
	
read_fw:
	cd ./user/apps/read_fw;make; \
	cp -f read_fw $(GTK_ROOT)/bin;
	#$(STRIP) read_fw;\
	
# Vista Related Applications

lltdset:
	cd ./user/apps/lltdset; make; \
	cp -f ./lltdset $(GTK_ROOT)/bin/;
	#$(STRIP) lltdset; \
	
ms-lltd:
	rm -f $(GTK_ROOT)/bin/lld2d ; \
	rm -f $(GTK_ROOT)/etc/*.ico ; \
	rm -f $(GTK_ROOT)/etc/lld2d.conf ; \
	cd ./user/apps/ms-lltd; make; \
	cp -f ./lld2d $(GTK_ROOT)/bin/; \
	cp -f ./*.ico $(GTK_ROOT)/etc/; \
	cp -f ./lld2d.conf $(GTK_ROOT)/etc/;
	#$(STRIP) lld2d; \
	
libupnp:
	cd ./user/apps/libupnp-1.6.0; make clean; make; \
	cp -f ./upnp/.libs/libupnp.so.3 $(GTK_ROOT)/lib/; \
	cp -f ./threadutil/.libs/libthreadutil.so.2 $(GTK_ROOT)/lib/; \
	cp -f ./ixml/.libs/libixml.so.2 $(GTK_ROOT)/lib/;

linuxigd:
	cd ./user/apps/linuxigd-0.95; make; \
	mkdir -p $(GTK_ROOT)/etc/linuxigd/; \
	cp -f ./etc/gateconnSCPD.xml $(GTK_ROOT)/etc/linuxigd/; \
	cp -f ./etc/gateicfgSCPD.xml $(GTK_ROOT)/etc/linuxigd/; \
	cp -f ./etc/Layer3ForwardingSCPD.xml $(GTK_ROOT)/etc/linuxigd/; \
	cp -f ./etc/WANCommonInterfaceConfigSCPD.xml $(GTK_ROOT)/etc/linuxigd/; \
	cp -f ./etc/WANIPConnectionSCPD.xml $(GTK_ROOT)/etc/linuxigd/; \
	cp -f ./etc/WANPPPConnectionSCPD.xml $(GTK_ROOT)/etc/linuxigd/; \
	cp -f ./etc/dummy.xml $(GTK_ROOT)/etc/linuxigd/; \
	cp -f ./upnpd $(GTK_ROOT)/bin/; 
	
linuxigdset:
	cd ./user/apps/linuxigd-0.95_set; make; \
	cp -f ./linuxigdset $(GTK_ROOT)/bin/; \
	
radvd:
	cd ./user/apps/radvd-0.9.1; make; \
	cp -f ./radvd $(GTK_ROOT)/bin/; \
	
radvdset:
	cd ./user/apps/radvdset; make; \
	cp -f ./radvdset $(GTK_ROOT)/bin/; \
	
flex:
	cd ./user/apps/flex-2.5.33; make;

dnsmasq:
	cd ./user/apps/dnsmasq; make; \
	cp -f ./dnsmasq $(GTK_ROOT)/bin/; 

#mdk_client:
#	cd ./user/apps/mdk_client; \
#	cp -f ./mdk_client.out $(GTK_ROOT)/sbin/; \
#	cp -f ./art.ko $(MODULEPATH)/art.ko; 
	
squashfs-tools:
	cd ./user/tools/squashfs-tools3.0 && $(MAKE)

image_root: 
	@echo making image root 
	# Remove unwanted files
	rm -rf $(GTK_ROOT)/include
	rm -rf $(GTK_ROOT)/man
	rm -rf $(GTK_ROOT)/lib/*.a
	# could be:  rm -f $(IMAGE_ROOT)/usr/bin/gdbserver
	# Apply library optimizer (optional)
	#$(LIBOPTTOOL) $(GTK_ROOT)
	# Now we can strip executables (also strip libraries if needed)
	cd $(GTK_ROOT)/lib && $(STRIP) *.so
	find $(GTK_ROOT)/sbin -type f -perm -u+x -exec $(STRIP) '{}' ';'
	find $(GTK_ROOT)/bin -type f -perm -u+x -exec $(STRIP) '{}' ';'
	find $(GTK_ROOT)/usr/bin -type f -perm -u+x -exec $(STRIP) '{}' ';'
	@echo DONE BUILDING $(GTK_ROOT)
	
release: squashfs-tools
	rm -rf ./user/rootdir
	cp -rf $(IMAGE_ROOT) ./user/rootdir 
	cd ./user/rootdir; find . -name .svn | xargs rm -rf
	rm -f ./user/image/rootfs.squash
	./user/tools/squashfs-tools3.0/mksquashfs-lzma ./user/rootdir ./user/image/rootfs.squash -be -all-root  -noappend
	cp $(TFTPPATH)/vmlinux.lzma.uImage ./user/image/vmlinux.lzma.uImage
#	cp $(TFTPPATH)/vmlinux.gz.uImage ./user/image/vmlinux.gz.uImage
	#cat $(TFTPPATH)/vmlinuxmin.info
	$(MAKE) generator

generator:
#	cd ./user/tools/FW-Generator;gcc -c FW-Generator.c -I../../include;
	cp ./user/tools/FW-Generator/FW-Generator.o ./user/image/
	cd ./user/image;rm -f FW-Generator;gcc FW-Generator.o -o FW-Generator;./FW-Generator vmlinux.lzma.uImage rootfs.squash;

#make langpackgen LANGPACK=en
langpackgen:
	cd ./user/tools/LangPackGen;rm -f Lang-Generator Lang-Generator.o;gcc Lang-Generator.c -o Lang-Generator -I../include;
	rm -f user/image/$(LANGPACK).bin;rm -rf ./tmplang
	mkdir ./tmplang	
	cp ./user/apps/www/VENDOR/language/$(LANGPACK)/* ./tmplang
	./user/tools/mksquashfs ./tmplang ./user/image/$(LANGPACK).bin -be -all-root
	./user/tools/LangPackGen/Lang-Generator $(LANGPACK).bin;
	chmod 777 ./user/image/LangPack_$(LANGPACK).bin

hotplug2:
	cd ./user/apps/hotplug2;make; \
	cp -f ./hotplug2 $(GTK_ROOT)/sbin

usb_storage_hotplug:
	cd ./user/apps/usb_storage_hotplug;make all; \
	cp -f ./usb_storage_hotplug $(GTK_ROOT)/sbin

udev:
	cd ./user/apps/udev; make; \
	cp -f ./etc/udev/udev.conf $(GTK_ROOT)/etc/udev;\
	cp -f ./udevtrigger $(GTK_ROOT)/sbin

disktype:
	cd ./user/apps/disktype-9; make; \
	cp -f ./disktype $(GTK_ROOT)/sbin

samba-3.0.2:
	cd ./user/apps/samba-3.0.2;make; \
	mkdir $(GTK_ROOT)/usr/local; \
	mkdir $(GTK_ROOT)/usr/local/samba; \
	mkdir $(GTK_ROOT)/usr/local/samba/bin; \
	mkdir $(GTK_ROOT)/usr/local/samba/sbin; \
	mkdir $(GTK_ROOT)/usr/local/samba/lib; \
	mkdir $(GTK_ROOT)/usr/local/samba/var; \
	cp -f ./source/bin/libbigballofmud.so $(GTK_ROOT)/lib; \
	cp -f ./source/bin/nmbd $(GTK_ROOT)/usr/local/samba/sbin/nmbd; \
	cp -f ./source/bin/smbd $(GTK_ROOT)/usr/local/samba/sbin/smbd; \
	cp -f ./source/bin/smbpasswd $(GTK_ROOT)/usr/local/samba/bin/smbpasswd; \
	cd $(GTK_ROOT)/usr/local/samba/lib && ln -sf /tmp/samba/lib/smb.conf smb.conf; \
	cd $(GTK_ROOT)/lib && ln -snf libbigballofmud.so libbigballofmud.so.0; \
	cd $(GTK_ROOT)/usr/local/samba && ln -sf /tmp/samba/private private; \
	cd $(GTK_ROOT)/usr/local/samba/var && ln -sf /tmp/var/lock locks; \
	cd $(GTK_ROOT)/etc && ln -sf /tmp/samba/lib/smb.conf smb.conf

samba-3.0.2_set:
	cd ./user/apps/samba-3.0.2_set;make all; \
	cp -f ./sambaset $(GTK_ROOT)/bin; \
	ln -sf /tmp/secrets.tdb $(GTK_ROOT)/etc/secrets.tdb

libiconv:
	cd ./user/apps/libiconv-1.12; \
	./configure CC=$(TOOLPREFIX)gcc --host=mips-linux-uclibc --prefix=$(PWD)/lib/.lib; \
	make; \
	cp -f ./lib/.libs/libiconv.so.2.4.0 $(GTK_ROOT)/lib; \
	cp -f ./lib/.libs/libiconv.so.2 $(GTK_ROOT)/lib; \
	cp -f ./lib/.libs/libiconv.so $(GTK_ROOT)/lib

libntfs:
	cd ./user/apps/libiconv-1.12; \
	./configure CC=$(TOOLPREFIX)gcc --host=mips-linux-uclibc --prefix=$(PWD)/lib/.lib; \
	make; \
	cp -f ./lib/.libs/libiconv.so.2.4.0 $(GTK_ROOT)/lib; \
	cp -f ./lib/.libs/libiconv.so.2 $(GTK_ROOT)/lib; \
	cp -f ./lib/.libs/libiconv.so $(GTK_ROOT)/lib

libntfs-3g:
	cd ./user/apps/libntfs-3g; make shared; \
	cp -f ./libntfs-3g.so.26 $(GTK_ROOT)/lib

fuse:
	cd ./user/apps/fuse-2.7.4; \
	./configure CC=$(TOOLPREFIX)gcc --host=mips-linux-uclibc --enable-kernel-module --with-kernel=$(KERNEL_DIR); \
	make; \
	cp -f ./kernel/fuse.ko $(GTK_ROOT)/lib/modules/2.6.15

ntfs-3g:
	cd ./user/apps/ntfs-3g; \
	./configure CC=$(TOOLPREFIX)gcc --host=mips-linux-uclibc --disable-library; \
	make; \
#	$(STRIP) ./src/ntfs-3g; \
	cp -f ./src/ntfs-3g $(GTK_ROOT)/bin

dlna-GTK-DMS:
	cd ./user/apps/dlna-GTK-DMS; make; \
	cp -f ./DMS $(GTK_ROOT)/bin/

DMS_set:
	cd ./user/apps/DMS_set; make; \
	cp -f ./DMS_set $(GTK_ROOT)/bin/

all:
	$(MAKE) gpio
#	$(MAKE) vlan
	$(MAKE) nvram
	$(MAKE) libutility
	$(MAKE) ntpclient
	$(MAKE) ddns_final
	$(MAKE) iptables
#	$(MAKE) zebra
#	$(MAKE) zebrip
	$(MAKE) igmp
	$(MAKE) iproute2
	$(MAKE) iptbl
	$(MAKE) rc
	$(MAKE) fwupgrade
	$(MAKE) tftpd
	$(MAKE) httpd
	$(MAKE) matrixtunnel
	$(MAKE) md5
	$(MAKE) conn_log
	$(MAKE) udhcpd
	$(MAKE) udhcpdset
	$(MAKE) udhcpc
	$(MAKE) ppp
#	$(MAKE) pptp
#	$(MAKE) l2tpd
#	$(MAKE) hbeat
#	$(MAKE) dslmode
	$(MAKE) easytest
#	$(MAKE) hnapssdp
#	$(MAKE) read_fw
	$(MAKE) www
#	$(MAKE) mdk_client
#Vista Related Applications
	$(MAKE) ms-lltd
	$(MAKE) lltdset
	$(MAKE) flex
	$(MAKE) libupnp
	$(MAKE) linuxigd
	$(MAKE) linuxigdset
	$(MAKE) radvd
	$(MAKE) radvdset
	$(MAKE) dnsmasq
#Vista Related Applications
	$(MAKE) httptools
	$(MAKE) device_check
	$(MAKE) vizio_qos
	$(MAKE) udev
	$(MAKE) hotplug2
	$(MAKE) disktype
	$(MAKE) usb_storage_hotplug
	$(MAKE) samba-3.0.2
	$(MAKE) samba-3.0.2_set
#	$(MAKE) libiconv
#	$(MAKE) libntfs-3g
#	$(MAKE) fuse
#	$(MAKE) ntfs-3g
	$(MAKE) dlna-GTK-DMS
	$(MAKE) DMS_set
	@echo "LSDK Build Complete"

clean:
	@echo "make clean of Gemtek Applications"
	rm -rf ./user/rootdir;
	rm -rf ./user/image/*;
	cd ./user/apps/nvram; make clean;
	cd ./user/apps/libutility; make clean;
	cd ./user/apps/ntpclient; make clean;
	cd ./user/apps/iptables-1.4.0rc1; make clean;
#	cd ./user/apps/zebra; make clean;
#	cd ./user/apps/zebrip; make clean;
	cd ./user/apps/ddns-1.4; make clean;
	cd ./user/apps/igmp; make clean;
	cd ./user/apps/iproute2; make clean;
	cd ./user/apps/iptbl; make clean;
	cd ./user/apps/rc; make clean;
	cd ./user/apps/fwupgrade; make clean;
	cd ./user/apps/tftpd-fw; make clean;
	cd ./user/apps/httpd; make clean;
	cd ./user/apps/matrixtunnel; make clean;
	cd ./user/apps/md5; make clean;
	cd ./user/apps/conn_log; make clean;
	cd ./user/apps/udhcpc-0.9.8; make clean;
	cd ./user/apps/udhcpd-0.9.8; make clean;
	cd ./user/apps/udhcpdset; make clean;
	cd ./user/apps/ppp/pppoecd; make clean;
#	cd ./user/apps/pptp; make clean;
#	cd ./user/apps/l2tpd; make clean;
#	cd ./user/apps/bpalogin-2.0.2; make clean;
	cd ./user/apps/www; make clean;
#	cd ./user/apps/dslmode; make clean;
	cd ./user/apps/easytest; make clean;
#	cd ./user/apps/read_fw; make clean;
#	cd ./user/apps/memdbg; make clean;
	cd ./user/apps/httptools; make clean;
	cd ./user/apps/ms-lltd; make clean;
	cd ./user/apps/lltdset; make clean;
	cd ./user/apps/libupnp-1.6.0; make clean;
	cd ./user/apps/linuxigd-0.95; make clean;
	cd ./user/apps/linuxigd-0.95_set; make clean;
	cd ./user/apps/radvd-0.9.1; make clean;
	cd ./user/apps/radvdset; make clean;
	cd ./user/apps/flex-2.5.33; make clean;
	cd ./user/apps/dnsmasq; make clean;
#	cd ./user/apps/vlan; make clean;
	cd ./user/apps/device_check; make clean;
	cd ./user/apps/vizio_qos; make clean;
	cd ./user/apps/udev; make clean;
	cd ./user/apps/hotplug2; make clean;
	cd ./user/apps/disktype-9; make clean;
	cd ./user/apps/usb_storage_hotplug; make clean;
	cd ./user/apps/samba-3.0.2; make clean;
	cd ./user/apps/samba-3.0.2_set; make clean;
#	cd ./user/apps/libiconv-1.12; make clean;
#	cd ./user/apps/libntfs-3g; make clean;
#	cd ./user/apps/ntfs-3g; make clean;
	cd ./user/apps/dlna-GTK-DMS; make clean;
	cd ./user/apps/DMS_set; make clean;
	cd ./user/tools/squashfs-tools3.0; make clean;
	
	
