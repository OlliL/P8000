# oldlibc/filbuf: the modules of the libraries, made by ../Makefile (target objs)

CC = cc
CCV1 = ccv1
CCV1Q = ccv1q
CCV1R = ccv1r
CCV0 = ccv0
CCV0P = ccv0p
CAS = cas

objs: 0_1.o 0_2.o 0_3.o 0_4.o

0_1.o: ../../libc/common/stdio/filbuf.c
	$(CCV1Q) -O -I../../../head/oldv7 -I../../libc/common/stdio -c ../../libc/common/stdio/filbuf.c ; mv filbuf.o 0_1.o

0_2.o: ../../libc/common/stdio/filbuf.c
	$(CCV1) -O -I../../../head/oldv7 -I../../libc/common/stdio -c ../../libc/common/stdio/filbuf.c ; mv filbuf.o 0_2.o

0_3.o: ../../libc/common/stdio/filbuf.c
	$(CCV0P) -O -I../../../head/oldv7 -I../../libc/common/stdio -c ../../libc/common/stdio/filbuf.c ; mv filbuf.o 0_3.o

0_4.o: ../../libc/common/stdio/filbuf.c
	$(CCV1R) -O -I../../../head/oldv7 -c ../../libc/common/stdio/filbuf.c ; mv filbuf.o 0_4.o

.PHONY: objs
