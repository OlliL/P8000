LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~strncpy:*/
                      5 
                      6 .code
                      7 
                      8 _strncpy::
                      9 {
0000 abff            10 	dec	fp,#~L2
0002 5cf9  0b02      11 	ldm	8(fp),r11,#3
0006 0008  
0008 2ff5            12 	ld	@fp,r5
000a a16d            13 	ld	r13,r6
000c a171            14 	ld	r1,r7
000e a17b            15 	ld	r11,r7
0010 bdc0            16 	ldk	r12,#0
0012 e811            17 	jpr	L14
                     18 L20003:
0014 a1d2            19 	ld	r2,r13
0016 a9d0            20 	inc	r13,#1
0018 a113            21 	ld	r3,r1
001a a910            22 	inc	r1,#1
001c 202c            23 	ldb	rl4,@r2
001e 2e3c            24 	ldb	@r3,rl4
0020 8cc4            25 	testb	rl4
0022 e603            26 	jpr	eq,L16
0024 e807            27 	jpr	L12
                     28 L20001:
0026 0c18            29 	clrb	@r1
0028 a910            30 	inc	r1,#1
                     31 L16:
002a a9c0            32 	inc	r12,#1
002c 4bfc  0000      33 	cp	r12,~L1(fp)
0030 e1fa            34 	jpr	lt,L20001
0032 e804            35 	jpr	L20004
                     36 L12:
0034 a9c0            37 	inc	r12,#1
                     38 L14:
0036 4bfc  0000      39 	cp	r12,~L1(fp)
003a e1ec            40 	jpr	lt,L20003
                     41 L20004:
003c a1b2            42 	ld	r2,r11
003e 5cf1  0b02      43 	ldm	r11,8(fp),#3
0042 0008  
0044 a9ff            44 	inc	fp,#~L2
0046 9e08            45 	ret
                     46 	~L1 := 0
                     47 	~L2 := 16
                     48 	}	/* _strncpy */
                     49 
                     50 .data
                     51 
