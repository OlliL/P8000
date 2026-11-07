# oldlibc/freopen: the modules of the libraries, made by ../Makefile (target objs)

CC = cc
CCV1 = ccv1
CCV1Q = ccv1q
CCV1R = ccv1r
CCV0 = ccv0
CCV0P = ccv0p
CAS = cas

objs: 0_1.o

0_1.o: ../../libc/stdio/freopen.c
	$(CCV0P) -O -I../../libc/stdio -c ../../libc/stdio/freopen.c ; mv freopen.o 0_1.o

.PHONY: objs
