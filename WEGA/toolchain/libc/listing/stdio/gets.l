LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~gets:*/
                      5 
                      6 .code
                      7 
                      8 _gets::
                      9 {
0000 abf7            10 	dec	fp,#~L2
0002 5cf9  0d01      11 	ldm	4(fp),r13,#2
0006 0004  
0008 2ff7            12 	ld	@fp,r7
000a a17d            13 	ld	r13,r7
000c e806            14 	jpr	L20
                     15 L20001:
000e 0b0e  0000      16 	cp	r14,#0
0012 e119            17 	jpr	lt,L21
0014 a1e2            18 	ld	r2,r14
0016 2eda            19 	ldb	@r13,rl2
0018 a9d0            20 	inc	r13,#1
                     21 L20:
001a 6b00  0002*     22 	dec	__iob+2,#1
001e 4d01  0002*     23 	cp	__iob+2,#0
0022 0000  
0024 e905            24 	jpr	ge,L10003
0026 2107  0000*     25 	ld	r7,#__iob
002a 5f00  0000*     26 	callr	__filbuf
002e e807            27 	jpr	L10004
                     28 L10003:
0030 6102  0000*     29 	ld	r2,__iob
0034 6900  0000*     30 	inc	__iob,#1
0038 202b            31 	ldb	rl3,@r2
003a 8233            32 	subb	rh3,rh3
003c a132            33 	ld	r2,r3
                     34 L10004:
003e a12e            35 	ld	r14,r2
0040 0b02  000a      36 	cp	r2,#10
0044 eee4            37 	jpr	ne,L20001
                     38 L21:
0046 0b0e  0000      39 	cp	r14,#0
004a e905            40 	jpr	ge,L22
004c 4bfd  0000      41 	cp	r13,~L1(fp)
0050 ee02            42 	jpr	ne,L22
0052 8322            43 	sub	r2,r2
0054 e804            44 	jpr	L10000
                     45 L22:
0056 0cd8            46 	clrb	@r13
0058 a9d0            47 	inc	r13,#1
005a 61f2  0000      48 	ld	r2,~L1(fp)
                     49 L10000:
005e 5cf1  0d01      50 	ldm	r13,4(fp),#2
0062 0004  
0064 a9f7            51 	inc	fp,#~L2
0066 9e08            52 	ret
                     53 	~L1 := 0
                     54 	~L2 := 8
                     55 	}	/* _gets */
                     56 
                     57 .data
                     58 
