#
# udhcpd scripts controler Makefile
#
# Copyright (C) 2002 Gemtek Corporation
#
# $Id: Makefile,v 0.1 2002/12/13 AM 09:47:55 , Made by PeterChen
#

CFLAGS	+= -s -O2 -Wall

CFLAGS	+= $(PATHNVRAM)
CFLAGS	+= $(PATHUSER)
CFLAGS	+= $(PATHGEM)
CFLAGS	+= $(PATHSHARED)

LDFLAGS += $(LIBUTILILTY)
LDFLAGS	+= $(LIBNVRAM)
LDFLAGS += $(LIBSHARED)

all:		udhcpdset

clean:
	rm -f udhcpdset

install:
	install -d $(INSTALLDIR)/bin
	install udhcpdset $(INSTALLDIR)/bin
	

#romfs:		all
#	cp udhcpdset $(ROMFSDIR)/sbin
	
udhcpdset:	udhcpdset.o Makefile
	$(CC) -o $@ udhcpdset.o $(LDFLAGS) -lnvram
	$(STRIP)	udhcpdset