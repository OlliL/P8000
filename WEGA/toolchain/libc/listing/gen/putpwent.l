LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 L21:
0000 25  73  3a       5 .byte	%25,%73,%3a,%25,%73,%0
0003 25  73  00  
                      6 .even
                      7 L23:
0006 2c  25  73       8 .byte	%2c,%25,%73,%0
0009 00  
                      9 .even
                     10 L24:
000a 3a  25  75      11 .byte	%3a,%25,%75,%3a,%25,%75,%3a,%25
000d 3a  25  75  
0010 3a  25  
0012 73  3a  25      12 .byte	%73,%3a,%25,%73,%3a,%25,%73,%0
0015 73  3a  25  
0018 73  00  
                     13 .even
                     14 /*~~putpwent:*/
                     15 
                     16 .code
                     17 
                     18 _putpwent::
                     19 {
0000 abfb            20 	dec	fp,#~L2
0002 5cf9  0d01      21 	ldm	8(fp),r13,#2
0006 0008  
0008 a16d            22 	ld	r13,r6
000a a17e            23 	ld	r14,r7
000c a167            24 	ld	r7,r6
000e 2106  0000'     25 	ld	r6,#L21
0012 21e5            26 	ld	r5,@r14
0014 61e4  0002      27 	ld	r4,2(r14)
0018 5f00  0000*     28 	callr	_fprintf
001c 61e2  0008      29 	ld	r2,8(r14)
0020 0c24            30 	testb	@r2
0022 e607            31 	jpr	eq,L22
0024 a1d7            32 	ld	r7,r13
0026 2106  0006'     33 	ld	r6,#L23
002a 61e5  0008      34 	ld	r5,8(r14)
002e 5f00  0000*     35 	callr	_fprintf
                     36 L22:
0032 61e2  0010      37 	ld	r2,16(r14)
0036 2ff2            38 	ld	@fp,r2
0038 a1d7            39 	ld	r7,r13
003a 2106  000a'     40 	ld	r6,#L24
003e 61e5  0004      41 	ld	r5,4(r14)
0042 61e4  0006      42 	ld	r4,6(r14)
0046 61e3  000c      43 	ld	r3,12(r14)
004a 61e2  000e      44 	ld	r2,14(r14)
004e 5f00  0000*     45 	callr	_fprintf
0052 6bd0  0002      46 	dec	2(r13),#1
0056 4dd1  0002      47 	cp	2(r13),#0
005a 0000  
005c e905            48 	jpr	ge,L10003
005e bd7a            49 	ldk	r7,#10
0060 a1d6            50 	ld	r6,r13
0062 5f00  0000*     51 	callr	__flsbuf
0066 e806            52 	jpr	L10004
                     53 L10003:
0068 21d2            54 	ld	r2,@r13
006a 29d0            55 	inc	@r13,#1
006c 0c25  0a0a      56 	ldb	@r2,#10
0070 ca0a            57 	ldb	rl2,#10
0072 8222            58 	subb	rh2,rh2
                     59 L10004:
0074 60da  0006      60 	ldb	rl2,6(r13)
0078 b120            61 	extsb	r2
007a 0702  0020      62 	and	r2,#32
007e 0b02  0000      63 	cp	r2,#0
0082 bd20            64 	ldk	r2,#0
0084 af2e            65 	tcc	ne,r2
0086 5cf1  0d01      66 	ldm	r13,8(fp),#2
008a 0008  
008c a9fb            67 	inc	fp,#~L2
008e 9e08            68 	ret
                     69 	~L1 := 2
                     70 	~L2 := 12
                     71 	}	/* _putpwent */
                     72 
                     73 .data
                     74 
