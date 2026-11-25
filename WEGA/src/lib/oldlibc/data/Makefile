# oldlibc/data: the modules of the libraries, made by ../Makefile (target objs)

CC = cc
CCV1 = ccv1
CCV1Q = ccv1q
CCV1R = ccv1r
CCV0 = ccv0
CCV0P = ccv0p
CAS = cas

objs: 0_1.o 0_2.o

0_1.o: 0_1.c
	$(CCV0P) -O -c 0_1.c

# libc-v14 (local): the data.c of ../libc with _NFILE 40
0_2.o: ../../libc/common/stdio/data.c
	-@mkdir 0_2.d 2>/dev/null
	cd 0_2.d ; $(CC) -O -I../../../../head/oldnfile40 -c ../../../libc/common/stdio/data.c ; mv data.o ../0_2.o

.PHONY: objs
