# oldlibc/strcpy: the modules of the libraries, made by ../Makefile (target objs)

CC = cc
CCV1 = ccv1
CCV1Q = ccv1q
CCV1R = ccv1r
CCV0 = ccv0
CCV0P = ccv0p
CAS = cas

objs: 0_1.o 0_2.o 0_3.o

0_1.o: ../../libc/gen/strcpy.c
	$(CCV1) -O -I../../libc/gen -c ../../libc/gen/strcpy.c ; mv strcpy.o 0_1.o

0_2.o: ../../libc/gen/strcpy.c
	$(CCV0P) -O -I../../libc/gen -c ../../libc/gen/strcpy.c ; mv strcpy.o 0_2.o

0_3.o: ../../libc/gen/strcpy.c
	$(CCV1R) -O -I.. -c ../../libc/gen/strcpy.c ; mv strcpy.o 0_3.o

.PHONY: objs
