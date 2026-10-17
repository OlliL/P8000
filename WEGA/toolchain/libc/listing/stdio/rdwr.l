LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~fread:*/
                      5 
                      6 .code
                      7 
                      8 _fread::
                      9 {
0000 030f  0014      10 	sub	fp,#~L2
0004 5cf9  0c02      11 	ldm	14(fp),r12,#3
0008 000e  
000a 5cf9  0501      12 	ldm	~L1+2(fp),r5,#2
000e 0002  
0010 a14d            13 	ld	r13,r4
0012 a17e            14 	ld	r14,r7
0014 4df8  0008      15 	clr	~L1+8(fp)
0018 8d64            16 	test	r6
001a ee09            17 	jpr	ne,L23
001c e824            18 	jpr	L20
                     19 L20001:
001e a1c2            20 	ld	r2,r12
0020 2eea            21 	ldb	@r14,rl2
0022 a9e0            22 	inc	r14,#1
0024 6bf0  000a      23 	dec	~L1+10(fp),#1
0028 ee0b            24 	jpr	ne,L26
002a 69f0  0008      25 	inc	~L1+8(fp),#1
                     26 L23:
002e 61f2  0008      27 	ld	r2,~L1+8(fp)
0032 4bf2  0002      28 	cp	r2,~L1+2(fp)
0036 ef17            29 	jpr	uge,L20
0038 61f3  0004      30 	ld	r3,~L1+4(fp)
003c 6ff3  000a      31 	ld	~L1+10(fp),r3
                     32 L26:
0040 6bd0  0002      33 	dec	2(r13),#1
0044 4dd1  0002      34 	cp	2(r13),#0
0048 0000  
004a e904            35 	jpr	ge,L10003
004c a1d7            36 	ld	r7,r13
004e 5f00  0000*     37 	callr	__filbuf
0052 e805            38 	jpr	L10004
                     39 L10003:
0054 21d2            40 	ld	r2,@r13
0056 29d0            41 	inc	@r13,#1
0058 202b            42 	ldb	rl3,@r2
005a 8233            43 	subb	rh3,rh3
005c a132            44 	ld	r2,r3
                     45 L10004:
005e a12c            46 	ld	r12,r2
0060 0b02  0000      47 	cp	r2,#0
0064 e9dc            48 	jpr	ge,L20001
                     49 L20:
0066 61f2  0008      50 	ld	r2,~L1+8(fp)
006a 5cf1  0c02      51 	ldm	r12,14(fp),#3
006e 000e  
0070 010f  0014      52 	add	fp,#~L2
0074 9e08            53 	ret
                     54 	~L1 := 0
                     55 	~L2 := 20
                     56 	}	/* _fread */
                     57 
                     58 .data
                     59 
                     60 /*~~fwrite:*/
                     61 
                     62 .code
                     63 
                     64 _fwrite::
                     65 {
0076 030f  0012      66 	sub	fp,#~L2
007a 5cf9  0c02      67 	ldm	12(fp),r12,#3
007e 000c  
0080 5cf9  0501      68 	ldm	~L1+2(fp),r5,#2
0084 0002  
0086 a14d            69 	ld	r13,r4
0088 a17e            70 	ld	r14,r7
008a 4df8  0008      71 	clr	~L1+8(fp)
008e 8d64            72 	test	r6
0090 ee24            73 	jpr	ne,L35
0092 e828            74 	jpr	L32
                     75 L20003:
0094 61fc  0004      76 	ld	r12,~L1+4(fp)
                     77 L38:
0098 6bd0  0002      78 	dec	2(r13),#1
009c 4dd1  0002      79 	cp	2(r13),#0
00a0 0000  
00a2 e909            80 	jpr	ge,L10008
00a4 a1e7            81 	ld	r7,r14
00a6 a9e0            82 	inc	r14,#1
00a8 207e            83 	ldb	rl6,@r7
00aa b160            84 	extsb	r6
00ac a167            85 	ld	r7,r6
00ae a1d6            86 	ld	r6,r13
00b0 5f00  0000*     87 	callr	__flsbuf
00b4 e808            88 	jpr	L37
                     89 L10008:
00b6 a1e2            90 	ld	r2,r14
00b8 a9e0            91 	inc	r14,#1
00ba 21d3            92 	ld	r3,@r13
00bc 29d0            93 	inc	@r13,#1
00be 202c            94 	ldb	rl4,@r2
00c0 2e3c            95 	ldb	@r3,rl4
00c2 8244            96 	subb	rh4,rh4
00c4 a142            97 	ld	r2,r4
                     98 L37:
00c6 fc98            99 	djnz	r12,L38
00c8 60da  0006     100 	ldb	rl2,6(r13)
00cc b120           101 	extsb	r2
00ce 0702  0020     102 	and	r2,#32
00d2 8d24           103 	test	r2
00d4 ee07           104 	jpr	ne,L32
00d6 69f0  0008     105 	inc	~L1+8(fp),#1
                    106 L35:
00da 61f2  0008     107 	ld	r2,~L1+8(fp)
00de 4bf2  0002     108 	cp	r2,~L1+2(fp)
00e2 e7d8           109 	jpr	ult,L20003
                    110 L32:
00e4 61f2  0008     111 	ld	r2,~L1+8(fp)
00e8 5cf1  0c02     112 	ldm	r12,12(fp),#3
00ec 000c  
00ee 010f  0012     113 	add	fp,#~L2
00f2 9e08           114 	ret
                    115 	~L1 := 0
                    116 	~L2 := 18
                    117 	}	/* _fwrite */
                    118 
                    119 .data
                    120 
