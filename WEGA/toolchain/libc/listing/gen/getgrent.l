LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 _GROUP:
0000 2f               5 .byte	%2f
0001 65               6 .byte	%65
0002 74               7 .byte	%74
0003 63               8 .byte	%63
0004 2f               9 .byte	%2f
0005 67              10 .byte	%67
0006 72              11 .byte	%72
0007 6f              12 .byte	%6f
0008 75              13 .byte	%75
0009 70              14 .byte	%70
000a 00              15 .byte	%0
000b 00              16 .byte	0
                     17 .even
                     18 _grf:
000c 0000            19 .word	%0
                     20 _line:
                     21 .=.+512
020e 00              22 .byte	0
020f 00              23 .byte	0
                     24 .even
                     25 _group:
                     26 .=.+8
                     27 .even
                     28 _gr_mem:
                     29 .=.+200
                     30 L26:
02e0 72  00          31 .byte	%72,%0
                     32 .even
                     33 /*~~setgrent:*/
                     34 
                     35 .code
                     36 
                     37 _setgrent::
                     38 {
0000 4d04  000c'     39 	test	_grf
0004 ee09            40 	jpr	ne,L25
0006 2107  0000'     41 	ld	r7,#_GROUP
000a 2106  02e0'     42 	ld	r6,#L26
000e 5f00  0000*     43 	callr	_fopen
0012 6f02  000c'     44 	ld	_grf,r2
0016 e804            45 	jpr	L10000
                     46 L25:
0018 6107  000c'     47 	ld	r7,_grf
001c 5f00  0000*     48 	callr	_rewind
                     49 L10000:
0020 9e08            50 	ret
                     51 	~L1 := 0
                     52 	~L2 := 0
                     53 	}	/* _setgrent */
                     54 
                     55 .data
                     56 
                     57 /*~~endgrent:*/
                     58 
                     59 .code
                     60 
                     61 _endgrent::
                     62 {
0022 4d04  000c'     63 	test	_grf
0026 e606            64 	jpr	eq,L10003
0028 6107  000c'     65 	ld	r7,_grf
002c 5f00  0000*     66 	callr	_fclose
0030 4d08  000c'     67 	clr	_grf
                     68 L10003:
0034 9e08            69 	ret
                     70 	~L1 := 0
                     71 	~L2 := 0
                     72 	}	/* _endgrent */
                     73 
                     74 .data
                     75 
                     76 
                     77 .code
                     78 
                     79 _grskip:
                     80 {
0036 abf9            81 	dec	fp,#~L2
0038 a163            82 	ld	r3,r6
003a a171            83 	ld	r1,r7
003c e805            84 	jpr	L35
                     85 L20001:
003e 201a            86 	ldb	rl2,@r1
0040 b120            87 	extsb	r2
0042 8b23            88 	cp	r3,r2
0044 e603            89 	jpr	eq,L36
0046 a910            90 	inc	r1,#1
                     91 L35:
0048 0c14            92 	testb	@r1
004a eef9            93 	jpr	ne,L20001
                     94 L36:
004c 0c14            95 	testb	@r1
004e e602            96 	jpr	eq,L37
0050 0c18            97 	clrb	@r1
0052 a910            98 	inc	r1,#1
                     99 L37:
0054 a112           100 	ld	r2,r1
0056 a9f9           101 	inc	fp,#~L2
0058 9e08           102 	ret
                    103 	~L1 := 0
                    104 	~L2 := 10
                    105 	}	/* _grskip */
                    106 
                    107 .data
                    108 
                    109 L40:
02e2 72  00         110 .byte	%72,%0
                    111 .even
                    112 /*~~getgrent:*/
                    113 
                    114 .code
                    115 
                    116 _getgrent::
                    117 {
005a abf7           118 	dec	fp,#~L2
005c 5cf9  0d01     119 	ldm	4(fp),r13,#2
0060 0004  
0062 4d04  000c'    120 	test	_grf
0066 ee0c           121 	jpr	ne,L41
0068 2107  0000'    122 	ld	r7,#_GROUP
006c 2106  02e2'    123 	ld	r6,#L40
0070 5f00  0000*    124 	callr	_fopen
0074 6f02  000c'    125 	ld	_grf,r2
0078 8d24           126 	test	r2
007a ee02           127 	jpr	ne,L41
                    128 L20004:
007c 8322           129 	sub	r2,r2
007e e83a           130 	jpr	L10009
                    131 L41:
0080 2107  000e'    132 	ld	r7,#_line
0084 2106  0200     133 	ld	r6,#512
0088 6105  000c'    134 	ld	r5,_grf
008c 5f00  0000*    135 	callr	_fgets
0090 a12e           136 	ld	r14,r2
0092 8d24           137 	test	r2
0094 e6f3           138 	jpr	eq,L20004
0096 6f02  0210'    139 	ld	_group,r2
009a a127           140 	ld	r7,r2
009c 2106  003a     141 	ld	r6,#58
00a0 d036           142 	callr	_grskip
00a2 a12e           143 	ld	r14,r2
00a4 6f02  0212'    144 	ld	_group+2,r2
00a8 a1e7           145 	ld	r7,r14
00aa 2106  003a     146 	ld	r6,#58
00ae d03d           147 	callr	_grskip
00b0 a12e           148 	ld	r14,r2
00b2 6ff2  0002     149 	ld	~L1+2(fp),r2
00b6 a127           150 	ld	r7,r2
00b8 5f00  0000*    151 	callr	_atoi
00bc 6f02  0214'    152 	ld	_group+4,r2
00c0 4d05  0216'    153 	ld	_group+6,#_gr_mem
00c4 0218' 
00c6 a1e7           154 	ld	r7,r14
00c8 2106  003a     155 	ld	r6,#58
00cc d04c           156 	callr	_grskip
00ce a12e           157 	ld	r14,r2
00d0 a127           158 	ld	r7,r2
00d2 bd6a           159 	ldk	r6,#10
00d4 d050           160 	callr	_grskip
00d6 210d  0218'    161 	ld	r13,#_gr_mem
00da e807           162 	jpr	L44
                    163 L20003:
00dc 2fde           164 	ld	@r13,r14
00de a9d1           165 	inc	r13,#2
00e0 a1e7           166 	ld	r7,r14
00e2 2106  002c     167 	ld	r6,#44
00e6 d059           168 	callr	_grskip
00e8 a12e           169 	ld	r14,r2
                    170 L44:
00ea 0ce4           171 	testb	@r14
00ec eef7           172 	jpr	ne,L20003
00ee 0dd8           173 	clr	@r13
00f0 2102  0210'    174 	ld	r2,#_group
                    175 L10009:
00f4 5cf1  0d01     176 	ldm	r13,4(fp),#2
00f8 0004  
00fa a9f7           177 	inc	fp,#~L2
00fc 9e08           178 	ret
                    179 	~L1 := 0
                    180 	~L2 := 8
                    181 	}	/* _getgrent */
                    182 
                    183 .data
                    184 
