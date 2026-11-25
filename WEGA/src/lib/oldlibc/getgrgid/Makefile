# oldlibc/getgrgid: the modules of the libraries, made by ../Makefile (target objs)

CC = cc
CCV1 = ccv1
CAS = cas

objs: 0_1.o

# 0_1 (ls 02/83): the getgrgid.c of libc, ccv1
0_1.o: ../../libc/common/gen/getgrgid.c
	$(CCV1) -O -I../../../head/oldv7 -c ../../libc/common/gen/getgrgid.c ; mv getgrgid.o 0_1.o

.PHONY: objs
