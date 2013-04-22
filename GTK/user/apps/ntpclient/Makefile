#
# Router NTP control scripts Makefile
#
# Copyright (C) 2002 Gemtek Corporation
#
# $Id: Makefile,v 1.1.1.1 2004/03/22 03:02:34 tom Exp $
#

CFLAGS	+=	-s -Os -Wall
CFLAGS   +=	-DTIMEZONE -DDAYLIGHTSAVE

CFLAGS	+=	$(PATHNVRAM)
CFLAGS	+=	$(PATHUSER)
LDFLAGS	+= $(LIBNVRAM)

OBJS	=	Getntp.o phaselock.o exeNtp.o


all	: 	exeNtp

install:
	install -D exeNtp $(INSTALLDIR)/bin/exeNtp
#	install -D Getntp $(INSTALLDIR)/bin/Getntp

romfs	:	all
	cp exeNtp $(ROMFSDIR)/bin
#	cp Getntp $(ROMFSDIR)/bin

exeNtp	:	$(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	$(STRIP) exeNtp

#Getntp	:	$(OBJS) Makefile
#	$(CC) $(CFLAGS) -lnvram  $(CLIB) -o $@ $(OBJS) $(LDFLAGS)
#	$(STRIP) Getntp

clean:
	rm -f *.o *~ exeNtp Getntp