LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~rewind:*/
                      5 
                      6 .code
                      7 
                      8 _rewind::
                      9 {
0000 abf3            10 	dec	fp,#~L2
0002 6ffe  0002      11 	ld	2(fp),r14
0006 a17e            12 	ld	r14,r7
0008 5f00  0000*     13 	callr	_fflush
000c 60ef  0007      14 	ldb	rl7,7(r14)
0010 b170            15 	extsb	r7
0012 9244            16 	subl	rr4,rr4
0014 8333            17 	sub	r3,r3
0016 5f00  0000*     18 	callr	_lseek
001a 4de8  0002      19 	clr	2(r14)
001e 61e2  0004      20 	ld	r2,4(r14)
0022 2fe2            21 	ld	@r14,r2
0024 60ea  0006      22 	ldb	rl2,6(r14)
0028 060a  cfcf      23 	andb	rl2,#-49
002c 6eea  0006      24 	ldb	6(r14),rl2
0030 60ea  0006      25 	ldb	rl2,6(r14)
0034 b120            26 	extsb	r2
0036 a727            27 	bit	r2,#7
0038 e606            28 	jpr	eq,L10000
003a 60ea  0006      29 	ldb	rl2,6(r14)
003e 060a  fcfc      30 	andb	rl2,#-4
0042 6eea  0006      31 	ldb	6(r14),rl2
                     32 L10000:
0046 61fe  0002      33 	ld	r14,2(fp)
004a a9f3            34 	inc	fp,#~L2
004c 9e08            35 	ret
                     36 	~L1 := 0
                     37 	~L2 := 4
                     38 	}	/* _rewind */
                     39 
                     40 .data
                     41 
