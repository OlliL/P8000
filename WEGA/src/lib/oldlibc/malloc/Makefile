# oldlibc/malloc: the modules of the libraries, made by ../Makefile (target objs)

CC = cc
CCV1 = ccv1
CCV1Q = ccv1q
CCV1R = ccv1r
CCV0 = ccv0
CCV0P = ccv0p
CAS = cas

objs: 0_1.o 0_2.o 0_3.o 0_4.o 0_6.o

0_1.o: malloc.c
	-@mkdir 0_1.d 2>/dev/null
	cd 0_1.d ; $(CCV1Q) -O -c ../malloc.c ; mv malloc.o ../0_1.o

0_2.o: malloc.c
	$(CCV1) -O -c malloc.c ; mv malloc.o 0_2.o

0_3.o: 0_3.s
	$(CAS) -u -o 0_3.o 0_3.s

0_4.o: 0_4.s
	$(CAS) -u -o 0_4.o 0_4.s

0_6.o: malloc.c
	-@mkdir 0_6.d 2>/dev/null
	cd 0_6.d ; $(CCV1R) -O -c ../malloc.c ; mv malloc.o ../0_6.o

.PHONY: objs
