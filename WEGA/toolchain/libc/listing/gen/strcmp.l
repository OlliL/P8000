LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~strcmp:*/
                      5 
                      6 .code
                      7 
                      8 _strcmp::
                      9 {
0000 abf9            10 	dec	fp,#~L2
0002 a164            11 	ld	r4,r6
0004 a171            12 	ld	r1,r7
                     13 L12:
0006 a142            14 	ld	r2,r4
0008 a940            15 	inc	r4,#1
000a 202b            16 	ldb	rl3,@r2
000c 0a1b            17 	cpb	rl3,@r1
000e ee07            18 	jpr	ne,L13
0010 a112            19 	ld	r2,r1
0012 a910            20 	inc	r1,#1
0014 202b            21 	ldb	rl3,@r2
0016 8cb4            22 	testb	rl3
0018 eef6            23 	jpr	ne,L12
001a 8322            24 	sub	r2,r2
001c e806            25 	jpr	L10000
                     26 L13:
001e 201a            27 	ldb	rl2,@r1
0020 b120            28 	extsb	r2
0022 ab40            29 	dec	r4,#1
0024 204b            30 	ldb	rl3,@r4
0026 b130            31 	extsb	r3
0028 8332            32 	sub	r2,r3
                     33 L10000:
002a a9f9            34 	inc	fp,#~L2
002c 9e08            35 	ret
                     36 	~L1 := 0
                     37 	~L2 := 10
                     38 	}	/* _strcmp */
                     39 
                     40 .data
                     41 
