LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 .even
                      5 L12:
0000 0000             6 .word	0
                      7 .even
                      8 L13:
0002 0000             9 .word	0
                     10 L17:
0004 6d  6f  6e      11 .byte	%6d,%6f,%6e,%2e,%6f,%75,%74,%0
0007 2e  6f  75  
000a 74  00  
                     12 .even
                     13 /*~~monitor:*/
                     14 
                     15 .code
                     16 
                     17 _monitor::
                     18 {
0000 abfb            19 	dec	fp,#~L2
0002 6ffe  000a      20 	ld	10(fp),r14
0006 1cf9  0304      21 	ldm	@fp,r3,#5
000a 8d74            22 	test	r7
000c ee19            23 	jpr	ne,L14
000e 8377            24 	sub	r7,r7
0010 8366            25 	sub	r6,r6
0012 8355            26 	sub	r5,r5
0014 8344            27 	sub	r4,r4
0016 5f00  0000*     28 	callr	_profil
001a 2107  0004'     29 	ld	r7,#L17
001e 2106  01b6      30 	ld	r6,#438
0022 5f00  0000*     31 	callr	_creat
0026 a12e            32 	ld	r14,r2
0028 6105  0002'     33 	ld	r5,L13
002c 8155            34 	add	r5,r5
002e 6106  0000'     35 	ld	r6,L12
0032 a127            36 	ld	r7,r2
0034 5f00  0000*     37 	callr	_write
0038 a1e7            38 	ld	r7,r14
003a 5f00  0000*     39 	callr	_close
003e e855            40 	jpr	L10000
                     41 L14:
0040 61f2  0002      42 	ld	r2,~L1+2(fp)
0044 6f02  0002'     43 	ld	L13,r2
0048 61f3  0004      44 	ld	r3,~L1+4(fp)
004c 61f4  0008      45 	ld	r4,~L1+8(fp)
0050 2f34            46 	ld	@r3,r4
0052 61f3  0004      47 	ld	r3,~L1+4(fp)
0056 61f4  0006      48 	ld	r4,~L1+6(fp)
005a 6f34  0002      49 	ld	2(r3),r4
005e 61f3  0004      50 	ld	r3,~L1+4(fp)
0062 61f4  0000      51 	ld	r4,~L1(fp)
0066 6f34  0004      52 	ld	4(r3),r4
006a 61f4  0004      53 	ld	r4,~L1+4(fp)
006e 6f04  0000'     54 	ld	L12,r4
0072 61f5  0000      55 	ld	r5,~L1(fp)
0076 a950            56 	inc	r5,#1
0078 a157            57 	ld	r7,r5
007a 1906  0003      58 	mult	rr6,#3
007e 8177            59 	add	r7,r7
0080 41f7  0004      60 	add	r7,~L1+4(fp)
0084 6ff7  0004      61 	ld	~L1+4(fp),r7
0088 61f5  0000      62 	ld	r5,~L1(fp)
008c a950            63 	inc	r5,#1
008e a157            64 	ld	r7,r5
0090 1906  0003      65 	mult	rr6,#3
0094 61f5  0002      66 	ld	r5,~L1+2(fp)
0098 8375            67 	sub	r5,r7
009a 6ff5  0002      68 	ld	~L1+2(fp),r5
009e 0b05  0000      69 	cp	r5,#0
00a2 e223            70 	jpr	le,L10000
00a4 61f2  0006      71 	ld	r2,~L1+6(fp)
00a8 43f2  0008      72 	sub	r2,~L1+8(fp)
00ac b329  ffff      73 	sra	r2,#1
00b0 0702  7fff      74 	and	r2,#32767
00b4 a12e            75 	ld	r14,r2
00b6 4bfe  0002      76 	cp	r14,~L1+2(fp)
00ba e20a            77 	jpr	le,L21
00bc a157            78 	ld	r7,r5
00be b16a            79 	exts	rr6
00c0 b36d  000f      80 	slal	rr6,#15
00c4 a123            81 	ld	r3,r2
00c6 b12a            82 	exts	rr2
00c8 b147            83 	extsl	rq4
00ca 9a24            84 	divl	rq4,rr2
00cc a17e            85 	ld	r14,r7
00ce e802            86 	jpr	L22
                     87 L21:
00d0 210e  7fff      88 	ld	r14,#32767
                     89 L22:
00d4 61f6  0002      90 	ld	r6,~L1+2(fp)
00d8 8166            91 	add	r6,r6
00da a1e4            92 	ld	r4,r14
00dc 81e4            93 	add	r4,r14
00de 61f5  0008      94 	ld	r5,~L1+8(fp)
00e2 61f7  0004      95 	ld	r7,~L1+4(fp)
00e6 5f00  0000*     96 	callr	_profil
                     97 L10000:
00ea 61fe  000a      98 	ld	r14,10(fp)
00ee a9fb            99 	inc	fp,#~L2
00f0 9e08           100 	ret
                    101 	~L1 := 0
                    102 	~L2 := 12
                    103 	}	/* _monitor */
                    104 
                    105 .data
                    106 
