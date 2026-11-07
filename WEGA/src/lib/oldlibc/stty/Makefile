# oldlibc/stty: the modules of the libraries, made by ../Makefile (target objs)

CC = cc
CCV1 = ccv1
CCV1Q = ccv1q
CCV1R = ccv1r
CCV0 = ccv0
CCV0P = ccv0p
CAS = cas

objs: 0_1.o 0_3.o 0_4.o

0_1.o: ../../libc/gen/stty.c
	$(CCV0) -O -I../../libc/gen -c ../../libc/gen/stty.c ; mv stty.o 0_1.o

0_3.o: ../../libc/gen/stty.c
	$(CCV0P) -O -I../../libc/gen -c ../../libc/gen/stty.c ; mv stty.o 0_3.o

0_4.o: ../../libc/gen/stty.c
	$(CCV1R) -O -I.. -c ../../libc/gen/stty.c ; mv stty.o 0_4.o

.PHONY: objs
