LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~scanf:*/
                      5 
                      6 .code
                      7 
                      8 _scanf::
                      9 {
0000 abf5            10 	dec	fp,#~L2
0002 1df6            11 	ldl	@fp,rr6
0004 76f5  0000      12 	lda	r5,~L1(fp)
0008 61f6  0002      13 	ld	r6,~L1+2(fp)
000c 2107  0000*     14 	ld	r7,#__iob
0010 5f00  0000*     15 	callr	__doscan
0014 a9f5            16 	inc	fp,#~L2
0016 9e08            17 	ret
                     18 	~L1 := 0
                     19 	~L2 := 6
                     20 	}	/* _scanf */
                     21 
                     22 .data
                     23 
                     24 /*~~fscanf:*/
                     25 
                     26 .code
                     27 
                     28 _fscanf::
                     29 {
0018 abf7            30 	dec	fp,#~L2
001a 1cf9  0502      31 	ldm	@fp,r5,#3
001e 76f5  0000      32 	lda	r5,~L1(fp)
0022 a164            33 	ld	r4,r6
0024 a172            34 	ld	r2,r7
0026 5f00  0000*     35 	callr	__doscan
002a a9f7            36 	inc	fp,#~L2
002c 9e08            37 	ret
                     38 	~L1 := 0
                     39 	~L2 := 8
                     40 	}	/* _fscanf */
                     41 
                     42 .data
                     43 
                     44 /*~~sscanf:*/
                     45 
                     46 .code
                     47 
                     48 _sscanf::
                     49 {
002e 030f  0012      50 	sub	fp,#~L2
0032 6ffe  0010      51 	ld	16(fp),r14
0036 1cf9  0501      52 	ldm	@fp,r5,#2
003a a17e            53 	ld	r14,r7
003c 4cf5  000c      54 	ldb	~L1+12(fp),#65
0040 4141  
0042 6ff7  000a      55 	ld	~L1+10(fp),r7
0046 6ff7  0006      56 	ld	~L1+6(fp),r7
004a 4df8  0008      57 	clr	~L1+8(fp)
004e e802            58 	jpr	L25
                     59 L20001:
0050 69f0  0008      60 	inc	~L1+8(fp),#1
                     61 L25:
0054 a1e2            62 	ld	r2,r14
0056 a9e0            63 	inc	r14,#1
0058 0c24            64 	testb	@r2
005a eefa            65 	jpr	ne,L20001
005c 76f7  0006      66 	lda	r7,~L1+6(fp)
0060 76f5  0000      67 	lda	r5,~L1(fp)
0064 61f6  0002      68 	ld	r6,~L1+2(fp)
0068 5f00  0000*     69 	callr	__doscan
006c 61fe  0010      70 	ld	r14,16(fp)
0070 010f  0012      71 	add	fp,#~L2
0074 9e08            72 	ret
                     73 	~L1 := 0
                     74 	~L2 := 18
                     75 	}	/* _sscanf */
                     76 
                     77 .data
                     78 
