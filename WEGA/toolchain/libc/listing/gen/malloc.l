LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 _sccsid:
0000 40               5 .byte	%40
0001 5b               6 .byte	%5b
0002 24               7 .byte	%24
0003 5d               8 .byte	%5d
0004 6d               9 .byte	%6d
0005 61              10 .byte	%61
0006 6c              11 .byte	%6c
0007 6c              12 .byte	%6c
0008 6f              13 .byte	%6f
0009 63              14 .byte	%63
000a 2e              15 .byte	%2e
000b 63              16 .byte	%63
000c 09              17 .byte	%9
000d 33              18 .byte	%33
000e 2e              19 .byte	%2e
000f 33              20 .byte	%33
0010 20              21 .byte	%20
0011 20              22 .byte	%20
0012 30              23 .byte	%30
0013 39              24 .byte	%39
0014 2f              25 .byte	%2f
0015 31              26 .byte	%31
0016 32              27 .byte	%32
0017 2f              28 .byte	%2f
0018 38              29 .byte	%38
0019 33              30 .byte	%33
001a 20              31 .byte	%20
001b 31              32 .byte	%31
001c 30              33 .byte	%30
001d 3a              34 .byte	%3a
001e 33              35 .byte	%33
001f 32              36 .byte	%32
0020 3a              37 .byte	%3a
0021 33              38 .byte	%33
0022 30              39 .byte	%30
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
                     53 .even
                     54 _maxbytes:
0030 fff8            55 .word	%fff8
                     56 .even
                     57 _maxblocks:
0032 0040            58 .word	%40
                     59 .even
                     60 _allocs:
                     61 .=.+4
                     62 .even
                     63 _allocp:
0038 0000            64 .word	0
                     65 .even
                     66 _alloct:
003a 0000            67 .word	0
                     68 .even
                     69 _grown:
003c 0000            70 .word	0
                     71 /*~~malloc:*/
                     72 
                     73 .code
                     74 
                     75 _malloc::
                     76 {
0000 030f  0012      77 	sub	fp,#~L2
0004 5cf9  0806      78 	ldm	4(fp),r8,#7
0008 0004  
000a a17e            79 	ld	r14,r7
000c bd80            80 	ldk	r8,#0
000e 4b07  0030'     81 	cp	r7,_maxbytes
0012 e303            82 	jpr	ule,L20
                     83 L20004:
0014 8322            84 	sub	r2,r2
0016 5e08  0184'     85 	jpr	L10000
                     86 L20:
001a 4d04  0038'     87 	test	_allocp
001e e603            88 	jpr	eq,L10003
0020 4d04  003c'     89 	test	_grown
0024 ee01            90 	jpr	ne,L21
                     91 L10003:
0026 bd81            92 	ldk	r8,#1
                     93 L21:
0028 4d04  0038'     94 	test	_allocp
002c ee14            95 	jpr	ne,L22
002e 4d05  0038'     96 	ld	_allocp,#_allocs
0032 0034' 
0034 4d05  003a'     97 	ld	_alloct,#_allocs+2
0038 0036' 
003a 2102  0036'     98 	ld	r2,#_allocs+2
003e 0502  0001      99 	or	r2,#1
0042 6103  0038'    100 	ld	r3,_allocp
0046 2f32           101 	ld	@r3,r2
0048 2102  0034'    102 	ld	r2,#_allocs
004c 0502  0001     103 	or	r2,#1
0050 6104  003a'    104 	ld	r4,_alloct
0054 2f42           105 	ld	@r4,r2
                    106 L22:
0056 76e2  0003     107 	lda	r2,3(r14)
005a a320           108 	res	r2,#0
005c b324           109 	rr	r2,#1
005e a12a           110 	ld	r10,r2
0060 610d  0038'    111 	ld	r13,_allocp
0064 21d2           112 	ld	r2,@r13
0066 0702  fffe     113 	and	r2,#-2
006a a12c           114 	ld	r12,r2
006c 610b  003a'    115 	ld	r11,_alloct
                    116 L25:
0070 27d0           117 	bit	@r13,#0
0072 e611           118 	jpr	eq,L27
0074 e820           119 	jpr	L26
                    120 L20001:
0076 21c2           121 	ld	r2,@r12
0078 0702  fffe     122 	and	r2,#-2
007c a12c           123 	ld	r12,r2
007e 2fd2           124 	ld	@r13,r2
0080 6f02  0038'    125 	ld	_allocp,r2
0084 e808           126 	jpr	L27
                    127 L20003:
0086 4b0c  0038'    128 	cp	r12,_allocp
008a e6f5           129 	jpr	eq,L20001
008c 21c2           130 	ld	r2,@r12
008e 0702  fffe     131 	and	r2,#-2
0092 a12c           132 	ld	r12,r2
0094 2fd2           133 	ld	@r13,r2
                    134 L27:
0096 27c0           135 	bit	@r12,#0
0098 e6f6           136 	jpr	eq,L20003
009a a1a2           137 	ld	r2,r10
009c 81a2           138 	add	r2,r10
009e 81d2           139 	add	r2,r13
00a0 8b2c           140 	cp	r12,r2
00a2 e705           141 	jpr	ult,L31
00a4 a1a2           142 	ld	r2,r10
00a6 81a2           143 	add	r2,r10
00a8 81d2           144 	add	r2,r13
00aa 8b2d           145 	cp	r13,r2
00ac e35b           146 	jpr	ule,L32
                    147 L31:
00ae 4b0c  003a'    148 	cp	r12,_alloct
00b2 ee01           149 	jpr	ne,L26
00b4 a1db           150 	ld	r11,r13
                    151 L26:
00b6 a1cd           152 	ld	r13,r12
00b8 21d2           153 	ld	r2,@r13
00ba 0702  fffe     154 	and	r2,#-2
00be a12c           155 	ld	r12,r2
00c0 4b0d  0038'    156 	cp	r13,_allocp
00c4 eed5           157 	jpr	ne,L25
00c6 6102  003a'    158 	ld	r2,_alloct
00ca 83b2           159 	sub	r2,r11
00cc b329  ffff     160 	sra	r2,#1
00d0 a1a6           161 	ld	r6,r10
00d2 8326           162 	sub	r6,r2
00d4 8186           163 	add	r6,r8
00d6 bd20           164 	ldk	r2,#0
00d8 a163           165 	ld	r3,r6
00da b32d  0001     166 	slal	rr2,#1
00de 1602  0000     167 	addl	rr2,#1023
00e2 03ff  
00e4 9426           168 	ldl	rr6,rr2
00e6 b147           169 	extsl	rq4
00e8 1a04  0000     170 	divl	rq4,#1024
00ec 0400  
00ee a179           171 	ld	r9,r7
00f0 4b07  0032'    172 	cp	r7,_maxblocks
00f4 eb8f           173 	jpr	ugt,L20004
00f6 a172           174 	ld	r2,r7
00f8 b329  000a     175 	sla	r2,#10
00fc a12e           176 	ld	r14,r2
00fe 8377           177 	sub	r7,r7
0100 5f00  0000*    178 	callr	_sbrk
0104 a12d           179 	ld	r13,r2
0106 81e2           180 	add	r2,r14
0108 8b2d           181 	cp	r13,r2
010a eb84           182 	jpr	ugt,L20004
010c a1e7           183 	ld	r7,r14
010e 5f00  0000*    184 	callr	_sbrk
0112 a12d           185 	ld	r13,r2
0114 0b02  ffff     186 	cp	r2,#-1
0118 5e06  0014'    187 	jpr	eq,L20004
011c 8d84           188 	test	r8
011e e610           189 	jpr	eq,L37
0120 0502  0001     190 	or	r2,#1
0124 6103  003a'    191 	ld	r3,_alloct
0128 2f32           192 	ld	@r3,r2
012a a192           193 	ld	r2,r9
012c b329  0009     194 	sla	r2,#9
0130 8122           195 	add	r2,r2
0132 81d2           196 	add	r2,r13
0134 ab21           197 	dec	r2,#2
0136 a12c           198 	ld	r12,r2
0138 4d05  003c'    199 	ld	_grown,#1
013c 0001  
013e e809           200 	jpr	L38
                    201 L37:
0140 a1bd           202 	ld	r13,r11
0142 a192           203 	ld	r2,r9
0144 b329  0009     204 	sla	r2,#9
0148 8122           205 	add	r2,r2
014a 4102  003a'    206 	add	r2,_alloct
014e ab21           207 	dec	r2,#2
0150 a12c           208 	ld	r12,r2
                    209 L38:
0152 2102  0034'    210 	ld	r2,#_allocs
0156 0502  0001     211 	or	r2,#1
015a 2fc2           212 	ld	@r12,r2
015c 21c2           213 	ld	r2,@r12
015e 2fc2           214 	ld	@r12,r2
0160 6f0c  003a'    215 	ld	_alloct,r12
                    216 L32:
0164 a1a2           217 	ld	r2,r10
0166 81a2           218 	add	r2,r10
0168 81d2           219 	add	r2,r13
016a 6f02  0038'    220 	ld	_allocp,r2
016e 4b0c  0038'    221 	cp	r12,_allocp
0172 e301           222 	jpr	ule,L39
0174 2f2c           223 	ld	@r2,r12
                    224 L39:
0176 6102  0038'    225 	ld	r2,_allocp
017a 0502  0001     226 	or	r2,#1
017e 2fd2           227 	ld	@r13,r2
0180 76d2  0002     228 	lda	r2,2(r13)
                    229 L10000:
0184 5cf1  0806     230 	ldm	r8,4(fp),#7
0188 0004  
018a 010f  0012     231 	add	fp,#~L2
018e 9e08           232 	ret
                    233 	~L1 := 0
                    234 	~L2 := 18
                    235 	}	/* _malloc */
                    236 
                    237 .data
                    238 
                    239 
                    240 .code
                    241 
                    242 _findfree:
                    243 {
0190 abfb           244 	dec	fp,#~L2
0192 a171           245 	ld	r1,r7
0194 7612  0003     246 	lda	r2,3(r1)
0198 a320           247 	res	r2,#0
019a b324           248 	rr	r2,#1
019c a123           249 	ld	r3,r2
019e 6104  0038'    250 	ld	r4,_allocp
01a2 2142           251 	ld	r2,@r4
01a4 0702  fffe     252 	and	r2,#-2
01a8 a125           253 	ld	r5,r2
                    254 L44:
01aa 2740           255 	bit	@r4,#0
01ac e611           256 	jpr	eq,L46
01ae e81e           257 	jpr	L45
                    258 L20006:
01b0 2152           259 	ld	r2,@r5
01b2 0702  fffe     260 	and	r2,#-2
01b6 a125           261 	ld	r5,r2
01b8 2f42           262 	ld	@r4,r2
01ba 6f02  0038'    263 	ld	_allocp,r2
01be e808           264 	jpr	L46
                    265 L20008:
01c0 4b05  0038'    266 	cp	r5,_allocp
01c4 e6f5           267 	jpr	eq,L20006
01c6 2152           268 	ld	r2,@r5
01c8 0702  fffe     269 	and	r2,#-2
01cc a125           270 	ld	r5,r2
01ce 2f42           271 	ld	@r4,r2
                    272 L46:
01d0 2750           273 	bit	@r5,#0
01d2 e6f6           274 	jpr	eq,L20008
01d4 a132           275 	ld	r2,r3
01d6 8132           276 	add	r2,r3
01d8 8142           277 	add	r2,r4
01da 8b25           278 	cp	r5,r2
01dc e707           279 	jpr	ult,L45
01de a132           280 	ld	r2,r3
01e0 8132           281 	add	r2,r3
01e2 8142           282 	add	r2,r4
01e4 8b24           283 	cp	r4,r2
01e6 eb02           284 	jpr	ugt,L45
01e8 a142           285 	ld	r2,r4
01ea e809           286 	jpr	L10004
                    287 L45:
01ec a154           288 	ld	r4,r5
01ee 2142           289 	ld	r2,@r4
01f0 0702  fffe     290 	and	r2,#-2
01f4 a125           291 	ld	r5,r2
01f6 4b04  0038'    292 	cp	r4,_allocp
01fa eed7           293 	jpr	ne,L44
01fc 8322           294 	sub	r2,r2
                    295 L10004:
01fe a9fb           296 	inc	fp,#~L2
0200 9e08           297 	ret
                    298 	~L1 := 0
                    299 	~L2 := 12
                    300 	}	/* _findfree */
                    301 
                    302 .data
                    303 
                    304 /*~~free:*/
                    305 
                    306 .code
                    307 
                    308 _free::
                    309 {
0202 abf3           310 	dec	fp,#~L2
0204 6ffe  0002     311 	ld	2(fp),r14
0208 2ff7           312 	ld	@fp,r7
020a 61fe  0000     313 	ld	r14,~L1(fp)
020e a1e6           314 	ld	r6,r14
0210 ab61           315 	dec	r6,#2
0212 a16e           316 	ld	r14,r6
0214 6f06  0038'    317 	ld	_allocp,r6
0218 21e6           318 	ld	r6,@r14
021a 0706  fffe     319 	and	r6,#-2
021e 2fe6           320 	ld	@r14,r6
0220 61fe  0002     321 	ld	r14,2(fp)
0224 a9f3           322 	inc	fp,#~L2
0226 9e08           323 	ret
                    324 	~L1 := 0
                    325 	~L2 := 4
                    326 	}	/* _free */
                    327 
                    328 .data
                    329 
                    330 /*~~realloc:*/
                    331 
                    332 .code
                    333 
                    334 _realloc::
                    335 {
0228 030f  0016     336 	sub	fp,#~L2
022c 5cf9  0806     337 	ldm	8(fp),r8,#7
0230 0008  
0232 2ff6           338 	ld	@fp,r6
0234 a17e           339 	ld	r14,r7
0236 ab71           340 	dec	r7,#2
0238 a17a           341 	ld	r10,r7
023a 21a7           342 	ld	r7,@r10
023c 0707  fffe     343 	and	r7,#-2
0240 83e7           344 	sub	r7,r14
0242 b379  ffff     345 	sra	r7,#1
0246 a178           346 	ld	r8,r7
0248 a960           347 	inc	r6,#1
024a a360           348 	res	r6,#0
024c b364           349 	rr	r6,#1
024e a16b           350 	ld	r11,r6
0250 27a0           351 	bit	@r10,#0
0252 e602           352 	jpr	eq,L55
0254 a1e7           353 	ld	r7,r14
0256 d02b           354 	callr	_free
                    355 L55:
0258 61f7  0000     356 	ld	r7,~L1(fp)
025c d067           357 	callr	_findfree
025e a129           358 	ld	r9,r2
0260 a1b2           359 	ld	r2,r11
0262 81b2           360 	add	r2,r11
0264 8192           361 	add	r2,r9
0266 8b2a           362 	cp	r10,r2
0268 ef16           363 	jpr	uge,L56
026a a1b2           364 	ld	r2,r11
026c 81b2           365 	add	r2,r11
026e 8192           366 	add	r2,r9
0270 a183           367 	ld	r3,r8
0272 8183           368 	add	r3,r8
0274 81a3           369 	add	r3,r10
0276 8b32           370 	cp	r2,r3
0278 ef0e           371 	jpr	uge,L56
027a 2192           372 	ld	r2,@r9
027c 0502  0001     373 	or	r2,#1
0280 2f92           374 	ld	@r9,r2
0282 61f7  0000     375 	ld	r7,~L1(fp)
0286 d144           376 	callr	_malloc
0288 6ff2  0004     377 	ld	~L1+4(fp),r2
028c 2192           378 	ld	r2,@r9
028e 0702  fffe     379 	and	r2,#-2
0292 2f92           380 	ld	@r9,r2
0294 e805           381 	jpr	L57
                    382 L56:
0296 61f7  0000     383 	ld	r7,~L1(fp)
029a d14e           384 	callr	_malloc
029c 6ff2  0004     385 	ld	~L1+4(fp),r2
                    386 L57:
02a0 4df4  0004     387 	test	~L1+4(fp)
02a4 e615           388 	jpr	eq,L20011
02a6 4bfe  0004     389 	cp	r14,~L1+4(fp)
02aa e612           390 	jpr	eq,L20011
02ac a1ed           391 	ld	r13,r14
02ae 61fc  0004     392 	ld	r12,~L1+4(fp)
02b2 8b8b           393 	cp	r11,r8
02b4 e302           394 	jpr	ule,L10014
02b6 a182           395 	ld	r2,r8
02b8 e801           396 	jpr	L10015
                    397 L10014:
02ba a1b2           398 	ld	r2,r11
                    399 L10015:
02bc a12b           400 	ld	r11,r2
02be e804           401 	jpr	L59
                    402 L20010:
02c0 21d2           403 	ld	r2,@r13
02c2 2fc2           404 	ld	@r12,r2
02c4 a9d1           405 	inc	r13,#2
02c6 a9c1           406 	inc	r12,#2
                    407 L59:
02c8 a1b2           408 	ld	r2,r11
02ca abb0           409 	dec	r11,#1
02cc 8d24           410 	test	r2
02ce eef8           411 	jpr	ne,L20010
                    412 L20011:
02d0 61f2  0004     413 	ld	r2,~L1+4(fp)
02d4 5cf1  0806     414 	ldm	r8,8(fp),#7
02d8 0008  
02da 010f  0016     415 	add	fp,#~L2
02de 9e08           416 	ret
                    417 	~L1 := 0
                    418 	~L2 := 22
                    419 	}	/* _realloc */
                    420 
                    421 .data
                    422 
                    423 /*~~calloc:*/
                    424 
                    425 .code
                    426 
                    427 _calloc::
                    428 {
02e0 abfb           429 	dec	fp,#~L2
02e2 5cf9  0c02     430 	ldm	6(fp),r12,#3
02e6 0006  
02e8 1df6           431 	ldl	@fp,rr6
02ea 61f3  0002     432 	ld	r3,~L1+2(fp)
02ee 59f2  0000     433 	mult	rr2,~L1(fp)
02f2 6ff3  0002     434 	ld	~L1+2(fp),r3
02f6 a137           435 	ld	r7,r3
02f8 d17d           436 	callr	_malloc
02fa a12c           437 	ld	r12,r2
02fc 8d24           438 	test	r2
02fe ee02           439 	jpr	ne,L63
0300 8322           440 	sub	r2,r2
0302 e80f           441 	jpr	L10016
                    442 L63:
0304 a1ce           443 	ld	r14,r12
0306 61f2  0002     444 	ld	r2,~L1+2(fp)
030a a920           445 	inc	r2,#1
030c a320           446 	res	r2,#0
030e b324           447 	rr	r2,#1
0310 a12d           448 	ld	r13,r2
0312 e802           449 	jpr	L64
                    450 L20013:
0314 0de8           451 	clr	@r14
0316 a9e1           452 	inc	r14,#2
                    453 L64:
0318 abd0           454 	dec	r13,#1
031a 0b0d  0000     455 	cp	r13,#0
031e e9fa           456 	jpr	ge,L20013
0320 a1c2           457 	ld	r2,r12
                    458 L10016:
0322 5cf1  0c02     459 	ldm	r12,6(fp),#3
0326 0006  
0328 a9fb           460 	inc	fp,#~L2
032a 9e08           461 	ret
                    462 	~L1 := 0
                    463 	~L2 := 12
                    464 	}	/* _calloc */
                    465 
                    466 .data
                    467 
                    468 /*~~cfree:*/
                    469 
                    470 .code
                    471 
                    472 _cfree::
                    473 {
032c abf5           474 	dec	fp,#~L2
032e 1cf9  0502     475 	ldm	@fp,r5,#3
0332 a174           476 	ld	r4,r7
0334 d09a           477 	callr	_free
0336 a9f5           478 	inc	fp,#~L2
0338 9e08           479 	ret
                    480 	~L1 := 0
                    481 	~L2 := 6
                    482 	}	/* _cfree */
                    483 
                    484 .data
                    485 
