LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~bsearch:*/
                      5 
                      6 .code
                      7 
                      8 _bsearch::
                      9 {
0000 030f  0014      10 	sub	fp,#~L2
0004 1cf9  0304      11 	ldm	@fp,r3,#5
0008 ab50            12 	dec	r5,#1
000a a151            13 	ld	r1,r5
000c 9940            14 	mult	rr0,r4
000e 8116            15 	add	r6,r1
0010 6ff6  000a      16 	ld	~L1+10(fp),r6
0014 61f2  0006      17 	ld	r2,~L1+6(fp)
0018 6ff2  000c      18 	ld	~L1+12(fp),r2
001c e83b            19 	jpr	L12
                     20 L20001:
001e 61f3  0004      21 	ld	r3,~L1+4(fp)
0022 ab30            22 	dec	r3,#1
0024 a135            23 	ld	r5,r3
0026 b14a            24 	exts	rr4
0028 1b04  0002      25 	div	rr4,#2
002c 61f7  0002      26 	ld	r7,~L1+2(fp)
0030 9956            27 	mult	rr6,r5
0032 41f7  000c      28 	add	r7,~L1+12(fp)
0036 6ff7  000e      29 	ld	~L1+14(fp),r7
003a 61f3  0000      30 	ld	r3,~L1(fp)
003e 6ff3  0012      31 	ld	~L1+18(fp),r3
0042 61f7  0008      32 	ld	r7,~L1+8(fp)
0046 61f6  000e      33 	ld	r6,~L1+14(fp)
004a a131            34 	ld	r1,r3
004c 1f10            35 	call	@r1
004e 6ff2  0010      36 	ld	~L1+16(fp),r2
0052 8d24            37 	test	r2
0054 ee03            38 	jpr	ne,L14
0056 61f2  000e      39 	ld	r2,~L1+14(fp)
005a e822            40 	jpr	L10000
                     41 L14:
005c 4df1  0010      42 	cp	~L1+16(fp),#0
0060 0000  
0062 e907            43 	jpr	ge,L15
0064 61f2  000e      44 	ld	r2,~L1+14(fp)
0068 43f2  0002      45 	sub	r2,~L1+2(fp)
006c 6ff2  000a      46 	ld	~L1+10(fp),r2
0070 e806            47 	jpr	L16
                     48 L15:
0072 61f2  000e      49 	ld	r2,~L1+14(fp)
0076 41f2  0002      50 	add	r2,~L1+2(fp)
007a 6ff2  000c      51 	ld	~L1+12(fp),r2
                     52 L16:
007e 61f2  000a      53 	ld	r2,~L1+10(fp)
0082 43f2  000c      54 	sub	r2,~L1+12(fp)
0086 a125            55 	ld	r5,r2
0088 b14a            56 	exts	rr4
008a 5bf4  0002      57 	div	rr4,~L1+2(fp)
008e a950            58 	inc	r5,#1
0090 6ff5  0004      59 	ld	~L1+4(fp),r5
                     60 L12:
0094 61f2  000a      61 	ld	r2,~L1+10(fp)
0098 4bf2  000c      62 	cp	r2,~L1+12(fp)
009c efc0            63 	jpr	uge,L20001
009e 8322            64 	sub	r2,r2
                     65 L10000:
00a0 010f  0014      66 	add	fp,#~L2
00a4 9e08            67 	ret
                     68 	~L1 := 0
                     69 	~L2 := 20
                     70 	}	/* _bsearch */
                     71 
                     72 .data
                     73 
