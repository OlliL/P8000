LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~ftell:*/
                      5 
                      6 .code
                      7 
                      8 _ftell::
                      9 {
0000 abfb            10 	dec	fp,#~L2
0002 6ffe  000a      11 	ld	10(fp),r14
0006 2ff7            12 	ld	@fp,r7
0008 a176            13 	ld	r6,r7
000a 4d61  0002      14 	cp	2(r6),#0
000e 0000  
0010 e902            15 	jpr	ge,L20
0012 4d78  0002      16 	clr	2(r7)
                     17 L20:
0016 61f2  0000      18 	ld	r2,~L1(fp)
001a 602b  0006      19 	ldb	rl3,6(r2)
001e b130            20 	extsb	r3
0020 a730            21 	bit	r3,#0
0022 e62f            22 	jpr	eq,L21
0024 6123  0002      23 	ld	r3,2(r2)
0028 8d32            24 	neg	r3
002a a13e            25 	ld	r14,r3
002c e819            26 	jpr	L22
                     27 L20001:
002e bde0            28 	ldk	r14,#0
0030 61f2  0000      29 	ld	r2,~L1(fp)
0034 602b  0006      30 	ldb	rl3,6(r2)
0038 b130            31 	extsb	r3
003a a731            32 	bit	r3,#1
003c e611            33 	jpr	eq,L22
003e 4d24  0004      34 	test	4(r2)
0042 e60e            35 	jpr	eq,L22
0044 602b  0006      36 	ldb	rl3,6(r2)
0048 b130            37 	extsb	r3
004a 0703  0004      38 	and	r3,#4
004e 8d34            39 	test	r3
0050 ee07            40 	jpr	ne,L22
0052 61f2  0000      41 	ld	r2,~L1(fp)
0056 a123            42 	ld	r3,r2
0058 2134            43 	ld	r4,@r3
005a 4324  0004      44 	sub	r4,4(r2)
005e a14e            45 	ld	r14,r4
                     46 L22:
0060 61f7  0000      47 	ld	r7,~L1(fp)
0064 607e  0007      48 	ldb	rl6,7(r7)
0068 a0ef            49 	ldb	rl7,rl6
006a b170            50 	extsb	r7
006c 9244            51 	subl	rr4,rr4
006e bd31            52 	ldk	r3,#1
0070 5f00  0000*     53 	callr	_lseek
0074 5df2  0002      54 	ldl	~L1+2(fp),rr2
0078 1002  0000      55 	cpl	rr2,#0
007c 0000  
007e e90d            56 	jpr	ge,L26
0080 e812            57 	jpr	L20002
                     58 L21:
0082 61f2  0000      59 	ld	r2,~L1(fp)
0086 602b  0006      60 	ldb	rl3,6(r2)
008a b130            61 	extsb	r3
008c 0703  0082      62 	and	r3,#130
0090 eece            63 	jpr	ne,L20001
0092 1402  ffff      64 	ldl	rr2,#-1
0096 ffff  
0098 e808            65 	jpr	L10000
                     66 L26:
009a a1e3            67 	ld	r3,r14
009c b12a            68 	exts	rr2
009e 56f2  0002      69 	addl	rr2,~L1+2(fp)
00a2 5df2  0002      70 	ldl	~L1+2(fp),rr2
                     71 L20002:
00a6 54f2  0002      72 	ldl	rr2,~L1+2(fp)
                     73 L10000:
00aa 61fe  000a      74 	ld	r14,10(fp)
00ae a9fb            75 	inc	fp,#~L2
00b0 9e08            76 	ret
                     77 	~L1 := 0
                     78 	~L2 := 12
                     79 	}	/* _ftell */
                     80 
                     81 .data
                     82 
