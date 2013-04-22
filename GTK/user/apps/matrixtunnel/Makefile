#
# EasyConf control scripts Makefile
#
# Copyright (C) 2002 Gemtek
#
# $Id: Makefile,v 1.1.1.1 2004/04/28 07:18:25 tom Exp $
#

CFLAGS	+= -s -O2 -Wall

CFLAGS	+= $(PATHNVRAM)
CFLAGS	+= $(PATHUSER)
CFLAGS  += $(PATHGEM)
CFLAGS  += $(PATHSHARED) -DLINUX -I./matrixssl/
#LDFLAGS	+= $(LIBNVRAM) -L./matrixssl/src -lmatrixsslstatic
#LDFLAGS += -L./matrixssl/src -lmatrixsslstatic
LDFLAGS += $(LIBNVRAM)
LDFLAGS += -L./matrixssl/src -lmatrixssl

OBJS		+= matrixtunnel.c  sslSocket.c

all: matrix_ssl matrixtunnel

clean:
	make -C ./matrixssl/src clean
	rm -f *.o *~ matrixtunnel

install: all
	install -d $(INSTALLDIR)/bin
	install matrixtunnel $(INSTALLDIR)/bin
	install -D matrixssl/certificate-Gemtek/privkeySrv.pem $(INSTALLDIR)/etc/privkeySrv.pem
	install -D matrixssl/certificate-Gemtek/certSrv.pem $(INSTALLDIR)/etc/certSrv.pem
	
matrixtunnel: $(OBJS) 
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS) 
	$(STRIP) matrixtunnel

matrix_ssl:
	cd matrixssl/src && $(MAKE) all

romfs:
	cp matrixtunnel $(ROMFSDIR)/bin
	cp matrixssl/src/libmatrixssl.so $(ROMFSDIR)/lib
	cp matrixssl/certificate-Gemtek/privkeySrv.pem $(ROMFSDIR)/etc/privkeySrv.pem
	cp matrixssl/certificate-Gemtek/certSrv.pem $(ROMFSDIR)/etc/certSrv.pem


$(OBJS): %.o: %.c Makefile
	 $(CC) $(CFLAGS) -c $< -o $@	
