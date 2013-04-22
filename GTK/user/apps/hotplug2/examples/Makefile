# vim:set sw=8 nosta:

BINS=
SUBDIRS=

all:

romfs install:
	$(INSTALL_DIR) $(ROMFSDIR)/hotplug2/etc
	$(INSTALL) hotplug2.rules-2.6kernel $(ROMFSDIR)/hotplug2/etc/hotplug2.rules

include ../common.mak
MAKEDEP:=
