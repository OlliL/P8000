# z8k-emu: user-mode Z8000 emulator that runs original WEGA binaries on Linux.
CC     ?= cc
CFLAGS ?= -O2 -Wall

all: z8k-emu

z8k-emu: z8k.c wega.c z8k.h
	$(CC) $(CFLAGS) -o $@ z8k.c wega.c

clean:
	rm -f z8k-emu *.o

.PHONY: all clean
