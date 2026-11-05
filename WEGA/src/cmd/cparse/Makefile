# cparse, scparse: pass 1 of cc and scc (PCC front end) of WEGA, reconstructed from /lib/cparse. scparse is the same source with -DSEG,
# the objects in seg. With strip byte-identical.
#
#   make             cparse
#   make scparse
#   make oldprep     what the versions of the old compiler need of this directory (crt0.o, cgram1.c)
#   make old         the versions of the old compiler in OLDDIRS (v1/Makefile ...)
#   make clean
#
# The parser cgram1.y is made into cgram1.c by yacc.

CC = ccv0p
CAS = cas
LD = ld
YACC = yacc
CFLAGS = -O
CRT = ../../lib/libc/crt
LIBC = ../../lib/libc-v7.a
OBJS = cgram1.o xdefs1.o scan1.o pftn1.o trees1.o optim1.o code1.o local1.o comm1.o vinit1.o sym1.o sym2.o
SOBJS = seg/cgram1.o seg/xdefs1.o seg/scan1.o seg/pftn1.o seg/trees1.o seg/optim1.o seg/code1.o seg/local1.o seg/comm1.o seg/vinit1.o seg/sym1.o seg/sym2.o
HDRS = mfile1 manifest macdefs stdio.h
OLDDIRS = v1 v0 v0p

all: cparse scparse

oldprep: crt0.o cgram1.c

old: oldprep
	for d in $(OLDDIRS); do cd $$d ; $(MAKE) ; cd .. ; done

cparse: crt0.o $(OBJS) $(LIBC)
	$(LD) -i -e start crt0.o $(OBJS) $(LIBC) -o $@

scparse: crt0.o cgram1.c $(HDRS) $(LIBC)
	-mkdir seg
	for f in cgram1 xdefs1 scan1 pftn1 trees1 optim1 code1 local1 comm1 vinit1 sym1 sym2 ; do ( cd seg ; $(CC) $(CFLAGS) -DSEG -c ../$$f.c ) || exit 1 ; done
	$(LD) -i -e start crt0.o $(SOBJS) $(LIBC) -o $@

$(OBJS): $(HDRS)
optim1.o: optim
comm1.o: common

.c.o:
	$(CC) $(CFLAGS) -c $<

cgram1.c: cgram1.y
	$(YACC) cgram1.y
	mv y.tab.c cgram1.c

crt0.s: $(CRT)/mkcrt0.sh $(CRT)/crt0.s.in $(CRT)/crt0.list
	sh $(CRT)/mkcrt0.sh cparse > $@

crt0.o: crt0.s
	$(CAS) -u -o $@ crt0.s

clean:
	rm -f crt0.s cgram1.c cparse scparse *.o
	rm -rf seg
	for d in $(OLDDIRS); do cd $$d ; $(MAKE) clean ; cd .. ; done

.PHONY: all oldprep old clean
