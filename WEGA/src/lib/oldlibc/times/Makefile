# oldlibc/times: the modules of the libraries, made by ../Makefile (target objs)

CC = cc
CCV1 = ccv1
CCV1Q = ccv1q
CCV1R = ccv1r
CCV0 = ccv0
CCV0P = ccv0p
CAS = cas

objs: 0_1.o

# 0_1 (time, 01/83): times returning an int (r2 = r4), the libc of 09/83 returns a long
0_1.o: 0_1.s
	$(CAS) -u -o 0_1.o 0_1.s

.PHONY: objs
