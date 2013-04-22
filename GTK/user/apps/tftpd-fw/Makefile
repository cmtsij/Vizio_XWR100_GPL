CFLAGS	+= -Os -Wall

CFLAGS	+=	$(PATHNVRAM)
CFLAGS	+=	$(PATHUSER)
CFLAGS	+= $(PATHGEM)
LDFLAGS	+= 	$(LIBNVRAM)
LDFLAGS +=	$(LIBUTILILTY)


OBJS		= pkthandler.o tftpd.o

all: tftpd

tftpd: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)
	$(STRIP) tftpd

clean:
	rm -f tftpd

romfs:
	cp tftpd $(ROMFSDIR)/bin/tftpd
	
install: all
	install -d $(INSTALLDIR)/bin
	install tftpd $(INSTALLDIR)/bin	
