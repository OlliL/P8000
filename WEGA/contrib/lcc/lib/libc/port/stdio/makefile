#
# makefile for libc/port/stdio
#

SHELL=/bin/sh
INCROOT =

CC=lcc
CFLAGS=-O

OBJECTS=\
clrerr.o    ctermid.o	\
cuserid.o   data.o      doscan.o    fdopen.o	\
fgetc.o     fgets.o     filbuf.o    findiop.o	\
flsbuf.o    fopen.o     fputc.o     fputs.o	\
fread.o     fseek.o     ftell.o     fwrite.o	\
getchar.o   getpass.o   gets.o      getw.o	\
popen.o     putchar.o   puts.o      putw.o	\
rew.o       scanf.o     setbuf.o    system.o	\
tempnam.o   tmpfile.o   tmpnam.o    ungetc.o

all: $(OBJECTS)

fgets.o flsbuf.o \
	fputs.o fread.o \
	fwrite.o gets.o \
	puts.o: stdiom.h

.c.o:
	@echo $*.c:
	$(CC) $(CFLAGS) -c $*.c
