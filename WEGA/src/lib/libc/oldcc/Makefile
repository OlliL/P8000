# Objects of libc/oldcc for ../obj, called by ../Makefile (target objs). WEGA make compatible.

CC = cc
CCVL = ccvl
CAS = cas
AS = as

OBJS = \
	../obj/symscan.o ../obj/malloc.o ../obj/nlist.o ../obj/swap.o

objs: $(OBJS)

# symscan, nlist: from ../gen by the compiler of the library, vl (OLDROOT; host/OLDCC.md, D53, D54)
../obj/symscan.o: ../gen/symscan.c
	cd ../gen ; $(CCVL) -O -c symscan.c ; mv symscan.o ../obj/symscan.o

../obj/malloc.o: malloc/3_3.s
	cd malloc ; $(CAS) -u -o ../../obj/malloc.o 3_3.s

../obj/nlist.o: ../gen/nlist.c
	cd ../gen ; $(CCVL) -O -c nlist.c ; mv nlist.o ../obj/nlist.o

../obj/swap.o: ../gen/swap.c
	cd ../gen ; $(CCVL) -O -c swap.c ; mv swap.o ../obj/swap.o

.PHONY: objs
