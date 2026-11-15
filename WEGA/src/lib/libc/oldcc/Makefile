# Objects of libc/oldcc for ../obj, called by ../Makefile (target objs). WEGA make compatible.

CC = cc
CCVL = ccvl
CAS = cas
AS = as

OBJS = \
	../obj/symscan.o ../obj/malloc.o ../obj/nlist.o ../obj/swap.o

objs: $(OBJS)

# symscan, nlist, swap, malloc: from ../gen by the compiler of the library, vl (OLDROOT; host/OLDCC.md, D53 to D58).
# These four were the members the shipped cc does not give; nothing is left here as an assembler source.
../obj/symscan.o: ../gen/symscan.c
	cd ../gen ; $(CCVL) -O -c symscan.c ; mv symscan.o ../obj/symscan.o

../obj/malloc.o: ../gen/malloc.c
	cd ../gen ; $(CCVL) -O -c malloc.c ; mv malloc.o ../obj/malloc.o

../obj/nlist.o: ../gen/nlist.c
	cd ../gen ; $(CCVL) -O -c nlist.c ; mv nlist.o ../obj/nlist.o

../obj/swap.o: ../gen/swap.c
	cd ../gen ; $(CCVL) -O -c swap.c ; mv swap.o ../obj/swap.o

.PHONY: objs
