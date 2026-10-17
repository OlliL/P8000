LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 .even
                      5 _opterr::
0000 0001             6 .word	%1
                      7 .even
                      8 _optind::
0002 0001             9 .word	%1
                     10 .comm	2,	_optopt;
                     11 .comm	2,	_optarg;
                     12 .even
                     13 L25:
0004 0001            14 .word	%1
                     15 L29:
0006 2d  2d  00      16 .byte	%2d,%2d,%0
                     17 .even
                     18 L34:
000a 3a  20  69      19 .byte	%3a,%20,%69,%6c,%6c,%65,%67,%61
000d 6c  6c  65  
0010 67  61  
0012 6c  20  6f      20 .byte	%6c,%20,%6f,%70,%74,%69,%6f,%6e
0015 70  74  69  
0018 6f  6e  
001a 20  2d  2d      21 .byte	%20,%2d,%2d,%20,%0
001d 20  00  
                     22 .even
                     23 L42:
0020 3a  20  6f      24 .byte	%3a,%20,%6f,%70,%74,%69,%6f,%6e
0023 70  74  69  
0026 6f  6e  
0028 20  72  65      25 .byte	%20,%72,%65,%71,%75,%69,%72,%65
002b 71  75  69  
002e 72  65  
0030 73  20  61      26 .byte	%73,%20,%61,%6e,%20,%61,%72,%67
0033 6e  20  61  
0036 72  67  
0038 75  6d  65      27 .byte	%75,%6d,%65,%6e,%74,%20,%2d,%2d
003b 6e  74  20  
003e 2d  2d  
0040 20  00          28 .byte	%20,%0
                     29 .even
                     30 /*~~getopt:*/
                     31 
                     32 .code
                     33 
                     34 _getopt::
                     35 {
0000 abfb            36 	dec	fp,#~L2
0002 5cf9  0d01      37 	ldm	8(fp),r13,#2
0006 0008  
0008 1cf9  0502      38 	ldm	@fp,r5,#3
000c 4d01  0004'     39 	cp	L25,#1
0010 0001  
0012 ee23            40 	jpr	ne,L26
0014 6104  0002'     41 	ld	r4,_optind
0018 8b74            42 	cp	r4,r7
001a e96c            43 	jpr	ge,L10003
001c 8144            44 	add	r4,r4
001e a162            45 	ld	r2,r6
0020 8142            46 	add	r2,r4
0022 2123            47 	ld	r3,@r2
0024 0c31  2d2d      48 	cpb	@r3,#45
0028 ee65            49 	jpr	ne,L10003
002a 6102  0002'     50 	ld	r2,_optind
002e 8122            51 	add	r2,r2
0030 61f3  0002      52 	ld	r3,~L1+2(fp)
0034 8123            53 	add	r3,r2
0036 2132            54 	ld	r2,@r3
0038 4c24  0001      55 	testb	1(r2)
003c e65b            56 	jpr	eq,L10003
003e 6102  0002'     57 	ld	r2,_optind
0042 8122            58 	add	r2,r2
0044 61f7  0002      59 	ld	r7,~L1+2(fp)
0048 8127            60 	add	r7,r2
004a 2176            61 	ld	r6,@r7
004c a167            62 	ld	r7,r6
004e 2106  0006'     63 	ld	r6,#L29
0052 5f00  0000*     64 	callr	_strcmp
0056 8d24            65 	test	r2
0058 e64b            66 	jpr	eq,L20004
                     67 L26:
005a 6102  0004'     68 	ld	r2,L25
005e 6103  0002'     69 	ld	r3,_optind
0062 8133            70 	add	r3,r3
0064 61f4  0002      71 	ld	r4,~L1+2(fp)
0068 8134            72 	add	r4,r3
006a 2143            73 	ld	r3,@r4
006c 8123            74 	add	r3,r2
006e 203c            75 	ldb	rl4,@r3
0070 b140            76 	extsb	r4
0072 a14e            77 	ld	r14,r4
0074 6f04  0000*     78 	ld	_optopt,r4
0078 0b0e  003a      79 	cp	r14,#58
007c e608            80 	jpr	eq,L10004
007e 61f7  0000      81 	ld	r7,~L1(fp)
0082 a1e6            82 	ld	r6,r14
0084 5f00  0000*     83 	callr	_strchr
0088 a12d            84 	ld	r13,r2
008a 8d24            85 	test	r2
008c ee36            86 	jpr	ne,L31
                     87 L10004:
008e 4d04  0000'     88 	test	_opterr
0092 e618            89 	jpr	eq,L32
0094 61f7  0002      90 	ld	r7,~L1+2(fp)
0098 2176            91 	ld	r6,@r7
009a a167            92 	ld	r7,r6
009c 2106  0010*     93 	ld	r6,#__iob+16
00a0 5f00  0000*     94 	callr	_fputs
00a4 2107  000a'     95 	ld	r7,#L34
00a8 2106  0010*     96 	ld	r6,#__iob+16
00ac 5f00  0000*     97 	callr	_fputs
00b0 a1e7            98 	ld	r7,r14
00b2 2106  0010*     99 	ld	r6,#__iob+16
00b6 5f00  0000*    100 	callr	_fputc
00ba bd7a           101 	ldk	r7,#10
00bc 2106  0010*    102 	ld	r6,#__iob+16
00c0 5f00  0000*    103 	callr	_fputc
                    104 L32:
00c4 6900  0004'    105 	inc	L25,#1
00c8 6102  0004'    106 	ld	r2,L25
00cc 6103  0002'    107 	ld	r3,_optind
00d0 8133           108 	add	r3,r3
00d2 61f4  0002     109 	ld	r4,~L1+2(fp)
00d6 8134           110 	add	r4,r3
00d8 2143           111 	ld	r3,@r4
00da 8123           112 	add	r3,r2
00dc 0c34           113 	testb	@r3
00de ee05           114 	jpr	ne,L36
00e0 6900  0002'    115 	inc	_optind,#1
                    116 L20002:
00e4 4d05  0004'    117 	ld	L25,#1
00e8 0001  
                    118 L36:
00ea 2102  003f     119 	ld	r2,#63
00ee e869           120 	jpr	L10000
                    121 L20004:
00f0 6900  0002'    122 	inc	_optind,#1
                    123 L10003:
00f4 2102  ffff     124 	ld	r2,#-1
00f8 e864           125 	jpr	L10000
                    126 L31:
00fa a9d0           127 	inc	r13,#1
00fc 0cd1  3a3a     128 	cpb	@r13,#58
0100 ee4a           129 	jpr	ne,L37
0102 6102  0004'    130 	ld	r2,L25
0106 a920           131 	inc	r2,#1
0108 6103  0002'    132 	ld	r3,_optind
010c 8133           133 	add	r3,r3
010e 61f4  0002     134 	ld	r4,~L1+2(fp)
0112 8134           135 	add	r4,r3
0114 2143           136 	ld	r3,@r4
0116 8123           137 	add	r3,r2
0118 0c34           138 	testb	@r3
011a e60b           139 	jpr	eq,L38
011c 6102  0002'    140 	ld	r2,_optind
0120 8122           141 	add	r2,r2
0122 61f3  0002     142 	ld	r3,~L1+2(fp)
0126 8123           143 	add	r3,r2
0128 6102  0004'    144 	ld	r2,L25
012c a920           145 	inc	r2,#1
012e 0132           146 	add	r2,@r3
0130 e82a           147 	jpr	L20001
                    148 L38:
0132 6900  0002'    149 	inc	_optind,#1
0136 6102  0002'    150 	ld	r2,_optind
013a 4bf2  0004     151 	cp	r2,~L1+4(fp)
013e e11c           152 	jpr	lt,L40
0140 4d04  0000'    153 	test	_opterr
0144 e6cf           154 	jpr	eq,L20002
0146 61f7  0002     155 	ld	r7,~L1+2(fp)
014a 2173           156 	ld	r3,@r7
014c a137           157 	ld	r7,r3
014e 2106  0010*    158 	ld	r6,#__iob+16
0152 5f00  0000*    159 	callr	_fputs
0156 2107  0020'    160 	ld	r7,#L42
015a 2106  0010*    161 	ld	r6,#__iob+16
015e 5f00  0000*    162 	callr	_fputs
0162 a1e7           163 	ld	r7,r14
0164 2106  0010*    164 	ld	r6,#__iob+16
0168 5f00  0000*    165 	callr	_fputc
016c bd7a           166 	ldk	r7,#10
016e 2106  0010*    167 	ld	r6,#__iob+16
0172 5f00  0000*    168 	callr	_fputc
0176 e8b6           169 	jpr	L20002
                    170 L40:
0178 6102  0002'    171 	ld	r2,_optind
017c 8122           172 	add	r2,r2
017e 61f3  0002     173 	ld	r3,~L1+2(fp)
0182 8123           174 	add	r3,r2
0184 2132           175 	ld	r2,@r3
                    176 L20001:
0186 6f02  0000*    177 	ld	_optarg,r2
018a 6900  0002'    178 	inc	_optind,#1
018e 4d05  0004'    179 	ld	L25,#1
0192 0001  
0194 e815           180 	jpr	L43
                    181 L37:
0196 6900  0004'    182 	inc	L25,#1
019a 6102  0004'    183 	ld	r2,L25
019e 6103  0002'    184 	ld	r3,_optind
01a2 8133           185 	add	r3,r3
01a4 61f4  0002     186 	ld	r4,~L1+2(fp)
01a8 8134           187 	add	r4,r3
01aa 2143           188 	ld	r3,@r4
01ac 8123           189 	add	r3,r2
01ae 0c34           190 	testb	@r3
01b0 ee05           191 	jpr	ne,L44
01b2 4d05  0004'    192 	ld	L25,#1
01b6 0001  
01b8 6900  0002'    193 	inc	_optind,#1
                    194 L44:
01bc 4d08  0000*    195 	clr	_optarg
                    196 L43:
01c0 a1e2           197 	ld	r2,r14
                    198 L10000:
01c2 5cf1  0d01     199 	ldm	r13,8(fp),#2
01c6 0008  
01c8 a9fb           200 	inc	fp,#~L2
01ca 9e08           201 	ret
                    202 	~L1 := 0
                    203 	~L2 := 12
                    204 	}	/* _getopt */
                    205 
                    206 .data
                    207 
