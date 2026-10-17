LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~swap:*/
                      5 
                      6 .code
                      7 
                      8 _swap::
                      9 {
0000 abf7            10 	dec	fp,#~L2
0002 6ffe  0006      11 	ld	6(fp),r14
0006 2ff7            12 	ld	@fp,r7
0008 76fe  0002      13 	lda	r14,~L1+2(fp)
000c 60fa  0001      14 	ldb	rl2,~L1+1(fp)
0010 2eea            15 	ldb	@r14,rl2
0012 60fa  0000      16 	ldb	rl2,~L1(fp)
0016 6eea  0001      17 	ldb	1(r14),rl2
001a 21e2            18 	ld	r2,@r14
001c 61fe  0006      19 	ld	r14,6(fp)
0020 a9f7            20 	inc	fp,#~L2
0022 9e08            21 	ret
                     22 	~L1 := 0
                     23 	~L2 := 8
                     24 	}	/* _swap */
                     25 
                     26 .data
                     27 
                     28 /*~~longswap:*/
                     29 
                     30 .code
                     31 
                     32 _longswap::
                     33 {
0024 abf3            34 	dec	fp,#~L2
0026 6ffe  0002      35 	ld	2(fp),r14
002a 2ff7            36 	ld	@fp,r7
002c a17e            37 	ld	r14,r7
002e 0de4            38 	test	@r14
0030 e603            39 	jpr	eq,L14
0032 21e7            40 	ld	r7,@r14
0034 d01b            41 	callr	_swap
0036 2fe2            42 	ld	@r14,r2
                     43 L14:
0038 a9e1            44 	inc	r14,#2
003a 0de4            45 	test	@r14
003c e603            46 	jpr	eq,L10003
003e 21e7            47 	ld	r7,@r14
0040 d021            48 	callr	_swap
0042 2fe2            49 	ld	@r14,r2
                     50 L10003:
0044 61fe  0002      51 	ld	r14,2(fp)
0048 a9f3            52 	inc	fp,#~L2
004a 9e08            53 	ret
                     54 	~L1 := 0
                     55 	~L2 := 4
                     56 	}	/* _longswap */
                     57 
                     58 .data
                     59 
                     60 /*~~swapsegt:*/
                     61 
                     62 .code
                     63 
                     64 _swapsegt::
                     65 {
004c abf3            66 	dec	fp,#~L2
004e 6ffe  0002      67 	ld	2(fp),r14
0052 2ff7            68 	ld	@fp,r7
0054 a17e            69 	ld	r14,r7
0056 4de4  0004      70 	test	4(r14)
005a e605            71 	jpr	eq,L18
005c 61e7  0004      72 	ld	r7,4(r14)
0060 d031            73 	callr	_swap
0062 6fe2  0004      74 	ld	4(r14),r2
                     75 L18:
0066 4de4  0006      76 	test	6(r14)
006a e605            77 	jpr	eq,L19
006c 61e7  0006      78 	ld	r7,6(r14)
0070 d039            79 	callr	_swap
0072 6fe2  0006      80 	ld	6(r14),r2
                     81 L19:
0076 4de4  0008      82 	test	8(r14)
007a e605            83 	jpr	eq,L20
007c 61e7  0008      84 	ld	r7,8(r14)
0080 d041            85 	callr	_swap
0082 6fe2  0008      86 	ld	8(r14),r2
                     87 L20:
0086 4de4  000a      88 	test	10(r14)
008a e605            89 	jpr	eq,L10006
008c 61e7  000a      90 	ld	r7,10(r14)
0090 d049            91 	callr	_swap
0092 6fe2  000a      92 	ld	10(r14),r2
                     93 L10006:
0096 61fe  0002      94 	ld	r14,2(fp)
009a a9f3            95 	inc	fp,#~L2
009c 9e08            96 	ret
                     97 	~L1 := 0
                     98 	~L2 := 4
                     99 	}	/* _swapsegt */
                    100 
                    101 .data
                    102 
