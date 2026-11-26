# adb: /bin/adb of WEGA, the adb of Zilog's ZEUS 3.2 (no source known): written from the binary on the base of Plexus'
# cmd/adb/z8000 (the System III adb for the Z8000). Segmented and nonsegmented s.out files, the user block of a segmented
# kernel (suser.h), crash dumps and /dev/mem with -k (kva.c and kpa.c of ps), the disassembler disasm(3) of libu8000.
# The shipped cc -O, separate I and D (-i); the objects in alphabetical order, fpcvt.s last; libu8000 and kva.o, kpa.o
# behind them.
#
#   make              build ./adb
#   make install PREFIX=dir
#   make clean

CC = cc
CAS = cas
LD = ld
CFLAGS = -O
PREFIX = /
STRIP = strip
LIBC = /lib/libc.a
LIBU = ../../lib/libu8000/build/nonseg/libu8000.a
CRT = ../../lib/libc/nonseg/crt

OBJECTS = access.o command.o expr.o findfn.o format.o init.o input.o main.o message.o opset.o output.o pcs.o \
	print.o runpcs.o setup.o sym.o version.o fpcvt.o

all: adb

adb: crt0.o $(OBJECTS) $(LIBU) kva.o kpa.o $(LIBC)
	$(LD) -i -e start -o $@ crt0.o $(OBJECTS) $(LIBU) kva.o kpa.o $(LIBC)

crt0.s: $(CRT)/crt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh adb > $@

crt0.o: crt0.s
	$(CAS) -u -o $@ crt0.s

fpcvt.o: fpcvt.s
	$(CAS) -u -o $@ fpcvt.s

kva.o: ../ps/kva.c
	$(CC) $(CFLAGS) -c ../ps/kva.c

kpa.o: ../ps/kpa.c
	$(CC) $(CFLAGS) -c ../ps/kpa.c

$(OBJECTS): defs.h glob.h mac.h mode.h machine.h suser.h

.c.o:
	$(CC) $(CFLAGS) -c $<

install: adb
	cp adb $(PREFIX)/bin/adb
	$(STRIP) $(PREFIX)/bin/adb

clean:
	rm -f *.o crt0.s adb

.PHONY: all install clean


# the libraries are not made here (the shipped build: /lib/libc.a, /usr/lib/libu8000.a)
/lib/libc.a /usr/lib/libu8000.a:
