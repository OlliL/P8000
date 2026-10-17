LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 .even
                      5 L28:
0000 0061             6 .word	97
0002 0072             7 .word	114
0004 0077             8 .word	119
                      9 .even
                     10 /*~~fdopen:*/
                     11 
                     12 .code
                     13 
                     14 _fdopen::
                     15 {
                     16 	.data
                     17 	.even
                     18 L29:
0006 0072'           19 	.word	L24
0008 0042'           20 	.word	L23
000a 007e'           21 	.word	L26
                     22 	.code
0000 abf9            23 	dec	fp,#~L2
0002 5cf9  0d01      24 	ldm	6(fp),r13,#2
0006 0006  
0008 6ff7  0002      25 	ld	~L1+2(fp),r7
000c a16e            26 	ld	r14,r6
000e 5f00  0000*     27 	callr	__findiop
0012 a12d            28 	ld	r13,r2
0014 8d24            29 	test	r2
0016 e636            30 	jpr	eq,L27
0018 4dd8  0002      31 	clr	2(r13)
001c 60fa  0003      32 	ldb	rl2,~L1+3(fp)
0020 6eda  0007      33 	ldb	7(r13),rl2
0024 20eb            34 	ldb	rl3,@r14
0026 b130            35 	extsb	r3
0028 a132            36 	ld	r2,r3
002a 2103  0000'     37 	ld	r3,#L28
002e 2104  0003      38 	ld	r4,#3
0032 bb34  0426      39 	cpir	r2,@r3,r4,eq
0036 ee26            40 	jpr	ne,L27
0038 8144            41 	add	r4,r4
003a 8d42            42 	neg	r4
003c 6142  000a'     43 	ld	r2,L29+4(r4)
0040 1e28            44 	jp	@r2
                     45 L23:
0042 64d0  0006      46 	setb	6(r13),#0
                     47 L21:
0046 4ce1  0001      48 	cpb	1(r14),#43
004a 2b2b  
004c e604            49 	jpr	eq,L10003
004e 4ce1  0001      50 	cpb	1(r14),#119
0052 7777  
0054 ee0c            51 	jpr	ne,L30
                     52 L10003:
0056 60da  0006      53 	ldb	rl2,6(r13)
005a 060a  fcfc      54 	andb	rl2,#-4
005e 6eda  0006      55 	ldb	6(r13),rl2
0062 60da  0006      56 	ldb	rl2,6(r13)
0066 040a  8080      57 	orb	rl2,#-128
006a 6eda  0006      58 	ldb	6(r13),rl2
                     59 L30:
006e a1d2            60 	ld	r2,r13
0070 e80a            61 	jpr	L10000
                     62 L24:
0072 61f7  0002      63 	ld	r7,~L1+2(fp)
0076 9244            64 	subl	rr4,rr4
0078 bd32            65 	ldk	r3,#2
007a 5f00  0000*     66 	callr	_lseek
                     67 L26:
007e 64d1  0006      68 	setb	6(r13),#1
0082 e8e1            69 	jpr	L21
                     70 L27:
0084 8322            71 	sub	r2,r2
                     72 L10000:
0086 5cf1  0d01      73 	ldm	r13,6(fp),#2
008a 0006  
008c a9f9            74 	inc	fp,#~L2
008e 9e08            75 	ret
                     76 	~L1 := 0
                     77 	~L2 := 10
                     78 	}	/* _fdopen */
                     79 
                     80 .data
                     81 
