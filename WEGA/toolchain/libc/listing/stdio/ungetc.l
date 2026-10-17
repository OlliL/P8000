LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~ungetc:*/
                      5 
                      6 .code
                      7 
                      8 _ungetc::
                      9 {
0000 abf7            10 	dec	fp,#~L2
0002 6ffe  0006      11 	ld	6(fp),r14
0006 6ff7  0002      12 	ld	~L1+2(fp),r7
000a a16e            13 	ld	r14,r6
000c 0b07  ffff      14 	cp	r7,#-1
0010 e61c            15 	jpr	eq,L22
0012 60ea  0006      16 	ldb	rl2,6(r14)
0016 b120            17 	extsb	r2
0018 0702  0001      18 	and	r2,#1
001c 8d24            19 	test	r2
001e e604            20 	jpr	eq,L10003
0020 21e2            21 	ld	r2,@r14
0022 4be2  0004      22 	cp	r2,4(r14)
0026 eb08            23 	jpr	ugt,L21
                     24 L10003:
0028 21e2            25 	ld	r2,@r14
002a 4be2  0004      26 	cp	r2,4(r14)
002e ee0d            27 	jpr	ne,L22
0030 4de4  0002      28 	test	2(r14)
0034 ee0a            29 	jpr	ne,L22
0036 29e0            30 	inc	@r14,#1
                     31 L21:
0038 69e0  0002      32 	inc	2(r14),#1
003c 2be0            33 	dec	@r14,#1
003e 21e2            34 	ld	r2,@r14
0040 60fb  0003      35 	ldb	rl3,~L1+3(fp)
0044 2e2b            36 	ldb	@r2,rl3
0046 8322            37 	sub	r2,r2
0048 e802            38 	jpr	L10000
                     39 L22:
004a 2102  ffff      40 	ld	r2,#-1
                     41 L10000:
004e 61fe  0006      42 	ld	r14,6(fp)
0052 a9f7            43 	inc	fp,#~L2
0054 9e08            44 	ret
                     45 	~L1 := 0
                     46 	~L2 := 8
                     47 	}	/* _ungetc */
                     48 
                     49 .data
                     50 
