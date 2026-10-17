LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~_strout:*/
                      5 
                      6 .code
                      7 
                      8 __strout::
                      9 {
0000 abff            10 	dec	fp,#~L2
0002 5cf9  0c02      11 	ldm	10(fp),r12,#3
0006 000a  
0008 1cf9  0302      12 	ldm	@fp,r3,#3
000c a14c            13 	ld	r12,r4
000e a16d            14 	ld	r13,r6
0010 a17e            15 	ld	r14,r7
0012 e81e            16 	jpr	L20
                     17 L10003:
0014 a1e2            18 	ld	r2,r14
0016 a9e0            19 	inc	r14,#1
0018 21c3            20 	ld	r3,@r12
001a 29c0            21 	inc	@r12,#1
001c 202c            22 	ldb	rl4,@r2
001e 2e3c            23 	ldb	@r3,rl4
0020 8244            24 	subb	rh4,rh4
0022 a142            25 	ld	r2,r4
                     26 L10004:
0024 abd0            27 	dec	r13,#1
                     28 L22:
0026 6bc0  0002      29 	dec	2(r12),#1
002a 4dc1  0002      30 	cp	2(r12),#0
002e 0000  
0030 e906            31 	jpr	ge,L10005
0032 61f7  0000      32 	ld	r7,~L1(fp)
0036 a1c6            33 	ld	r6,r12
0038 5f00  0000*     34 	callr	__flsbuf
003c e807            35 	jpr	L10006
                     36 L10005:
003e 21c2            37 	ld	r2,@r12
0040 29c0            38 	inc	@r12,#1
0042 60fb  0001      39 	ldb	rl3,~L1+1(fp)
0046 2e2b            40 	ldb	@r2,rl3
0048 8233            41 	subb	rh3,rh3
004a a132            42 	ld	r2,r3
                     43 L10006:
004c 69f0  0004      44 	inc	~L1+4(fp),#1
                     45 L20:
0050 4df1  0004      46 	cp	~L1+4(fp),#0
0054 0000  
0056 e92d            47 	jpr	ge,L24
0058 0ce1  2d2d      48 	cpb	@r14,#45
005c eee4            49 	jpr	ne,L22
005e 4df1  0000      50 	cp	~L1(fp),#48
0062 0030  
0064 eee0            51 	jpr	ne,L22
0066 6bc0  0002      52 	dec	2(r12),#1
006a 4dc1  0002      53 	cp	2(r12),#0
006e 0000  
0070 e9d1            54 	jpr	ge,L10003
0072 a1e7            55 	ld	r7,r14
0074 a9e0            56 	inc	r14,#1
0076 207e            57 	ldb	rl6,@r7
0078 b160            58 	extsb	r6
007a a167            59 	ld	r7,r6
007c a1c6            60 	ld	r6,r12
007e 5f00  0000*     61 	callr	__flsbuf
0082 e8d0            62 	jpr	L10004
                     63 L20003:
0084 a1e7            64 	ld	r7,r14
0086 a9e0            65 	inc	r14,#1
0088 207e            66 	ldb	rl6,@r7
008a b160            67 	extsb	r6
008c a167            68 	ld	r7,r6
008e a1c6            69 	ld	r6,r12
0090 5f00  0000*     70 	callr	__flsbuf
0094 e80e            71 	jpr	L24
                     72 L20005:
0096 6bc0  0002      73 	dec	2(r12),#1
009a 4dc1  0002      74 	cp	2(r12),#0
009e 0000  
00a0 e1f1            75 	jpr	lt,L20003
00a2 a1e2            76 	ld	r2,r14
00a4 a9e0            77 	inc	r14,#1
00a6 21c3            78 	ld	r3,@r12
00a8 29c0            79 	inc	@r12,#1
00aa 202c            80 	ldb	rl4,@r2
00ac 2e3c            81 	ldb	@r3,rl4
00ae 8244            82 	subb	rh4,rh4
00b0 a142            83 	ld	r2,r4
                     84 L24:
00b2 abd0            85 	dec	r13,#1
00b4 0b0d  0000      86 	cp	r13,#0
00b8 e9ee            87 	jpr	ge,L20005
00ba e815            88 	jpr	L26
                     89 L20007:
00bc 6bc0  0002      90 	dec	2(r12),#1
00c0 4dc1  0002      91 	cp	2(r12),#0
00c4 0000  
00c6 e906            92 	jpr	ge,L10009
00c8 61f7  0000      93 	ld	r7,~L1(fp)
00cc a1c6            94 	ld	r6,r12
00ce 5f00  0000*     95 	callr	__flsbuf
00d2 e807            96 	jpr	L10010
                     97 L10009:
00d4 21c2            98 	ld	r2,@r12
00d6 29c0            99 	inc	@r12,#1
00d8 60fb  0001     100 	ldb	rl3,~L1+1(fp)
00dc 2e2b           101 	ldb	@r2,rl3
00de 8233           102 	subb	rh3,rh3
00e0 a132           103 	ld	r2,r3
                    104 L10010:
00e2 6bf0  0004     105 	dec	~L1+4(fp),#1
                    106 L26:
00e6 4df4  0004     107 	test	~L1+4(fp)
00ea eee8           108 	jpr	ne,L20007
00ec 5cf1  0c02     109 	ldm	r12,10(fp),#3
00f0 000a  
00f2 a9ff           110 	inc	fp,#~L2
00f4 9e08           111 	ret
                    112 	~L1 := 0
                    113 	~L2 := 16
                    114 	}	/* __strout */
                    115 
                    116 .data
                    117 
