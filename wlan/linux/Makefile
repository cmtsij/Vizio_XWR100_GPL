#
# Makefile for the HAL-based Atheros driver.
#
ifeq ($(obj),)
obj := $(shell pwd)
endif

DEPTH := ..

ATH_BUILD= $(shell pwd)/ath


ifneq ($(strip ${ATH_WLAN_COMBINE}),1)
obj-m += ../common/ ath/ net80211/ ath_hal/
else
obj-m += ath/ ../common/lmac/ath_pktlog/
endif

include $(obj)/$(DEPTH)/linux/Makefile.inc

all: hal_build
	$(MAKE) -C $(KERNELPATH) $(OBJ_OUT) SUBDIRS=$(shell pwd) modules

hal_build:
	cd $(ATH_HAL) && make

install:
	echo "Making Linux Install"
	cd $(ATH_HAL) && make install
	echo IN DIRECTORY `pwd`
	cp ath/*.ko $(MODULEPATH)
ifneq ($(strip ${ATH_WLAN_COMBINE}),1)
	cp net80211/*.ko $(MODULEPATH)
	cp ../common/lmac/ath_dev/*.ko $(MODULEPATH)
	cp ../common/lmac/ratectrl/*.ko $(MODULEPATH)
	cp ../common/lmac/ath_pktlog/*.ko $(MODULEPATH)
endif
ifneq ($(strip ${ATH_SUPPORT_DFS}),0)
	cp ../common/lmac/dfs/*.ko $(MODULEPATH)
endif
#       $(MAKE) INSTALL_MOD_PATH=$(INSTALLROOT) -C $(KERNELPATH) SUBDIRS=$(shell pwd) modules_install
#ifeq ($(DESTDIR),)
#       (export MODULEPATH=${MODULEPATH}; /sbin/depmod -b ${MODULEPATH} -ae)
#endif

clean:
	cd $(ATH_HAL) && make clean
	$(MAKE) -C $(KERNELPATH) SUBDIRS=$(ATH_BUILD) clean
	$(MAKE) -C $(KERNELPATH) SUBDIRS=$(ATH) clean
	$(MAKE) -C $(KERNELPATH) SUBDIRS=$(ATH_RATE) clean
	$(MAKE) -C $(KERNELPATH) SUBDIRS=$(ATH_DFS) clean
	$(MAKE) -C $(KERNELPATH) SUBDIRS=$(ATH_PKTLOG) clean
