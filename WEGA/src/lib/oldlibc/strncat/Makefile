# oldlibc/strncat: the modules of the libraries, made by ../Makefile (target objs)

CC = cc
CCV1 = ccv1
CAS = cas

objs: 0_1.o

# 0_1 (ls 02/83): the strncat.c of libc, ccv1
0_1.o: ../../libc/common/gen/strncat.c
	$(CCV1) -O -I../../libc/common/gen -c ../../libc/common/gen/strncat.c ; mv strncat.o 0_1.o

.PHONY: objs
