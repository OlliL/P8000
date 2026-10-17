LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~isatty:*/
                      5 
                      6 .code
                      7 
                      8 _isatty::
                      9 {
0000 abf9            10 	dec	fp,#~L2
0002 2ff7            11 	ld	@fp,r7
0004 76f6  0002      12 	lda	r6,~L1+2(fp)
0008 a172            13 	ld	r2,r7
000a 5f00  0000*     14 	callr	_gtty
000e 0b02  0000      15 	cp	r2,#0
0012 e902            16 	jpr	ge,L13
0014 8322            17 	sub	r2,r2
0016 e801            18 	jpr	L10000
                     19 L13:
0018 bd21            20 	ldk	r2,#1
                     21 L10000:
001a a9f9            22 	inc	fp,#~L2
001c 9e08            23 	ret
                     24 	~L1 := 0
                     25 	~L2 := 10
                     26 	}	/* _isatty */
                     27 
                     28 .data
                     29 
