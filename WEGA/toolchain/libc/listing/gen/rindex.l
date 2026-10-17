LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~rindex:*/
                      5 
                      6 .code
                      7 
                      8 _rindex::
                      9 {
0000 abf9            10 	dec	fp,#~L2
0002 2ff6            11 	ld	@fp,r6
0004 a171            12 	ld	r1,r7
0006 bd30            13 	ldk	r3,#0
                     14 L14:
0008 201a            15 	ldb	rl2,@r1
000a 4afa  0001      16 	cpb	rl2,~L1+1(fp)
000e ee01            17 	jpr	ne,L13
0010 a113            18 	ld	r3,r1
                     19 L13:
0012 a112            20 	ld	r2,r1
0014 a910            21 	inc	r1,#1
0016 0c24            22 	testb	@r2
0018 eef7            23 	jpr	ne,L14
001a a132            24 	ld	r2,r3
001c a9f9            25 	inc	fp,#~L2
001e 9e08            26 	ret
                     27 	~L1 := 0
                     28 	~L2 := 10
                     29 	}	/* _rindex */
                     30 
                     31 .data
                     32 
