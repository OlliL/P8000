LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~sleep:*/
                      5 
                      6 .code
                      7 
                      8 _sleep::
                      9 {
0000 abfd            10 	dec	fp,#~L2
0002 2ff7            11 	ld	@fp,r7
0004 8d74            12 	test	r7
0006 ee02            13 	jpr	ne,L15
0008 8322            14 	sub	r2,r2
000a e85c            15 	jpr	L10000
                     16 L15:
000c 8377            17 	sub	r7,r7
000e 5f00  0000*     18 	callr	_alarm
0012 6ff2  0008      19 	ld	~L1+8(fp),r2
0016 bd7e            20 	ldk	r7,#14
0018 2106  00c8'     21 	ld	r6,#_awake
001c 5f00  0000*     22 	callr	_signal
0020 6ff2  0004      23 	ld	~L1+4(fp),r2
0024 4df8  0002      24 	clr	~L1+2(fp)
0028 4df8  000a      25 	clr	~L1+10(fp)
002c 4df4  0008      26 	test	~L1+8(fp)
0030 e620            27 	jpr	eq,L16
0032 61f2  0008      28 	ld	r2,~L1+8(fp)
0036 4bf2  0000      29 	cp	r2,~L1(fp)
003a e308            30 	jpr	ule,L17
003c a123            31 	ld	r3,r2
003e 43f3  0000      32 	sub	r3,~L1(fp)
0042 6ff3  0008      33 	ld	~L1+8(fp),r3
0046 69f0  0002      34 	inc	~L1+2(fp),#1
004a e813            35 	jpr	L16
                     36 L17:
004c 61f2  0000      37 	ld	r2,~L1(fp)
0050 43f2  0008      38 	sub	r2,~L1+8(fp)
0054 6ff2  000a      39 	ld	~L1+10(fp),r2
0058 61f2  0008      40 	ld	r2,~L1+8(fp)
005c 6ff2  0000      41 	ld	~L1(fp),r2
0060 4df8  0008      42 	clr	~L1+8(fp)
0064 6bf0  0002      43 	dec	~L1+2(fp),#1
0068 bd7e            44 	ldk	r7,#14
006a 61f6  0004      45 	ld	r6,~L1+4(fp)
006e 5f00  0000*     46 	callr	_signal
                     47 L16:
0072 61f7  0000      48 	ld	r7,~L1(fp)
0076 5f00  0000*     49 	callr	_alarm
007a 5f00  0000*     50 	callr	_pause
007e 8377            51 	sub	r7,r7
0080 5f00  0000*     52 	callr	_alarm
0084 6ff2  0006      53 	ld	~L1+6(fp),r2
0088 4df1  0002      54 	cp	~L1+2(fp),#0
008c 0000  
008e e105            55 	jpr	lt,L20
0090 bd7e            56 	ldk	r7,#14
0092 61f6  0004      57 	ld	r6,~L1+4(fp)
0096 5f00  0000*     58 	callr	_signal
                     59 L20:
009a 4df1  0002      60 	cp	~L1+2(fp),#0
009e 0000  
00a0 ea07            61 	jpr	gt,L10003
00a2 4df1  0002      62 	cp	~L1+2(fp),#0
00a6 0000  
00a8 e909            63 	jpr	ge,L21
00aa 4df4  0006      64 	test	~L1+6(fp)
00ae e606            65 	jpr	eq,L21
                     66 L10003:
00b0 61f7  0008      67 	ld	r7,~L1+8(fp)
00b4 41f7  0006      68 	add	r7,~L1+6(fp)
00b8 5f00  0000*     69 	callr	_alarm
                     70 L21:
00bc 61f2  000a      71 	ld	r2,~L1+10(fp)
00c0 41f2  0006      72 	add	r2,~L1+6(fp)
                     73 L10000:
00c4 a9fd            74 	inc	fp,#~L2
00c6 9e08            75 	ret
                     76 	~L1 := 0
                     77 	~L2 := 14
                     78 	}	/* _sleep */
                     79 
                     80 .data
                     81 
                     82 
                     83 .code
                     84 
                     85 _awake:
                     86 {
00c8 9e08            87 	ret
                     88 	~L1 := 0
                     89 	~L2 := 0
                     90 	}	/* _awake */
                     91 
                     92 .data
                     93 
