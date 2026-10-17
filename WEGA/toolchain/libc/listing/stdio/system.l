LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~system:*/
                      5 
                      6 .code
                      7 
                      8 _system::
                      9 {
0000 abf3            10 	dec	fp,#~L2
0002 2ff7            11 	ld	@fp,r7
0004 bd61            12 	ldk	r6,#1
0006 dff7            13 	callr	__system
0008 a9f3            14 	inc	fp,#~L2
000a 9e08            15 	ret
                     16 	~L1 := 0
                     17 	~L2 := 4
                     18 	}	/* _system */
                     19 
                     20 .data
                     21 
                     22 /*~~csystem:*/
                     23 
                     24 .code
                     25 
                     26 _csystem::
                     27 {
000c abf3            28 	dec	fp,#~L2
000e 2ff7            29 	ld	@fp,r7
0010 a176            30 	ld	r6,r7
0012 8376            31 	sub	r6,r7
0014 dffe            32 	callr	__system
0016 a9f3            33 	inc	fp,#~L2
0018 9e08            34 	ret
                     35 	~L1 := 0
                     36 	~L2 := 4
                     37 	}	/* _csystem */
                     38 
                     39 .data
                     40 
                     41 L21:
0000 2f  62  69      42 .byte	%2f,%62,%69,%6e,%2f,%63,%73,%68
0003 6e  2f  63  
0006 73  68  
0008 00              43 .byte	%0
                     44 .even
                     45 L22:
000a 63  73  68      46 .byte	%63,%73,%68,%0
000d 00  
                     47 .even
                     48 L23:
000e 2d  63  00      49 .byte	%2d,%63,%0
                     50 .even
                     51 L25:
0012 2f  62  69      52 .byte	%2f,%62,%69,%6e,%2f,%73,%68,%0
0015 6e  2f  73  
0018 68  00  
                     53 .even
                     54 L26:
001a 73  68  00      55 .byte	%73,%68,%0
                     56 .even
                     57 L27:
001e 2d  63  00      58 .byte	%2d,%63,%0
                     59 .even
                     60 /*~~_system:*/
                     61 
                     62 .code
                     63 
                     64 __system::
                     65 {
001a abff            66 	dec	fp,#~L2
001c 5cf9  0d01      67 	ldm	12(fp),r13,#2
0020 000c  
0022 1df6            68 	ldl	@fp,rr6
0024 5f00  0000*     69 	callr	_fork
0028 6ff2  0006      70 	ld	~L1+6(fp),r2
002c 8d24            71 	test	r2
002e ee19            72 	jpr	ne,L18
0030 4df4  0000      73 	test	~L1(fp)
0034 ee07            74 	jpr	ne,L19
0036 2107  0000'     75 	ld	r7,#L21
003a 2106  000a'     76 	ld	r6,#L22
003e 2105  000e'     77 	ld	r5,#L23
0042 e806            78 	jpr	L20002
                     79 L19:
0044 2107  0012'     80 	ld	r7,#L25
0048 2106  001a'     81 	ld	r6,#L26
004c 2105  001e'     82 	ld	r5,#L27
                     83 L20002:
0050 61f4  0002      84 	ld	r4,~L1+2(fp)
0054 8333            85 	sub	r3,r3
0056 5f00  0000*     86 	callr	_execl
005a 2107  007f      87 	ld	r7,#127
005e 5f00  0000*     88 	callr	__exit
                     89 L18:
0062 bd72            90 	ldk	r7,#2
0064 bd61            91 	ldk	r6,#1
0066 5f00  0000*     92 	callr	_signal
006a a12e            93 	ld	r14,r2
006c bd73            94 	ldk	r7,#3
006e bd61            95 	ldk	r6,#1
0070 5f00  0000*     96 	callr	_signal
0074 a12d            97 	ld	r13,r2
                     98 L30:
0076 76f7  0004      99 	lda	r7,~L1+4(fp)
007a 5f00  0000*    100 	callr	_wait
007e 6ff2  0008     101 	ld	~L1+8(fp),r2
0082 4bf2  0006     102 	cp	r2,~L1+6(fp)
0086 e603           103 	jpr	eq,L31
0088 0b02  ffff     104 	cp	r2,#-1
008c eef4           105 	jpr	ne,L30
                    106 L31:
008e 4df1  0008     107 	cp	~L1+8(fp),#-1
0092 ffff  
0094 ee03           108 	jpr	ne,L32
0096 4df5  0004     109 	ld	~L1+4(fp),#-1
009a ffff  
                    110 L32:
009c bd72           111 	ldk	r7,#2
009e a1e6           112 	ld	r6,r14
00a0 5f00  0000*    113 	callr	_signal
00a4 bd73           114 	ldk	r7,#3
00a6 a1d6           115 	ld	r6,r13
00a8 5f00  0000*    116 	callr	_signal
00ac 61f2  0004     117 	ld	r2,~L1+4(fp)
00b0 5cf1  0d01     118 	ldm	r13,12(fp),#2
00b4 000c  
00b6 a9ff           119 	inc	fp,#~L2
00b8 9e08           120 	ret
                    121 	~L1 := 0
                    122 	~L2 := 16
                    123 	}	/* __system */
                    124 
                    125 .data
                    126 
