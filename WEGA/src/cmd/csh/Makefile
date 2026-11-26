# csh: /bin/csh from Plexus' cmd/plx/csh (the Berkeley csh of 2.7BSD) as WEGA changed it (SCCS lines 08/10/83):
# variables from the environment, cshprofile before .cshrc, script type by its first line (#!), echo with escapes and -n,
# time -v, unsetenv, if( and while(, the ESC-B builtin; printf.c is the small printf of ls. ccv1 -DSYS3 (no TELL, BUFSIZ 512),
# separate I and D (-i), the files in alphabetical order (cc *.o). Four functions are open (popt block order:
# compare/csh/findings.json). libc-v9 (extended for csh, owner 2026-11-26).
#
#   make              build ./csh
#   make install PREFIX=dir
#   make clean

CC = ccv1
CAS = cas
LD = ld
CFLAGS = -O -I../../head/oldv7
DEFS = -DSYS3
PREFIX = /
STRIP = strip
LIBC = ../../lib/libc-v9.a
CRT = ../../lib/libc/nonseg/crt

OBJECTS = alloc.o getpwent.o getpwnam.o getpwuid.o printf.o sh.o sh.dol.o sh.err.o sh.exec.o sh.exp.o sh.func.o \
	sh.glob.o sh.hist.o sh.init.o sh.lex.o sh.misc.o sh.parse.o sh.print.o sh.sem.o sh.set.o sh.wait.o

all: csh

csh: crt0.o $(OBJECTS) $(LIBC)
	$(LD) -i -e start -o $@ crt0.o $(OBJECTS) $(LIBC)

crt0.s: $(CRT)/crt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh csh > $@

crt0.o: crt0.s
	$(CAS) -u -o $@ crt0.s

$(OBJECTS): sh.h sh.local.h

.c.o:
	$(CC) $(CFLAGS) $(DEFS) -c $<

install: csh
	cp csh $(PREFIX)/bin/csh
	$(STRIP) $(PREFIX)/bin/csh

clean:
	rm -f *.o crt0.s csh

.PHONY: all install clean


# the library of /lib is not made here (the shipped build)
/lib/libc.a:
