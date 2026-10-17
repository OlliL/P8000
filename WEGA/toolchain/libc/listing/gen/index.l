LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~index:*/
                      5 
                      6 .code
                      7 
                      8 _index::
                      9 {
0000 abf7            10 	dec	fp,#~L2
0002 6ffe  0006      11 	ld	6(fp),r14
0006 2ff6            12 	ld	@fp,r6
0008 a17e            13 	ld	r14,r7
                     14 L14:
000a 20ea            15 	ldb	rl2,@r14
000c 4afa  0001      16 	cpb	rl2,~L1+1(fp)
0010 ee02            17 	jpr	ne,L13
0012 a1e2            18 	ld	r2,r14
0014 e805            19 	jpr	L10000
                     20 L13:
0016 a1e2            21 	ld	r2,r14
0018 a9e0            22 	inc	r14,#1
001a 0c24            23 	testb	@r2
001c eef6            24 	jpr	ne,L14
001e 8322            25 	sub	r2,r2
                     26 L10000:
0020 61fe  0006      27 	ld	r14,6(fp)
0024 a9f7            28 	inc	fp,#~L2
0026 9e08            29 	ret
                     30 	~L1 := 0
                     31 	~L2 := 8
                     32 	}	/* _index */
                     33 
                     34 .data
                     35 
