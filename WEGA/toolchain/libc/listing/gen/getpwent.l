LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 _PASSWD:
0000 2f               5 .byte	%2f
0001 65               6 .byte	%65
0002 74               7 .byte	%74
0003 63               8 .byte	%63
0004 2f               9 .byte	%2f
0005 70              10 .byte	%70
0006 61              11 .byte	%61
0007 73              12 .byte	%73
0008 73              13 .byte	%73
0009 77              14 .byte	%77
000a 64              15 .byte	%64
000b 00              16 .byte	%0
                     17 _EMPTY:
000c 00              18 .byte	%0
000d 00              19 .byte	0
                     20 .even
                     21 _pwf:
000e 0000            22 .word	%0
                     23 _line:
                     24 .=.+512
0210 00              25 .byte	0
0211 00              26 .byte	0
                     27 .even
                     28 _passwd:
                     29 .=.+18
                     30 L26:
0224 72  00          31 .byte	%72,%0
                     32 .even
                     33 /*~~setpwent:*/
                     34 
                     35 .code
                     36 
                     37 _setpwent::
                     38 {
0000 4d04  000e'     39 	test	_pwf
0004 ee09            40 	jpr	ne,L25
0006 2107  0000'     41 	ld	r7,#_PASSWD
000a 2106  0224'     42 	ld	r6,#L26
000e 5f00  0000*     43 	callr	_fopen
0012 6f02  000e'     44 	ld	_pwf,r2
0016 e804            45 	jpr	L10000
                     46 L25:
0018 6107  000e'     47 	ld	r7,_pwf
001c 5f00  0000*     48 	callr	_rewind
                     49 L10000:
0020 9e08            50 	ret
                     51 	~L1 := 0
                     52 	~L2 := 0
                     53 	}	/* _setpwent */
                     54 
                     55 .data
                     56 
                     57 /*~~endpwent:*/
                     58 
                     59 .code
                     60 
                     61 _endpwent::
                     62 {
0022 4d04  000e'     63 	test	_pwf
0026 e606            64 	jpr	eq,L10003
0028 6107  000e'     65 	ld	r7,_pwf
002c 5f00  0000*     66 	callr	_fclose
0030 4d08  000e'     67 	clr	_pwf
                     68 L10003:
0034 9e08            69 	ret
                     70 	~L1 := 0
                     71 	~L2 := 0
                     72 	}	/* _endpwent */
                     73 
                     74 .data
                     75 
                     76 
                     77 .code
                     78 
                     79 _pwskip:
                     80 {
0036 abf5            81 	dec	fp,#~L2
0038 6ffe  0004      82 	ld	4(fp),r14
003c a17e            83 	ld	r14,r7
003e e807            84 	jpr	L35
                     85 L20001:
0040 0ce1  3a3a      86 	cpb	@r14,#58
0044 e606            87 	jpr	eq,L36
0046 0ce1  0a0a      88 	cpb	@r14,#10
004a e603            89 	jpr	eq,L36
004c a9e0            90 	inc	r14,#1
                     91 L35:
004e 0ce4            92 	testb	@r14
0050 eef7            93 	jpr	ne,L20001
                     94 L36:
0052 0ce1  0a0a      95 	cpb	@r14,#10
0056 ee02            96 	jpr	ne,L37
0058 0ce8            97 	clrb	@r14
005a e804            98 	jpr	L38
                     99 L37:
005c 0ce4           100 	testb	@r14
005e e602           101 	jpr	eq,L38
0060 0ce8           102 	clrb	@r14
0062 a9e0           103 	inc	r14,#1
                    104 L38:
0064 a1e2           105 	ld	r2,r14
0066 61fe  0004     106 	ld	r14,4(fp)
006a a9f5           107 	inc	fp,#~L2
006c 9e08           108 	ret
                    109 	~L1 := 0
                    110 	~L2 := 6
                    111 	}	/* _pwskip */
                    112 
                    113 .data
                    114 
                    115 L43:
0226 72  00         116 .byte	%72,%0
                    117 .even
                    118 /*~~getpwent:*/
                    119 
                    120 .code
                    121 
                    122 _getpwent::
                    123 {
006e abf3           124 	dec	fp,#~L2
0070 6ffe  0002     125 	ld	2(fp),r14
0074 4d04  000e'    126 	test	_pwf
0078 ee0c           127 	jpr	ne,L42
007a 2107  0000'    128 	ld	r7,#_PASSWD
007e 2106  0226'    129 	ld	r6,#L43
0082 5f00  0000*    130 	callr	_fopen
0086 6f02  000e'    131 	ld	_pwf,r2
008a 8d24           132 	test	r2
008c ee02           133 	jpr	ne,L42
                    134 L20004:
008e 8322           135 	sub	r2,r2
0090 e843           136 	jpr	L10009
                    137 L42:
0092 2107  0010'    138 	ld	r7,#_line
0096 2106  0200     139 	ld	r6,#512
009a 6105  000e'    140 	ld	r5,_pwf
009e 5f00  0000*    141 	callr	_fgets
00a2 a12e           142 	ld	r14,r2
00a4 8d24           143 	test	r2
00a6 e6f3           144 	jpr	eq,L20004
00a8 6f02  0212'    145 	ld	_passwd,r2
00ac a127           146 	ld	r7,r2
00ae d03d           147 	callr	_pwskip
00b0 6f02  0214'    148 	ld	_passwd+2,r2
00b4 a127           149 	ld	r7,r2
00b6 d041           150 	callr	_pwskip
00b8 a12e           151 	ld	r14,r2
00ba a127           152 	ld	r7,r2
00bc 5f00  0000*    153 	callr	_atoi
00c0 6f02  0216'    154 	ld	_passwd+4,r2
00c4 a1e7           155 	ld	r7,r14
00c6 d049           156 	callr	_pwskip
00c8 a12e           157 	ld	r14,r2
00ca a127           158 	ld	r7,r2
00cc 5f00  0000*    159 	callr	_atoi
00d0 6f02  0218'    160 	ld	_passwd+6,r2
00d4 4d05  021c'    161 	ld	_passwd+10,#_EMPTY
00d8 000c' 
00da a1e7           162 	ld	r7,r14
00dc d054           163 	callr	_pwskip
00de 6f02  021e'    164 	ld	_passwd+12,r2
00e2 a127           165 	ld	r7,r2
00e4 d058           166 	callr	_pwskip
00e6 6f02  0220'    167 	ld	_passwd+14,r2
00ea a127           168 	ld	r7,r2
00ec d05c           169 	callr	_pwskip
00ee 6f02  0222'    170 	ld	_passwd+16,r2
00f2 a127           171 	ld	r7,r2
00f4 d060           172 	callr	_pwskip
00f6 610e  0214'    173 	ld	r14,_passwd+2
00fa e804           174 	jpr	L47
                    175 L20003:
00fc 0ce1  2c2c     176 	cpb	@r14,#44
0100 e603           177 	jpr	eq,L48
0102 a9e0           178 	inc	r14,#1
                    179 L47:
0104 0ce4           180 	testb	@r14
0106 eefa           181 	jpr	ne,L20003
                    182 L48:
0108 0ce4           183 	testb	@r14
010a e602           184 	jpr	eq,L49
010c 0ce8           185 	clrb	@r14
010e a9e0           186 	inc	r14,#1
                    187 L49:
0110 6f0e  021a'    188 	ld	_passwd+8,r14
0114 2102  0212'    189 	ld	r2,#_passwd
                    190 L10009:
0118 61fe  0002     191 	ld	r14,2(fp)
011c a9f3           192 	inc	fp,#~L2
011e 9e08           193 	ret
                    194 	~L1 := 0
                    195 	~L2 := 4
                    196 	}	/* _getpwent */
                    197 
                    198 .data
                    199 
