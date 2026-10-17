LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~putchar:*/
                      5 
                      6 .code
                      7 
                      8 _putchar::
                      9 {
0000 abf3            10 	dec	fp,#~L2
0002 6ffe  0002      11 	ld	2(fp),r14
0006 a17e            12 	ld	r14,r7
0008 6b00  000a*     13 	dec	__iob+10,#1
000c 4d01  000a*     14 	cp	__iob+10,#0
0010 0000  
0012 e905            15 	jpr	ge,L10003
0014 2106  0008*     16 	ld	r6,#__iob+8
0018 5f00  0000*     17 	callr	__flsbuf
001c e807            18 	jpr	L10000
                     19 L10003:
001e a1e2            20 	ld	r2,r14
0020 6103  0008*     21 	ld	r3,__iob+8
0024 6900  0008*     22 	inc	__iob+8,#1
0028 2e3a            23 	ldb	@r3,rl2
002a 8222            24 	subb	rh2,rh2
                     25 L10000:
002c 61fe  0002      26 	ld	r14,2(fp)
0030 a9f3            27 	inc	fp,#~L2
0032 9e08            28 	ret
                     29 	~L1 := 0
                     30 	~L2 := 4
                     31 	}	/* _putchar */
                     32 
                     33 .data
                     34 
