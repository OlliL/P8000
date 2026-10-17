LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~getpwuid:*/
                      5 
                      6 .code
                      7 
                      8 _getpwuid::
                      9 {
0000 abf7            10 	dec	fp,#~L2
0002 5cf9  0d01      11 	ldm	4(fp),r13,#2
0006 0004  
0008 a17e            12 	ld	r14,r7
000a 5f00  0000*     13 	callr	_setpwent
                     14 L14:
000e 5f00  0000*     15 	callr	_getpwent
0012 a12d            16 	ld	r13,r2
0014 8d24            17 	test	r2
0016 e603            18 	jpr	eq,L15
0018 4bde  0004      19 	cp	r14,4(r13)
001c eef8            20 	jpr	ne,L14
                     21 L15:
001e 5f00  0000*     22 	callr	_endpwent
0022 a1d2            23 	ld	r2,r13
0024 5cf1  0d01      24 	ldm	r13,4(fp),#2
0028 0004  
002a a9f7            25 	inc	fp,#~L2
002c 9e08            26 	ret
                     27 	~L1 := 0
                     28 	~L2 := 8
                     29 	}	/* _getpwuid */
                     30 
                     31 .data
                     32 
