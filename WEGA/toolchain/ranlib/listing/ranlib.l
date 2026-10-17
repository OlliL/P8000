LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 _sccsid:
0000 40               5 .byte	%40
0001 5b               6 .byte	%5b
0002 24               7 .byte	%24
0003 5d               8 .byte	%5d
0004 72               9 .byte	%72
0005 61              10 .byte	%61
0006 6e              11 .byte	%6e
0007 6c              12 .byte	%6c
0008 69              13 .byte	%69
0009 62              14 .byte	%62
000a 2e              15 .byte	%2e
000b 63              16 .byte	%63
000c 09              17 .byte	%9
000d 31              18 .byte	%31
000e 2e              19 .byte	%2e
000f 31              20 .byte	%31
0010 20              21 .byte	%20
0011 20              22 .byte	%20
0012 30              23 .byte	%30
0013 37              24 .byte	%37
0014 2f              25 .byte	%2f
0015 32              26 .byte	%32
0016 34              27 .byte	%34
0017 2f              28 .byte	%2f
0018 38              29 .byte	%38
0019 32              30 .byte	%32
001a 20              31 .byte	%20
001b 30              32 .byte	%30
001c 32              33 .byte	%32
001d 3a              34 .byte	%3a
001e 34              35 .byte	%34
001f 39              36 .byte	%39
0020 3a              37 .byte	%3a
0021 32              38 .byte	%32
0022 33              39 .byte	%33
0023 20              40 .byte	%20
0024 2d              41 .byte	%2d
0025 20              42 .byte	%20
0026 38              43 .byte	%38
0027 37              44 .byte	%37
0028 77              45 .byte	%77
0029 65              46 .byte	%65
002a 67              47 .byte	%67
002b 61              48 .byte	%61
002c 33              49 .byte	%33
002d 2e              50 .byte	%2e
002e 32              51 .byte	%32
002f 00              52 .byte	%0
                     53 .comm	26,	_archdr;
                     54 .comm	24,	_exp;
                     55 .comm	4,	_arsize;
                     56 .comm	2,	_fi;
                     57 .comm	2,	_fo;
                     58 .comm	2,	_new;
                     59 .comm	2,	_longnm;
                     60 .comm	4,	_off;
                     61 .comm	4,	_oldoff;
                     62 .comm	4,	_offdelta;
                     63 .comm	8400,	_tab;
                     64 .comm	2,	_tnum;
                     65 _tempnm::
0030 5f              66 .byte	%5f
0031 5f              67 .byte	%5f
0032 2e              68 .byte	%2e
0033 53              69 .byte	%53
0034 59              70 .byte	%59
0035 4d              71 .byte	%4d
0036 44              72 .byte	%44
0037 45              73 .byte	%45
0038 46              74 .byte	%46
0039 00              75 .byte	%0
                     76 .comm	18,	_firstname;
                     77 .comm	2,	_av0;
                     78 /*~~trim:*/
                     79 
                     80 .code
                     81 
                     82 _trim::
                     83 {
0000 abf7            84 	dec	fp,#~L2
0002 2ff7            85 	ld	@fp,r7
0004 a171            86 	ld	r1,r7
0006 a174            87 	ld	r4,r7
0008 e809            88 	jpr	L38
                     89 L20001:
000a a142            90 	ld	r2,r4
000c a940            91 	inc	r4,#1
000e 202b            92 	ldb	rl3,@r2
0010 0a0b  2f2f      93 	cpb	rl3,#47
0014 ee03            94 	jpr	ne,L38
0016 0c44            95 	testb	@r4
0018 e601            96 	jpr	eq,L38
001a a141            97 	ld	r1,r4
                     98 L38:
001c 0c44            99 	testb	@r4
001e eef5           100 	jpr	ne,L20001
0020 a112           101 	ld	r2,r1
0022 a9f7           102 	inc	fp,#~L2
0024 9e08           103 	ret
                    104 	~L1 := 0
                    105 	~L2 := 8
                    106 	}	/* _trim */
                    107 
                    108 .data
                    109 
                    110 L46:
003a 72  00         111 .byte	%72,%0
                    112 .even
                    113 L49:
003c 25  73  3a     114 .byte	%25,%73,%3a,%20,%63,%61,%6e,%6e
003f 20  63  61  
0042 6e  6e  
0044 6f  74  20     115 .byte	%6f,%74,%20,%6f,%70,%65,%6e,%20
0047 6f  70  65  
004a 6e  20  
004c 25  73  0a     116 .byte	%25,%73,%a,%0
004f 00  
                    117 .even
                    118 L52:
0050 6e  6f  74     119 .byte	%6e,%6f,%74,%20,%61,%72,%63,%68
0053 20  61  72  
0056 63  68  
0058 69  76  65     120 .byte	%69,%76,%65,%3a,%20,%25,%73,%a
005b 3a  20  25  
005e 73  0a  
0060 00             121 .byte	%0
                    122 .even
                    123 L66:
0062 25  73  3a     124 .byte	%25,%73,%3a,%20,%25,%73,%2d,%2d
0065 20  25  73  
0068 2d  2d  
006a 20  6e  6f     125 .byte	%20,%6e,%6f,%20,%6e,%61,%6d,%65
006d 20  6e  61  
0070 6d  65  
0072 20  6c  69     126 .byte	%20,%6c,%69,%73,%74,%a,%0
0075 73  74  0a  
0078 00  
                    127 .even
                    128 L74:
007a 77  00         129 .byte	%77,%0
                    130 .even
                    131 L76:
007c 63  61  6e     132 .byte	%63,%61,%6e,%27,%74,%20,%63,%72
007f 27  74  20  
0082 63  72  
0084 65  61  74     133 .byte	%65,%61,%74,%65,%20,%74,%65,%6d
0087 65  20  74  
008a 65  6d  
008c 70  6f  72     134 .byte	%70,%6f,%72,%61,%72,%79,%a,%0
008f 61  72  79  
0092 0a  00  
                    135 .even
                    136 L81:
0094 61  72  20     137 .byte	%61,%72,%20,%72,%6c,%62,%20,%25
0097 72  6c  62  
009a 20  25  
009c 73  20  25     138 .byte	%73,%20,%25,%73,%20,%25,%73,%a
009f 73  20  25  
00a2 73  0a  
00a4 00             139 .byte	%0
                    140 .even
                    141 L83:
00a6 61  72  20     142 .byte	%61,%72,%20,%72,%6c,%20,%25,%73
00a9 72  6c  20  
00ac 25  73  
00ae 20  25  73     143 .byte	%20,%25,%73,%a,%0
00b1 0a  00  
                    144 .even
                    145 L86:
00b4 63  61  6e     146 .byte	%63,%61,%6e,%27,%74,%20,%65,%78
00b7 27  74  20  
00ba 65  78  
00bc 65  63  75     147 .byte	%65,%63,%75,%74,%65,%20,%25,%73
00bf 74  65  20  
00c2 25  73  
00c4 0a  00         148 .byte	%a,%0
                    149 .even
                    150 /*~~main:*/
                    151 
                    152 .code
                    153 
                    154 _main::
                    155 {
0026 030f  0198     156 	sub	fp,#~L2
002a 6ffe  0196     157 	ld	406(fp),r14
002e 1df6           158 	ldl	@fp,rr6
0030 a167           159 	ld	r7,r6
0032 2172           160 	ld	r2,@r7
0034 a127           161 	ld	r7,r2
0036 d01c           162 	callr	_trim
0038 6f02  0000*    163 	ld	_av0,r2
003c 6bf0  0002     164 	dec	~L1+2(fp),#1
0040 5e08  025c'    165 	jpr	L44
                    166 L20003:
0044 61f4  0000     167 	ld	r4,~L1(fp)
0048 2145           168 	ld	r5,@r4
004a a154           169 	ld	r4,r5
004c 2106  003c'    170 	ld	r6,#L49
0050 6105  0000*    171 	ld	r5,_av0
                    172 L20005:
0054 2107  0010*    173 	ld	r7,#__iob+16
0058 5f00  0000*    174 	callr	_fprintf
005c 5e08  025c'    175 	jpr	L44
                    176 L20007:
0060 61f5  0000     177 	ld	r5,~L1(fp)
0064 2154           178 	ld	r4,@r5
0066 a145           179 	ld	r5,r4
0068 2106  0050'    180 	ld	r6,#L52
006c e8f3           181 	jpr	L20005
                    182 L59:
006e 2107  0000*    183 	ld	r7,#_exp
0072 bd61           184 	ldk	r6,#1
0074 2105  0018     185 	ld	r5,#24
0078 6104  0000*    186 	ld	r4,_fi
007c 5f00  0000*    187 	callr	_fread
0080 2107  0000*    188 	ld	r7,#_exp
0084 5f00  0000*    189 	callr	_goodmagic
0088 8d24           190 	test	r2
008a e636           191 	jpr	eq,L58
008c 4d04  0000*    192 	test	_swap_flg
0090 e607           193 	jpr	eq,L62
0092 2107  0000*    194 	ld	r7,#_exp
0096 a176           195 	ld	r6,r7
0098 2105  0018     196 	ld	r5,#24
009c 5f00  0000*    197 	callr	_swab
                    198 L62:
00a0 5402  0002*    199 	ldl	rr2,_exp+2
00a4 5df2  0104     200 	ldl	~L1+260(fp),rr2
00a8 6700  0012*    201 	bit	_exp+18,#0
00ac ee06           202 	jpr	ne,L64
00ae 9426           203 	ldl	rr6,rr2
00b0 1804  0000     204 	multl	rq4,#2
00b4 0002  
00b6 5df6  0104     205 	ldl	~L1+260(fp),rr6
                    206 L64:
00ba bd20           207 	ldk	r2,#0
00bc 6103  000a*    208 	ld	r3,_exp+10
00c0 56f2  0104     209 	addl	rr2,~L1+260(fp)
00c4 5df2  0104     210 	ldl	~L1+260(fp),rr2
00c8 6107  0000*    211 	ld	r7,_fi
00cc 9424           212 	ldl	rr4,rr2
00ce bd31           213 	ldk	r3,#1
00d0 5f00  0000*    214 	callr	_fseek
00d4 6103  000c*    215 	ld	r3,_exp+12
00d8 bd20           216 	ldk	r2,#0
00da 1b02  000e     217 	div	rr2,#14
00de a13e           218 	ld	r14,r3
00e0 8d34           219 	test	r3
00e2 ee46           220 	jpr	ne,L67
00e4 2107  0010*    221 	ld	r7,#__iob+16
00e8 2106  0062'    222 	ld	r6,#L66
00ec 6105  0000*    223 	ld	r5,_av0
00f0 2104  0000*    224 	ld	r4,#_archdr
00f4 5f00  0000*    225 	callr	_fprintf
                    226 L58:
00f8 6107  0000*    227 	ld	r7,_fi
00fc df43           228 	callr	_nextel
00fe 8d24           229 	test	r2
0100 eeb6           230 	jpr	ne,L59
0102 deb8           231 	callr	_fnew
0104 6f02  0000*    232 	ld	_new,r2
0108 6107  0000*    233 	ld	r7,_fi
010c 5f00  0000*    234 	callr	_fclose
0110 2107  0030'    235 	ld	r7,#_tempnm
0114 2106  007a'    236 	ld	r6,#L74
0118 5f00  0000*    237 	callr	_fopen
011c 6f02  0000*    238 	ld	_fo,r2
0120 8d24           239 	test	r2
0122 ee09           240 	jpr	ne,L75
0124 2107  0010*    241 	ld	r7,#__iob+16
0128 2106  007c'    242 	ld	r6,#L76
012c 5f00  0000*    243 	callr	_fprintf
0130 bd71           244 	ldk	r7,#1
0132 5f00  0000*    245 	callr	_exit
                    246 L75:
0136 2107  0000*    247 	ld	r7,#_tab
013a 6106  0000*    248 	ld	r6,_tnum
013e bd5c           249 	ldk	r5,#12
0140 6104  0000*    250 	ld	r4,_fo
0144 5f00  0000*    251 	callr	_fwrite
0148 6107  0000*    252 	ld	r7,_fo
014c 5f00  0000*    253 	callr	_fclose
0150 4d04  0000*    254 	test	_new
0154 e629           255 	jpr	eq,L79
0156 76f7  0004     256 	lda	r7,~L1+4(fp)
015a 61f4  0000     257 	ld	r4,~L1(fp)
015e 2145           258 	ld	r5,@r4
0160 a154           259 	ld	r4,r5
0162 2105  0000*    260 	ld	r5,#_firstname
0166 2106  0094'    261 	ld	r6,#L81
016a 2103  0030'    262 	ld	r3,#_tempnm
016e e826           263 	jpr	L20008
                    264 L67:
0170 abe0           265 	dec	r14,#1
0172 0b0e  0000     266 	cp	r14,#0
0176 e1c0           267 	jpr	lt,L58
0178 76f7  0108     268 	lda	r7,~L1+264(fp)
017c 6106  0000*    269 	ld	r6,_fi
0180 5f00  0000*    270 	callr	_readsym
0184 6f02  0000*    271 	ld	_longnm,r2
0188 832e           272 	sub	r14,r2
018a 60fa  010c     273 	ldb	rl2,~L1+268(fp)
018e b120           274 	extsb	r2
0190 a725           275 	bit	r2,#5
0192 e6ee           276 	jpr	eq,L67
0194 60fa  010c     277 	ldb	rl2,~L1+268(fp)
0198 b120           278 	extsb	r2
019a 0702  001f     279 	and	r2,#31
019e e6e8           280 	jpr	eq,L67
01a0 76f7  0108     281 	lda	r7,~L1+264(fp)
01a4 df5d           282 	callr	_stash
01a6 e8e4           283 	jpr	L67
                    284 L79:
01a8 76f7  0004     285 	lda	r7,~L1+4(fp)
01ac 61f4  0000     286 	ld	r4,~L1(fp)
01b0 2145           287 	ld	r5,@r4
01b2 a154           288 	ld	r4,r5
01b4 2105  0030'    289 	ld	r5,#_tempnm
01b8 2106  00a6'    290 	ld	r6,#L83
                    291 L20008:
01bc 5f00  0000*    292 	callr	_sprintf
01c0 76f7  0004     293 	lda	r7,~L1+4(fp)
01c4 5f00  0000*    294 	callr	_system
01c8 8d24           295 	test	r2
01ca e609           296 	jpr	eq,L85
01cc 76f5  0004     297 	lda	r5,~L1+4(fp)
01d0 2106  00b4'    298 	ld	r6,#L86
01d4 2107  0010*    299 	ld	r7,#__iob+16
01d8 5f00  0000*    300 	callr	_fprintf
01dc e805           301 	jpr	L87
                    302 L85:
01de 61f7  0000     303 	ld	r7,~L1(fp)
01e2 2176           304 	ld	r6,@r7
01e4 a167           305 	ld	r7,r6
01e6 deb2           306 	callr	_fixdate
                    307 L87:
01e8 2107  0030'    308 	ld	r7,#_tempnm
01ec 5f00  0000*    309 	callr	_unlink
01f0 e835           310 	jpr	L44
                    311 L20010:
01f2 69f1  0000     312 	inc	~L1(fp),#2
01f6 61f7  0000     313 	ld	r7,~L1(fp)
01fa 2176           314 	ld	r6,@r7
01fc a167           315 	ld	r7,r6
01fe 2106  003a'    316 	ld	r6,#L46
0202 5f00  0000*    317 	callr	_fopen
0206 6f02  0000*    318 	ld	_fi,r2
020a 8d24           319 	test	r2
020c 5e06  0044'    320 	jpr	eq,L20003
0210 1402  0000     321 	ldl	rr2,#2
0214 0002  
0216 5d02  0000*    322 	ldl	_off,rr2
021a 2107  0000*    323 	ld	r7,#_exp
021e bd61           324 	ldk	r6,#1
0220 bd52           325 	ldk	r5,#2
0222 6104  0000*    326 	ld	r4,_fi
0226 5f00  0000*    327 	callr	_fread
022a 4d01  0000*    328 	cp	_exp,#65381
022e ff65  
0230 5e0e  0060'    329 	jpr	ne,L20007
0234 6107  0000*    330 	ld	r7,_fi
0238 9244           331 	subl	rr4,rr4
023a 8333           332 	sub	r3,r3
023c 5f00  0000*    333 	callr	_fseek
0240 4d08  0000*    334 	clr	_tnum
0244 4d08  0000*    335 	clr	_new
0248 6107  0000*    336 	ld	r7,_fi
024c dfeb           337 	callr	_nextel
024e 8d24           338 	test	r2
0250 5e0e  006e'    339 	jpr	ne,L59
0254 6107  0000*    340 	ld	r7,_fi
0258 5f00  0000*    341 	callr	_fclose
                    342 L44:
025c 61f2  0002     343 	ld	r2,~L1+2(fp)
0260 6bf0  0002     344 	dec	~L1+2(fp),#1
0264 8d24           345 	test	r2
0266 eec5           346 	jpr	ne,L20010
0268 8377           347 	sub	r7,r7
026a 5f00  0000*    348 	callr	_exit
026e 61fe  0196     349 	ld	r14,406(fp)
0272 010f  0198     350 	add	fp,#~L2
0276 9e08           351 	ret
                    352 	~L1 := 0
                    353 	~L2 := 408
                    354 	}	/* _main */
                    355 
                    356 .data
                    357 
                    358 /*~~nextel:*/
                    359 
                    360 .code
                    361 
                    362 _nextel::
                    363 {
0278 abf5           364 	dec	fp,#~L2
027a 6ffe  0004     365 	ld	4(fp),r14
027e 2ff7           366 	ld	@fp,r7
0280 5402  0000*    367 	ldl	rr2,_off
0284 5d02  0000*    368 	ldl	_oldoff,rr2
0288 a176           369 	ld	r6,r7
028a 5404  0000*    370 	ldl	rr4,_off
028e 8333           371 	sub	r3,r3
0290 5f00  0000*    372 	callr	_fseek
0294 2107  0000*    373 	ld	r7,#_archdr
0298 bd61           374 	ldk	r6,#1
029a 2105  001a     375 	ld	r5,#26
029e 61f4  0000     376 	ld	r4,~L1(fp)
02a2 5f00  0000*    377 	callr	_fread
02a6 a12e           378 	ld	r14,r2
02a8 0b02  0000     379 	cp	r2,#0
02ac ea02           380 	jpr	gt,L91
02ae 8322           381 	sub	r2,r2
02b0 e819           382 	jpr	L10006
                    383 L91:
02b2 5402  0016*    384 	ldl	rr2,_archdr+22
02b6 9424           385 	ldl	rr4,rr2
02b8 bd40           386 	ldk	r4,#0
02ba 0705  0001     387 	and	r5,#1
02be 9442           388 	ldl	rr2,rr4
02c0 9c48           389 	testl	rr4
02c2 e607           390 	jpr	eq,L92
02c4 5404  0016*    391 	ldl	rr4,_archdr+22
02c8 1604  0000     392 	addl	rr4,#1
02cc 0001  
02ce 5d04  0016*    393 	ldl	_archdr+22,rr4
                    394 L92:
02d2 61f7  0000     395 	ld	r7,~L1(fp)
02d6 5f00  0000*    396 	callr	_ftell
02da 5602  0016*    397 	addl	rr2,_archdr+22
02de 5d02  0000*    398 	ldl	_off,rr2
02e2 bd21           399 	ldk	r2,#1
                    400 L10006:
02e4 61fe  0004     401 	ld	r14,4(fp)
02e8 a9f5           402 	inc	fp,#~L2
02ea 9e08           403 	ret
                    404 	~L1 := 0
                    405 	~L2 := 6
                    406 	}	/* _nextel */
                    407 
                    408 .data
                    409 
                    410 L95:
00c6 73  79  6d     411 .byte	%73,%79,%6d,%62,%6f,%6c,%20,%74
00c9 62  6f  6c  
00cc 20  74  
00ce 61  62  6c     412 .byte	%61,%62,%6c,%65,%20,%6f,%76,%65
00d1 65  20  6f  
00d4 76  65  
00d6 72  66  6c     413 .byte	%72,%66,%6c,%6f,%77,%a,%0
00d9 6f  77  0a  
00dc 00  
                    414 .even
                    415 /*~~stash:*/
                    416 
                    417 .code
                    418 
                    419 _stash::
                    420 {
02ec abf5           421 	dec	fp,#~L2
02ee 2ff7           422 	ld	@fp,r7
02f0 4d01  0000*    423 	cp	_tnum,#700
02f4 02bc  
02f6 e109           424 	jpr	lt,L94
02f8 2107  0010*    425 	ld	r7,#__iob+16
02fc 2106  00c6'    426 	ld	r6,#L95
0300 5f00  0000*    427 	callr	_fprintf
0304 bd71           428 	ldk	r7,#1
0306 5f00  0000*    429 	callr	_exit
                    430 L94:
030a 4d04  0000*    431 	test	_longnm
030e ee04           432 	jpr	ne,L96
0310 4df5  0004     433 	ld	~L1+4(fp),#8
0314 0008  
0316 e809           434 	jpr	L97
                    435 L96:
0318 61f2  0000     436 	ld	r2,~L1(fp)
031c 602b  0006     437 	ldb	rl3,6(r2)
0320 b130           438 	extsb	r3
0322 0703  007f     439 	and	r3,#127
0326 6ff3  0004     440 	ld	~L1+4(fp),r3
                    441 L97:
032a 4df8  0002     442 	clr	~L1+2(fp)
032e e811           443 	jpr	L100
                    444 L20012:
0330 61f2  0002     445 	ld	r2,~L1+2(fp)
0334 61f3  0000     446 	ld	r3,~L1(fp)
0338 a935           447 	inc	r3,#6
033a 8123           448 	add	r3,r2
033c 6105  0000*    449 	ld	r5,_tnum
0340 1904  000c     450 	mult	rr4,#12
0344 0105  0004*    451 	add	r5,#_tab+4
0348 8125           452 	add	r5,r2
034a 203c           453 	ldb	rl4,@r3
034c 2e5c           454 	ldb	@r5,rl4
034e 69f0  0002     455 	inc	~L1+2(fp),#1
                    456 L100:
0352 61f2  0002     457 	ld	r2,~L1+2(fp)
0356 4bf2  0004     458 	cp	r2,~L1+4(fp)
035a e1ea           459 	jpr	lt,L20012
035c 6103  0000*    460 	ld	r3,_tnum
0360 1902  000c     461 	mult	rr2,#12
0364 5404  0000*    462 	ldl	rr4,_oldoff
0368 5d34  0000*    463 	ldl	_tab(r3),rr4
036c 6900  0000*    464 	inc	_tnum,#1
0370 4d04  0000*    465 	test	_longnm
0374 e60d           466 	jpr	eq,L10009
0376 6bf7  0004     467 	dec	~L1+4(fp),#8
037a 61f2  0004     468 	ld	r2,~L1+4(fp)
037e a92a           469 	inc	r2,#11
0380 a127           470 	ld	r7,r2
0382 b16a           471 	exts	rr6
0384 1b06  000c     472 	div	rr6,#12
0388 4107  0000*    473 	add	r7,_tnum
038c 6f07  0000*    474 	ld	_tnum,r7
                    475 L10009:
0390 a9f5           476 	inc	fp,#~L2
0392 9e08           477 	ret
                    478 	~L1 := 0
                    479 	~L2 := 6
                    480 	}	/* _stash */
                    481 
                    482 .data
                    483 
                    484 /*~~fnew:*/
                    485 
                    486 .code
                    487 
                    488 _fnew::
                    489 {
0394 abf5           490 	dec	fp,#~L2
0396 6103  0000*    491 	ld	r3,_tnum
039a 1902  000c     492 	mult	rr2,#12
039e 0103  001a     493 	add	r3,#26
03a2 a135           494 	ld	r5,r3
03a4 b14a           495 	exts	rr4
03a6 5d04  0000*    496 	ldl	_offdelta,rr4
03aa 1402  0000     497 	ldl	rr2,#2
03ae 0002  
03b0 5d02  0000*    498 	ldl	_off,rr2
03b4 6107  0000*    499 	ld	r7,_fi
03b8 d0a1           500 	callr	_nextel
03ba 2107  0000*    501 	ld	r7,#_archdr
03be 2106  0030'    502 	ld	r6,#_tempnm
03c2 bd5e           503 	ldk	r5,#14
03c4 5f00  0000*    504 	callr	_strncmp
03c8 8d24           505 	test	r2
03ca ee0d           506 	jpr	ne,L104
03cc 4d08  0000*    507 	clr	_new
03d0 5402  0016*    508 	ldl	rr2,_archdr+22
03d4 1602  0000     509 	addl	rr2,#26
03d8 001a  
03da 5404  0000*    510 	ldl	rr4,_offdelta
03de 9224           511 	subl	rr4,rr2
03e0 5d04  0000*    512 	ldl	_offdelta,rr4
03e4 e80a           513 	jpr	L105
                    514 L104:
03e6 4d05  0000*    515 	ld	_new,#1
03ea 0001  
03ec 2107  0000*    516 	ld	r7,#_firstname
03f0 2106  0000*    517 	ld	r6,#_archdr
03f4 bd5e           518 	ldk	r5,#14
03f6 5f00  0000*    519 	callr	_strncpy
                    520 L105:
03fa 4df8  0000     521 	clr	~L1(fp)
03fe e839           522 	jpr	L109
                    523 L20014:
0400 61f5  0000     524 	ld	r5,~L1(fp)
0404 1904  000c     525 	mult	rr4,#12
0408 5456  0000*    526 	ldl	rr6,_tab(r5)
040c 5606  0000*    527 	addl	rr6,_offdelta
0410 5d56  0000*    528 	ldl	_tab(r5),rr6
0414 4d04  0000*    529 	test	_swap_flg
0418 e609           530 	jpr	eq,L110
041a 61f5  0000     531 	ld	r5,~L1(fp)
041e 1904  000c     532 	mult	rr4,#12
0422 0105  0000*    533 	add	r5,#_tab
0426 a157           534 	ld	r7,r5
0428 5f00  0000*    535 	callr	_longswap
                    536 L110:
042c 61f3  0000     537 	ld	r3,~L1(fp)
0430 1902  000c     538 	mult	rr2,#12
0434 603a  0004*    539 	ldb	rl2,_tab+4(r3)
0438 b120           540 	extsb	r2
043a a727           541 	bit	r2,#7
043c e618           542 	jpr	eq,L107
043e 61f3  0000     543 	ld	r3,~L1(fp)
0442 1902  000c     544 	mult	rr2,#12
0446 603a  0004*    545 	ldb	rl2,_tab+4(r3)
044a b120           546 	extsb	r2
044c 0702  007f     547 	and	r2,#127
0450 6ff2  0002     548 	ld	~L1+2(fp),r2
0454 6bf7  0002     549 	dec	~L1+2(fp),#8
0458 61f2  0002     550 	ld	r2,~L1+2(fp)
045c a92a           551 	inc	r2,#11
045e a125           552 	ld	r5,r2
0460 b14a           553 	exts	rr4
0462 1b04  000c     554 	div	rr4,#12
0466 41f5  0000     555 	add	r5,~L1(fp)
046a 6ff5  0000     556 	ld	~L1(fp),r5
                    557 L107:
046e 69f0  0000     558 	inc	~L1(fp),#1
                    559 L109:
0472 61f2  0000     560 	ld	r2,~L1(fp)
0476 4b02  0000*    561 	cp	r2,_tnum
047a e1c2           562 	jpr	lt,L20014
047c 6102  0000*    563 	ld	r2,_new
0480 a9f5           564 	inc	fp,#~L2
0482 9e08           565 	ret
                    566 	~L1 := 0
                    567 	~L2 := 6
                    568 	}	/* _fnew */
                    569 
                    570 .data
                    571 
                    572 L115:
00de 63  61  6e     573 .byte	%63,%61,%6e,%27,%74,%20,%72,%65
00e1 27  74  20  
00e4 72  65  
00e6 6f  70  65     574 .byte	%6f,%70,%65,%6e,%20,%25,%73,%a
00e9 6e  20  25  
00ec 73  0a  
00ee 00             575 .byte	%0
                    576 .even
                    577 /*~~fixdate:*/
                    578 
                    579 .code
                    580 
                    581 _fixdate::
                    582 {
0484 abf7           583 	dec	fp,#~L2
0486 6ffe  0006     584 	ld	6(fp),r14
048a 2ff7           585 	ld	@fp,r7
048c bd61           586 	ldk	r6,#1
048e 5f00  0000*    587 	callr	_open
0492 a12e           588 	ld	r14,r2
0494 0b02  0000     589 	cp	r2,#0
0498 e909           590 	jpr	ge,L114
049a 2107  0010*    591 	ld	r7,#__iob+16
049e 2106  00de'    592 	ld	r6,#L115
04a2 61f5  0000     593 	ld	r5,~L1(fp)
04a6 5f00  0000*    594 	callr	_fprintf
04aa e81f           595 	jpr	L10015
                    596 L114:
04ac 8377           597 	sub	r7,r7
04ae 5f00  0000*    598 	callr	_time
04b2 a924           599 	inc	r2,#5
04b4 a125           600 	ld	r5,r2
04b6 b14a           601 	exts	rr4
04b8 5df4  0002     602 	ldl	~L1+2(fp),rr4
04bc 2104  000e*    603 	ld	r4,#_archdr+14
04c0 0304  0000*    604 	sub	r4,#_archdr
04c4 a143           605 	ld	r3,r4
04c6 b12a           606 	exts	rr2
04c8 1602  0000     607 	addl	rr2,#2
04cc 0002  
04ce 9424           608 	ldl	rr4,rr2
04d0 a1e7           609 	ld	r7,r14
04d2 8333           610 	sub	r3,r3
04d4 5f00  0000*    611 	callr	_lseek
04d8 76f6  0002     612 	lda	r6,~L1+2(fp)
04dc a1e7           613 	ld	r7,r14
04de bd54           614 	ldk	r5,#4
04e0 5f00  0000*    615 	callr	_write
04e4 a1e7           616 	ld	r7,r14
04e6 5f00  0000*    617 	callr	_close
                    618 L10015:
04ea 61fe  0006     619 	ld	r14,6(fp)
04ee a9f7           620 	inc	fp,#~L2
04f0 9e08           621 	ret
                    622 	~L1 := 0
                    623 	~L2 := 8
                    624 	}	/* _fixdate */
                    625 
                    626 .data
                    627 
