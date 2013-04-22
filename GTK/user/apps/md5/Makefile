
CFLAGS  += -s -O2 -Wall

all:  md5.o md5main.o
	$(CC) $(CFLAGS) -o md5 -lm md5main.c md5.c
	$(STRIP) md5


clean:
	rm -f *.o *~ md5

romfs: all
	cp md5 $(ROMFSDIR)/bin/

