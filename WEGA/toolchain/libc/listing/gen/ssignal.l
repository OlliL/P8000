LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 .even
                      5 _sigs:
                      6 .=.+42
                      7 /*~~ssignal:*/
                      8 
                      9 .code
                     10 
                     11 _ssignal::
                     12 {
0000 abfb            13 	dec	fp,#~L2
0002 a163            14 	ld	r3,r6
0004 a171            15 	ld	r1,r7
0006 0b07  fffc      16 	cp	r7,#-4
000a e10e            17 	jpr	lt,L14
000c 0b07  0010      18 	cp	r7,#16
0010 ea0b            19 	jpr	gt,L14
0012 a172            20 	ld	r2,r7
0014 ab23            21 	dec	r2,#4
0016 8122            22 	add	r2,r2
0018 6124  0000'     23 	ld	r4,_sigs(r2)
001c a112            24 	ld	r2,r1
001e ab23            25 	dec	r2,#4
0020 8122            26 	add	r2,r2
0022 6f26  0000'     27 	ld	_sigs(r2),r6
0026 e801            28 	jpr	L15
                     29 L14:
0028 bd40            30 	ldk	r4,#0
                     31 L15:
002a a142            32 	ld	r2,r4
002c a9fb            33 	inc	fp,#~L2
002e 9e08            34 	ret
                     35 	~L1 := 0
                     36 	~L2 := 12
                     37 	}	/* _ssignal */
                     38 
                     39 .data
                     40 
                     41 /*~~gsignal:*/
                     42 
                     43 .code
                     44 
                     45 _gsignal::
                     46 {
0030 abf9            47 	dec	fp,#~L2
0032 5cf9  0d01      48 	ldm	6(fp),r13,#2
0036 0006  
0038 a17e            49 	ld	r14,r7
003a 0b07  fffc      50 	cp	r7,#-4
003e e10a            51 	jpr	lt,L10006
0040 0b07  0010      52 	cp	r7,#16
0044 ea07            53 	jpr	gt,L10006
0046 a172            54 	ld	r2,r7
0048 ab23            55 	dec	r2,#4
004a 8122            56 	add	r2,r2
004c 612d  0000'     57 	ld	r13,_sigs(r2)
0050 8dd4            58 	test	r13
0052 ee02            59 	jpr	ne,L18
                     60 L10006:
0054 8322            61 	sub	r2,r2
0056 e80f            62 	jpr	L10003
                     63 L18:
0058 0b0d  0001      64 	cp	r13,#1
005c ee02            65 	jpr	ne,L19
005e bd21            66 	ldk	r2,#1
0060 e80a            67 	jpr	L10003
                     68 L19:
0062 a1e2            69 	ld	r2,r14
0064 ab23            70 	dec	r2,#4
0066 8122            71 	add	r2,r2
0068 4d28  0000'     72 	clr	_sigs(r2)
006c 6ffd  0004      73 	ld	~L1+4(fp),r13
0070 a1e7            74 	ld	r7,r14
0072 a1d1            75 	ld	r1,r13
0074 1f10            76 	call	@r1
                     77 L10003:
0076 5cf1  0d01      78 	ldm	r13,6(fp),#2
007a 0006  
007c a9f9            79 	inc	fp,#~L2
007e 9e08            80 	ret
                     81 	~L1 := 0
                     82 	~L2 := 10
                     83 	}	/* _gsignal */
                     84 
                     85 .data
                     86 
