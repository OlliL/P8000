LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~setbuf:*/
                      5 
                      6 .code
                      7 
                      8 _setbuf::
                      9 {
0000 abf5            10 	dec	fp,#~L2
0002 6ffe  0004      11 	ld	4(fp),r14
0006 2ff6            12 	ld	@fp,r6
0008 a17e            13 	ld	r14,r7
000a 4de4  0004      14 	test	4(r14)
000e e609            15 	jpr	eq,L20
0010 60ef  0006      16 	ldb	rl7,6(r14)
0014 b170            17 	extsb	r7
0016 a773            18 	bit	r7,#3
0018 e604            19 	jpr	eq,L20
001a 61e7  0004      20 	ld	r7,4(r14)
001e 5f00  0000*     21 	callr	_free
                     22 L20:
0022 60ea  0006      23 	ldb	rl2,6(r14)
0026 060a  f3f3      24 	andb	rl2,#-13
002a 6eea  0006      25 	ldb	6(r14),rl2
002e 61f2  0000      26 	ld	r2,~L1(fp)
0032 6fe2  0004      27 	ld	4(r14),r2
0036 8d24            28 	test	r2
0038 ee03            29 	jpr	ne,L22
003a 64e2  0006      30 	setb	6(r14),#2
003e e803            31 	jpr	L23
                     32 L22:
0040 61e2  0004      33 	ld	r2,4(r14)
0044 2fe2            34 	ld	@r14,r2
                     35 L23:
0046 4de8  0002      36 	clr	2(r14)
004a 61fe  0004      37 	ld	r14,4(fp)
004e a9f5            38 	inc	fp,#~L2
0050 9e08            39 	ret
                     40 	~L1 := 0
                     41 	~L2 := 6
                     42 	}	/* _setbuf */
                     43 
                     44 .data
                     45 
