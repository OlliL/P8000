# format: Floppy formatter. Compiled with the WEGA cc, linked through the cc
# driver (crt0 and libc).
#
#   make                       build format
#   make install PREFIX=dir    install below dir (default /)
#   make clean                 remove the objects and the program

CC = cc
CFLAGS = -O
PREFIX = /

all: format

format.o: format.c

format: format.o
	$(CC) $(CFLAGS) format.o -s -o $@

install: format
	cp format $(PREFIX)/bin/format

clean:
	rm -f format format.o

.PHONY: all install clean
