LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 _dev:
0000 2f               5 .byte	%2f
0001 64               6 .byte	%64
0002 65               7 .byte	%65
0003 76               8 .byte	%76
0004 2f               9 .byte	%2f
0005 00              10 .byte	%0
                     11 L15:
                     12 .=.+32
                     13 /*~~ttyname:*/
                     14 
                     15 .code
                     16 
                     17 _ttyname::
                     18 {
0000 030f  0052      19 	sub	fp,#~L2
0004 6ffe  0050      20 	ld	80(fp),r14
0008 2ff7            21 	ld	@fp,r7
000a a176            22 	ld	r6,r7
000c 5f00  0000*     23 	callr	_isatty
0010 8d24            24 	test	r2
0012 e653            25 	jpr	eq,L20000
0014 76f6  0002      26 	lda	r6,~L1+2(fp)
0018 61f7  0000      27 	ld	r7,~L1(fp)
001c 5f00  0000*     28 	callr	_fstat
0020 0b02  0000      29 	cp	r2,#0
0024 e14a            30 	jpr	lt,L20000
0026 61f2  0006      31 	ld	r2,~L1+6(fp)
002a 0702  f000      32 	and	r2,#61440
002e 0b02  2000      33 	cp	r2,#8192
0032 ee43            34 	jpr	ne,L20000
0034 2107  0000'     35 	ld	r7,#_dev
0038 8366            36 	sub	r6,r6
003a 5f00  0000*     37 	callr	_open
003e a12e            38 	ld	r14,r2
0040 0b02  0000      39 	cp	r2,#0
0044 e13a            40 	jpr	lt,L20000
                     41 L24:
0046 76f6  003e      42 	lda	r6,~L1+62(fp)
004a a1e7            43 	ld	r7,r14
004c 2105  0010      44 	ld	r5,#16
0050 5f00  0000*     45 	callr	_read
0054 0b02  0010      46 	cp	r2,#16
0058 ee2d            47 	jpr	ne,L25
005a 4df4  003e      48 	test	~L1+62(fp)
005e e6f3            49 	jpr	eq,L24
0060 61f2  003e      50 	ld	r2,~L1+62(fp)
0064 4bf2  0004      51 	cp	r2,~L1+4(fp)
0068 eeee            52 	jpr	ne,L24
006a 2107  0006'     53 	ld	r7,#L15
006e 2106  0000'     54 	ld	r6,#_dev
0072 5f00  0000*     55 	callr	_strcpy
0076 76f6  0040      56 	lda	r6,~L1+64(fp)
007a 2107  0006'     57 	ld	r7,#L15
007e 5f00  0000*     58 	callr	_strcat
0082 76f6  0020      59 	lda	r6,~L1+32(fp)
0086 2107  0006'     60 	ld	r7,#L15
008a 5f00  0000*     61 	callr	_stat
008e 0b02  0000      62 	cp	r2,#0
0092 e1d9            63 	jpr	lt,L24
0094 61f2  0020      64 	ld	r2,~L1+32(fp)
0098 4bf2  0002      65 	cp	r2,~L1+2(fp)
009c eed4            66 	jpr	ne,L24
009e 61f3  0022      67 	ld	r3,~L1+34(fp)
00a2 4bf3  0004      68 	cp	r3,~L1+4(fp)
00a6 eecf            69 	jpr	ne,L24
00a8 a1e7            70 	ld	r7,r14
00aa 5f00  0000*     71 	callr	_close
00ae 2102  0006'     72 	ld	r2,#L15
00b2 e804            73 	jpr	L10000
                     74 L25:
00b4 a1e7            75 	ld	r7,r14
00b6 5f00  0000*     76 	callr	_close
                     77 L20000:
00ba 8322            78 	sub	r2,r2
                     79 L10000:
00bc 61fe  0050      80 	ld	r14,80(fp)
00c0 010f  0052      81 	add	fp,#~L2
00c4 9e08            82 	ret
                     83 	~L1 := 0
                     84 	~L2 := 82
                     85 	}	/* _ttyname */
                     86 
                     87 .data
                     88 
