#
# easytest (Product Test Program) Makefile
#

CFLAGS	+= -I. -I$(SRCBASE)/include -Wall
CFLAGS	+= -Os #-s
CFLAGS	+= $(PATHNVRAM)
CFLAGS	+= $(PATHUSER)
CFLAGS  += $(PATHGEM)
CFLAGS  += $(PATHSHARED)
LDFLAGS	+= $(LIBNVRAM)
LDFLAGS += $(LIBUTILILTY)
LDFLAGS += $(LIBSHARED)

vpath %.c $(TOP)/shared

OBJS	= easytest.o

all: clean easytest

easytest: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

clean:
	rm -rf easytest

romfs:
	cp easytest $(ROMFSDIR)/bin/easytest
	$(STRIP) $(ROMFSDIR)/bin/easytest

install:
	install -D easytest $(INSTALLDIR)/bin/easytest
	$(STRIP) $(INSTALLDIR)/bin/easytest
