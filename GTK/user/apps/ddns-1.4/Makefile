# Philipp Benner		#
# http://philippb.tk		#

BIN = updatedd
IDIR = $(INSTALLDIR)/sbin
LDIR = $(INSTALLDIR)/lib
XDIR = /lib
MARCH = `uname -m`

CFLAGS	+= -s -O2 -Wall

CFLAGS	+=	$(PATHNVRAM)
CFLAGS	+=	$(PATHUSER)
CFLAGS	+=	$(PATHSHARED)
CFLAGS	+=	$(PATHGEM)
LDFLAGS += $(LIBUTILILTY)
LDFLAGS	+= $(LIBNVRAM)
LDFLAGS += $(LIBSHARED)

all: Makefile
	$(CC) ${CFLAGS} -c src/updatedd.c src/modules/base64encode.c src/modules/get_connection.c src/modules/error.c src/modules/get_ifaddr.c src/modules/GetHostByName.c src/libs/dyndns.c src/libs/tzoupdate.c
	#src/libs/getcurrentip_tzo.c
	$(CC) ${LDFLAGS} -o ${BIN} updatedd.o error.o base64encode.o get_connection.o get_ifaddr.o GetHostByName.o dyndns.o tzoupdate.o
	# tzo.o getcurrentip_tzo.o
	$(STRIP) $(BIN)

debug:
	${CC} ${GFLAGS} -fPIC -c src/modules/base64encode.c src/modules/get_connection.c src/modules/error.c src/modules/get_ifaddr.c src/modules/GetHostByName.c src/libs/dyndns.c src/libs/ovh.c src/libs/ods.c src/libs/hn.c src/libs/noip.c src/libs/tzo.c src/libs/changeip.c
	rm -f *.o


clean:
	rm -f *.o
	rm -f ${BIN}

install:
	install -D updatedd $(INSTALLDIR)/bin/updatedd
	$(STRIP) $(INSTALLDIR)/bin/updatedd
#	$(SSTRIP) $(INSTALLDIR)/bin/updatedd
	

uninstall:
	rm -f ${IDIR}/${BIN}
	
%.o: %.c Makefile
	 $(CC) $(CFLAGS) -c $< -o $@	
