LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~getenv:*/
                      5 
                      6 .code
                      7 
                      8 _getenv::
                      9 {
0000 abf9            10 	dec	fp,#~L2
0002 5cf9  0c02      11 	ldm	4(fp),r12,#3
0006 0004  
0008 a17e            12 	ld	r14,r7
000a 610d  0000*     13 	ld	r13,_environ
000e e809            14 	jpr	L14
                     15 L20001:
0010 a1d6            16 	ld	r6,r13
0012 a9d1            17 	inc	r13,#2
0014 2167            18 	ld	r7,@r6
0016 a176            19 	ld	r6,r7
0018 a1e7            20 	ld	r7,r14
001a dff5            21 	callr	_nvmatch
001c a12c            22 	ld	r12,r2
001e 8d24            23 	test	r2
0020 ee03            24 	jpr	ne,L10000
                     25 L14:
0022 0dd4            26 	test	@r13
0024 eef5            27 	jpr	ne,L20001
0026 8322            28 	sub	r2,r2
                     29 L10000:
0028 5cf1  0c02      30 	ldm	r12,4(fp),#3
002c 0004  
002e a9f9            31 	inc	fp,#~L2
0030 9e08            32 	ret
                     33 	~L1 := 0
                     34 	~L2 := 10
                     35 	}	/* _getenv */
                     36 
                     37 .data
                     38 
                     39 
                     40 .code
                     41 
                     42 _nvmatch:
                     43 {
0032 abf9            44 	dec	fp,#~L2
0034 a164            45 	ld	r4,r6
0036 a171            46 	ld	r1,r7
                     47 L18:
0038 a142            48 	ld	r2,r4
003a a940            49 	inc	r4,#1
003c 202b            50 	ldb	rl3,@r2
003e 0a1b            51 	cpb	rl3,@r1
0040 ee08            52 	jpr	ne,L19
0042 a112            53 	ld	r2,r1
0044 a910            54 	inc	r1,#1
0046 202b            55 	ldb	rl3,@r2
0048 0a0b  3d3d      56 	cpb	rl3,#61
004c eef5            57 	jpr	ne,L18
                     58 L20002:
004e a142            59 	ld	r2,r4
0050 e807            60 	jpr	L10003
                     61 L19:
0052 0c14            62 	testb	@r1
0054 ee04            63 	jpr	ne,L21
0056 4c41  ffff      64 	cpb	-1(r4),#61
005a 3d3d  
005c e6f8            65 	jpr	eq,L20002
                     66 L21:
005e 8322            67 	sub	r2,r2
                     68 L10003:
0060 a9f9            69 	inc	fp,#~L2
0062 9e08            70 	ret
                     71 	~L1 := 0
                     72 	~L2 := 10
                     73 	}	/* _nvmatch */
                     74 
                     75 .data
                     76 
