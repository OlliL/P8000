LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 .comm	2,	_errno;
                      5 .comm	2,	_sys_nerr;
                      6 .comm	0,	_sys_errlist;
                      7 .comm	2,	_deverr;
                      8 .comm	2,	_dev_nerr;
                      9 .comm	0,	_dev_errlist;
                     10 L18:
0000 55  6e  6b      11 .byte	%55,%6e,%6b,%6e,%6f,%77,%6e,%20
0003 6e  6f  77  
0006 6e  20  
0008 65  72  72      12 .byte	%65,%72,%72,%6f,%72,%0
000b 6f  72  00  
                     13 .even
                     14 L30:
000e 3a  20  00      15 .byte	%3a,%20,%0
                     16 .even
                     17 L31:
0012 0a  00          18 .byte	%a,%0
                     19 .even
                     20 /*~~perror:*/
                     21 
                     22 .code
                     23 
                     24 _perror::
                     25 {
0000 abf7            26 	dec	fp,#~L2
0002 5cf9  0d01      27 	ldm	4(fp),r13,#2
0006 0004  
0008 2ff7            28 	ld	@fp,r7
000a 210e  0000'     29 	ld	r14,#L18
000e 4d01  0000*     30 	cp	_errno,#5
0012 0005  
0014 e604            31 	jpr	eq,L20
0016 4d01  0000*     32 	cp	_errno,#6
001a 0006  
001c ee0c            33 	jpr	ne,L22
                     34 L20:
001e 4d04  0000*     35 	test	_deverr
0022 e609            36 	jpr	eq,L22
0024 6102  0000*     37 	ld	r2,_deverr
0028 4b02  0000*     38 	cp	r2,_dev_nerr
002c e90c            39 	jpr	ge,L25
002e 8122            40 	add	r2,r2
0030 612e  0000*     41 	ld	r14,_dev_errlist(r2)
0034 e808            42 	jpr	L25
                     43 L22:
0036 6102  0000*     44 	ld	r2,_errno
003a 4b02  0000*     45 	cp	r2,_sys_nerr
003e e903            46 	jpr	ge,L25
0040 8122            47 	add	r2,r2
0042 612e  0000*     48 	ld	r14,_sys_errlist(r2)
                     49 L25:
0046 61f7  0000      50 	ld	r7,~L1(fp)
004a 5f00  0000*     51 	callr	_strlen
004e a12d            52 	ld	r13,r2
0050 8d24            53 	test	r2
0052 e60c            54 	jpr	eq,L28
0054 bd72            55 	ldk	r7,#2
0056 61f6  0000      56 	ld	r6,~L1(fp)
005a a125            57 	ld	r5,r2
005c 5f00  0000*     58 	callr	_write
0060 bd72            59 	ldk	r7,#2
0062 2106  000e'     60 	ld	r6,#L30
0066 a175            61 	ld	r5,r7
0068 5f00  0000*     62 	callr	_write
                     63 L28:
006c a1e7            64 	ld	r7,r14
006e 5f00  0000*     65 	callr	_strlen
0072 6ff2  0002      66 	ld	~L1+2(fp),r2
0076 bd72            67 	ldk	r7,#2
0078 a1e6            68 	ld	r6,r14
007a a125            69 	ld	r5,r2
007c 5f00  0000*     70 	callr	_write
0080 bd72            71 	ldk	r7,#2
0082 2106  0012'     72 	ld	r6,#L31
0086 bd51            73 	ldk	r5,#1
0088 5f00  0000*     74 	callr	_write
008c 5cf1  0d01      75 	ldm	r13,4(fp),#2
0090 0004  
0092 a9f7            76 	inc	fp,#~L2
0094 9e08            77 	ret
                     78 	~L1 := 0
                     79 	~L2 := 8
                     80 	}	/* _perror */
                     81 
                     82 .data
                     83 
