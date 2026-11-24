# make: /bin/make from System III's make (usr/src/cmd/make) with WEGA's changes (wegarules.c), built by ccvm (the
# cparse of 11/18/83, OLD_D1S) with /lib/libc.a, separate I and D (-i) as make.mk; byte-identical. defs includes the
# headers of files.c: their symbol entries give the frame words of the register locals (host/OLDCC_REGSLOTS.md).
#
#   make              build ./make
#   make install PREFIX=dir
#   make clean

CC = ccvm
CAS = cas
LD = ld
YACC = yacc
CFLAGS = -O
IFLAG = -i
PREFIX = /
STRIP = strip
LIBC = /lib/libc.a
CRT = ../../lib/libc/nonseg/crt
RULES = wegarules

OBJECTS = main.o doname.o misc.o files.o $(RULES).o dosys.o gram.o dyndep.o prtmem.o

all: make

make: crt0.o $(OBJECTS)
	$(LD) $(IFLAG) -e start -o $@ crt0.o $(OBJECTS) $(LIBC)

crt0.s: $(CRT)/crt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh make > $@

crt0.o: crt0.s
	$(CAS) -u -o $@ crt0.s

gram.c: gram.y
	$(YACC) gram.y ; mv y.tab.c gram.c

$(OBJECTS): defs

.c.o:
	$(CC) $(CFLAGS) -c $<

install: make
	cp make $(PREFIX)/bin/make
	$(STRIP) $(PREFIX)/bin/make

clean:
	rm -f *.o crt0.s gram.c make

.PHONY: all install clean

# the library of /lib is not made here (make shipped)
/lib/libc.a:
