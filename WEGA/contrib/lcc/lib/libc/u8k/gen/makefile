#
# makefile for libc/u8k/gen
#


SHELL=/bin/sh
CC=lcc
CFLAGS=-O

OBJECTS=\
abs.o         cuexit.o      setjmp.o      malloc.o    \
memccpy.o     memchr.o      memcmp.o      memcpy.o      \
memset.o      strtok.o \
strcat.o      strchr.o      strcmp.o      strcpy.o      \
strcspn.o     strlen.o      strncat.o     strncmp.o     \
strncpy.o     strpbrk.o     strrchr.o     strspn.o

all: $(OBJECTS)

#-------------------------

.s.o:
	@echo $*.s:
	$(CC) $(CFLAGS) -c $*.s

.c.o:
	@echo $*.c:
	$(CC) $(CFLAGS) -c $*.c

