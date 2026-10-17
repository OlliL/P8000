LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~toupper:*/
                      5 
                      6 .code
                      7 
                      8 _toupper::
                      9 {
0000 abf5            10 	dec	fp,#~L2
0002 6ffe  0004      11 	ld	4(fp),r14
0006 a17e            12 	ld	r14,r7
0008 0b07  0061      13 	cp	r7,#97
000c e105            14 	jpr	lt,L12
000e 0b07  007a      15 	cp	r7,#122
0012 ea02            16 	jpr	gt,L12
0014 010e  ffe0      17 	add	r14,#-32
                     18 L12:
0018 a1e2            19 	ld	r2,r14
001a 61fe  0004      20 	ld	r14,4(fp)
001e a9f5            21 	inc	fp,#~L2
0020 9e08            22 	ret
                     23 	~L1 := 0
                     24 	~L2 := 6
                     25 	}	/* _toupper */
                     26 
                     27 .data
                     28 
