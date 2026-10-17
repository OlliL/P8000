# pburn: PROM burner. Compiled with the WEGA cc, linked through the cc
# driver (crt0 and libc).
#
#   make                       build pburn
#   make install PREFIX=dir    install below dir (default /)
#   make clean                 remove the objects and the program

CC ?= cc
CFLAGS ?= -O
PREFIX ?= /

OBJECTS = pb.o pb.buf.o pb.cmdline.o pb.command.o pb.devud.o pb.file.o

all: pburn

.c.o:
	$(CC) $(CFLAGS) -c $<

pburn: $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ -s

install: pburn
	cp pburn $(PREFIX)/bin/pburn

clean:
	rm -f pburn *.o

.PHONY: all install clean
