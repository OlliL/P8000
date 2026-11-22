# oldlibc/fputc: the modules of the libraries, made by ../Makefile (target objs)

CC = cc
CCV1 = ccv1
CCV1Q = ccv1q
CCV1R = ccv1r
CCV0 = ccv0
CCV0P = ccv0p
CAS = cas

objs: 0_1.o

0_1.o: ../../libc/common/stdio/fputc.c
	$(CCV1) -O -I../../../head/oldv7 -c ../../libc/common/stdio/fputc.c ; mv fputc.o 0_1.o

.PHONY: objs
