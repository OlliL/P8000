LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 L20:
                      5 .=.+8
0008 00               6 .byte	0
0009 00               7 .byte	0
                      8 L22:
000a 2f  64  65       9 .byte	%2f,%64,%65,%76,%2f,%74,%74,%79
000d 76  2f  74  
0010 74  79  
0012 00              10 .byte	%0
                     11 .even
                     12 /*~~ctermid:*/
                     13 
                     14 .code
                     15 
                     16 _ctermid::
                     17 {
0000 abf5            18 	dec	fp,#~L2
0002 6ffe  0004      19 	ld	4(fp),r14
0006 2ff7            20 	ld	@fp,r7
0008 8d74            21 	test	r7
000a ee03            22 	jpr	ne,L10003
000c 2106  0000'     23 	ld	r6,#L20
0010 e802            24 	jpr	L10004
                     25 L10003:
0012 61f6  0000      26 	ld	r6,~L1(fp)
                     27 L10004:
0016 a16e            28 	ld	r14,r6
0018 a167            29 	ld	r7,r6
001a 2106  000a'     30 	ld	r6,#L22
001e 5f00  0000*     31 	callr	_strcpy
0022 a1e2            32 	ld	r2,r14
0024 61fe  0004      33 	ld	r14,4(fp)
0028 a9f5            34 	inc	fp,#~L2
002a 9e08            35 	ret
                     36 	~L1 := 0
                     37 	~L2 := 6
                     38 	}	/* _ctermid */
                     39 
                     40 .data
                     41 
