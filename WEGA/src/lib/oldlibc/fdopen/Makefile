# oldlibc/fdopen: the modules of the libraries, made by ../Makefile (target objs)

CC = cc
CCV1 = ccv1
CCV1Q = ccv1q
CCV1R = ccv1r
CCV0 = ccv0
CCV0P = ccv0p
CAS = cas

objs: 0_1.o

0_1.o: ../../libc/stdio/fdopen.c
	$(CCV0P) -O -I../../libc/stdio -c ../../libc/stdio/fdopen.c ; mv fdopen.o 0_1.o

.PHONY: objs
