# oldlibc/calloc: the modules of the libraries, made by ../Makefile (target objs)

CC = cc
CCV1 = ccv1
CCV1Q = ccv1q
CCV1R = ccv1r
CCV0 = ccv0
CCV0P = ccv0p
CAS = cas

objs: 0_1.o

# 0_1 (mkstr): the System III calloc.c with ccv0 gives it except `mult rr2,%0000(r15)` (ccv0 takes size from r6; forms tried: host/OLDCC_LOG.md)
0_1.o: 0_1.s
	$(CAS) -u -o 0_1.o 0_1.s

.PHONY: objs
