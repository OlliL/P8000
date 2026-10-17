LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~_length:*/
                      5 
                      6 .code
                      7 
                      8 __length::
                      9 {
0000 abf7            10 	dec	fp,#~L2
0002 a171            11 	ld	r1,r7
0004 bd30            12 	ldk	r3,#0
                     13 L14:
0006 a112            14 	ld	r2,r1
0008 a910            15 	inc	r1,#1
000a 0c24            16 	testb	@r2
000c e604            17 	jpr	eq,L12
000e a930            18 	inc	r3,#1
0010 0b03  4000      19 	cp	r3,#16384
0014 e2f8            20 	jpr	le,L14
                     21 L12:
0016 a132            22 	ld	r2,r3
0018 a9f7            23 	inc	fp,#~L2
001a 9e08            24 	ret
                     25 	~L1 := 0
                     26 	~L2 := 8
                     27 	}	/* __length */
                     28 
                     29 .data
                     30 
