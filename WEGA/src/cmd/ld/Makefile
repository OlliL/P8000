# ld: the nonsegmented loader of WEGA (/bin/ld, E711, SCCS ld.c 1.18), reconstructed from /bin/ld.
#
#   make link        ld, linked with libc-v9.a
#   make clean

CC = ccv1
CRT = ../../lib/libc/crt
LDBIN = ld
CAS = cas
LIBC = ../../lib/libc-v9.a
CFLAGS = -O

COBJS = ld.o copyname.o printname.o symbols.o
LINKOBJS = crt0.o $(COBJS)

all: ld

link: ld

ld: $(COBJS) $(LINKOBJS)
	$(LDBIN) -i -e start $(LINKOBJS) $(LIBC) -o $@

copyname.o: copyname.c ld.h
	$(CC) $(CFLAGS) -c copyname.c

ld.o: ld.c ld.h
	$(CC) $(CFLAGS) -c ld.c

printname.o: printname.c ld.h
	$(CC) $(CFLAGS) -c printname.c

symbols.o: symbols.c ld.h
	$(CC) $(CFLAGS) -c symbols.c

crt0.s: $(CRT)/mkcrt0.sh $(CRT)/crt0.s.in $(CRT)/crt0.list
	sh $(CRT)/mkcrt0.sh ld > $@

crt0.o: crt0.s
	$(CAS) -u -o $@ crt0.s

clean:
	rm -f *.o crt0.s ld
