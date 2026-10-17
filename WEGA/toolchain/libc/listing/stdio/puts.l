LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~puts:*/
                      5 
                      6 .code
                      7 
                      8 _puts::
                      9 {
0000 abf7            10 	dec	fp,#~L2
0002 5cf9  0d01      11 	ldm	4(fp),r13,#2
0006 0004  
0008 a17e            12 	ld	r14,r7
000a e813            13 	jpr	L20
                     14 L20001:
000c a1d7            15 	ld	r7,r13
000e 2106  0008*     16 	ld	r6,#__iob+8
0012 5f00  0000*     17 	callr	__flsbuf
0016 e80d            18 	jpr	L20
                     19 L20003:
0018 6b00  000a*     20 	dec	__iob+10,#1
001c 4d01  000a*     21 	cp	__iob+10,#0
0020 0000  
0022 e1f4            22 	jpr	lt,L20001
0024 a1d2            23 	ld	r2,r13
0026 6103  0008*     24 	ld	r3,__iob+8
002a 6900  0008*     25 	inc	__iob+8,#1
002e 2e3a            26 	ldb	@r3,rl2
0030 8222            27 	subb	rh2,rh2
                     28 L20:
0032 a1e2            29 	ld	r2,r14
0034 a9e0            30 	inc	r14,#1
0036 202b            31 	ldb	rl3,@r2
0038 b130            32 	extsb	r3
003a a13d            33 	ld	r13,r3
003c 8d34            34 	test	r3
003e eeec            35 	jpr	ne,L20003
0040 6b00  000a*     36 	dec	__iob+10,#1
0044 4d01  000a*     37 	cp	__iob+10,#0
0048 0000  
004a e906            38 	jpr	ge,L10005
004c bd7a            39 	ldk	r7,#10
004e 2106  0008*     40 	ld	r6,#__iob+8
0052 5f00  0000*     41 	callr	__flsbuf
0056 e808            42 	jpr	L10000
                     43 L10005:
0058 6102  0008*     44 	ld	r2,__iob+8
005c 6900  0008*     45 	inc	__iob+8,#1
0060 0c25  0a0a      46 	ldb	@r2,#10
0064 ca0a            47 	ldb	rl2,#10
0066 8222            48 	subb	rh2,rh2
                     49 L10000:
0068 5cf1  0d01      50 	ldm	r13,4(fp),#2
006c 0004  
006e a9f7            51 	inc	fp,#~L2
0070 9e08            52 	ret
                     53 	~L1 := 0
                     54 	~L2 := 8
                     55 	}	/* _puts */
                     56 
                     57 .data
                     58 
