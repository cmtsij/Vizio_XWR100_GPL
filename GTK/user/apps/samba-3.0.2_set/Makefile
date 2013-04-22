#
# Copyright (C) 2007 Gemtek Technology Co., Ltd.
#
#	General rule
#		The following rules should be able to support most case		
#
ifeq (Rules.make,$(wildcard Rules.make))
include Rules.make
ifeq (.depend,$(wildcard .depend))
include .depend
endif
endif

#
#	INSTALLBIN MUST be the same as EXEC
#
CFLAGS  += -I. -I../shared

INSTALLBIN	= sambaset

all: $(EXEC)

dep: $(DEP)

clean: $(EXEC_CLEAN)

install: all
	install -d $(INSTALLDIR)/bin
	install $(INSTALLBIN) $(INSTALLDIR)/bin

romfs: all
	cp $(INSTALLBIN) $(ROMFSDIR)/bin
	ln -sf /tmp/secrets.tdb $(ROMFSDIR)/etc/secrets.tdb
	
$(EXEC): $(OBJS) 
	$(CC) -o $@ $(OBJS) $(LDFLAGS) 
#	$(STRIP) $(EXEC)

#$(OBJS): %.o: %.c Makefile
#	 $(CC) $(CFLAGS) -c $< -o $@
	 
gpl:
	rm -f Rules.make *.c *.h *.o .depend
