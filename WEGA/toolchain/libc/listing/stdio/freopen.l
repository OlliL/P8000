LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~freopen:*/
                      5 
                      6 .code
                      7 
                      8 _freopen::
                      9 {
0000 abf9            10 	dec	fp,#~L2
0002 6ffe  0008      11 	ld	8(fp),r14
0006 5df6  0002      12 	ldl	~L1+2(fp),rr6
000a a15e            13 	ld	r14,r5
000c a157            14 	ld	r7,r5
000e 5f00  0000*     15 	callr	_fclose
0012 61f7  0004      16 	ld	r7,~L1+4(fp)
0016 61f6  0002      17 	ld	r6,~L1+2(fp)
001a a1e5            18 	ld	r5,r14
001c 5f00  0000*     19 	callr	__endopen
0020 61fe  0008      20 	ld	r14,8(fp)
0024 a9f9            21 	inc	fp,#~L2
0026 9e08            22 	ret
                     23 	~L1 := 0
                     24 	~L2 := 10
                     25 	}	/* _freopen */
                     26 
                     27 .data
                     28 
