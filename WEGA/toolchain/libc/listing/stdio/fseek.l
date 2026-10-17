LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~fseek:*/
                      5 
                      6 .code
                      7 
                      8 _fseek::
                      9 {
0000 030f  0014      10 	sub	fp,#~L2
0004 5cf9  0d01      11 	ldm	16(fp),r13,#2
0008 0010  
000a 1cf9  0302      12 	ldm	@fp,r3,#3
000e a17e            13 	ld	r14,r7
0010 62e4  0006      14 	resb	6(r14),#4
0014 60ea  0006      15 	ldb	rl2,6(r14)
0018 b120            16 	extsb	r2
001a a720            17 	bit	r2,#0
001c e663            18 	jpr	eq,L21
001e 0b03  0002      19 	cp	r3,#2
0022 e948            20 	jpr	ge,L22
0024 4de4  0004      21 	test	4(r14)
0028 e645            22 	jpr	eq,L22
002a 60ea  0006      23 	ldb	rl2,6(r14)
002e b120            24 	extsb	r2
0030 a722            25 	bit	r2,#2
0032 ee40            26 	jpr	ne,L22
0034 61ed  0002      27 	ld	r13,2(r14)
0038 54f2  0002      28 	ldl	rr2,~L1+2(fp)
003c 5df2  000a      29 	ldl	~L1+10(fp),rr2
0040 4df4  0000      30 	test	~L1(fp)
0044 ee0f            31 	jpr	ne,L23
0046 60ef  0007      32 	ldb	rl7,7(r14)
004a b170            33 	extsb	r7
004c 9244            34 	subl	rr4,rr4
004e bd31            35 	ldk	r3,#1
0050 5f00  0000*     36 	callr	_lseek
0054 a1d5            37 	ld	r5,r13
0056 b14a            38 	exts	rr4
0058 9224            39 	subl	rr4,rr2
005a 56f4  000a      40 	addl	rr4,~L1+10(fp)
005e 5df4  000a      41 	ldl	~L1+10(fp),rr4
0062 e807            42 	jpr	L24
                     43 L23:
0064 a1d3            44 	ld	r3,r13
0066 b12a            45 	exts	rr2
0068 54f4  0002      46 	ldl	rr4,~L1+2(fp)
006c 9224            47 	subl	rr4,rr2
006e 5df4  0002      48 	ldl	~L1+2(fp),rr4
                     49 L24:
0072 60ea  0006      50 	ldb	rl2,6(r14)
0076 b120            51 	extsb	r2
0078 a727            52 	bit	r2,#7
007a ee1c            53 	jpr	ne,L22
007c 0b0d  0000      54 	cp	r13,#0
0080 e219            55 	jpr	le,L22
0082 a1d3            56 	ld	r3,r13
0084 b12a            57 	exts	rr2
0086 50f2  000a      58 	cpl	rr2,~L1+10(fp)
008a e114            59 	jpr	lt,L22
008c 61e2  0004      60 	ld	r2,4(r14)
0090 03e2            61 	sub	r2,@r14
0092 a125            62 	ld	r5,r2
0094 b14a            63 	exts	rr4
0096 50f4  000a      64 	cpl	rr4,~L1+10(fp)
009a ea0c            65 	jpr	gt,L22
009c 61f2  000c      66 	ld	r2,~L1+12(fp)
00a0 01e2            67 	add	r2,@r14
00a2 2fe2            68 	ld	@r14,r2
00a4 61f2  000c      69 	ld	r2,~L1+12(fp)
00a8 61e3  0002      70 	ld	r3,2(r14)
00ac 8323            71 	sub	r3,r2
00ae 6fe3  0002      72 	ld	2(r14),r3
00b2 e841            73 	jpr	L10003
                     74 L22:
00b4 60ea  0006      75 	ldb	rl2,6(r14)
00b8 b120            76 	extsb	r2
00ba a727            77 	bit	r2,#7
00bc e605            78 	jpr	eq,L26
00be 61e2  0004      79 	ld	r2,4(r14)
00c2 2fe2            80 	ld	@r14,r2
00c4 62e0  0006      81 	resb	6(r14),#0
                     82 L26:
00c8 60ef  0007      83 	ldb	rl7,7(r14)
00cc b170            84 	extsb	r7
00ce 54f4  0002      85 	ldl	rr4,~L1+2(fp)
00d2 61f3  0000      86 	ld	r3,~L1(fp)
00d6 5f00  0000*     87 	callr	_lseek
00da 5df2  000a      88 	ldl	~L1+10(fp),rr2
00de 4de8  0002      89 	clr	2(r14)
00e2 e820            90 	jpr	L27
                     91 L21:
00e4 60ea  0006      92 	ldb	rl2,6(r14)
00e8 b120            93 	extsb	r2
00ea 0702  0082      94 	and	r2,#130
00ee e61a            95 	jpr	eq,L27
00f0 a1e7            96 	ld	r7,r14
00f2 5f00  0000*     97 	callr	_fflush
00f6 60ea  0006      98 	ldb	rl2,6(r14)
00fa b120            99 	extsb	r2
00fc a727           100 	bit	r2,#7
00fe e607           101 	jpr	eq,L30
0100 4de8  0002     102 	clr	2(r14)
0104 62e1  0006     103 	resb	6(r14),#1
0108 61e2  0004     104 	ld	r2,4(r14)
010c 2fe2           105 	ld	@r14,r2
                    106 L30:
010e 60ef  0007     107 	ldb	rl7,7(r14)
0112 b170           108 	extsb	r7
0114 54f4  0002     109 	ldl	rr4,~L1+2(fp)
0118 61f3  0000     110 	ld	r3,~L1(fp)
011c 5f00  0000*    111 	callr	_lseek
0120 5df2  000a     112 	ldl	~L1+10(fp),rr2
                    113 L27:
0124 54f2  000a     114 	ldl	rr2,~L1+10(fp)
0128 1002  ffff     115 	cpl	rr2,#-1
012c ffff  
012e ee03           116 	jpr	ne,L10003
0130 2102  ffff     117 	ld	r2,#-1
0134 e801           118 	jpr	L10000
                    119 L10003:
0136 8322           120 	sub	r2,r2
                    121 L10000:
0138 5cf1  0d01     122 	ldm	r13,16(fp),#2
013c 0010  
013e 010f  0014     123 	add	fp,#~L2
0142 9e08           124 	ret
                    125 	~L1 := 0
                    126 	~L2 := 20
                    127 	}	/* _fseek */
                    128 
                    129 .data
                    130 
