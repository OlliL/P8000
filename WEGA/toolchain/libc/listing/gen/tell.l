LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~tell:*/
                      5 
                      6 .code
                      7 
                      8 _tell::
                      9 {
0000 abf5            10 	dec	fp,#~L2
0002 2ff7            11 	ld	@fp,r7
0004 a176            12 	ld	r6,r7
0006 9244            13 	subl	rr4,rr4
0008 bd31            14 	ldk	r3,#1
000a 5f00  0000*     15 	callr	_lseek
000e a9f5            16 	inc	fp,#~L2
0010 9e08            17 	ret
                     18 	~L1 := 0
                     19 	~L2 := 6
                     20 	}	/* _tell */
                     21 
                     22 .data
                     23 
