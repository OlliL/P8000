# oldlibc/swab: the modules of the libraries, made by ../Makefile (target objs)

CC = cc
CCV1 = ccv1
CCV1Q = ccv1q
CCV1R = ccv1r
CCV0 = ccv0
CCV0P = ccv0p
CAS = cas

objs: 0_1.o

0_1.o: ../../libc/gen/swab.c
	$(CCV0P) -O -I../../libc/gen -c ../../libc/gen/swab.c ; mv swab.o 0_1.o

.PHONY: objs
