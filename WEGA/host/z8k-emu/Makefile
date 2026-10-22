# z8k-emu: user-mode Z8000 emulator that runs original WEGA binaries on Linux.
CC     ?= cc
CFLAGS ?= -O2 -Wall

all: z8k-emu

z8k-emu: z8k.c wega.c epu.c fpe_gen.c z8k.h z8k_int.h
	$(CC) $(CFLAGS) -o $@ z8k.c wega.c epu.c

# fpe_gen.c is checked in; regenerate after changing fpe.s or fpe2c.py
fpe:
	python3 fpe2c.py ../../src/uts/conf/fpe.s > fpe_gen.c

clean:
	rm -f z8k-emu *.o

.PHONY: all clean fpe
