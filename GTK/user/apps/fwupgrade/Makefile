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
LD = $(CROSS)ld

## Uncomment this to build against this kernel
KERNEL_SRC = $(KERNELPATH)

CFLAGS		+= -O1 -Wall
CFLAGS	+= $(PATHNVRAM)
CFLAGS	+= $(PATHUSER)
CFLAGS	+= $(PATHGEM)
CFLAGS	+= $(PATHSHARED)

LDFLAGS += $(LIBUTILILTY)
LDFLAGS	+= $(LIBNVRAM)
LDFLAGS += $(LIBSHARED)

OBJS		= fwupgrade.o mtd.o

all: fwupgrade

fwupgrade: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

clean:
	rm -f fwupgrade

romfs:
	cp fwupgrade $(ROMFSDIR)/bin/fwupgrade
#	$(ROMFSINST)    /usr/sbin/fwupgrade
