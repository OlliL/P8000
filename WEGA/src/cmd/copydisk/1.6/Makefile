# copydisk: copy Floppy to Floppy. Compiled with the WEGA cc, linked through the cc
# driver (crt0 and libc).
#
#   make                       build copydisk
#   make install PREFIX=dir    install below dir (default /)
#   make clean                 remove the objects and the program

CC = cc
CFLAGS = -O
PREFIX = /

all: copydisk

copydisk.o: copydisk.c

copydisk: copydisk.o
	$(CC) $(CFLAGS) copydisk.o -s -o $@

install: copydisk
	cp copydisk $(PREFIX)/bin/copydisk

clean:
	rm -f copydisk copydisk.o

.PHONY: all install clean
