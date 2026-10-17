LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 L21:
0000 2f  00           5 .byte	%2f,%0
                      6 .even
                      7 /*~~mktemp:*/
                      8 
                      9 .code
                     10 
                     11 _mktemp::
                     12 {
0000 abf9            13 	dec	fp,#~L2
0002 5cf9  0c02      14 	ldm	4(fp),r12,#3
0006 0004  
0008 2ff7            15 	ld	@fp,r7
000a 5f00  0000*     16 	callr	_getpid
000e a12d            17 	ld	r13,r2
0010 61fe  0000      18 	ld	r14,~L1(fp)
                     19 L13:
0014 a1e2            20 	ld	r2,r14
0016 a9e0            21 	inc	r14,#1
0018 0c24            22 	testb	@r2
001a eefc            23 	jpr	ne,L13
001c abe0            24 	dec	r14,#1
001e e80c            25 	jpr	L15
                     26 L20001:
0020 a1d3            27 	ld	r3,r13
0022 bd20            28 	ldk	r2,#0
0024 1b02  000a      29 	div	rr2,#10
0028 0102  0030      30 	add	r2,#48
002c 2eea            31 	ldb	@r14,rl2
002e a1d3            32 	ld	r3,r13
0030 bd20            33 	ldk	r2,#0
0032 1b02  000a      34 	div	rr2,#10
0036 a13d            35 	ld	r13,r3
                     36 L15:
0038 abe0            37 	dec	r14,#1
003a 20ea            38 	ldb	rl2,@r14
003c 0a0a  5858      39 	cpb	rl2,#88
0040 e6ef            40 	jpr	eq,L20001
0042 a9e0            41 	inc	r14,#1
0044 210c  0061      42 	ld	r12,#97
0048 e809            43 	jpr	L18
                     44 L20003:
004a 0b0c  007a      45 	cp	r12,#122
004e ee03            46 	jpr	ne,L20
0050 2102  0000'     47 	ld	r2,#L21
0054 e80d            48 	jpr	L10000
                     49 L20:
0056 a1c2            50 	ld	r2,r12
0058 2eea            51 	ldb	@r14,rl2
005a a9c0            52 	inc	r12,#1
                     53 L18:
005c 61f7  0000      54 	ld	r7,~L1(fp)
0060 8366            55 	sub	r6,r6
0062 5f00  0000*     56 	callr	_access
0066 0b02  ffff      57 	cp	r2,#-1
006a eeef            58 	jpr	ne,L20003
006c 61f2  0000      59 	ld	r2,~L1(fp)
                     60 L10000:
0070 5cf1  0c02      61 	ldm	r12,4(fp),#3
0074 0004  
0076 a9f9            62 	inc	fp,#~L2
0078 9e08            63 	ret
                     64 	~L1 := 0
                     65 	~L2 := 10
                     66 	}	/* _mktemp */
                     67 
                     68 .data
                     69 
