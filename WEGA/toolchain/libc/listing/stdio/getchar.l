LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~getchar:*/
                      5 
                      6 .code
                      7 
                      8 _getchar::
                      9 {
0000 abf1            10 	dec	fp,#~L2
0002 6b00  0002*     11 	dec	__iob+2,#1
0006 4d01  0002*     12 	cp	__iob+2,#0
000a 0000  
000c e905            13 	jpr	ge,L10003
000e 2107  0000*     14 	ld	r7,#__iob
0012 5f00  0000*     15 	callr	__filbuf
0016 e807            16 	jpr	L10000
                     17 L10003:
0018 6102  0000*     18 	ld	r2,__iob
001c 6900  0000*     19 	inc	__iob,#1
0020 202b            20 	ldb	rl3,@r2
0022 8233            21 	subb	rh3,rh3
0024 a132            22 	ld	r2,r3
                     23 L10000:
0026 a9f1            24 	inc	fp,#~L2
0028 9e08            25 	ret
                     26 	~L1 := 0
                     27 	~L2 := 2
                     28 	}	/* _getchar */
                     29 
                     30 .data
                     31 
