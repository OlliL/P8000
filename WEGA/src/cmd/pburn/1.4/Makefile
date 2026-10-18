# pburn: PROM burner. Compiled with the WEGA cc, linked through the cc
# driver (crt0 and libc).
#
#   make                       build pburn
#   make install PREFIX=dir    install below dir (default /)
#   make clean                 remove the objects and the program

CC = cc
PREFIX = /

OBJECTS = pb.o pb.command.o pb.cmdline.o pb.file.o pb.buf.o pb.devud.o

all: pburn

.c.o:
	$(CC) -c $<

pb.buf.c.o: pb.buf.c
pb.c.o: pb.c
pb.cmdline.c.o: pb.cmdline.c
pb.command.c.o: pb.command.c
pb.devud.c.o: pb.devud.c
pb.file.c.o: pb.file.c

pburn: $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

install: pburn
	cp pburn $(PREFIX)/bin/pburn

clean:
	rm -f pburn $(OBJECTS)

.PHONY: all install clean
