LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~clearerr:*/
                      5 
                      6 .code
                      7 
                      8 _clearerr::
                      9 {
0000 abf3            10 	dec	fp,#~L2
0002 6ffe  0002      11 	ld	2(fp),r14
0006 a17e            12 	ld	r14,r7
0008 60ea  0006      13 	ldb	rl2,6(r14)
000c 060a  cfcf      14 	andb	rl2,#-49
0010 6eea  0006      15 	ldb	6(r14),rl2
0014 61fe  0002      16 	ld	r14,2(fp)
0018 a9f3            17 	inc	fp,#~L2
001a 9e08            18 	ret
                     19 	~L1 := 0
                     20 	~L2 := 4
                     21 	}	/* _clearerr */
                     22 
                     23 .data
                     24 
