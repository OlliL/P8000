LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 _utmp:
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
                     15 /*~~ttyslot:*/
                     16 
                     17 .code
                     18 
                     19 _ttyslot::
                     20 {
0000 030f  001e      21 	sub	fp,#~L2
0004 5cf9  0b03      22 	ldm	22(fp),r11,#4
0008 0016  
000a 8377            23 	sub	r7,r7
000c 5f00  0000*     24 	callr	_ttyname
0010 a12e            25 	ld	r14,r2
0012 8d24            26 	test	r2
0014 ee0c            27 	jpr	ne,L15
0016 bd71            28 	ldk	r7,#1
0018 5f00  0000*     29 	callr	_ttyname
001c a12e            30 	ld	r14,r2
001e 8d24            31 	test	r2
0020 ee06            32 	jpr	ne,L15
0022 bd72            33 	ldk	r7,#2
0024 5f00  0000*     34 	callr	_ttyname
0028 a12e            35 	ld	r14,r2
002a 8d24            36 	test	r2
002c e631            37 	jpr	eq,L20002
                     38 L15:
002e a1e7            39 	ld	r7,r14
0030 2106  002f      40 	ld	r6,#47
0034 5f00  0000*     41 	callr	_strrchr
0038 a12d            42 	ld	r13,r2
003a 8d24            43 	test	r2
003c ee02            44 	jpr	ne,L16
003e a1ed            45 	ld	r13,r14
0040 e801            46 	jpr	L17
                     47 L16:
0042 a9d0            48 	inc	r13,#1
                     49 L17:
0044 2107  0000'     50 	ld	r7,#_utmp
0048 8366            51 	sub	r6,r6
004a 5f00  0000*     52 	callr	_open
004e a12b            53 	ld	r11,r2
0050 0b02  0000      54 	cp	r2,#0
0054 e11d            55 	jpr	lt,L20002
0056 bdc0            56 	ldk	r12,#0
0058 e80e            57 	jpr	L21
                     58 L20001:
005a 76f6  0000      59 	lda	r6,~L1(fp)
005e a1d7            60 	ld	r7,r13
0060 bd58            61 	ldk	r5,#8
0062 5f00  0000*     62 	callr	_strncmp
0066 8d24            63 	test	r2
0068 ee05            64 	jpr	ne,L24
006a a1b7            65 	ld	r7,r11
006c 5f00  0000*     66 	callr	_close
0070 a1c2            67 	ld	r2,r12
0072 e810            68 	jpr	L10000
                     69 L24:
0074 a9c0            70 	inc	r12,#1
                     71 L21:
0076 76f6  0000      72 	lda	r6,~L1(fp)
007a a1b7            73 	ld	r7,r11
007c 2105  0014      74 	ld	r5,#20
0080 5f00  0000*     75 	callr	_read
0084 0b02  0014      76 	cp	r2,#20
0088 e6e8            77 	jpr	eq,L20001
008a a1b7            78 	ld	r7,r11
008c 5f00  0000*     79 	callr	_close
                     80 L20002:
0090 2102  ffff      81 	ld	r2,#-1
                     82 L10000:
0094 5cf1  0b03      83 	ldm	r11,22(fp),#4
0098 0016  
009a 010f  001e      84 	add	fp,#~L2
009e 9e08            85 	ret
                     86 	~L1 := 0
                     87 	~L2 := 30
                     88 	}	/* _ttyslot */
                     89 
                     90 .data
                     91 
