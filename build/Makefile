BOARD_TYPE=ap94
ifndef BOARD_TYPE
$(error "You have to define Board Type to use this Makefile")
endif

ifndef BUILD_TYPE
export BUILD_TYPE=jffs2
endif

#
# Include the specific configuration files from the config.boardtype file
# here.  This removes the need to set environmental variables through a
# script before building
#

include scripts/$(BOARD_TYPE)/config.$(BOARD_TYPE)

#
# Put in safety checks here to ensure all required variables are defined in
# the configuration file
#

#ifndef TOOLPREFIX
#$(error "Must specify TOOLPREFIX value")
#endif

ifndef TOOLCHAIN
$(error "Must specify TOOLCHAIN value")
endif

ifndef TOOLARCH
$(error "Must specify TOOLARCH value")
endif

ifndef KERNEL
$(error "Must specify KERNEL value")
endif

ifndef KERNELVER
$(error "Must specify KERNELVER value")
endif

ifndef KERNELTARGET
$(error "Must specify KERNELTARGET value")
endif

ifndef KERNELARCH
$(error "Must specify KERNELARCH value")
endif

ifndef BUSYBOX
$(error "Must specify BUSYBOX value")
endif

ifndef TFTPPATH
export TFTPPATH=$(PWD)/../tftpboot/$(shell whoami)
endif

export JFFS2FILE=$(BOARD_TYPE)$(BUILD_CONFIG)-jffs2
export UBOOTFILE=u-boot$(BUILD_CONFIG).bin
export INFOFILE=vmlinux$(BUILD_CONFIG).info
export KTFILE=$(KERNELTARGET:.bin=$(BUILD_CONFIG).bin)

#
# Other environmental variables that are configured as per the configuration file
# specified above.  These contain all platform specific configuration items.
#

export TOPDIR=$(PWD)/..
export INSTALL_ROOT=$(TOPDIR)/rootfs-$(BOARD_TYPE)$(BUILD_CONFIG).build
$(shell mkdir -p $(INSTALL_ROOT))
$(shell mkdir -p $(INSTALL_ROOT)/sbin)
$(shell mkdir -p $(INSTALL_ROOT)/lib)
$(shell mkdir -p $(INSTALL_ROOT)/etc)
export IMAGE_ROOT=$(TOPDIR)/rootfs-$(BOARD_TYPE)$(BUILD_CONFIG).optbuild
export IMAGEPATH=$(TOPDIR)/images/$(BOARD_TYPE)
temp_BOARD_TYPE = $(strip $(subst fus, , $(BOARD_TYPE)))

export STRIP=$(TOOLPREFIX)strip
export KERNELPATH=$(TOPDIR)/linux/kernels/$(KERNEL)
export MAKEARCH=$(MAKE) ARCH=$(KERNELARCH) CROSS_COMPILE=$(TOOLPREFIX)

export TOOLPATH=$(TOPDIR)/build/$(TOOLCHAIN)/$(TOOLARCH)/
export BOOTLOADERDIR=$(TOPDIR)/boot/redboot

export UBOOTDIR=$(TOPDIR)/boot/u-boot

# Save PATH for later use for compiling host-only tools etc.
export ORIGINAL_PATH:=$(PATH)
# Set PATH so we find target compiler when say "gcc", etc.
# as well as other tools we expect to find.
export PATH:=$(TOPDIR)/build/util:$(TOOLPATH)/bin:$(TOPDIR)/linux:$(TOPDIR)/build:$(BOOTLOADERDIR)/ecos/tools/bin:`pwd`:${PATH}

# madwifi
export HAL=$(TOPDIR)/wlan/madwifi/hal/main
export ATH_PHYERR=$(TOPDIR)/wlan/madwifi/dfs
export ATH_RATE=$(TOPDIR)/wlan/madwifi/ratectrl11n/
export MODULEPATH=$(INSTALL_ROOT)/lib/modules/$(KERNELVER)/net

#
# This is to allow the target file system size to be specified on the command
# line, if desired
#

ifndef TARGETFSSIZE
export TARGETFSSIZE=2621440
endif


ENTRY=`readelf -a vmlinux|grep "Entry"|cut -d":" -f 2`
LDADR=`readelf -a vmlinux|grep "\[ 1\]"|cut -d" " -f 26`

#
# Include the board specific make file
#

include scripts/$(BOARD_TYPE)/Makefile.$(BOARD_TYPE)
#Gemtek Marmot Added 20080820
include $(TOPDIR)/GTK/Makefile.mk
#Gemtek Marmot Added 20080820

ifndef WIRELESSTOOLNAMES
$(warning "Should specify WIRELESSTOOLNAMES value")
## Note: WIRELESSTOOLNAMES can contain more files that we actually have...
## e.g. WIRELESSTOOLNAMES := athstats athstatsclr athdebug 80211stats 80211debug \
		athkey athampdutrc athcwm atrc pktlogconf pktlogdump radartool
WIRELESSTOOLNAMES :=
endif


#
# Common targts
#

ifndef AP_TYPE
# The INSTALL_ROOT is similar but not exactly what appears on the
# target filesystem; it is copied and converted into IMAGE_ROOT
# which is space-optimized.
rootfs_prep:	# this is prep of the INSTALL_ROOT, not the final fs directory
	@echo $(NEWPATH)
	rm -rf $(IMAGE_ROOT)
	rm -rf $(INSTALL_ROOT); mkdir $(INSTALL_ROOT)
	cp -R ../rootfs/common/* $(INSTALL_ROOT)
	cp -Rf ../rootfs/$(BOARD_TYPE)/* $(INSTALL_ROOT)
	chmod 755 $(INSTALL_ROOT)/etc/rc.d/*
	chmod 755 $(INSTALL_ROOT)/etc/ath/*
	chmod 755 $(INSTALL_ROOT)/etc/ath/default/*

else
#
# For the retail AP designs, a new common area is used that contains the
# web interface and supporting files.  Also provides a common fusion based
# filesystem.
#

rootfs_prep:
	@echo $(NEWPATH)
	rm -rf $(IMAGE_ROOT)
	rm -rf $(INSTALL_ROOT); mkdir $(INSTALL_ROOT)
	cp -R ../rootfs/cgiCommon/* $(INSTALL_ROOT)
	cp -rf ../rootfs/cgiCommon/etc/ath.$(AP_TYPE)/* $(INSTALL_ROOT)/etc/ath
	rm -rf $(INSTALL_ROOT)/etc/ath.single $(INSTALL_ROOT)/etc/ath.dual
	cp -rf ../rootfs/cgiCommon/usr/www.$(AP_TYPE)/* $(INSTALL_ROOT)/usr/www
	rm -rf $(INSTALL_ROOT)/usr/www.single
	rm -rf $(INSTALL_ROOT)/usr/www.dual
	cp -Rf ../rootfs/$(BOARD_TYPE)/* $(INSTALL_ROOT)
	chmod 755 $(INSTALL_ROOT)/etc/rc.d/*
	chmod 755 $(INSTALL_ROOT)/etc/ath/*
endif
	
ifeq ($(BUILD_UCLIBC_DEBUG),y)
BUILD_UCLIBC_DEBUG_FILTER = sed -e '/^DODEBUG=n/s/=n/=y/'
else
BUILD_UCLIBC_DEBUG_FILTER = "cat"
endif
toolchain_build: rootfs_prep
	@echo making toolchain_build
	cd $(TOOLCHAIN) && \
	cp -f $(BOARD_TYPE).config .config && \
	rm -f toolchain/uClibc/uClibc.config && \
	cat toolchain/uClibc/$(BOARD_TYPE).config | $(BUILD_UCLIBC_DEBUG_FILTER) >toolchain/uClibc/uClibc.config && \
	$(MAKE)
	#Gemtek Marmot Added 20080825
	cp -a $(TOOLPATH)/gettext-0.11.5/intl/.libs/* $(TOOLPATH)/lib
	#Gemtek Marmot Added 20080825
	# install libraries
	mkdir -p $(INSTALL_ROOT)/lib
	cd $(TOOLCHAIN) && make PREFIX=$(INSTALL_ROOT) install
	# gdbserver to support debugging (if it has been created)
	if [ -f $(TOOLPATH)/root/usr/bin/gdbserver ] ; then \
	    mkdir -p $(INSTALL_ROOT)/usr/bin ; \
	    cp -f $(TOOLPATH)/root/usr/bin/gdbserver $(INSTALL_ROOT)/usr/bin/. ; \
	    fi

check_tftp: image_prep
	mkdir -p $(IMAGEPATH)
	if test -d $(TFTPPATH); then echo $(TFTPPATH) exists; else mkdir ../tftpboot ;mkdir $(TFTPPATH); fi;

image_prep:
	if test -d $(TOPDIR)/images; then echo $(TOPDIR)/images exists; \
	else \
	mkdir $(TOPDIR)/images; \
	fi;
	if test -d $(IMAGEPATH); then echo $(IMAGEPATH) exists; \
	else \
	mkdir $(IMAGEPATH); \
	fi;

kernel_clean: 
	cd $(KERNELPATH) &&  $(MAKEARCH) mrproper 

kernel_info:
	cd $(KERNELPATH) && rm -f $(INFOFILE)
	cd $(KERNELPATH) && echo "entry:"${ENTRY} >> $(INFOFILE)
	cd $(KERNELPATH) && echo "link: 0x"${LDADR} >> $(INFOFILE)
	cp $(KERNELPATH)/$(INFOFILE) $(TFTPPATH)
	cp $(KERNELPATH)/$(INFOFILE) $(IMAGEPATH)

#
# Use different kernel_build rules depending on the type of target
#

ifeq ($(KERNELTARGET), zImage)
kernel_build: image_prep
	@echo " Making Kernel Build Version $(EXTRAVERSION)"
	exit
	cd $(KERNELPATH) && $(MAKEARCH) $(BOARD_TYPE)_defconfig
	cd $(KERNELPATH) && $(MAKEARCH)
	cd $(KERNELPATH)/arch/$(KERNELARCH)/boot && \
	cp $(KERNELTARGET) $(TFTPPATH) && cp $(KERNELTARGET) $(IMAGEPATH)
else	
kernel_build: image_prep
	@echo " Making Kernel Build Version $(EXTRAVERSION)"
	cd $(KERNELPATH) && $(MAKEARCH) $(BOARD_TYPE)$(BUILD_CONFIG)_defconfig
	cd $(KERNELPATH) && $(MAKEARCH)
	cd $(KERNELPATH) && $(MAKEARCH) $(KERNELTARGET)
	cd $(KERNELPATH)/arch/$(KERNELARCH)/boot && \
	cp $(KERNELTARGET) $(TFTPPATH)/$(KTFILE) && \
	cp $(KERNELTARGET) $(IMAGEPATH)/$(KTFILE) && \
	gzip -c $(KERNELTARGET) > $(KERNELTARGET).gz && \
	cp $(KERNELTARGET).gz $(TFTPPATH)/$(KTFILE).gz && \
	cp $(KERNELTARGET).gz $(IMAGEPATH)/$(KTFILE).gz;
endif

redboot_build:
	@echo making redboot...
	cd $(BOOTLOADERDIR) && make $(BOARD_TYPE)_rom
	cp $(BOOTLOADERDIR)/rom_bld/install/bin/redboot.rom $(IMAGEPATH)/redboot.$(BOARD_TYPE).rom
	cd $(BOOTLOADERDIR) && make $(BOARD_TYPE)_ram
	cp $(BOOTLOADERDIR)/ram_bld/install/bin/redboot.bin $(IMAGEPATH)/redboot.$(BOARD_TYPE).bin
	cp $(BOOTLOADERDIR)/ram_bld/install/bin/redboot.srec $(IMAGEPATH)/redboot.$(BOARD_TYPE).srec

uboot:
	cd $(UBOOTDIR) && $(MAKEARCH) mrproper
ifneq ($(BOARD_TYPE), $(temp_BOARD_TYPE))
	echo ====Using $(temp_BOARD_TYPE) config for $(BOARD_TYPE) ===
	cd $(UBOOTDIR) && $(MAKEARCH) $(temp_BOARD_TYPE)_config
else
	cd $(UBOOTDIR) && $(MAKEARCH) $(BOARD_TYPE)_config
endif
	cd $(UBOOTDIR) && $(MAKEARCH) all
	cp $(UBOOTDIR)/u-boot.bin ${IMAGEPATH}/${UBOOTFILE}

busybox_clean:
	@echo Cleaning busybox
	cd ../apps/$(BUSYBOX) && make clean;

busybox_build:
	@echo making busybox
	cd ../apps/$(BUSYBOX); $(MAKE) PREFIX=$(INSTALL_ROOT) uninstall; \
	make clean; \
	cp -f defconfig-$(BOARD_TYPE)$(BUILD_DEBUG) .config; \
	$(MAKE); $(MAKE) PREFIX=$(INSTALL_ROOT) install

# TODO: The following for "wireless_tools" should be moved into the 
# Makefile.target files that invoke it...
wireless_tools:
	@echo making wireless_tools
	if test -n "$(WIRELESSTOOLS)"; then \
	cd ../apps/$(WIRELESSTOOLS); make clean; make; \
	mkdir -p $(INSTALL_ROOT)/sbin; \
	cp -f $(WIRELESSTOOLSLIB) $(INSTALL_ROOT)/lib; \
	cp -f iwpriv $(INSTALL_ROOT)/sbin; \
	cp -f iwconfig $(INSTALL_ROOT)/sbin; \
	cp -f iwlist $(INSTALL_ROOT)/sbin; \
	cd $(TOPDIR)/wlan/madwifi/madwifi/tools && make clean \
	&&  make BUILD_STATIC=$(BUILD_STATIC) &&  \
	cp -f wlanconfig $(INSTALL_ROOT)/sbin && \
	for file in $(WIRELESSTOOLNAMES) ; do \
		if [ -f $$file ] ; then \
			cp -f $$file $(INSTALL_ROOT)/sbin/.; fi; done; \
	fi

diag_build:
	@echo diag_build
	cd $(HAL)/diag && make 
	
hal_build: 
	@echo making hal
	if test -n "$(MADWIFITARGET)"; then \
	cd $(HAL)/linux && make TARGET=$(MADWIFITARGET) clean &&  \
	make TARGET=$(MADWIFITARGET) && make TARGET=$(MADWIFITARGET) release; \
	fi

madwifi_build: hal_build wireless_tools
	@echo making madwifi
	cd $(TOPDIR)/wlan/madwifi/madwifi && \
	$(MAKEARCH) TARGET=$(MADWIFITARGET) clean \
	&&  $(MAKEARCH) TARGET=$(MADWIFITARGET) && \
	$(MAKEARCH) TARGET=$(MADWIFITARGET) install
	$(MAKE) $(BUILD_TYPE)_build
	
ifeq ($(BUILD_WPA2),y)  ####################################
# apps/wpa2 provides a second generation (as compared with apps/wpa)
#       of authentication (including WPS) programs:
#       hostapd, wpa_supplicant, etc.
# 	It installs via $(INSTALL_ROOT).
# 	It depends only on header files from the driver, and linux driver
# 	(madwifi) header files specified by $(MADWIFIPATH)

ifndef MADWIFIPATH
# madwifi includes for wpa2 are found below $(MADWIFIPATH) 
# (e.g. #include <net80211/ieee80211.h>)
export MADWIFIPATH=$(TOPDIR)/wlan/madwifi/madwifi
endif
wpa2: wpa2_clean rootfs_prep
	ln -fs ../include $(TOPDIR)/wlan/linux/include
	cd ../apps/wpa2 && $(MAKE) 	\
		CC=$(TOOLPREFIX)gcc AR=$(TOOLPREFIX)ar LD=$(TOOLPREFIX)ld

wpa2_clean:
	cd ../apps/wpa2 && $(MAKE) clean
clean: wpa2_clean

else  ##################################################
# apps/wpa provides older generation of hostapd, wpa_supplicant, etc.

hostapd: openssl
	if ( test -e ../apps/wpa/hostapd-0.4.8 ) then \
	cd ../apps/wpa/hostapd-0.4.8; cp -f defconfig .config; make clean; make; \
	make PREFIX=$(INSTALL_ROOT)/sbin CONFIG_PATH=$(INSTALL_ROOT)/etc/ath DEFAULT_CFG=$(INSTALL_ROOT)/etc/ath/default install; \
	fi

openssl:
	if (test -e ../apps/wpa/wsc/lib/openssl-0.9.8a ) then \
	chmod -R 755 ../apps/wpa/wsc/lib/openssl-0.9.8a; \
	cd ../apps/wpa/wsc/lib/openssl-0.9.8a; make clean; make; fi

wsc: hostapd
	if (test -e ../apps/wpa/wsc/src/linux ) then \
	chmod -R 755 ../apps/wpa/wsc/src/lunux; \
	cd ../apps/wpa/wsc/src/linux; make clean; \
	make PREFIX=$(INSTALL_ROOT)/sbin CONFIG_PATH=$(INSTALL_ROOT)/etc/ath DEFAULT_CFG=$(INSTALL_ROOT)/etc/ath/default install; \
	fi

wpa_supplicant:
	if (test -e ../apps/wpa/wpa_supplicant-0.5.5 ) then \
	cd ../apps/wpa/wpa_supplicant-0.5.5; cp -f defconfig .config; make clean; \
    make; make PREFIX=$(INSTALL_ROOT)/sbin install; \
	fi

wps_enrollee:    
	if (test -e ../apps/wpa/wps_enrollee) then \
	cd ../apps/wpa/wps_enrollee; make clean; make; \
    make PREFIX=$(INSTALL_ROOT)/sbin install; \
	fi
endif ##################################################

sar:
	@echo making sar
	cd ../apps/sysstat-6.0.1/ && rm -f sysstat.cron.daily && rm -f sysstat.cron.hourly && $(MAKE) CC=$(TOOLPREFIX)gcc 
	cd ../apps/sysstat-6.0.1/ && cp sar $(INSTALL_ROOT)/usr/bin/
	cd ../apps/sysstat-6.0.1/ && cp sadc $(INSTALL_ROOT)/usr/bin/

flash_mac:
	@echo making flash_mac 
	cd ../apps/flash_mac/ && make CC=$(TOOLPREFIX)gcc STRIP=$(TOOLPREFIX)strip && cp read_cfg $(INSTALL_ROOT)/usr/bin/
	

# Optional library optimization (upon IMAGE_ROOT), enable with 
#            BUILD_LIBRARYOPT := y
#
# Library optimization removes unused code from shared libraries.
# Read libraryopt/README-libraryopt.txt for details.
# libraryopt/libopttemp contains the libopt_temp psuedo-target
# which creates $(LIBOPTTEMP) ... we do NOT examine $(INSTALL_ROOT)
# or any other target files at this point and IMAGE_ROOT is not even
# created yet.
LIBOPTTEMP=$(TOPDIR)/build/libopt.temp
#
# Second stage of library optimization is done on the copy of $(INSTALL_ROOT)
# which is $(IMAGE_ROOT) .
# NOTE: $(INSTALL_ROOT) executables must NOT be stripped! 
# that would interfere with debugging and perhaps with library optimization.
#
# The main problems with using library optimizer are:
# -- The build procedure is fragile and can break with any tools
#    or c library upgrade.
# -- Executables not visible at build time but added later may fail
#    unless they have been staticly linked.
#
ifeq ($(BUILD_LIBRARYOPT),y)

include libraryopt/libopttemp.mak
LIBOPTTOOL := $(LIBOPTTEMP)/bin/$(TOOLPREFIX)libopt

else

# If not using library optimizer, remove old junk and don't create new junk.
libopt_temp:
	rm -rf $(LIBOPTTEMP)
LIBOPTTOOL := echo DO NOT libopt

endif

# The IMAGE_ROOT is created from the INSTALL_ROOT by making
# a copy and optimizing the amount of file system space consumed
# through a variety of methods.
# This will be the file system actually installed on target.
image_root: libopt_temp # also depends on all installs done into INSTALL_ROOT!
	@echo making image root $(IMAGE_ROOT)
	rm -rf $(IMAGE_ROOT)
	cp -a $(INSTALL_ROOT) $(IMAGE_ROOT)
	# Remove unwanted files
	for file in $(INSTALL_BLACKLIST) ; do rm -rf $(IMAGE_ROOT)/$$file ; done
	rm -rf $(IMAGE_ROOT)/include
	rm -rf $(IMAGE_ROOT)/man
	rm -rf $(IMAGE_ROOT)/lib/*.a
	# could be:  rm -f $(IMAGE_ROOT)/usr/bin/gdbserver
	# Apply library optimizer (optional)
	#$(LIBOPTTOOL) $(IMAGE_ROOT)
	# Now we can strip executables (also strip libraries if needed)
	cd $(IMAGE_ROOT)/lib && $(STRIP) *.so
	find $(IMAGE_ROOT)/sbin -type f -perm -u+x -exec $(STRIP) '{}' ';'
	find $(IMAGE_ROOT)/bin -type f -perm -u+x -exec $(STRIP) '{}' ';'
	find $(IMAGE_ROOT)/usr/bin -type f -perm -u+x -exec $(STRIP) '{}' ';'
	# Some additional space savings is gained by using tar/gzip compression
	# on wireless tools, which get unpacked by rcS script into /tmp
	# ram disk... perhaps a waste of ram however.
	# The amount of flash space thus saved is not large.
	echo Warnings from tar about missing files are normal for some targets.
	if [ -n "$(WIRELESSTOOLNAMES)" ] ; then cd $(IMAGE_ROOT)/sbin && \
	    tar --ignore-failed-read -czf debug.tgz $(WIRELESSTOOLNAMES) && \
	    rm -f $(WIRELESSTOOLNAMES) && \
	    for tool in $(WIRELESSTOOLNAMES) ; do ln -s /tmp/tools/$$tool .; done; \
	    fi
	@echo DONE BUILDING image_root

ram_build: image_root
	@echo making ramfs
	cd util && \
        genext2fs -b 8192 -i 512 -D ../scripts/$(BOARD_TYPE)/dev.txt -d $(IMAGE_ROOT) $(BOARD_TYPE)-ramdisk
	cd util && gzip --best -f $(BOARD_TYPE)-ramdisk
	cd util && cp $(BOARD_TYPE)-ramdisk.gz $(TFTPPATH) 
	cd util && cp $(BOARD_TYPE)-ramdisk.gz $(IMAGEPATH) 

#uimage:	kernel_build
uimage:	gemtek_prepare kernel_build
	@echo making uImage
	cd util && mkuImage.sh $(UBOOTDIR)/tools $(KERNELPATH) $(BUILD_CONFIG)

jffs2_build: image_root
	@echo making jffs2
	find $(IMAGE_ROOT) -name .svn | xargs rm -rf ; 
	cd $(IMAGEPATH) && \
	$(TOPDIR)/build/util/mkfs.jffs2 --root=$(IMAGE_ROOT) --eraseblock=0x10000 -b -D $(TOPDIR)/build/scripts/$(BOARD_TYPE)/dev.txt --squash -o $(JFFS2FILE) --pad=$(TARGETFSSIZE)
	# The file system image is installed in ? /tftpboot/<username>
	# for ? convenience ?
	cp $(IMAGEPATH)/$(JFFS2FILE) $(TFTPPATH)

clean: kernel_clean busybox_clean gemtek_clean
	@echo CAUTION THIS WILL NOT CLEAN EVERYTHING
	rm -rf $(TOOLPATH)
	rm -rf $(TOPDIR)/build/$(TOOLCHAIN)/toolchain_$(TOOLARCH)/
	rm -rf $(TOPDIR)/images
	rm -rf $(IMAGE_ROOT) 
	rm -rf $(INSTALL_ROOT)
	rm -rf $(LIBOPTTEMP)
	rm -rf $(TFTPPATH)
	rm -f $(TOPDIR)/wlan/include/include
	rm -f $(TOPDIR)/wlan/linux/include
	cd $(TOPDIR)/apps;find -name .*.cmd|xargs rm; \
	cd $(TOPDIR)/GTK;find -name .*.cmd|xargs rm; \
	cd $(TOPDIR)/linux;find -name .*.cmd|xargs rm; \
	cd $(TOPDIR)/wlan;find -name .*.cmd|xargs rm
	@echo CAUTION THIS WILL NOT CLEAN EVERYTHING



