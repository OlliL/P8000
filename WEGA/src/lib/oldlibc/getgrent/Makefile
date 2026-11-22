# oldlibc/getgrent: the modules of the libraries, made by ../Makefile (target objs)

CC = cc
CCV1 = ccv1
CCV1Q = ccv1q
CCV1R = ccv1r
CCV0 = ccv0
CCV0P = ccv0p
CAS = cas

objs: 0_1.o

# 0_1 (chgrp, chown): the System III getgrent.c, without the unused "char *foo" of the shipped one
0_1.o: getgrent.c
	$(CCV1) -O -I../../../head/oldv7 -c getgrent.c ; mv getgrent.o 0_1.o

.PHONY: objs
