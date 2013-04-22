## Installation directory. By default, go in /usr/local
## Distributions should probably use /usr, but they probably know better...
ifndef PREFIX
  PREFIX = $(KERNELPATH)/arch/mips/ar531x/RAMDISK/rootdir
endif

## Compiler to use
ifneq ($(strip $(TOOLPREFIX)),)
CROSS:=$(TOOLPREFIX)
endif

#CC = $(CROSS)gcc
#LD = $(CROSS)ld

## Uncomment this to build against this kernel
KERNEL_SRC = $(KERNELPATH)

CFLAGS += -O3 -pipe -Wall -Wno-unused -s -g -fpack-struct -Wpadded
CFLAGS += -D__BIG_ENDIAN
OS_LAYER = osl-linux.c

all: lld2d

include ./common.mk

depend:
	$(CC) $(CFLAGS) -M $(DCFILES) >.depend
	
romfs:
	cp lld2d $(ROMFSDIR)/bin
	cp lld2d.conf $(ROMFSDIR)/etc	
	cp wrt54g.large.ico $(ROMFSDIR)/etc	
	cp icon.ico $(ROMFSDIR)/etc	

-include .depend
