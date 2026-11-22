# oldlibc/atoi: the modules of the libraries, made by ../Makefile (target objs)

CC = cc
CCV1 = ccv1
CCV1Q = ccv1q
CCV1R = ccv1r
CCV0 = ccv0
CCV0P = ccv0p
CAS = cas

objs: 0_1.o 0_2.o

0_1.o: ../../libc/common/gen/atoi.c
	$(CCV0P) -O -I../../libc/common/gen -c ../../libc/common/gen/atoi.c ; mv atoi.o 0_1.o

0_2.o: ../../libc/common/gen/atoi.c
	$(CCV1) -O -I../../libc/common/gen -c ../../libc/common/gen/atoi.c ; mv atoi.o 0_2.o

.PHONY: objs
