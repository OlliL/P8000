LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~strncmp:*/
                      5 
                      6 .code
                      7 
                      8 _strncmp::
                      9 {
0000 abfd            10 	dec	fp,#~L2
0002 6ffc  0008      11 	ld	8(fp),r12
0006 a15c            12 	ld	r12,r5
0008 a164            13 	ld	r4,r6
000a a171            14 	ld	r1,r7
                     15 L12:
000c abc0            16 	dec	r12,#1
000e 0b0c  0000      17 	cp	r12,#0
0012 e10c            18 	jpr	lt,L13
0014 a142            19 	ld	r2,r4
0016 a940            20 	inc	r4,#1
0018 202b            21 	ldb	rl3,@r2
001a 0a1b            22 	cpb	rl3,@r1
001c ee07            23 	jpr	ne,L13
001e a112            24 	ld	r2,r1
0020 a910            25 	inc	r1,#1
0022 202b            26 	ldb	rl3,@r2
0024 8cb4            27 	testb	rl3
0026 eef2            28 	jpr	ne,L12
                     29 L20000:
0028 8322            30 	sub	r2,r2
002a e809            31 	jpr	L10000
                     32 L13:
002c 0b0c  0000      33 	cp	r12,#0
0030 e1fb            34 	jpr	lt,L20000
0032 201a            35 	ldb	rl2,@r1
0034 b120            36 	extsb	r2
0036 ab40            37 	dec	r4,#1
0038 204b            38 	ldb	rl3,@r4
003a b130            39 	extsb	r3
003c 8332            40 	sub	r2,r3
                     41 L10000:
003e 61fc  0008      42 	ld	r12,8(fp)
0042 a9fd            43 	inc	fp,#~L2
0044 9e08            44 	ret
                     45 	~L1 := 0
                     46 	~L2 := 14
                     47 	}	/* _strncmp */
                     48 
                     49 .data
                     50 
