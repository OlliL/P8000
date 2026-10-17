LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 .even
                      5 L22:
0000 0009             6 .word	9
0002 0020             7 .word	32
0004 002b             8 .word	43
0006 002d             9 .word	45
                     10 .even
                     11 /*~~atol:*/
                     12 
                     13 .code
                     14 
                     15 _atol::
                     16 {
                     17 	.data
                     18 	.even
                     19 L23:
0008 0036'           20 	.word	L12
000a 0036'           21 	.word	L12
000c 0058'           22 	.word	L20
000e 0056'           23 	.word	L19
                     24 	.code
0000 abfd            25 	dec	fp,#~L2
0002 6ffd  000a      26 	ld	10(fp),r13
0006 a171            27 	ld	r1,r7
0008 9222            28 	subl	rr2,rr2
000a 5df2  0002      29 	ldl	~L1+2(fp),rr2
000e bdd0            30 	ldk	r13,#0
0010 e813            31 	jpr	L14
                     32 L20001:
0012 0c11  3939      33 	cpb	@r1,#57
0016 ea24            34 	jpr	gt,L25
0018 54f6  0002      35 	ldl	rr6,~L1+2(fp)
001c 1804  0000      36 	multl	rq4,#10
0020 000a  
0022 201b            37 	ldb	rl3,@r1
0024 b130            38 	extsb	r3
0026 b12a            39 	exts	rr2
0028 9626            40 	addl	rr6,rr2
002a 1206  0000      41 	subl	rr6,#48
002e 0030  
0030 5df6  0002      42 	ldl	~L1+2(fp),rr6
0034 e811            43 	jpr	L20
                     44 L12:
0036 a910            45 	inc	r1,#1
                     46 L14:
0038 201b            47 	ldb	rl3,@r1
003a b130            48 	extsb	r3
003c a132            49 	ld	r2,r3
003e 2103  0000'     50 	ld	r3,#L22
0042 2104  0004      51 	ld	r4,#4
0046 bb34  0426      52 	cpir	r2,@r3,r4,eq
004a ee07            53 	jpr	ne,L24
004c 8144            54 	add	r4,r4
004e 8d42            55 	neg	r4
0050 6142  000e'     56 	ld	r2,L23+6(r4)
0054 1e28            57 	jp	@r2
                     58 L19:
0056 a9d0            59 	inc	r13,#1
                     60 L20:
0058 a910            61 	inc	r1,#1
                     62 L24:
005a 0c11  3030      63 	cpb	@r1,#48
005e e9d9            64 	jpr	ge,L20001
                     65 L25:
0060 8dd4            66 	test	r13
0062 e604            67 	jpr	eq,L10003
0064 9222            68 	subl	rr2,rr2
0066 52f2  0002      69 	subl	rr2,~L1+2(fp)
006a e802            70 	jpr	L10000
                     71 L10003:
006c 54f2  0002      72 	ldl	rr2,~L1+2(fp)
                     73 L10000:
0070 61fd  000a      74 	ld	r13,10(fp)
0074 a9fd            75 	inc	fp,#~L2
0076 9e08            76 	ret
                     77 	~L1 := 0
                     78 	~L2 := 14
                     79 	}	/* _atol */
                     80 
                     81 .data
                     82 
