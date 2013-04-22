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
CFLAGS  += $(PATHSHARED)
LDFLAGS	+= $(LIBNVRAM)
LDFLAGS += $(LIBUTILILTY)

OBJS		+= conn_log.o

all: conn_log

clean:
#	rm -f *.o *~ conn_log
	rm -f conn_log

install: all
	install -d $(INSTALLDIR)/bin
	install conn_log $(INSTALLDIR)/bin
	
conn_log: $(OBJS) 
	$(CC) -o $@ $(OBJS) $(LDFLAGS) 
	$(STRIP) conn_log

#$(OBJS): %.o: %.c Makefile
#	 $(CC) $(CFLAGS) -c $< -o $@	
