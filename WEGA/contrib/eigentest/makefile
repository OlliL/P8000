# Makefile zur Bildung des Programms: eigentest

# muss anschliessend unter UDOS mit UDOSCNVT eigentest EIGENTEST in eine
# lauffaehige UDOS-Procedure konvertiert werden

GFLAGS=-t
CC=u80cc
AS=u80as
LD=u80ld
CFLAGS=-O
OBJECTS=udosprom.o menu.o u880tr.o

eigentest : $(OBJECTS)
	u80ld -bt 0x4000 -e start $(OBJECTS) -o eigentest -lu80c
