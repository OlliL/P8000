# oldlibc/endopen: the modules of the libraries, made by ../Makefile (target objs)

CC = cc
CCV1 = ccv1
CCV1Q = ccv1q
CCV1R = ccv1r
CCV0 = ccv0
CCV0P = ccv0p
CAS = cas

objs: 0_1.o 0_2.o 0_3.o 0_4.o 0_5.o

0_1.o: 0_5.c
	-@mkdir 0_1.d 2>/dev/null
	cd 0_1.d ; $(CCV1Q) -O -I../.. -c ../0_5.c ; mv 0_5.o ../0_1.o

0_2.o: ../../libc/stdio/endopen.c
	$(CCV1) -O -I../../libc/stdio -c ../../libc/stdio/endopen.c ; mv endopen.o 0_2.o

0_3.o: 0_3.c
	$(CCV0P) -O -c 0_3.c

0_4.o: ../../libc/stdio/endopen.c
	$(CCV0P) -O -I../../libc/stdio -c ../../libc/stdio/endopen.c ; mv endopen.o 0_4.o

0_5.o: 0_5.c
	$(CCV1R) -O -I.. -c 0_5.c

.PHONY: objs
