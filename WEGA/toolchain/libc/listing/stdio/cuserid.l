LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 L20:
                      5 .=.+8
0008 00               6 .byte	0
0009 00               7 .byte	0
                      8 /*~~cuserid:*/
                      9 
                     10 .code
                     11 
                     12 _cuserid::
                     13 {
0000 abf9            14 	dec	fp,#~L2
0002 5cf9  0d01      15 	ldm	6(fp),r13,#2
0006 0006  
0008 2ff7            16 	ld	@fp,r7
000a 8d74            17 	test	r7
000c ee03            18 	jpr	ne,L10003
000e 2106  0000'     19 	ld	r6,#L20
0012 e802            20 	jpr	L10004
                     21 L10003:
0014 61f6  0000      22 	ld	r6,~L1(fp)
                     23 L10004:
0018 a16d            24 	ld	r13,r6
001a 5f00  0000*     25 	callr	_getlogin
001e a12e            26 	ld	r14,r2
0020 8d24            27 	test	r2
0022 e605            28 	jpr	eq,L24
0024 a1d7            29 	ld	r7,r13
0026 a126            30 	ld	r6,r2
0028 5f00  0000*     31 	callr	_strcpy
002c e817            32 	jpr	L26
                     33 L24:
002e 5f00  0000*     34 	callr	_getuid
0032 6ff2  0004      35 	ld	~L1+4(fp),r2
0036 a127            36 	ld	r7,r2
0038 5f00  0000*     37 	callr	_getpwuid
003c 6ff2  0002      38 	ld	~L1+2(fp),r2
0040 8d24            39 	test	r2
0042 ee03            40 	jpr	ne,L27
0044 0cd8            41 	clrb	@r13
0046 bdd0            42 	ldk	r13,#0
0048 e807            43 	jpr	L28
                     44 L27:
004a 61f6  0002      45 	ld	r6,~L1+2(fp)
004e 2167            46 	ld	r7,@r6
0050 a176            47 	ld	r6,r7
0052 a1d7            48 	ld	r7,r13
0054 5f00  0000*     49 	callr	_strcpy
                     50 L28:
0058 5f00  0000*     51 	callr	_endpwent
                     52 L26:
005c a1d2            53 	ld	r2,r13
005e 5cf1  0d01      54 	ldm	r13,6(fp),#2
0062 0006  
0064 a9f9            55 	inc	fp,#~L2
0066 9e08            56 	ret
                     57 	~L1 := 0
                     58 	~L2 := 10
                     59 	}	/* _cuserid */
                     60 
                     61 .data
                     62 
