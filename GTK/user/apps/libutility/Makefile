
CFLAGS	+= -s -O2 -Wall -Werror
CFLAGS	+= $(PATHNVRAM)
CFLAGS	+= $(PATHUSER)
CFLAGS	+= $(PATHSHARED)
CFLAGS	+= $(PATHGEM)
CFLAGS	+= -I$(TOPDIR)/wlan/linux
LDFLAGS	+= $(LIBNVRAM)
LDFLAGS += $(LIBUTILILTY)


all: libutility.so Makefile

install:
	install -D libutility.so $(INSTALLDIR)/usr/lib/libutility.so
	$(STRIP) $(INSTALLDIR)/usr/lib/libutility.so

clean:
	rm -f *.so

libutility.so: utility.o crc32.o shutils.o services.o gmtk_process.o QoS.o fw_upgrade.o # leduser.o
	$(LD) -shared -o $@ $^
	
test: test.c
	${CC} -o test ${CFLAGS} ${LDFLAGS} test.c
	
vizio_qos :
	${CC} -o vizio_qos ${CFLAGS} ${LDFLAGS} vizio_qos.c
