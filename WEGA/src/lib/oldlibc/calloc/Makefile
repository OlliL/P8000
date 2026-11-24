# oldlibc/calloc: the modules of the libraries, made by ../Makefile (target objs)

CC = cc
CCV1 = ccv1
CCV1Q = ccv1q
CCV1R = ccv1r
CCV0 = ccv0
CCV0P = ccv0p
CAS = cas

objs: 0_1.o 0_2.o

# 0_1 (mkstr): the System III calloc.c with an unused label in front of num *= size (size from its frame home)
0_1.o: 0_1.c
	$(CCV0) -O -c 0_1.c

# 0_2 (df 04/83, lex 01/83): the System III calloc.c, ccv1 (ccv1q gives the same bytes)
0_2.o: 0_2.c
	$(CCV1) -O -c 0_2.c

.PHONY: objs
