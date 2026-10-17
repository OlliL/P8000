LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~strcat:*/
                      5 
                      6 .code
                      7 
                      8 _strcat::
                      9 {
0000 abfb            10 	dec	fp,#~L2
0002 6ffc  0006      11 	ld	6(fp),r12
0006 a165            12 	ld	r5,r6
0008 a171            13 	ld	r1,r7
000a a17c            14 	ld	r12,r7
                     15 L12:
000c a112            16 	ld	r2,r1
000e a910            17 	inc	r1,#1
0010 0c24            18 	testb	@r2
0012 eefc            19 	jpr	ne,L12
0014 ab10            20 	dec	r1,#1
                     21 L14:
0016 a152            22 	ld	r2,r5
0018 a950            23 	inc	r5,#1
001a a113            24 	ld	r3,r1
001c a910            25 	inc	r1,#1
001e 202c            26 	ldb	rl4,@r2
0020 2e3c            27 	ldb	@r3,rl4
0022 8cc4            28 	testb	rl4
0024 eef8            29 	jpr	ne,L14
0026 a1c2            30 	ld	r2,r12
0028 61fc  0006      31 	ld	r12,6(fp)
002c a9fb            32 	inc	fp,#~L2
002e 9e08            33 	ret
                     34 	~L1 := 0
                     35 	~L2 := 12
                     36 	}	/* _strcat */
                     37 
                     38 .data
                     39 
