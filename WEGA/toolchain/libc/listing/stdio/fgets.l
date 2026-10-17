LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~fgets:*/
                      5 
                      6 .code
                      7 
                      8 _fgets::
                      9 {
0000 abfd            10 	dec	fp,#~L2
0002 5cf9  0c02      11 	ldm	8(fp),r12,#3
0006 0008  
0008 5df6  0002      12 	ldl	~L1+2(fp),rr6
000c a15e            13 	ld	r14,r5
000e a17c            14 	ld	r12,r7
                     15 L20:
0010 6bf0  0002      16 	dec	~L1+2(fp),#1
0014 4df1  0002      17 	cp	~L1+2(fp),#0
0018 0000  
001a e218            18 	jpr	le,L21
001c 6be0  0002      19 	dec	2(r14),#1
0020 4de1  0002      20 	cp	2(r14),#0
0024 0000  
0026 e904            21 	jpr	ge,L10003
0028 a1e7            22 	ld	r7,r14
002a 5f00  0000*     23 	callr	__filbuf
002e e805            24 	jpr	L10004
                     25 L10003:
0030 21e2            26 	ld	r2,@r14
0032 29e0            27 	inc	@r14,#1
0034 202b            28 	ldb	rl3,@r2
0036 8233            29 	subb	rh3,rh3
0038 a132            30 	ld	r2,r3
                     31 L10004:
003a a12d            32 	ld	r13,r2
003c 0b02  0000      33 	cp	r2,#0
0040 e105            34 	jpr	lt,L21
0042 2eca            35 	ldb	@r12,rl2
0044 a9c0            36 	inc	r12,#1
0046 0b0d  000a      37 	cp	r13,#10
004a eee2            38 	jpr	ne,L20
                     39 L21:
004c 0b0d  0000      40 	cp	r13,#0
0050 e905            41 	jpr	ge,L23
0052 4bfc  0004      42 	cp	r12,~L1+4(fp)
0056 ee02            43 	jpr	ne,L23
0058 8322            44 	sub	r2,r2
005a e804            45 	jpr	L10000
                     46 L23:
005c 0cc8            47 	clrb	@r12
005e a9c0            48 	inc	r12,#1
0060 61f2  0004      49 	ld	r2,~L1+4(fp)
                     50 L10000:
0064 5cf1  0c02      51 	ldm	r12,8(fp),#3
0068 0008  
006a a9fd            52 	inc	fp,#~L2
006c 9e08            53 	ret
                     54 	~L1 := 0
                     55 	~L2 := 14
                     56 	}	/* _fgets */
                     57 
                     58 .data
                     59 
