# Objects of libc/oldcc for ../obj, called by ../Makefile (target objs). WEGA make compatible.

CC = cc
CAS = cas
AS = as

OBJS = \
	../../obj/symscan.o ../../obj/malloc.o ../../obj/nlist.o ../../obj/swap.o

objs: $(OBJS)

../../obj/symscan.o: symscan/0_1.s
	cd symscan ; $(CAS) -u -o ../../obj/symscan.o 0_1.s

../../obj/malloc.o: malloc/3_3.s
	cd malloc ; $(CAS) -u -o ../../obj/malloc.o 3_3.s

../../obj/nlist.o: nlist/0_1.s
	cd nlist ; $(CAS) -u -o ../../obj/nlist.o 0_1.s

../../obj/swap.o: swap/0_1.s
	cd swap ; $(CAS) -u -o ../../obj/swap.o 0_1.s

.PHONY: objs
