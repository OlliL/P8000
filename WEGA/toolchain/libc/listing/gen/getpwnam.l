LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~getpwnam:*/
                      5 
                      6 .code
                      7 
                      8 _getpwnam::
                      9 {
0000 abf5            10 	dec	fp,#~L2
0002 6ffe  0004      11 	ld	4(fp),r14
0006 2ff7            12 	ld	@fp,r7
0008 5f00  0000*     13 	callr	_setpwent
                     14 L15:
000c 5f00  0000*     15 	callr	_getpwent
0010 a12e            16 	ld	r14,r2
0012 8d24            17 	test	r2
0014 e607            18 	jpr	eq,L16
0016 61f7  0000      19 	ld	r7,~L1(fp)
001a 21e6            20 	ld	r6,@r14
001c 5f00  0000*     21 	callr	_strcmp
0020 8d24            22 	test	r2
0022 eef4            23 	jpr	ne,L15
                     24 L16:
0024 5f00  0000*     25 	callr	_endpwent
0028 a1e2            26 	ld	r2,r14
002a 61fe  0004      27 	ld	r14,4(fp)
002e a9f5            28 	inc	fp,#~L2
0030 9e08            29 	ret
                     30 	~L1 := 0
                     31 	~L2 := 6
                     32 	}	/* _getpwnam */
                     33 
                     34 .data
                     35 
