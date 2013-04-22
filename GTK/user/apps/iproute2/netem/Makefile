DISTGEN = maketable normal pareto paretonormal
DISTDATA = normal.dist pareto.dist paretonormal.dist experimental.dist
CFLAGS =  -D_GNU_SOURCE -Wstrict-prototypes -Wall -I../include -DRESOLVE_HOSTNAMES
LDLIBS += -lm 

%.dist: %.c
	$(HOSTCC) $(CFLAGS) -o $* $< -lm
	./$* >$@

%.dist: %.dat
	./maketable $< >$@

all: $(DISTGEN) $(DISTDATA)

install: all
	mkdir -p $(DESTDIR)/usr/lib/tc
	for i in $(DISTDATA); \
	do install -m 755 $$i $(DESTDIR)/usr/lib/tc; \
	done

clean:
	rm -f $(DISTDATA) $(DISTGEN)

maketable: maketable.c
	$(HOSTCC) $(CFLAGS) -o $@ $< -lm


