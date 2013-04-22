#
# radvd scripts controler Makefile
#
# Copyright (C) 2002 Gemtek Corporation
#
# $Id: Makefile,v 0.1 2002/12/13 AM 09:47:55 , Made by PeterChen
#

CFLAGS	+= -s -O2 -Wall

CFLAGS	+=	$(PATHNVRAM)
CFLAGS	+=	$(PATHUSER)
LDFLAGS	+=  $(LIBNVRAM)

EXEC := lltdset


all: $(EXEC)

clean:
	rm -f $(EXEC)

install:
	install -d $(INSTALLDIR)/bin
	install $(EXEC) $(INSTALLDIR)/bin
	

romfs: all
	cp $(EXEC) $(ROMFSDIR)/bin
	
#$(EXEC):	lltdset.o Makefile
#	$(CC) -o $@ lltdset.o
#	$(STRIP)	$(EXEC)
