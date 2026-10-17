LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~strcpy:*/
                      5 
                      6 .code
                      7 
                      8 _strcpy::
                      9 {
0000 abfb            10 	dec	fp,#~L2
0002 6ffc  0006      11 	ld	6(fp),r12
0006 a165            12 	ld	r5,r6
0008 a171            13 	ld	r1,r7
000a a17c            14 	ld	r12,r7
                     15 L12:
000c a152            16 	ld	r2,r5
000e a950            17 	inc	r5,#1
0010 a113            18 	ld	r3,r1
0012 a910            19 	inc	r1,#1
0014 202c            20 	ldb	rl4,@r2
0016 2e3c            21 	ldb	@r3,rl4
0018 8cc4            22 	testb	rl4
001a eef8            23 	jpr	ne,L12
001c a1c2            24 	ld	r2,r12
001e 61fc  0006      25 	ld	r12,6(fp)
0022 a9fb            26 	inc	fp,#~L2
0024 9e08            27 	ret
                     28 	~L1 := 0
                     29 	~L2 := 12
                     30 	}	/* _strcpy */
                     31 
                     32 .data
                     33 
