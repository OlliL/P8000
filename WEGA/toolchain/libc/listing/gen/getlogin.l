LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 _UTMP:
0000 2f               5 .byte	%2f
0001 65               6 .byte	%65
0002 74               7 .byte	%74
0003 63               8 .byte	%63
0004 2f               9 .byte	%2f
0005 75              10 .byte	%75
0006 74              11 .byte	%74
0007 6d              12 .byte	%6d
0008 70              13 .byte	%70
0009 00              14 .byte	%0
                     15 .even
                     16 _ubuf:
                     17 .=.+20
                     18 /*~~getlogin:*/
                     19 
                     20 .code
                     21 
                     22 _getlogin::
                     23 {
0000 abf7            24 	dec	fp,#~L2
0002 5cf9  0c02      25 	ldm	2(fp),r12,#3
0006 0002  
0008 5f00  0000*     26 	callr	_ttyslot
000c a12e            27 	ld	r14,r2
000e 0b02  0000      28 	cp	r2,#0
0012 e104            29 	jpr	lt,L20000
0014 e805            30 	jpr	L15
                     31 L20002:
0016 a1d7            32 	ld	r7,r13
0018 5f00  0000*     33 	callr	_close
                     34 L20000:
001c 8322            35 	sub	r2,r2
001e e832            36 	jpr	L10000
                     37 L15:
0020 2107  0000'     38 	ld	r7,#_UTMP
0024 8366            39 	sub	r6,r6
0026 5f00  0000*     40 	callr	_open
002a a12d            41 	ld	r13,r2
002c 0b02  0000      42 	cp	r2,#0
0030 e1f5            43 	jpr	lt,L20000
0032 a1e5            44 	ld	r5,r14
0034 1904  0014      45 	mult	rr4,#20
0038 a153            46 	ld	r3,r5
003a b12a            47 	exts	rr2
003c 9424            48 	ldl	rr4,rr2
003e a1d7            49 	ld	r7,r13
0040 8333            50 	sub	r3,r3
0042 5f00  0000*     51 	callr	_lseek
0046 a1d7            52 	ld	r7,r13
0048 2106  000a'     53 	ld	r6,#_ubuf
004c 2105  0014      54 	ld	r5,#20
0050 5f00  0000*     55 	callr	_read
0054 0b02  0014      56 	cp	r2,#20
0058 eede            57 	jpr	ne,L20002
005a a1d7            58 	ld	r7,r13
005c 5f00  0000*     59 	callr	_close
0060 4c04  0012'     60 	testb	_ubuf+8
0064 e6db            61 	jpr	eq,L20000
0066 4c05  001a'     62 	ldb	_ubuf+16,#32
006a 2020  
006c 210c  0012'     63 	ld	r12,#_ubuf+8
                     64 L25:
0070 a1c2            65 	ld	r2,r12
0072 a9c0            66 	inc	r12,#1
0074 202b            67 	ldb	rl3,@r2
0076 0a0b  2020      68 	cpb	rl3,#32
007a eefa            69 	jpr	ne,L25
007c abc0            70 	dec	r12,#1
007e 0cc8            71 	clrb	@r12
0080 2102  0012'     72 	ld	r2,#_ubuf+8
                     73 L10000:
0084 5cf1  0c02      74 	ldm	r12,2(fp),#3
0088 0002  
008a a9f7            75 	inc	fp,#~L2
008c 9e08            76 	ret
                     77 	~L1 := 0
                     78 	~L2 := 8
                     79 	}	/* _getlogin */
                     80 
                     81 .data
                     82 
