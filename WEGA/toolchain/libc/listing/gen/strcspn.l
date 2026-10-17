LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~strcspn:*/
                      5 
                      6 .code
                      7 
                      8 _strcspn::
                      9 {
0000 abfb            10 	dec	fp,#~L2
0002 6ff7  0002      11 	ld	~L1+2(fp),r7
0006 a161            12 	ld	r1,r6
0008 a174            13 	ld	r4,r7
000a e80b            14 	jpr	L14
                     15 L20003:
000c a113            16 	ld	r3,r1
000e e804            17 	jpr	L17
                     18 L20001:
0010 203a            19 	ldb	rl2,@r3
0012 0a4a            20 	cpb	rl2,@r4
0014 e603            21 	jpr	eq,L16
0016 a930            22 	inc	r3,#1
                     23 L17:
0018 0c34            24 	testb	@r3
001a eefa            25 	jpr	ne,L20001
                     26 L16:
001c 0c34            27 	testb	@r3
001e ee03            28 	jpr	ne,L13
0020 a940            29 	inc	r4,#1
                     30 L14:
0022 0c44            31 	testb	@r4
0024 eef3            32 	jpr	ne,L20003
                     33 L13:
0026 a142            34 	ld	r2,r4
0028 43f2  0002      35 	sub	r2,~L1+2(fp)
002c a9fb            36 	inc	fp,#~L2
002e 9e08            37 	ret
                     38 	~L1 := 0
                     39 	~L2 := 12
                     40 	}	/* _strcspn */
                     41 
                     42 .data
                     43 
