LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~fgetc:*/
                      5 
                      6 .code
                      7 
                      8 _fgetc::
                      9 {
0000 abf3            10 	dec	fp,#~L2
0002 2ff7            11 	ld	@fp,r7
0004 a176            12 	ld	r6,r7
0006 6b60  0002      13 	dec	2(r6),#1
000a 6162  0002      14 	ld	r2,2(r6)
000e 0b02  0000      15 	cp	r2,#0
0012 e904            16 	jpr	ge,L10003
0014 a176            17 	ld	r6,r7
0016 5f00  0000*     18 	callr	__filbuf
001a e807            19 	jpr	L10000
                     20 L10003:
001c 61f2  0000      21 	ld	r2,~L1(fp)
0020 2920            22 	inc	@r2,#1
0022 2123            23 	ld	r3,@r2
0024 ab30            24 	dec	r3,#1
0026 203a            25 	ldb	rl2,@r3
0028 8222            26 	subb	rh2,rh2
                     27 L10000:
002a a9f3            28 	inc	fp,#~L2
002c 9e08            29 	ret
                     30 	~L1 := 0
                     31 	~L2 := 4
                     32 	}	/* _fgetc */
                     33 
                     34 .data
                     35 
