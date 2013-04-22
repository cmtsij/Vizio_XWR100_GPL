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
CFLAGS  += -DSEND_LOG
LDFLAGS	+= $(LIBNVRAM)
LDFLAGS += $(LIBUTILILTY)

OBJS		+= device_check.o

all: device_check

clean:
#	rm -f *.o *~ device_check
	rm -f *~ device_check

install: all
	install -d $(INSTALLDIR)/bin
	install conn_log $(INSTALLDIR)/bin
	
conn_log: $(OBJS) 
	$(CC) -o $@ $(OBJS) $(LDFLAGS) 
	$(STRIP) conn_log

#$(OBJS): %.o: %.c Makefile
#	 $(CC) $(CFLAGS) -c $< -o $@	