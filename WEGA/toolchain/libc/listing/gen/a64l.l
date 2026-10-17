LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~a64l:*/
                      5 
                      6 .code
                      7 
                      8 _a64l::
                      9 {
0000 abff            10 	dec	fp,#~L2
0002 6ffc  000a      11 	ld	10(fp),r12
0006 a171            12 	ld	r1,r7
0008 bd60            13 	ldk	r6,#0
000a 9222            14 	subl	rr2,rr2
000c 5df2  0002      15 	ldl	~L1+2(fp),rr2
0010 e816            16 	jpr	L12
                     17 L20001:
0012 0b0c  0061      18 	cp	r12,#97
0016 e101            19 	jpr	lt,L14
0018 abc5            20 	dec	r12,#6
                     21 L14:
001a 0b0c  0041      22 	cp	r12,#65
001e e101            23 	jpr	lt,L15
0020 abc6            24 	dec	r12,#7
                     25 L15:
0022 a1c2            26 	ld	r2,r12
0024 0302  002e      27 	sub	r2,#46
0028 a125            28 	ld	r5,r2
002a b14a            29 	exts	rr4
002c b34f  0600      30 	sdal	rr4,r6
0030 54f2  0002      31 	ldl	rr2,~L1+2(fp)
0034 8542            32 	or	r2,r4
0036 8553            33 	or	r3,r5
0038 5df2  0002      34 	ldl	~L1+2(fp),rr2
003c a965            35 	inc	r6,#6
                     36 L12:
003e a112            37 	ld	r2,r1
0040 a910            38 	inc	r1,#1
0042 202b            39 	ldb	rl3,@r2
0044 b130            40 	extsb	r3
0046 a13c            41 	ld	r12,r3
0048 8d34            42 	test	r3
004a eee3            43 	jpr	ne,L20001
004c 54f2  0002      44 	ldl	rr2,~L1+2(fp)
0050 61fc  000a      45 	ld	r12,10(fp)
0054 a9ff            46 	inc	fp,#~L2
0056 9e08            47 	ret
                     48 	~L1 := 0
                     49 	~L2 := 16
                     50 	}	/* _a64l */
                     51 
                     52 .data
                     53 
