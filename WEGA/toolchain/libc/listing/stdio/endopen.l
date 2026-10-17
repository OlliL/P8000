LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 .even
                      5 L34:
0000 0061             6 .word	97
0002 0072             7 .word	114
0004 0077             8 .word	119
                      9 .even
                     10 /*~~_endopen:*/
                     11 
                     12 .code
                     13 
                     14 __endopen::
                     15 {
                     16 	.data
                     17 	.even
                     18 L35:
0006 008a'           19 	.word	L27
0008 00c2'           20 	.word	L32
000a 0080'           21 	.word	L25
                     22 	.code
0000 abfd            23 	dec	fp,#~L2
0002 5cf9  0c02      24 	ldm	8(fp),r12,#3
0006 0008  
0008 5df6  0002      25 	ldl	~L1+2(fp),rr6
000c a15e            26 	ld	r14,r5
000e 8d54            27 	test	r5
0010 ee02            28 	jpr	ne,L21
                     29 L20001:
0012 8322            30 	sub	r2,r2
0014 e87c            31 	jpr	L10000
                     32 L21:
0016 61f2  0002      33 	ld	r2,~L1+2(fp)
001a 0c21  7272      34 	cpb	@r2,#114
001e ee04            35 	jpr	ne,L10004
0020 4c21  0001      36 	cpb	1(r2),#119
0024 7777  
0026 e609            37 	jpr	eq,L10003
                     38 L10004:
0028 61f2  0002      39 	ld	r2,~L1+2(fp)
002c 0c21  7777      40 	cpb	@r2,#119
0030 ee0d            41 	jpr	ne,L22
0032 4c21  0001      42 	cpb	1(r2),#114
0036 7272  
0038 ee09            43 	jpr	ne,L22
                     44 L10003:
003a 61f2  0002      45 	ld	r2,~L1+2(fp)
003e 0c25  7272      46 	ldb	@r2,#114
0042 61f2  0002      47 	ld	r2,~L1+2(fp)
0046 4c25  0001      48 	ldb	1(r2),#43
004a 2b2b  
                     49 L22:
004c 61f2  0002      50 	ld	r2,~L1+2(fp)
0050 4c21  0001      51 	cpb	1(r2),#43
0054 2b2b  
0056 bd30            52 	ldk	r3,#0
0058 af36            53 	tcc	eq,r3
005a b130            54 	extsb	r3
005c a13d            55 	ld	r13,r3
005e 61f2  0002      56 	ld	r2,~L1+2(fp)
0062 202c            57 	ldb	rl4,@r2
0064 b140            58 	extsb	r4
0066 a143            59 	ld	r3,r4
0068 2102  0000'     60 	ld	r2,#L34
006c 2104  0003      61 	ld	r4,#3
0070 bb24  0436      62 	cpir	r3,@r2,r4,eq
0074 eece            63 	jpr	ne,L20001
0076 8144            64 	add	r4,r4
0078 8d42            65 	neg	r4
007a 6143  000a'     66 	ld	r3,L35+4(r4)
007e 1e38            67 	jp	@r3
                     68 L25:
0080 61f7  0004      69 	ld	r7,~L1+4(fp)
0084 a1d6            70 	ld	r6,r13
0086 dfb8            71 	callr	_create
0088 e825            72 	jpr	L20000
                     73 L27:
008a 8dd4            74 	test	r13
008c e602            75 	jpr	eq,L10006
008e bd62            76 	ldk	r6,#2
0090 e801            77 	jpr	L10007
                     78 L10006:
0092 bd61            79 	ldk	r6,#1
                     80 L10007:
0094 61f7  0004      81 	ld	r7,~L1+4(fp)
0098 5f00  0000*     82 	callr	_open
009c a12c            83 	ld	r12,r2
009e 0b02  0000      84 	cp	r2,#0
00a2 e909            85 	jpr	ge,L29
00a4 4d01  0000*     86 	cp	_errno,#2
00a8 0002  
00aa ee05            87 	jpr	ne,L29
00ac 61f7  0004      88 	ld	r7,~L1+4(fp)
00b0 a1d6            89 	ld	r6,r13
00b2 dfce            90 	callr	_create
00b4 a12c            91 	ld	r12,r2
                     92 L29:
00b6 a1c7            93 	ld	r7,r12
00b8 9244            94 	subl	rr4,rr4
00ba bd32            95 	ldk	r3,#2
00bc 5f00  0000*     96 	callr	_lseek
00c0 e80a            97 	jpr	L23
                     98 L32:
00c2 8dd4            99 	test	r13
00c4 e602           100 	jpr	eq,L10008
00c6 bd62           101 	ldk	r6,#2
00c8 e801           102 	jpr	L10009
                    103 L10008:
00ca 8366           104 	sub	r6,r6
                    105 L10009:
00cc 61f7  0004     106 	ld	r7,~L1+4(fp)
00d0 5f00  0000*    107 	callr	_open
                    108 L20000:
00d4 a12c           109 	ld	r12,r2
                    110 L23:
00d6 0b0c  0000     111 	cp	r12,#0
00da e19b           112 	jpr	lt,L20001
00dc 4de8  0002     113 	clr	2(r14)
00e0 a1c2           114 	ld	r2,r12
00e2 6eea  0007     115 	ldb	7(r14),rl2
00e6 8dd4           116 	test	r13
00e8 e607           117 	jpr	eq,L37
00ea 60ea  0006     118 	ldb	rl2,6(r14)
00ee 040a  8080     119 	orb	rl2,#-128
00f2 6eea  0006     120 	ldb	6(r14),rl2
00f6 e80a           121 	jpr	L38
                    122 L37:
00f8 61f2  0002     123 	ld	r2,~L1+2(fp)
00fc 0c21  7272     124 	cpb	@r2,#114
0100 ee03           125 	jpr	ne,L39
0102 64e0  0006     126 	setb	6(r14),#0
0106 e802           127 	jpr	L38
                    128 L39:
0108 64e1  0006     129 	setb	6(r14),#1
                    130 L38:
010c a1e2           131 	ld	r2,r14
                    132 L10000:
010e 5cf1  0c02     133 	ldm	r12,8(fp),#3
0112 0008  
0114 a9fd           134 	inc	fp,#~L2
0116 9e08           135 	ret
                    136 	~L1 := 0
                    137 	~L2 := 14
                    138 	}	/* __endopen */
                    139 
                    140 .data
                    141 
                    142 
                    143 .code
                    144 
                    145 _create:
                    146 {
0118 abfb           147 	dec	fp,#~L2
011a 5cf9  0c02     148 	ldm	6(fp),r12,#3
011e 0006  
0120 2ff6           149 	ld	@fp,r6
0122 a17e           150 	ld	r14,r7
0124 8d64           151 	test	r6
0126 e620           152 	jpr	eq,L42
0128 8377           153 	sub	r7,r7
012a 5f00  0000*    154 	callr	_umask
012e a12d           155 	ld	r13,r2
0130 a1e7           156 	ld	r7,r14
0132 2106  01b6     157 	ld	r6,#438
0136 5f00  0000*    158 	callr	_creat
013a a12c           159 	ld	r12,r2
013c 0b02  0000     160 	cp	r2,#0
0140 e10f           161 	jpr	lt,L45
0142 a127           162 	ld	r7,r2
0144 5f00  0000*    163 	callr	_close
0148 a1e7           164 	ld	r7,r14
014a bd62           165 	ldk	r6,#2
014c 5f00  0000*    166 	callr	_open
0150 a12c           167 	ld	r12,r2
0152 a1d6           168 	ld	r6,r13
0154 8d60           169 	com	r6
0156 0706  01b6     170 	and	r6,#438
015a a1e7           171 	ld	r7,r14
015c 5f00  0000*    172 	callr	_chmod
                    173 L45:
0160 a1d7           174 	ld	r7,r13
0162 5f00  0000*    175 	callr	_umask
0166 e806           176 	jpr	L48
                    177 L42:
0168 a1e7           178 	ld	r7,r14
016a 2106  01b6     179 	ld	r6,#438
016e 5f00  0000*    180 	callr	_creat
0172 a12c           181 	ld	r12,r2
                    182 L48:
0174 a1c2           183 	ld	r2,r12
0176 5cf1  0c02     184 	ldm	r12,6(fp),#3
017a 0006  
017c a9fb           185 	inc	fp,#~L2
017e 9e08           186 	ret
                    187 	~L1 := 0
                    188 	~L2 := 12
                    189 	}	/* _create */
                    190 
                    191 .data
                    192 
