# oldlibc/doprnt: the modules of the libraries, made by ../Makefile (target objs)

CC = cc
CCV1 = ccv1
CCV1Q = ccv1q
CCV1R = ccv1r
CCV0 = ccv0
CCV0P = ccv0p
CAS = cas

objs: 0_1.o 0_2.o 0_3.o 0_4.o 0_5.o 0_6.o 0_7.o 0_8.o

0_1.o: 0_1.s
	$(CAS) -u -o 0_1.o 0_1.s

0_2.o: 0_2.c
	$(CCV1) -O -c 0_2.c

0_3.o: 0_3.s
	$(CAS) -u -o 0_3.o 0_3.s

0_4.o: 0_4.c
	$(CC) -O -c 0_4.c

0_5.o: 0_5.s
	$(CAS) -u -o 0_5.o 0_5.s

0_6.o: 0_6.c
	$(CCV1) -O -c 0_6.c

0_7.o: 0_7.s
	$(CAS) -u -o 0_7.o 0_7.s

0_8.o: 0_8.s
	$(CAS) -u -o 0_8.o 0_8.s

.PHONY: objs
