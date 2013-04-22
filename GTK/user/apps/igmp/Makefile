#
# Simple build file for now
#
#

CFLAGS = -s -Os -fomit-frame-pointer
LDFLAGS = -Wl,-allow-shlib-undefined 

CFLAGS	+= $(PATHNVRAM)
CFLAGS	+= $(PATHUSER)
CFLAGS	+= $(PATHGEM)
CFLAGS	+= $(PATHSHARED)

LDFLAGS += $(LIBUTILILTY)
LDFLAGS	+= $(LIBNVRAM)
LDFLAGS += $(LIBSHARED)

ifeq ($(SYS_ENDIAN),LITTLE_ENDIAN)
	CFLAGS += -D__LITTLE_ENDIAN 
endif

ifneq ($(SYS_ENDIAN),LITTLE_ENDIAN)
	CFLAGS += -D__BIG_ENDIAN
endif

CFLAGS +=-Wall

OBJS = igmprt.o proxy.o igmpv2.o util.o

all: igmp

$(OBJS): %.o: %.c Makefile
	 $(CC) $(CFLAGS) -c $< -o $@
	 
igmp: $(OBJS) Makefile
	$(CC) $(LDFLAGS) -o $@ $(OBJS)
	$(STRIP) igmp
	
install:
	install -d $(INSTALLDIR)/bin
	install igmp $(INSTALLDIR)/bin
	
	
#romfs: all
#	cp igmp $(ROMFSDIR)/bin/igmp

clean:
	-rm -f igmp *.o  igmp.a

