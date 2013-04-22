CFLAGS				+= -s -O2 -Wall
CFLAGS				+= $(PATHSHARED)
CFLAGS				+= $(PATHUSER)
CFLAGS				+= $(PATHNVRAM)

LDFLAGS				+= $(LIBNVRAM)
LDFLAGS				+= $(LIBUTILILTY)
LDFLAGS				+= $(LIBNVRAM)
LDFLAGS				+= $(LIBSHARED) 

OBJS					= usb_storage_hotplug.o
EXEC					= usb_storage_hotplug


#
#	Keeps following rule, DONOT MODIFY
#
EXEC_CLEAN		= objclean
DEP						= depend

objclean:
#	rm -f *.o *~ $(EXEC) .depend
	rm -f *~ $(EXEC) .depend

depend:
	$(MKDEPTOOL)/mkdep $(CFLAGS) -- *.c > .depend 
