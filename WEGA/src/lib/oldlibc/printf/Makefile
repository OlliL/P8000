# oldlibc/printf: the modules of the libraries, made by ../Makefile (target objs)

CC = cc
CCV1 = ccv1
CCV1Q = ccv1q
CCV1R = ccv1r
CCV0 = ccv0
CCV0P = ccv0p
CAS = cas

objs: 0_1.o 0_2.o 0_3.o 0_4.o 0_5.o 0_6.o 0_7.o

0_1.o: 0_2.c
	-@mkdir 0_1.d 2>/dev/null
	cd 0_1.d ; $(CCV1Q) -O -I../.. -c ../0_2.c ; mv 0_2.o ../0_1.o

0_2.o: 0_2.c
	$(CCV1) -O -I.. -c 0_2.c

0_3.o: 0_5.c
	-@mkdir 0_3.d 2>/dev/null
	cd 0_3.d ; $(CCV0) -O -I../.. -c ../0_5.c ; mv 0_5.o ../0_3.o

0_4.o: 0_4.c
	$(CCV1) -O -c 0_4.c

0_5.o: 0_5.c
	$(CCV0P) -O -I.. -c 0_5.c

0_6.o: 0_4.c
	-@mkdir 0_6.d 2>/dev/null
	cd 0_6.d ; $(CCV1) -O -I../.. -c ../0_4.c ; mv 0_4.o ../0_6.o

0_7.o: 0_2.c
	-@mkdir 0_7.d 2>/dev/null
	cd 0_7.d ; $(CCV1R) -O -I../.. -c ../0_2.c ; mv 0_2.o ../0_7.o

.PHONY: objs
