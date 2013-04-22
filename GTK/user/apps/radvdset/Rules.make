CFLAGS				+= -s -O2 -Wall

CFLAGS				+= $(PATHNVRAM)
CFLAGS				+= $(PATHUSER)
LDFLAGS				+= $(LIBNVRAM)

OBJS					= radvdset.o
EXEC					= radvdset


#
#	Keeps following rule, DONOT MODIFY
#
EXEC_CLEAN		= objclean
DEP						= depend

objclean:
	rm -f *.o *~ $(EXEC) .depend

depend:
	$(MKDEPTOOL)/mkdep $(CFLAGS) -- *.c > .depend 