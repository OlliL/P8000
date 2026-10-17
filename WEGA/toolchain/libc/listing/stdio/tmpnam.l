LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 L20:
                      5 .=.+18
0012 00               6 .byte	0
0013 00               7 .byte	0
                      8 L21:
0014 61               9 .byte	%61
0015 61              10 .byte	%61
0016 61              11 .byte	%61
0017 00              12 .byte	%0
                     13 L23:
0018 2f  75  73      14 .byte	%2f,%75,%73,%72,%2f,%74,%6d,%70
001b 72  2f  74  
001e 6d  70  
0020 2f  00          15 .byte	%2f,%0
                     16 .even
                     17 L25:
0022 58  58  58      18 .byte	%58,%58,%58,%58,%58,%58,%0
0025 58  58  58  
0028 00  
                     19 .even
                     20 /*~~tmpnam:*/
                     21 
                     22 .code
                     23 
                     24 _tmpnam::
                     25 {
0000 abf7            26 	dec	fp,#~L2
0002 5cf9  0d01      27 	ldm	4(fp),r13,#2
0006 0004  
0008 2ff7            28 	ld	@fp,r7
000a 8d74            29 	test	r7
000c ee03            30 	jpr	ne,L10003
000e 2106  0000'     31 	ld	r6,#L20
0012 e802            32 	jpr	L10004
                     33 L10003:
0014 61f6  0000      34 	ld	r6,~L1(fp)
                     35 L10004:
0018 a16e            36 	ld	r14,r6
001a a167            37 	ld	r7,r6
001c 2106  0018'     38 	ld	r6,#L23
0020 5f00  0000*     39 	callr	_strcpy
0024 a1e7            40 	ld	r7,r14
0026 2106  0014'     41 	ld	r6,#L21
002a 5f00  0000*     42 	callr	_strcat
002e a1e7            43 	ld	r7,r14
0030 2106  0022'     44 	ld	r6,#L25
0034 5f00  0000*     45 	callr	_strcat
0038 210d  0014'     46 	ld	r13,#L21
003c e803            47 	jpr	L26
                     48 L20001:
003e 0cd5  6161      49 	ldb	@r13,#97
0042 a9d0            50 	inc	r13,#1
                     51 L26:
0044 0cd1  7a7a      52 	cpb	@r13,#122
0048 e6fa            53 	jpr	eq,L20001
004a 28d0            54 	incb	@r13,#1
004c a1e7            55 	ld	r7,r14
004e 5f00  0000*     56 	callr	_mktemp
0052 a1e2            57 	ld	r2,r14
0054 5cf1  0d01      58 	ldm	r13,4(fp),#2
0058 0004  
005a a9f7            59 	inc	fp,#~L2
005c 9e08            60 	ret
                     61 	~L1 := 0
                     62 	~L2 := 8
                     63 	}	/* _tmpnam */
                     64 
                     65 .data
                     66 
