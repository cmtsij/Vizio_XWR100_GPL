#
# Copyright (C) 2007 Gemtek Technology Co., Ltd.
#
#	General rule
#		The following rules should be able to support most case		
#
CFLAGS				+= -s -O2 -Wall

CFLAGS				+= $(PATHNVRAM)
CFLAGS				+= $(PATHUSER)
CFLAGS				+= $(PATHGEM)
LDFLAGS				+= $(LIBNVRAM)

OBJS					= radvdset.o
EXEC					= radvdset


#
#	INSTALLBIN MUST be the same as EXEC
#
INSTALLBIN	= radvdset

all: $(EXEC)

dep: $(DEP)

clean:
	rm -f $(EXEC)

install: all
	install -d $(INSTALLDIR)/bin
	install $(INSTALLBIN) $(INSTALLDIR)/bin

romfs: all
	cp $(INSTALLBIN) $(ROMFSDIR)/bin
	
$(EXEC): $(OBJS) 
	$(CC) -o $@ $(OBJS) $(LDFLAGS) 
	$(STRIP) $(EXEC)

#$(OBJS): %.o: %.c Makefile
#	 $(CC) $(CFLAGS) -c $< -o $@
	 
gpl:
	rm -f Rules.make *.c *.h *.o .depend