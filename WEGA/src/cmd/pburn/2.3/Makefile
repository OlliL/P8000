# pburn: PROM burner. Compiled with the WEGA cc, linked through the cc
# driver (crt0 and libc).
#
#   make                       build pburn
#   make install PREFIX=dir    install below dir (default /)
#   make clean                 remove the objects and the program

SCC ?= scc
GLD ?= gld
SCRT0 ?= /lib/scrt0.o
LIBC ?= /lib/slibc.a
PREFIX ?= /

OBJECTS = pb.o pb.command.o pb.cmdline.o pb.file.o pb.buf.o pb.devud.o

all: pburn

.c.o:
	$(SCC) -c $<

pburn: $(OBJECTS)
	$(GLD) $(SCRT0) $(OBJECTS) $(LIBC) -o $@

install: pburn
	cp pburn $(PREFIX)/bin/pburn

clean:
	rm -f pburn *.o

.PHONY: all install clean
