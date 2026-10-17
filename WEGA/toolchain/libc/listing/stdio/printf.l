LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~printf:*/
                      5 
                      6 .code
                      7 
                      8 _printf::
                      9 {
0000 abf5            10 	dec	fp,#~L2
0002 1df6            11 	ldl	@fp,rr6
0004 76f6  0000      12 	lda	r6,~L1(fp)
0008 a172            13 	ld	r2,r7
000a 2105  0008*     14 	ld	r5,#__iob+8
000e 5f00  0000*     15 	callr	__doprnt
0012 600a  000e*     16 	ldb	rl2,__iob+14
0016 b120            17 	extsb	r2
0018 0702  0020      18 	and	r2,#32
001c 8d24            19 	test	r2
001e e603            20 	jpr	eq,L10003
0020 2102  ffff      21 	ld	r2,#-1
0024 e801            22 	jpr	L10000
                     23 L10003:
0026 8322            24 	sub	r2,r2
                     25 L10000:
0028 a9f5            26 	inc	fp,#~L2
002a 9e08            27 	ret
                     28 	~L1 := 0
                     29 	~L2 := 6
                     30 	}	/* _printf */
                     31 
                     32 .data
                     33 
