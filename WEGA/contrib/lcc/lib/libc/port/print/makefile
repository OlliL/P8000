# makefile for libc/port/print
#

SHELL=/bin/sh
INCROOT=/z/LCC

CC=lcc
CFLAGS=-O

OBJECTS=\
doprnt.o    fprintf.o   printf.o    sprintf.o

all: $(OBJECTS)

doprnt.o: print.h

.c.o:
	@echo $*.c:
	$(CC) $(CFLAGS) -c $*.c
