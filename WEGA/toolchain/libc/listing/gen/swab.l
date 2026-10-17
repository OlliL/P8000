LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~swab:*/
                      5 
                      6 .code
                      7 
                      8 _swab::
                      9 {
0000 abfb            10 	dec	fp,#~L2
0002 6ffc  0006      11 	ld	6(fp),r12
0006 a164            12 	ld	r4,r6
0008 a171            13 	ld	r1,r7
000a a153            14 	ld	r3,r5
000c b12a            15 	exts	rr2
000e 1b02  0002      16 	div	rr2,#2
0012 a13c            17 	ld	r12,r3
0014 e80c            18 	jpr	L12
                     19 L20001:
0016 2112            20 	ld	r2,@r1
0018 a0a2            21 	ldb	rh2,rl2
001a 8ca8            22 	clrb	rl2
001c 2113            23 	ld	r3,@r1
001e a03b            24 	ldb	rl3,rh3
0020 b130            25 	extsb	r3
0022 0703  00ff      26 	and	r3,#255
0026 8132            27 	add	r2,r3
0028 2f42            28 	ld	@r4,r2
002a a941            29 	inc	r4,#2
002c a911            30 	inc	r1,#2
                     31 L12:
002e abc0            32 	dec	r12,#1
0030 0b0c  0000      33 	cp	r12,#0
0034 e9f0            34 	jpr	ge,L20001
0036 61fc  0006      35 	ld	r12,6(fp)
003a a9fb            36 	inc	fp,#~L2
003c 9e08            37 	ret
                     38 	~L1 := 0
                     39 	~L2 := 12
                     40 	}	/* _swab */
                     41 
                     42 .data
                     43 
