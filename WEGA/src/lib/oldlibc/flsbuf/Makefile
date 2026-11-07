# oldlibc/flsbuf: the modules of the libraries, made by ../Makefile (target objs)

CC = cc
CCV1 = ccv1
CCV1Q = ccv1q
CCV1R = ccv1r
CCV0 = ccv0
CCV0P = ccv0p
CAS = cas

objs: 0_1.o 0_2.o 0_3.o

0_1.o: flsbuf.c
	$(CCV1) -O -c flsbuf.c ; mv flsbuf.o 0_1.o

0_2.o: flsbuf.c
	$(CCV0P) -O -c flsbuf.c ; mv flsbuf.o 0_2.o

0_3.o: flsbuf.c
	$(CCV1R) -O -I.. -c flsbuf.c ; mv flsbuf.o 0_3.o

.PHONY: objs
