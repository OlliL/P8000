LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~readsym:*/
                      5 
                      6 .code
                      7 
                      8 _readsym::
                      9 {
0000 abf9            10 	dec	fp,#~L2
0002 6ffe  0008      11 	ld	8(fp),r14
0006 1df6            12 	ldl	@fp,rr6
0008 a172            13 	ld	r2,r7
000a bd6e            14 	ldk	r6,#14
000c bd51            15 	ldk	r5,#1
000e 61f4  0000      16 	ld	r4,~L1(fp)
0012 5f00  0000*     17 	callr	_fread
0016 0b02  0001      18 	cp	r2,#1
001a e603            19 	jpr	eq,L21
001c 2102  ffff      20 	ld	r2,#65535
0020 e828            21 	jpr	L10000
                     22 L21:
0022 61f2  0002      23 	ld	r2,~L1+2(fp)
0026 602b  0006      24 	ldb	rl3,6(r2)
002a b130            25 	extsb	r3
002c a737            26 	bit	r3,#7
002e ee02            27 	jpr	ne,L22
0030 8322            28 	sub	r2,r2
0032 e81f            29 	jpr	L10000
                     30 L22:
0034 61f2  0002      31 	ld	r2,~L1+2(fp)
0038 602b  0006      32 	ldb	rl3,6(r2)
003c b130            33 	extsb	r3
003e 0703  007f      34 	and	r3,#127
0042 ab37            35 	dec	r3,#8
0044 6efb  0005      36 	ldb	~L1+5(fp),rl3
0048 b130            37 	extsb	r3
004a a93c            38 	inc	r3,#13
004c a135            39 	ld	r5,r3
004e b14a            40 	exts	rr4
0050 1b04  000e      41 	div	rr4,#14
0054 a15e            42 	ld	r14,r5
0056 69fd  0002      43 	inc	~L1+2(fp),#14
005a 61f7  0002      44 	ld	r7,~L1+2(fp)
005e bd6e            45 	ldk	r6,#14
0060 61f4  0000      46 	ld	r4,~L1(fp)
0064 5f00  0000*     47 	callr	_fread
0068 8b2e            48 	cp	r14,r2
006a e602            49 	jpr	eq,L23
006c 210e  ffff      50 	ld	r14,#65535
                     51 L23:
0070 a1e2            52 	ld	r2,r14
                     53 L10000:
0072 61fe  0008      54 	ld	r14,8(fp)
0076 a9f9            55 	inc	fp,#~L2
0078 9e08            56 	ret
                     57 	~L1 := 0
                     58 	~L2 := 10
                     59 	}	/* _readsym */
                     60 
                     61 .data
                     62 
