ifndef PREFIX
  PREFIX = $(KERNELPATH)/arch/mips/ar531x/RAMDISK/rootdir
endif

## Compiler to use
ifneq ($(strip $(TOOLPREFIX)),)
CROSS:=$(TOOLPREFIX)
endif

CC = $(CROSS)gcc
LD = $(CROSS)ld

CFLAGS	+= -O2 -Wall -I../include
CFLAGS	+= $(PATHNVRAM)
CFLAGS	+= $(PATHUSER)
CFLAGS	+= $(PATHGEM)
CFLAGS	+= $(PATHSHARED) 
#-DNV_DEBUG
LDFLAGS		+= -L. -lnvram

OBJS		=  crc32.o mtd.o default.o nvram.o nvramd.o nvserv.o

all: libnvram.so nvram nvserv

nvserv: $(OBJS)
	echo "nvserv_new......"
	$(CC) $(CFLAGS) -o $@ $^

nvram: main.o
	echo "nvram_new......"
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

libnvram.so: default.o nvramd.o nvram.o crc32.o mtd.o
	$(LD) -EB -shared -o $@ $^

clean:
	rm -f $(OBJS) nvserv *.so nvram
