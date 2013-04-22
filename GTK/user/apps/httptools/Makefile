#
# Remove Space tool.
#

vpath %.c $(TOP)/shared

OBJS	= downsize.o

all: downsize

downsize: $(OBJS)
	gcc $(OBJS) -o $@

%.o: %.c
	 gcc -c $<

clean:
	rm -rf downsize

install:
	install -d $(INSTALLDIR)/www
	install -m 777 downsize $(INSTALLDIR)/www/downsize
