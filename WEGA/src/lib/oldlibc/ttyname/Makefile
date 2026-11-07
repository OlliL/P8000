# oldlibc/ttyname: the modules of the libraries, made by ../Makefile (target objs)

CC = cc
CCV1 = ccv1
CCV1Q = ccv1q
CCV1R = ccv1r
CCV0 = ccv0
CCV0P = ccv0p
CAS = cas

objs: 0_1.o

0_1.o: ../../libc/gen/ttyname.c
	$(CCV1R) -O -I.. -c ../../libc/gen/ttyname.c ; mv ttyname.o 0_1.o

.PHONY: objs
