# verify: Floppy verifier. Compiled with the WEGA cc, linked through the cc
# driver (crt0 and libc).
#
#   make                       build verify
#   make install PREFIX=dir    install below dir (default /)
#   make clean                 remove the objects and the program

CC = cc
CFLAGS = -O
PREFIX = /

all: verify

verify.o: verify.c

verify: verify.o
	$(CC) $(CFLAGS) verify.o -s -o $@

install: verify
	cp verify $(PREFIX)/bin/verify

clean:
	rm -f verify verify.o

.PHONY: all install clean
