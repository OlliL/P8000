# oldlibc/nlist: the modules of the libraries, made by ../Makefile (target objs)

CC = cc
CCV1 = ccv1
CCV1Q = ccv1q
CCV1R = ccv1r
CCV0 = ccv0
CCV0P = ccv0p
CAS = cas

objs: 0_1.o

# 0_1 (df, 04/83): the nlist.c of ../../libc with ccv1
0_1.o: ../../libc/common/gen/nlist.c
	$(CCV1) -O -c ../../libc/common/gen/nlist.c ; mv nlist.o 0_1.o

.PHONY: objs
