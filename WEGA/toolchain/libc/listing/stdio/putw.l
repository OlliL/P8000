LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~putw:*/
                      5 
                      6 .code
                      7 
                      8 _putw::
                      9 {
0000 abf7            10 	dec	fp,#~L2
0002 5cf9  0d01      11 	ldm	4(fp),r13,#2
0006 0004  
0008 a16d            12 	ld	r13,r6
000a a17e            13 	ld	r14,r7
000c 6bd0  0002      14 	dec	2(r13),#1
0010 4dd1  0002      15 	cp	2(r13),#0
0014 0000  
0016 e907            16 	jpr	ge,L10003
0018 a1e7            17 	ld	r7,r14
001a a07f            18 	ldb	rl7,rh7
001c 8c78            19 	clrb	rh7
001e a1d6            20 	ld	r6,r13
0020 5f00  0000*     21 	callr	__flsbuf
0024 e807            22 	jpr	L10004
                     23 L10003:
0026 a1e2            24 	ld	r2,r14
0028 a02a            25 	ldb	rl2,rh2
002a b120            26 	extsb	r2
002c 21d3            27 	ld	r3,@r13
002e 29d0            28 	inc	@r13,#1
0030 2e3a            29 	ldb	@r3,rl2
0032 8222            30 	subb	rh2,rh2
                     31 L10004:
0034 6bd0  0002      32 	dec	2(r13),#1
0038 4dd1  0002      33 	cp	2(r13),#0
003c 0000  
003e e905            34 	jpr	ge,L10005
0040 a1e7            35 	ld	r7,r14
0042 a1d6            36 	ld	r6,r13
0044 5f00  0000*     37 	callr	__flsbuf
0048 e805            38 	jpr	L10000
                     39 L10005:
004a a1e2            40 	ld	r2,r14
004c 21d3            41 	ld	r3,@r13
004e 29d0            42 	inc	@r13,#1
0050 2e3a            43 	ldb	@r3,rl2
0052 8222            44 	subb	rh2,rh2
                     45 L10000:
0054 5cf1  0d01      46 	ldm	r13,4(fp),#2
0058 0004  
005a a9f7            47 	inc	fp,#~L2
005c 9e08            48 	ret
                     49 	~L1 := 0
                     50 	~L2 := 8
                     51 	}	/* _putw */
                     52 
                     53 .data
                     54 
