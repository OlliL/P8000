LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~fopen:*/
                      5 
                      6 .code
                      7 
                      8 _fopen::
                      9 {
0000 abf7            10 	dec	fp,#~L2
0002 1df6            11 	ldl	@fp,rr6
0004 5f00  0000*     12 	callr	__findiop
0008 6ff2  0006      13 	ld	~L1+6(fp),r2
000c 61f7  0002      14 	ld	r7,~L1+2(fp)
0010 61f6  0000      15 	ld	r6,~L1(fp)
0014 a125            16 	ld	r5,r2
0016 5f00  0000*     17 	callr	__endopen
001a a9f7            18 	inc	fp,#~L2
001c 9e08            19 	ret
                     20 	~L1 := 0
                     21 	~L2 := 8
                     22 	}	/* _fopen */
                     23 
                     24 .data
                     25 
