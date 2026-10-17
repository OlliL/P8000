LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 L21:
0000 77  2b  00       5 .byte	%77,%2b,%0
                      6 .even
                      7 /*~~tmpfile:*/
                      8 
                      9 .code
                     10 
                     11 _tmpfile::
                     12 {
0000 030f  0018      13 	sub	fp,#~L2
0004 6ffe  0016      14 	ld	22(fp),r14
0008 76f7  0000      15 	lda	r7,~L1(fp)
000c 5f00  0000*     16 	callr	_tmpnam
0010 76f7  0000      17 	lda	r7,~L1(fp)
0014 2106  0000'     18 	ld	r6,#L21
0018 5f00  0000*     19 	callr	_fopen
001c a12e            20 	ld	r14,r2
001e 76f7  0000      21 	lda	r7,~L1(fp)
0022 5f00  0000*     22 	callr	_unlink
0026 a1e2            23 	ld	r2,r14
0028 61fe  0016      24 	ld	r14,22(fp)
002c 010f  0018      25 	add	fp,#~L2
0030 9e08            26 	ret
                     27 	~L1 := 0
                     28 	~L2 := 24
                     29 	}	/* _tmpfile */
                     30 
                     31 .data
                     32 
