LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~fputc:*/
                      5 
                      6 .code
                      7 
                      8 _fputc::
                      9 {
0000 abf5            10 	dec	fp,#~L2
0002 1df6            11 	ldl	@fp,rr6
0004 a162            12 	ld	r2,r6
0006 6b20  0002      13 	dec	2(r2),#1
000a 6123  0002      14 	ld	r3,2(r2)
000e 0b03  0000      15 	cp	r3,#0
0012 e906            16 	jpr	ge,L10003
0014 61f7  0002      17 	ld	r7,~L1+2(fp)
0018 a162            18 	ld	r2,r6
001a 5f00  0000*     19 	callr	__flsbuf
001e e809            20 	jpr	L10000
                     21 L10003:
0020 61f2  0000      22 	ld	r2,~L1(fp)
0024 2920            23 	inc	@r2,#1
0026 2123            24 	ld	r3,@r2
0028 ab30            25 	dec	r3,#1
002a 60fa  0003      26 	ldb	rl2,~L1+3(fp)
002e 2e3a            27 	ldb	@r3,rl2
0030 8222            28 	subb	rh2,rh2
                     29 L10000:
0032 a9f5            30 	inc	fp,#~L2
0034 9e08            31 	ret
                     32 	~L1 := 0
                     33 	~L2 := 6
                     34 	}	/* _fputc */
                     35 
                     36 .data
                     37 
