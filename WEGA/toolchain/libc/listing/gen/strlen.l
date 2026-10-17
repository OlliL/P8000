LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~strlen:*/
                      5 
                      6 .code
                      7 
                      8 _strlen::
                      9 {
0000 abf7            10 	dec	fp,#~L2
0002 a171            11 	ld	r1,r7
0004 bd30            12 	ldk	r3,#0
0006 e801            13 	jpr	L12
                     14 L20001:
0008 a930            15 	inc	r3,#1
                     16 L12:
000a a112            17 	ld	r2,r1
000c a910            18 	inc	r1,#1
000e 0c24            19 	testb	@r2
0010 eefb            20 	jpr	ne,L20001
0012 a132            21 	ld	r2,r3
0014 a9f7            22 	inc	fp,#~L2
0016 9e08            23 	ret
                     24 	~L1 := 0
                     25 	~L2 := 8
                     26 	}	/* _strlen */
                     27 
                     28 .data
                     29 
