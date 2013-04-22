# $Id: //depot/sw/releases/7.3_AP/wlan/linux/ath_hal/Makefile#2 $

#
# Makefile for the Atheros HAL Module.
#
# NB: we can't support MODVERSIONS because it requires building
#     from source which may not be available
#
ifeq ($(obj),)
obj := .
endif

DEPTH := ../..

obj-m		+= ath_hal.o
ath_hal-objs	:= ah_osdep.o hal.o
ifeq ($(findstring 2.6,$(KERNELRELEASE)),)
export-objs	:= ah_osdep.o
list-multi	:= ath_hal.o
endif

# If building into kernel

ifneq ($(MAKING_MODULES),1)
obj-$(CONFIG_ATHEROS_HAL) := ah_osdep.o hal.o
ifeq ($(findstring 2.6,$(KERNELRELEASE)),)
O_TARGET	:= ath_hal.o
endif
endif

include $(obj)/${DEPTH}/linux/Makefile.inc

UUDECODE?=uudecode

INCS+=	-I${HAL} -I${HAL}/${OS} -I${obj}/${ATH_HAL} -I${obj}/${DEPTH} -I${COMMON} -I${ATH_RATE} -I${ATH} -I${WLAN}

EXTRA_CFLAGS+= ${INCS} ${COPTS} -DBUILD_HAL

-include $(WLAN_TOP)/Rules.make

all:	hal.o opt_ah.h
	$(MAKE) -C $(KERNELPATH) SUBDIRS=$(shell pwd) modules

#ah_osdep.c: opt_ah.h
#	cp -f ${HAL}/${OS}/ah_osdep.c ah_osdep.c

$(obj)/hal.o:
	cp ${HAL}/${OS}/obj/${TARGET}/hal.o .
#${UUDECODE} ${HAL}/public/${TARGET}.hal.o.uu

opt_ah.h: ${HAL}/${OS}/public/${TARGET}.opt_ah.h
	cp -f ${HAL}/${OS}/public/${TARGET}.opt_ah.h opt_ah.h

# Copy the unstripped version of the driver to /tftpboot/`whoami`/drivers/
# if debug is enabled.
install: all
ifeq ($(AP71_DEBUG),1)
	test -d ${TFTPPATH}/drivers || mkdir -p ${TFTPPATH}/drivers
	cp ath_hal.${KMODSUF} ${TFTPPATH}/drivers/
endif
	test -d ${DESTDIR}/${MODULEPATH} || mkdir -p ${DESTDIR}/${MODULEPATH}
	$(STRIP) -S ath_hal.$(KMODSUF)
	cp ath_hal.${KMODSUF} ${DESTDIR}/${MODULEPATH}

clean:
	rm -f *~ *.o *.ko *.mod.c
	rm -f .depend .version .*.o.flags .*.o.d .*.o.cmd .*.ko.cmd
# remove the files we copied to this dir
	rm -f ah_osdep.c opt_ah.h hal.o

ath_hal.o: ${ath_hal-objs}
	$(LD) $(LDOPTS) -o ath_hal.$(KMODSUF) -r $(ath_hal-objs)
