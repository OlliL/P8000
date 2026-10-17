LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~getgrnam:*/
                      5 
                      6 .code
                      7 
                      8 _getgrnam::
                      9 {
0000 abf7            10 	dec	fp,#~L2
0002 5cf9  0d01      11 	ldm	4(fp),r13,#2
0006 0004  
0008 a17e            12 	ld	r14,r7
000a 5f00  0000*     13 	callr	_setgrent
                     14 L15:
000e 5f00  0000*     15 	callr	_getgrent
0012 a12d            16 	ld	r13,r2
0014 8d24            17 	test	r2
0016 e606            18 	jpr	eq,L16
0018 21d7            19 	ld	r7,@r13
001a a1e6            20 	ld	r6,r14
001c 5f00  0000*     21 	callr	_strcmp
0020 8d24            22 	test	r2
0022 eef5            23 	jpr	ne,L15
                     24 L16:
0024 5f00  0000*     25 	callr	_endgrent
0028 a1d2            26 	ld	r2,r13
002a 5cf1  0d01      27 	ldm	r13,4(fp),#2
002e 0004  
0030 a9f7            28 	inc	fp,#~L2
0032 9e08            29 	ret
                     30 	~L1 := 0
                     31 	~L2 := 8
                     32 	}	/* _getgrnam */
                     33 
                     34 .data
                     35 
