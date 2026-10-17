LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~strpbrk:*/
                      5 
                      6 .code
                      7 
                      8 _strpbrk::
                      9 {
0000 abfb            10 	dec	fp,#~L2
0002 a163            11 	ld	r3,r6
0004 a171            12 	ld	r1,r7
                     13 L14:
0006 a134            14 	ld	r4,r3
0008 e804            15 	jpr	L17
                     16 L20001:
000a 204a            17 	ldb	rl2,@r4
000c 0a1a            18 	cpb	rl2,@r1
000e e603            19 	jpr	eq,L16
0010 a940            20 	inc	r4,#1
                     21 L17:
0012 0c44            22 	testb	@r4
0014 eefa            23 	jpr	ne,L20001
                     24 L16:
0016 0c44            25 	testb	@r4
0018 e602            26 	jpr	eq,L13
001a a112            27 	ld	r2,r1
001c e805            28 	jpr	L10000
                     29 L13:
001e a112            30 	ld	r2,r1
0020 a910            31 	inc	r1,#1
0022 0c24            32 	testb	@r2
0024 eef0            33 	jpr	ne,L14
0026 8322            34 	sub	r2,r2
                     35 L10000:
0028 a9fb            36 	inc	fp,#~L2
002a 9e08            37 	ret
                     38 	~L1 := 0
                     39 	~L2 := 12
                     40 	}	/* _strpbrk */
                     41 
                     42 .data
                     43 
