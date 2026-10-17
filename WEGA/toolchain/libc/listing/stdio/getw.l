LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~getw:*/
                      5 
                      6 .code
                      7 
                      8 _getw::
                      9 {
0000 abf7            10 	dec	fp,#~L2
0002 5cf9  0d01      11 	ldm	4(fp),r13,#2
0006 0004  
0008 a17e            12 	ld	r14,r7
000a 6be0  0002      13 	dec	2(r14),#1
000e 4de1  0002      14 	cp	2(r14),#0
0012 0000  
0014 e904            15 	jpr	ge,L10003
0016 a1e7            16 	ld	r7,r14
0018 5f00  0000*     17 	callr	__filbuf
001c e805            18 	jpr	L10004
                     19 L10003:
001e 21e2            20 	ld	r2,@r14
0020 29e0            21 	inc	@r14,#1
0022 202b            22 	ldb	rl3,@r2
0024 8233            23 	subb	rh3,rh3
0026 a132            24 	ld	r2,r3
                     25 L10004:
0028 a12d            26 	ld	r13,r2
002a 60ea  0006      27 	ldb	rl2,6(r14)
002e b120            28 	extsb	r2
0030 a724            29 	bit	r2,#4
0032 e603            30 	jpr	eq,L21
0034 2102  ffff      31 	ld	r2,#-1
0038 e814            32 	jpr	L10000
                     33 L21:
003a 6be0  0002      34 	dec	2(r14),#1
003e 4de1  0002      35 	cp	2(r14),#0
0042 0000  
0044 e904            36 	jpr	ge,L10005
0046 a1e7            37 	ld	r7,r14
0048 5f00  0000*     38 	callr	__filbuf
004c e805            39 	jpr	L10006
                     40 L10005:
004e 21e2            41 	ld	r2,@r14
0050 29e0            42 	inc	@r14,#1
0052 202b            43 	ldb	rl3,@r2
0054 8233            44 	subb	rh3,rh3
0056 a132            45 	ld	r2,r3
                     46 L10006:
0058 a1d3            47 	ld	r3,r13
005a a0b3            48 	ldb	rh3,rl3
005c 8cb8            49 	clrb	rl3
005e 8523            50 	or	r3,r2
0060 a132            51 	ld	r2,r3
                     52 L10000:
0062 5cf1  0d01      53 	ldm	r13,4(fp),#2
0066 0004  
0068 a9f7            54 	inc	fp,#~L2
006a 9e08            55 	ret
                     56 	~L1 := 0
                     57 	~L2 := 8
                     58 	}	/* _getw */
                     59 
                     60 .data
                     61 
