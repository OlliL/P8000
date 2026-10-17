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
                     11 /*~~atoi:*/
                     12 
                     13 .code
                     14 
                     15 _atoi::
                     16 {
                     17 	.data
                     18 	.even
                     19 L23:
0008 0024'           20 	.word	L12
000a 0024'           21 	.word	L12
000c 0046'           22 	.word	L20
000e 0044'           23 	.word	L19
                     24 	.code
0000 abf9            25 	dec	fp,#~L2
0002 a171            26 	ld	r1,r7
0004 bd50            27 	ldk	r5,#0
0006 bd60            28 	ldk	r6,#0
0008 e80e            29 	jpr	L14
                     30 L20001:
000a 0c11  3939      31 	cpb	@r1,#57
000e ea1f            32 	jpr	gt,L25
0010 a153            33 	ld	r3,r5
0012 1902  000a      34 	mult	rr2,#10
0016 201a            35 	ldb	rl2,@r1
0018 b120            36 	extsb	r2
001a 8123            37 	add	r3,r2
001c 0303  0030      38 	sub	r3,#48
0020 a135            39 	ld	r5,r3
0022 e811            40 	jpr	L20
                     41 L12:
0024 a910            42 	inc	r1,#1
                     43 L14:
0026 201b            44 	ldb	rl3,@r1
0028 b130            45 	extsb	r3
002a a132            46 	ld	r2,r3
002c 2103  0000'     47 	ld	r3,#L22
0030 2104  0004      48 	ld	r4,#4
0034 bb34  0426      49 	cpir	r2,@r3,r4,eq
0038 ee07            50 	jpr	ne,L24
003a 8144            51 	add	r4,r4
003c 8d42            52 	neg	r4
003e 6142  000e'     53 	ld	r2,L23+6(r4)
0042 1e28            54 	jp	@r2
                     55 L19:
0044 a960            56 	inc	r6,#1
                     57 L20:
0046 a910            58 	inc	r1,#1
                     59 L24:
0048 0c11  3030      60 	cpb	@r1,#48
004c e9de            61 	jpr	ge,L20001
                     62 L25:
004e 8d64            63 	test	r6
0050 e603            64 	jpr	eq,L10003
0052 a152            65 	ld	r2,r5
0054 8d22            66 	neg	r2
0056 e801            67 	jpr	L10000
                     68 L10003:
0058 a152            69 	ld	r2,r5
                     70 L10000:
005a a9f9            71 	inc	fp,#~L2
005c 9e08            72 	ret
                     73 	~L1 := 0
                     74 	~L2 := 10
                     75 	}	/* _atoi */
                     76 
                     77 .data
                     78 
