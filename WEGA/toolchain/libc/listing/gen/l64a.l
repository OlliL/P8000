LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 L12:
                      5 .=.+6
0006 00               6 .byte	0
0007 00               7 .byte	0
                      8 /*~~l64a:*/
                      9 
                     10 .code
                     11 
                     12 _l64a::
                     13 {
0000 abfb            14 	dec	fp,#~L2
0002 1df6            15 	ldl	@fp,rr6
0004 bd16            16 	ldk	r1,#6
0006 2105  0000'     17 	ld	r5,#L12
000a e81c            18 	jpr	L13
                     19 L20001:
000c 54f2  0000      20 	ldl	rr2,~L1(fp)
0010 9c28            21 	testl	rr2
0012 e61c            22 	jpr	eq,L14
0014 bd20            23 	ldk	r2,#0
0016 0703  003f      24 	and	r3,#63
001a 1602  0000      25 	addl	rr2,#46
001e 002e  
0020 a134            26 	ld	r4,r3
0022 0b03  0039      27 	cp	r3,#57
0026 e201            28 	jpr	le,L15
0028 a946            29 	inc	r4,#7
                     30 L15:
002a 0b04  005a      31 	cp	r4,#90
002e e201            32 	jpr	le,L16
0030 a945            33 	inc	r4,#6
                     34 L16:
0032 a142            35 	ld	r2,r4
0034 2e5a            36 	ldb	@r5,rl2
0036 a950            37 	inc	r5,#1
0038 54f2  0000      38 	ldl	rr2,~L1(fp)
003c b32d  fffa      39 	sral	rr2,#6
0040 5df2  0000      40 	ldl	~L1(fp),rr2
                     41 L13:
0044 ab10            42 	dec	r1,#1
0046 0b01  0000      43 	cp	r1,#0
004a eae0            44 	jpr	gt,L20001
                     45 L14:
004c 0c58            46 	clrb	@r5
004e 2102  0000'     47 	ld	r2,#L12
0052 a9fb            48 	inc	fp,#~L2
0054 9e08            49 	ret
                     50 	~L1 := 0
                     51 	~L2 := 12
                     52 	}	/* _l64a */
                     53 
                     54 .data
                     55 
