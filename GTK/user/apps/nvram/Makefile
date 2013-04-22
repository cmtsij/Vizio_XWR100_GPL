## Installation directory. By default, go in /usr/local
## Distributions should probably use /usr, but they probably know better...
ifndef PREFIX
  PREFIX = $(KERNELPATH)/arch/mips/ar531x/RAMDISK/rootdir
endif

## Compiler to use
ifneq ($(strip $(TOOLPREFIX)),)
CROSS:=$(TOOLPREFIX)
endif

CC = $(CROSS)gcc

## Uncomment this to build against this kernel
KERNEL_SRC = $(KERNELPATH)


all:
#	make -C nvram clean
	make -C nvram
	mkdir -p bin
	cp nvram/nvram bin/nvram
	cp nvram/libnvram.so bin/libnvram.so
	cp nvram/nvserv	bin/nvserv
#	$(STRIP) bin/nvram
#	$(STRIP) bin/libnvram.so
#	$(STRIP) bin/nvserv

clean:
	rm -f nvram/nvram
	rm -f nvram/nvserv
	rm -f nvram/*.so
	rm -f bin/*
	rm -f *.bak
	
romfs:
	cp bin/libnvram.so $(ROOTDIR)/romfs/lib/libnvram.so
	cp bin/nvram $(ROOTDIR)/romfs/usr/sbin/nvram
	cp bin/nvserv $(ROOTDIR)/romfs/usr/sbin/nvserv
	chmod 644 $(ROOTDIR)/romfs/lib/libnvram.so
	chmod 711 $(ROOTDIR)/romfs/usr/sbin/nvram
	chmod 711 $(ROOTDIR)/romfs/usr/sbin/nvserv

