#
# Copyright (C) 2008 Gemtek Technology Co., Ltd.
#
#	General rule
#		The following rules should be able to support most case		
#

CFLAGS				+= -s -O2 -Wall

CFLAGS				+= $(PATHNVRAM)
CFLAGS				+= $(PATHUSER)
LDFLAGS				+= $(LIBNVRAM)

OBJS					= DMS_set.o
EXEC					= DMS_set

#
#	INSTALLBIN MUST be the same as EXEC
#
INSTALLBIN	= DMS_set

all: $(EXEC)

clean:
#	rm -f *.o *~ $(EXEC) *.bak
	rm -f *~ $(EXEC) *.bak
	
install: all
	install -d $(INSTALLDIR)/bin
	install $(INSTALLBIN) $(INSTALLDIR)/bin

ifeq ($(GMTK_DEVFLAG_NFS),Y)

romfs:
	mkdir -p $(NFSROOTDIR)/bin
	cp $(INSTALLBIN) $(NFSROOTDIR)/bin/

else

romfs:
	mkdir -p $(ROMFSDIR)/bin
	cp $(INSTALLBIN) $(ROMFSDIR)/bin

endif

$(EXEC): $(OBJS) 
	$(CC) -o $@ $(OBJS) $(LDFLAGS) 
	$(STRIP) $(EXEC)

#$(OBJS): %.o: %.c Makefile
#	 $(CC) $(CFLAGS) -c $< -o $@
