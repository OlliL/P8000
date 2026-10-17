LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~ltol3:*/
                      5 
                      6 .code
                      7 
                      8 _ltol3::
                      9 {
0000 abfb            10 	dec	fp,#~L2
0002 6ff6  0002      11 	ld	~L1+2(fp),r6
0006 a153            12 	ld	r3,r5
0008 a171            13 	ld	r1,r7
000a a164            14 	ld	r4,r6
000c e80d            15 	jpr	L12
                     16 L20001:
000e a940            17 	inc	r4,#1
0010 204a            18 	ldb	rl2,@r4
0012 2e1a            19 	ldb	@r1,rl2
0014 a940            20 	inc	r4,#1
0016 a910            21 	inc	r1,#1
0018 204a            22 	ldb	rl2,@r4
001a 2e1a            23 	ldb	@r1,rl2
001c a940            24 	inc	r4,#1
001e a910            25 	inc	r1,#1
0020 204a            26 	ldb	rl2,@r4
0022 2e1a            27 	ldb	@r1,rl2
0024 a940            28 	inc	r4,#1
0026 a910            29 	inc	r1,#1
                     30 L12:
0028 a132            31 	ld	r2,r3
002a ab30            32 	dec	r3,#1
002c 8d24            33 	test	r2
002e eeef            34 	jpr	ne,L20001
0030 a9fb            35 	inc	fp,#~L2
0032 9e08            36 	ret
                     37 	~L1 := 0
                     38 	~L2 := 12
                     39 	}	/* _ltol3 */
                     40 
                     41 .data
                     42 
                     43 /*~~l3tol:*/
                     44 
                     45 .code
                     46 
                     47 _l3tol::
                     48 {
0034 abfb            49 	dec	fp,#~L2
0036 6ff7  0004      50 	ld	~L1+4(fp),r7
003a a153            51 	ld	r3,r5
003c a161            52 	ld	r1,r6
003e a174            53 	ld	r4,r7
0040 e80e            54 	jpr	L16
                     55 L20003:
0042 0c48            56 	clrb	@r4
0044 a940            57 	inc	r4,#1
0046 201a            58 	ldb	rl2,@r1
0048 2e4a            59 	ldb	@r4,rl2
004a a910            60 	inc	r1,#1
004c a940            61 	inc	r4,#1
004e 201a            62 	ldb	rl2,@r1
0050 2e4a            63 	ldb	@r4,rl2
0052 a910            64 	inc	r1,#1
0054 a940            65 	inc	r4,#1
0056 201a            66 	ldb	rl2,@r1
0058 2e4a            67 	ldb	@r4,rl2
005a a910            68 	inc	r1,#1
005c a940            69 	inc	r4,#1
                     70 L16:
005e a132            71 	ld	r2,r3
0060 ab30            72 	dec	r3,#1
0062 8d24            73 	test	r2
0064 eeee            74 	jpr	ne,L20003
0066 a9fb            75 	inc	fp,#~L2
0068 9e08            76 	ret
                     77 	~L1 := 0
                     78 	~L2 := 12
                     79 	}	/* _l3tol */
                     80 
                     81 .data
                     82 
