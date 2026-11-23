# oldlibc/fseek: the modules of the libraries, made by ../Makefile (target objs)

CC = cc
CCV1 = ccv1
CCV1Q = ccv1q
CCV1R = ccv1r
CCV0 = ccv0
CCV0P = ccv0p
CAS = cas

objs: 0_1.o 0_2.o 0_3.o

0_1.o: ../../libc/common/stdio/fseek.c
	$(CCV1) -O -I../../libc/common/stdio -c ../../libc/common/stdio/fseek.c ; mv fseek.o 0_1.o

# 0_2 (libc-v1): v0 with OLD_D3K2 (offset, the second parameter, stays known at the labels)
0_2.o: ../../libc/common/stdio/fseek.c
	$(CCV0) -O -I../../libc/common/stdio -c ../../libc/common/stdio/fseek.c ; mv fseek.o 0_2.o

0_3.o: ../../libc/common/stdio/fseek.c
	$(CCV1Q) -O -I../../libc/common/stdio -c ../../libc/common/stdio/fseek.c ; mv fseek.o 0_3.o

.PHONY: objs
