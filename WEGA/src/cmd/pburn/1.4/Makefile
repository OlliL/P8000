# pburn: PROM burner. Compiled with the WEGA cc, linked through the cc
# driver (crt0 and libc).
#
#   make                       build pburn
#   make install PREFIX=dir    install below dir (default /)
#   make clean                 remove the objects and the program

CC ?= cc
PREFIX ?= /

OBJECTS = pb.o pb.command.o pb.cmdline.o pb.file.o pb.buf.o pb.devud.o

all: pburn

.c.o:
	$(CC) -c $<

pburn: $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

install: pburn
	cp pburn $(PREFIX)/bin/pburn

clean:
	rm -f pburn *.o

.PHONY: all install clean
