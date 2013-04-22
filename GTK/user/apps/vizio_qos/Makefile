#
# hnapssdp (SSDP handler for hnap) Makefile
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

OBJS	= vizio_qos.o

all: clean vizio_qos

#vizio_qos: $(OBJS)
vizio_qos:
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

clean:
#	rm -rf $(OBJS) vizio_qos
	rm -rf vizio_qos

romfs:
	cp vizio_qos $(ROMFSDIR)/bin/vizio_qos
	$(STRIP) $(ROMFSDIR)/bin/vizio_qos

install:
	install -D vizio_qos $(INSTALLDIR)/bin/vizio_qos
	$(STRIP) $(INSTALLDIR)/bin/vizio_qos
