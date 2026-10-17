LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~strncat:*/
                      5 
                      6 .code
                      7 
                      8 _strncat::
                      9 {
0000 abff            10 	dec	fp,#~L2
0002 5cf9  0b02      11 	ldm	8(fp),r11,#3
0006 0008  
0008 a15c            12 	ld	r12,r5
000a a16d            13 	ld	r13,r6
000c a171            14 	ld	r1,r7
000e a17b            15 	ld	r11,r7
                     16 L12:
0010 a112            17 	ld	r2,r1
0012 a910            18 	inc	r1,#1
0014 0c24            19 	testb	@r2
0016 eefc            20 	jpr	ne,L12
0018 ab10            21 	dec	r1,#1
                     22 L14:
001a a1d2            23 	ld	r2,r13
001c a9d0            24 	inc	r13,#1
001e a113            25 	ld	r3,r1
0020 a910            26 	inc	r1,#1
0022 202c            27 	ldb	rl4,@r2
0024 2e3c            28 	ldb	@r3,rl4
0026 8cc4            29 	testb	rl4
0028 e606            30 	jpr	eq,L15
002a abc0            31 	dec	r12,#1
002c 0b0c  0000      32 	cp	r12,#0
0030 e9f4            33 	jpr	ge,L14
0032 ab10            34 	dec	r1,#1
0034 0c18            35 	clrb	@r1
                     36 L15:
0036 a1b2            37 	ld	r2,r11
0038 5cf1  0b02      38 	ldm	r11,8(fp),#3
003c 0008  
003e a9ff            39 	inc	fp,#~L2
0040 9e08            40 	ret
                     41 	~L1 := 0
                     42 	~L2 := 16
                     43 	}	/* _strncat */
                     44 
                     45 .data
                     46 
