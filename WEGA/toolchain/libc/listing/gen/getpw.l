LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 .even
                      5 L20:
0000 0000             6 .word	0
                      7 L22:
0002 2f  65  74       8 .byte	%2f,%65,%74,%63,%2f,%70,%61,%73
0005 63  2f  70  
0008 61  73  
000a 73  77  64       9 .byte	%73,%77,%64,%0
000d 00  
                     10 .even
                     11 L23:
000e 72  00          12 .byte	%72,%0
                     13 .even
                     14 /*~~getpw:*/
                     15 
                     16 .code
                     17 
                     18 _getpw::
                     19 {
0000 abfb            20 	dec	fp,#~L2
0002 5cf9  0c02      21 	ldm	6(fp),r12,#3
0006 0006  
0008 1df6            22 	ldl	@fp,rr6
000a 4d04  0000'     23 	test	L20
000e ee08            24 	jpr	ne,L21
0010 2107  0002'     25 	ld	r7,#L22
0014 2106  000e'     26 	ld	r6,#L23
0018 5f00  0000*     27 	callr	_fopen
001c 6f02  0000'     28 	ld	L20,r2
                     29 L21:
0020 4d04  0000'     30 	test	L20
0024 ee02            31 	jpr	ne,L24
                     32 L20002:
0026 bd21            33 	ldk	r2,#1
0028 e853            34 	jpr	L10000
                     35 L24:
002a 6107  0000'     36 	ld	r7,L20
002e 5f00  0000*     37 	callr	_rewind
                     38 L28:
0032 61fc  0000      39 	ld	r12,~L1(fp)
0036 e806            40 	jpr	L30
                     41 L20001:
0038 0b0d  ffff      42 	cp	r13,#-1
003c e6f4            43 	jpr	eq,L20002
003e a1d2            44 	ld	r2,r13
0040 2eca            45 	ldb	@r12,rl2
0042 a9c0            46 	inc	r12,#1
                     47 L30:
0044 6102  0000'     48 	ld	r2,L20
0048 6b20  0002      49 	dec	2(r2),#1
004c 6123  0002      50 	ld	r3,2(r2)
0050 0b03  0000      51 	cp	r3,#0
0054 e905            52 	jpr	ge,L10003
0056 6107  0000'     53 	ld	r7,L20
005a 5f00  0000*     54 	callr	__filbuf
005e e807            55 	jpr	L10004
                     56 L10003:
0060 6102  0000'     57 	ld	r2,L20
0064 2920            58 	inc	@r2,#1
0066 2123            59 	ld	r3,@r2
0068 ab30            60 	dec	r3,#1
006a 203a            61 	ldb	rl2,@r3
006c 8222            62 	subb	rh2,rh2
                     63 L10004:
006e a12d            64 	ld	r13,r2
0070 0b02  000a      65 	cp	r2,#10
0074 eee1            66 	jpr	ne,L20001
0076 0cc8            67 	clrb	@r12
0078 a9c0            68 	inc	r12,#1
007a 61fc  0000      69 	ld	r12,~L1(fp)
007e bde3            70 	ldk	r14,#3
                     71 L33:
0080 abe0            72 	dec	r14,#1
0082 e60c            73 	jpr	eq,L38
                     74 L35:
0084 a1c2            75 	ld	r2,r12
0086 a9c0            76 	inc	r12,#1
0088 202b            77 	ldb	rl3,@r2
008a b130            78 	extsb	r3
008c a13d            79 	ld	r13,r3
008e 0b03  003a      80 	cp	r3,#58
0092 e6f6            81 	jpr	eq,L33
0094 0b03  000a      82 	cp	r3,#10
0098 eef5            83 	jpr	ne,L35
009a e8c5            84 	jpr	L20002
                     85 L38:
009c a1c2            86 	ld	r2,r12
009e a9c0            87 	inc	r12,#1
00a0 202b            88 	ldb	rl3,@r2
00a2 b130            89 	extsb	r3
00a4 a13d            90 	ld	r13,r3
00a6 0b03  003a      91 	cp	r3,#58
00aa e60e            92 	jpr	eq,L39
00ac 0b03  0030      93 	cp	r3,#48
00b0 e1f5            94 	jpr	lt,L38
00b2 0b03  0039      95 	cp	r3,#57
00b6 eaf2            96 	jpr	gt,L38
00b8 a1e3            97 	ld	r3,r14
00ba 1902  000a      98 	mult	rr2,#10
00be 81d3            99 	add	r3,r13
00c0 0303  0030     100 	sub	r3,#48
00c4 a13e           101 	ld	r14,r3
00c6 e8ea           102 	jpr	L38
                    103 L39:
00c8 4bfe  0002     104 	cp	r14,~L1+2(fp)
00cc eeb2           105 	jpr	ne,L28
00ce 8322           106 	sub	r2,r2
                    107 L10000:
00d0 5cf1  0c02     108 	ldm	r12,6(fp),#3
00d4 0006  
00d6 a9fb           109 	inc	fp,#~L2
00d8 9e08           110 	ret
                    111 	~L1 := 0
                    112 	~L2 := 12
                    113 	}	/* _getpw */
                    114 
                    115 .data
                    116 
