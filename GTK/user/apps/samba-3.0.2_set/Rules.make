CFLAGS				+=  -s -O2 -Wall

CFLAGS				+=	$(PATHGEM)
CFLAGS				+=	$(PATHNVRAM)
CFLAGS				+=	$(PATHUSER)
LDFLAGS				+= 	$(LIBNVRAM)
#LDFLAGS				+=  $(LIBUTILILTY)

OBJS					= sambaset.o shutils.o gmtk_process.o
EXEC					= sambaset


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
