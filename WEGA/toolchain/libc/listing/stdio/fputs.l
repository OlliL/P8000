LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~fputs:*/
                      5 
                      6 .code
                      7 
                      8 _fputs::
                      9 {
0000 abfd            10 	dec	fp,#~L2
0002 5cf9  0b03      11 	ldm	6(fp),r11,#4
0006 0006  
0008 a16d            12 	ld	r13,r6
000a a17e            13 	ld	r14,r7
000c e811            14 	jpr	L20
                     15 L20001:
000e 6bd0  0002      16 	dec	2(r13),#1
0012 4dd1  0002      17 	cp	2(r13),#0
0016 0000  
0018 e905            18 	jpr	ge,L10003
001a a1b7            19 	ld	r7,r11
001c a1d6            20 	ld	r6,r13
001e 5f00  0000*     21 	callr	__flsbuf
0022 e805            22 	jpr	L10004
                     23 L10003:
0024 a1b2            24 	ld	r2,r11
0026 21d3            25 	ld	r3,@r13
0028 29d0            26 	inc	@r13,#1
002a 2e3a            27 	ldb	@r3,rl2
002c 8222            28 	subb	rh2,rh2
                     29 L10004:
002e a12c            30 	ld	r12,r2
                     31 L20:
0030 a1e2            32 	ld	r2,r14
0032 a9e0            33 	inc	r14,#1
0034 202b            34 	ldb	rl3,@r2
0036 b130            35 	extsb	r3
0038 a13b            36 	ld	r11,r3
003a 8d34            37 	test	r3
003c eee8            38 	jpr	ne,L20001
003e a1c2            39 	ld	r2,r12
0040 5cf1  0b03      40 	ldm	r11,6(fp),#4
0044 0006  
0046 a9fd            41 	inc	fp,#~L2
0048 9e08            42 	ret
                     43 	~L1 := 0
                     44 	~L2 := 14
                     45 	}	/* _fputs */
                     46 
                     47 .data
                     48 
