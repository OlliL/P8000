LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 L21:
                      5 .=.+20
                      6 /*~~_filbuf:*/
                      7 
                      8 .code
                      9 
                     10 __filbuf::
                     11 {
0000 abf5            12 	dec	fp,#~L2
0002 6ffe  0004      13 	ld	4(fp),r14
0006 a17e            14 	ld	r14,r7
0008 60ea  0006      15 	ldb	rl2,6(r14)
000c b120            16 	extsb	r2
000e a727            17 	bit	r2,#7
0010 e602            18 	jpr	eq,L22
0012 64e0  0006      19 	setb	6(r14),#0
                     20 L22:
0016 60ea  0006      21 	ldb	rl2,6(r14)
001a b120            22 	extsb	r2
001c 0702  0001      23 	and	r2,#1
0020 8d24            24 	test	r2
0022 e605            25 	jpr	eq,L10003
0024 60ea  0006      26 	ldb	rl2,6(r14)
0028 b120            27 	extsb	r2
002a a726            28 	bit	r2,#6
002c e60d            29 	jpr	eq,L24
                     30 L10003:
002e 2102  ffff      31 	ld	r2,#-1
0032 e853            32 	jpr	L10000
                     33 L26:
0034 2107  0200      34 	ld	r7,#512
0038 5f00  0000*     35 	callr	_malloc
003c 6fe2  0004      36 	ld	4(r14),r2
0040 8d24            37 	test	r2
0042 ee12            38 	jpr	ne,L27
0044 64e2  0006      39 	setb	6(r14),#2
                     40 L24:
0048 4de4  0004      41 	test	4(r14)
004c ee0f            42 	jpr	ne,L25
004e 60ea  0006      43 	ldb	rl2,6(r14)
0052 b120            44 	extsb	r2
0054 a722            45 	bit	r2,#2
0056 e6ee            46 	jpr	eq,L26
0058 60ea  0007      47 	ldb	rl2,7(r14)
005c b120            48 	extsb	r2
005e 0102  0000'     49 	add	r2,#L21
0062 6fe2  0004      50 	ld	4(r14),r2
0066 e8f0            51 	jpr	L24
                     52 L27:
0068 64e3  0006      53 	setb	6(r14),#3
                     54 L25:
006c 61e2  0004      55 	ld	r2,4(r14)
0070 2fe2            56 	ld	@r14,r2
0072 60ed  0006      57 	ldb	rl5,6(r14)
0076 b150            58 	extsb	r5
0078 a752            59 	bit	r5,#2
007a e602            60 	jpr	eq,L10004
007c bd51            61 	ldk	r5,#1
007e e802            62 	jpr	L10005
                     63 L10004:
0080 2105  0200      64 	ld	r5,#512
                     65 L10005:
0084 21e6            66 	ld	r6,@r14
0086 60ef  0007      67 	ldb	rl7,7(r14)
008a b170            68 	extsb	r7
008c 5f00  0000*     69 	callr	_read
0090 6fe2  0002      70 	ld	2(r14),r2
0094 6be0  0002      71 	dec	2(r14),#1
0098 4de1  0002      72 	cp	2(r14),#0
009c 0000  
009e e913            73 	jpr	ge,L29
00a0 4de1  0002      74 	cp	2(r14),#-1
00a4 ffff  
00a6 ee0a            75 	jpr	ne,L30
00a8 64e4  0006      76 	setb	6(r14),#4
00ac 60ea  0006      77 	ldb	rl2,6(r14)
00b0 b120            78 	extsb	r2
00b2 a727            79 	bit	r2,#7
00b4 e605            80 	jpr	eq,L32
00b6 62e0  0006      81 	resb	6(r14),#0
00ba e802            82 	jpr	L32
                     83 L30:
00bc 64e5  0006      84 	setb	6(r14),#5
                     85 L32:
00c0 4de8  0002      86 	clr	2(r14)
00c4 e8b4            87 	jpr	L10003
                     88 L29:
00c6 21e2            89 	ld	r2,@r14
00c8 202b            90 	ldb	rl3,@r2
00ca 8233            91 	subb	rh3,rh3
00cc 0703  00ff      92 	and	r3,#255
00d0 6ff3  0002      93 	ld	~L1+2(fp),r3
00d4 29e0            94 	inc	@r14,#1
00d6 61f2  0002      95 	ld	r2,~L1+2(fp)
                     96 L10000:
00da 61fe  0004      97 	ld	r14,4(fp)
00de a9f5            98 	inc	fp,#~L2
00e0 9e08            99 	ret
                    100 	~L1 := 0
                    101 	~L2 := 6
                    102 	}	/* __filbuf */
                    103 
                    104 .data
                    105 
