LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 _sccsid:
0000 40               5 .byte	%40
0001 5b               6 .byte	%5b
0002 24               7 .byte	%24
0003 5d               8 .byte	%5d
0004 61               9 .byte	%61
0005 72              10 .byte	%72
0006 2e              11 .byte	%2e
0007 63              12 .byte	%63
0008 09              13 .byte	%9
0009 32              14 .byte	%32
000a 2e              15 .byte	%2e
000b 32              16 .byte	%32
000c 20              17 .byte	%20
000d 20              18 .byte	%20
000e 31              19 .byte	%31
000f 31              20 .byte	%31
0010 2f              21 .byte	%2f
0011 31              22 .byte	%31
0012 38              23 .byte	%38
0013 2f              24 .byte	%2f
0014 38              25 .byte	%38
0015 33              26 .byte	%33
0016 20              27 .byte	%20
0017 31              28 .byte	%31
0018 32              29 .byte	%32
0019 3a              30 .byte	%3a
001a 34              31 .byte	%34
001b 38              32 .byte	%38
001c 3a              33 .byte	%3a
001d 30              34 .byte	%30
001e 30              35 .byte	%30
001f 20              36 .byte	%20
0020 2d              37 .byte	%2d
0021 20              38 .byte	%20
0022 38              39 .byte	%38
0023 37              40 .byte	%37
0024 77              41 .byte	%77
0025 65              42 .byte	%65
0026 67              43 .byte	%67
0027 61              44 .byte	%61
0028 33              45 .byte	%33
0029 2e              46 .byte	%2e
002a 32              47 .byte	%32
002b 00              48 .byte	%0
                     49 .comm	30,	_stbuf;
                     50 .comm	26,	_arbuf;
                     51 .even
                     52 _man::
002c 002e'           53 .addr	L23
                     54 L23:
002e 6d  72  78      55 .byte	%6d,%72,%78,%74,%64,%70,%71,%0
0031 74  64  70  
0034 71  00  
                     56 .even
                     57 .even
                     58 _opt::
0036 0038'           59 .addr	L25
                     60 L25:
0038 63  75  76      61 .byte	%63,%75,%76,%6e,%62,%61,%69,%6c
003b 6e  62  61  
003e 69  6c  
0040 00              62 .byte	%0
                     63 .even
                     64 .even
                     65 _signum::
0042 0001            66 .word	%1
0044 0002            67 .word	%2
0046 0003            68 .word	%3
0048 0000            69 .word	%0
                     70 .even
                     71 _tmp0::
004a 004c'           72 .addr	L28
                     73 L28:
004c 2f  74  6d      74 .byte	%2f,%74,%6d,%70,%2f,%76,%58,%58
004f 70  2f  76  
0052 58  58  
0054 58  58  58      75 .byte	%58,%58,%58,%0
0057 00  
                     76 .even
                     77 .even
                     78 _tmp1::
0058 005a'           79 .addr	L30
                     80 L30:
005a 2f  74  6d      81 .byte	%2f,%74,%6d,%70,%2f,%76,%31,%58
005d 70  2f  76  
0060 31  58  
0062 58  58  58      82 .byte	%58,%58,%58,%58,%0
0065 58  00  
                     83 .even
                     84 .even
                     85 _tmp2::
0068 006a'           86 .addr	L32
                     87 L32:
006a 2f  74  6d      88 .byte	%2f,%74,%6d,%70,%2f,%76,%32,%58
006d 70  2f  76  
0070 32  58  
0072 58  58  58      89 .byte	%58,%58,%58,%58,%0
0075 58  00  
                     90 .even
                     91 .comm	2,	_comfun;
                     92 .comm	26,	_flg;
                     93 .comm	2,	_namv;
                     94 .comm	2,	_namc;
                     95 .comm	2,	_arnam;
                     96 .comm	2,	_ponam;
                     97 .comm	2,	_tfnam;
                     98 .comm	2,	_tf1nam;
                     99 .comm	2,	_tf2nam;
                    100 .comm	2,	_file;
                    101 .comm	16,	_name;
                    102 .comm	2,	_af;
                    103 .comm	2,	_tf;
                    104 .comm	2,	_tf1;
                    105 .comm	2,	_tf2;
                    106 .comm	2,	_qf;
                    107 .comm	2,	_bastate;
                    108 .comm	512,	_buf;
                    109 .comm	2,	_av0;
                    110 L94:
0078 25  73  3a     111 .byte	%25,%73,%3a,%20,%62,%61,%64,%20
007b 20  62  61  
007e 64  20  
0080 6f  70  74     112 .byte	%6f,%70,%74,%69,%6f,%6e,%20,%60
0083 69  6f  6e  
0086 20  60  
0088 25  63  27     113 .byte	%25,%63,%27,%a,%0
008b 0a  00  
                    114 .even
                    115 .even
                    116 L98:
008e 76  58  58     117 .byte	%76,%58,%58,%58,%58,%58,%0
0091 58  58  58  
0094 00  
                    118 .even
                    119 L99:
0096 76  31  58     120 .byte	%76,%31,%58,%58,%58,%58,%58,%0
0099 58  58  58  
009c 58  00  
                    121 .even
                    122 L100:
009e 76  32  58     123 .byte	%76,%32,%58,%58,%58,%58,%58,%0
00a1 58  58  58  
00a4 58  00  
                    124 .even
                    125 L106:
00a6 25  73  3a     126 .byte	%25,%73,%3a,%20,%6f,%6e,%65,%20
00a9 20  6f  6e  
00ac 65  20  
00ae 6f  66  20     127 .byte	%6f,%66,%20,%5b,%25,%73,%5d,%20
00b1 5b  25  73  
00b4 5d  20  
00b6 6d  75  73     128 .byte	%6d,%75,%73,%74,%20,%62,%65,%20
00b9 74  20  62  
00bc 65  20  
00be 73  70  65     129 .byte	%73,%70,%65,%63,%69,%66,%69,%65
00c1 63  69  66  
00c4 69  65  
00c6 64  0a  00     130 .byte	%64,%a,%0
                    131 .even
                    132 /*~~main:*/
                    133 
                    134 .code
                    135 
                    136 _main::
                    137 {
                    138 	.data
                    139 	.even
                    140 L96:
00ca 007a'          141 	.word	L83
00cc 007a'          142 	.word	L83
00ce 007a'          143 	.word	L83
00d0 009e'          144 	.word	L86
00d2 00c2'          145 	.word	L92
00d4 00c2'          146 	.word	L92
00d6 00c2'          147 	.word	L92
00d8 00c2'          148 	.word	L92
00da 007a'          149 	.word	L83
00dc 00c2'          150 	.word	L92
00de 00c2'          151 	.word	L92
00e0 007a'          152 	.word	L83
00e2 00b6'          153 	.word	L90
00e4 007a'          154 	.word	L83
00e6 00c2'          155 	.word	L92
00e8 00b0'          156 	.word	L89
00ea 00bc'          157 	.word	L91
00ec 0096'          158 	.word	L84
00ee 00c2'          159 	.word	L92
00f0 00aa'          160 	.word	L88
00f2 007a'          161 	.word	L83
00f4 007a'          162 	.word	L83
00f6 00c2'          163 	.word	L92
00f8 00a4'          164 	.word	L87
                    165 	.code
0000 abf9           166 	dec	fp,#~L2
0002 5cf9  0d01     167 	ldm	6(fp),r13,#2
0006 0006  
0008 1df6           168 	ldl	@fp,rr6
000a bde0           169 	ldk	r14,#0
000c e813           170 	jpr	L67
                    171 L20001:
000e a1e2           172 	ld	r2,r14
0010 81e2           173 	add	r2,r14
0012 6127  0042'    174 	ld	r7,_signum(r2)
0016 bd61           175 	ldk	r6,#1
0018 5f00  0000*    176 	callr	_signal
001c 0b02  0001     177 	cp	r2,#1
0020 e608           178 	jpr	eq,L65
0022 a1e2           179 	ld	r2,r14
0024 81e2           180 	add	r2,r14
0026 6127  0042'    181 	ld	r7,_signum(r2)
002a 2106  06bc'    182 	ld	r6,#_sigdone
002e 5f00  0000*    183 	callr	_signal
                    184 L65:
0032 a9e0           185 	inc	r14,#1
                    186 L67:
0034 a1e2           187 	ld	r2,r14
0036 81e2           188 	add	r2,r14
0038 4d24  0042'    189 	test	_signum(r2)
003c eee8           190 	jpr	ne,L20001
003e 61f7  0000     191 	ld	r7,~L1(fp)
0042 2176           192 	ld	r6,@r7
0044 a167           193 	ld	r7,r6
0046 da12           194 	callr	_trim
0048 6f02  0000*    195 	ld	_av0,r2
004c 4df1  0002     196 	cp	~L1+2(fp),#3
0050 0003  
0052 e901           197 	jpr	ge,L69
0054 dce7           198 	callr	_usage
                    199 L69:
0056 61f2  0000     200 	ld	r2,~L1(fp)
005a 612d  0002     201 	ld	r13,2(r2)
                    202 L73:
005e 0cd4           203 	testb	@r13
0060 e63d           204 	jpr	eq,L72
0062 20db           205 	ldb	rl3,@r13
0064 b130           206 	extsb	r3
0066 a132           207 	ld	r2,r3
0068 0302  0061     208 	sub	r2,#97
006c 0b02  0017     209 	cp	r2,#23
0070 eb28           210 	jpr	ugt,L92
0072 b320           211 	rl	r2,#1
0074 6122  00ca'    212 	ld	r2,L96(r2)
0078 1e28           213 	jp	@r2
                    214 L83:
007a 20db           215 	ldb	rl3,@r13
007c b130           216 	extsb	r3
007e 0303  0061     217 	sub	r3,#97
0082 0103  0000*    218 	add	r3,#_flg
0086 20db           219 	ldb	rl3,@r13
0088 b130           220 	extsb	r3
008a 0303  0061     221 	sub	r3,#97
008e 6830  0000*    222 	incb	_flg(r3),#1
                    223 L71:
0092 a9d0           224 	inc	r13,#1
0094 e8e4           225 	jpr	L73
                    226 L84:
0096 2107  01c0'    227 	ld	r7,#_rcmd
                    228 L20002:
009a df85           229 	callr	_setcom
009c e8fa           230 	jpr	L71
                    231 L86:
009e 2107  0250'    232 	ld	r7,#_dcmd
00a2 e8fb           233 	jpr	L20002
                    234 L87:
00a4 2107  0292'    235 	ld	r7,#_xcmd
00a8 e8f8           236 	jpr	L20002
                    237 L88:
00aa 2107  03fe'    238 	ld	r7,#_tcmd
00ae e8f5           239 	jpr	L20002
                    240 L89:
00b0 2107  0320'    241 	ld	r7,#_pcmd
00b4 e8f2           242 	jpr	L20002
                    243 L90:
00b6 2107  0370'    244 	ld	r7,#_mcmd
00ba e8ef           245 	jpr	L20002
                    246 L91:
00bc 2107  0448'    247 	ld	r7,#_qcmd
00c0 e8ec           248 	jpr	L20002
                    249 L92:
00c2 2107  0010*    250 	ld	r7,#__iob+16
00c6 2106  0078'    251 	ld	r6,#L94
00ca 6105  0000*    252 	ld	r5,_av0
00ce 20dc           253 	ldb	rl4,@r13
00d0 b140           254 	extsb	r4
00d2 5f00  0000*    255 	callr	_fprintf
00d6 bd71           256 	ldk	r7,#1
00d8 dd0b           257 	callr	_done
00da e8db           258 	jpr	L71
                    259 L72:
00dc 4c04  000b*    260 	testb	_flg+11
00e0 e609           261 	jpr	eq,L97
00e2 4d05  004a'    262 	ld	_tmp0,#L98
00e6 008e' 
00e8 4d05  0058'    263 	ld	_tmp1,#L99
00ec 0096' 
00ee 4d05  0068'    264 	ld	_tmp2,#L100
00f2 009e' 
                    265 L97:
00f4 4c04  0008*    266 	testb	_flg+8
00f8 e602           267 	jpr	eq,L101
00fa 6800  0001*    268 	incb	_flg+1,#1
                    269 L101:
00fe 4c04  0000*    270 	testb	_flg
0102 ee03           271 	jpr	ne,L10003
0104 4c04  0001*    272 	testb	_flg+1
0108 e614           273 	jpr	eq,L102
                    274 L10003:
010a 4d05  0000*    275 	ld	_bastate,#1
010e 0001  
0110 61f7  0000     276 	ld	r7,~L1(fp)
0114 6176  0004     277 	ld	r6,4(r7)
0118 a167           278 	ld	r7,r6
011a da7c           279 	callr	_trim
011c 6f02  0000*    280 	ld	_ponam,r2
0120 69f1  0000     281 	inc	~L1(fp),#2
0124 6bf0  0002     282 	dec	~L1+2(fp),#1
0128 4df1  0002     283 	cp	~L1+2(fp),#3
012c 0003  
012e e901           284 	jpr	ge,L102
0130 dd55           285 	callr	_usage
                    286 L102:
0132 61f2  0000     287 	ld	r2,~L1(fp)
0136 6123  0004     288 	ld	r3,4(r2)
013a 6f03  0000*    289 	ld	_arnam,r3
013e a925           290 	inc	r2,#6
0140 6f02  0000*    291 	ld	_namv,r2
0144 61f2  0002     292 	ld	r2,~L1+2(fp)
0148 ab22           293 	dec	r2,#3
014a 6f02  0000*    294 	ld	_namc,r2
014e 4d04  0000*    295 	test	_comfun
0152 ee12           296 	jpr	ne,L104
0154 4c04  0014*    297 	testb	_flg+20
0158 ee0c           298 	jpr	ne,L105
015a 2107  0010*    299 	ld	r7,#__iob+16
015e 2106  00a6'    300 	ld	r6,#L106
0162 6105  0000*    301 	ld	r5,_av0
0166 6104  002c'    302 	ld	r4,_man
016a 5f00  0000*    303 	callr	_fprintf
016e bd71           304 	ldk	r7,#1
0170 dd57           305 	callr	_done
                    306 L105:
0172 2107  01c0'    307 	ld	r7,#_rcmd
0176 dff3           308 	callr	_setcom
                    309 L104:
0178 6102  0000*    310 	ld	r2,_comfun
017c 1f20           311 	call	@r2
017e dd41           312 	callr	_notfound
0180 6ff2  0004     313 	ld	~L1+4(fp),r2
0184 a127           314 	ld	r7,r2
0186 dd62           315 	callr	_done
0188 5cf1  0d01     316 	ldm	r13,6(fp),#2
018c 0006  
018e a9f9           317 	inc	fp,#~L2
0190 9e08           318 	ret
                    319 	~L1 := 0
                    320 	~L2 := 10
                    321 	}	/* _main */
                    322 
                    323 .data
                    324 
                    325 L110:
00fa 25  73  3a     326 .byte	%25,%73,%3a,%20,%6f,%6e,%6c,%79
00fd 20  6f  6e  
0100 6c  79  
0102 20  6f  6e     327 .byte	%20,%6f,%6e,%65,%20,%6f,%66,%20
0105 65  20  6f  
0108 66  20  
010a 5b  25  73     328 .byte	%5b,%25,%73,%5d,%20,%61,%6c,%6c
010d 5d  20  61  
0110 6c  6c  
0112 6f  77  65     329 .byte	%6f,%77,%65,%64,%a,%0
0115 64  0a  00  
                    330 .even
                    331 /*~~setcom:*/
                    332 
                    333 .code
                    334 
                    335 _setcom::
                    336 {
0192 abf1           337 	dec	fp,#~L2
0194 2ff7           338 	ld	@fp,r7
0196 4d04  0000*    339 	test	_comfun
019a e60c           340 	jpr	eq,L109
019c 2107  0010*    341 	ld	r7,#__iob+16
01a0 2106  00fa'    342 	ld	r6,#L110
01a4 6105  0000*    343 	ld	r5,_av0
01a8 6104  002c'    344 	ld	r4,_man
01ac 5f00  0000*    345 	callr	_fprintf
01b0 bd71           346 	ldk	r7,#1
01b2 dd78           347 	callr	_done
                    348 L109:
01b4 61f2  0000     349 	ld	r2,~L1(fp)
01b8 6f02  0000*    350 	ld	_comfun,r2
01bc a9f1           351 	inc	fp,#~L2
01be 9e08           352 	ret
                    353 	~L1 := 0
                    354 	~L2 := 2
                    355 	}	/* _setcom */
                    356 
                    357 .data
                    358 
                    359 L123:
0118 25  73  3a     360 .byte	%25,%73,%3a,%20,%63,%61,%6e,%6e
011b 20  63  61  
011e 6e  6e  
0120 6f  74  20     361 .byte	%6f,%74,%20,%6f,%70,%65,%6e,%20
0123 6f  70  65  
0126 6e  20  
0128 25  73  0a     362 .byte	%25,%73,%a,%0
012b 00  
                    363 .even
                    364 /*~~rcmd:*/
                    365 
                    366 .code
                    367 
                    368 _rcmd::
                    369 {
01c0 abf3           370 	dec	fp,#~L2
01c2 6ffe  0002     371 	ld	2(fp),r14
01c6 de60           372 	callr	_init
01c8 de2f           373 	callr	_getaf
01ca e83a           374 	jpr	L115
                    375 L20004:
01cc 4d04  0000*    376 	test	_namc
01d0 e613           377 	jpr	eq,L124
01d2 2107  0010*    378 	ld	r7,#__iob+16
01d6 2106  0118'    379 	ld	r6,#L123
01da 6105  0000*    380 	ld	r5,_av0
01de 6104  0000*    381 	ld	r4,_file
01e2 5f00  0000*    382 	callr	_fprintf
01e6 e808           383 	jpr	L124
                    384 L20006:
01e8 5402  0016*    385 	ldl	rr2,_stbuf+22
01ec 5002  000e*    386 	cpl	rr2,_arbuf+14
01f0 ea1c           387 	jpr	gt,L125
01f2 a1e7           388 	ld	r7,r14
01f4 5f00  0000*    389 	callr	_close
                    390 L124:
01f8 2107  0063     391 	ld	r7,#99
01fc db03           392 	callr	_mesg
01fe 6107  0000*    393 	ld	r7,_af
0202 6106  0000*    394 	ld	r6,_tf
0206 bd5e           395 	ldk	r5,#14
0208 dc1d           396 	callr	_copyfil
020a e81a           397 	jpr	L115
                    398 L20008:
020c db5f           399 	callr	_bamatch
020e 4d04  0000*    400 	test	_namc
0212 e603           401 	jpr	eq,L10010
0214 db99           402 	callr	_match
0216 8d24           403 	test	r2
0218 e6ef           404 	jpr	eq,L124
                    405 L10010:
021a dc45           406 	callr	_stats
021c a12e           407 	ld	r14,r2
021e 0b02  0000     408 	cp	r2,#0
0222 e1d4           409 	jpr	lt,L20004
0224 4c04  0014*    410 	testb	_flg+20
0228 eedf           411 	jpr	ne,L20006
                    412 L125:
022a 2107  0072     413 	ld	r7,#114
022e db1c           414 	callr	_mesg
0230 6107  0000*    415 	ld	r7,_af
0234 2106  ffff     416 	ld	r6,#-1
0238 bd53           417 	ldk	r5,#3
023a dc36           418 	callr	_copyfil
023c a1e7           419 	ld	r7,r14
023e dc93           420 	callr	_movefil
                    421 L115:
0240 dbdc           422 	callr	_getdir
0242 8d24           423 	test	r2
0244 e6e3           424 	jpr	eq,L20008
0246 dd6a           425 	callr	_cleanup
0248 61fe  0002     426 	ld	r14,2(fp)
024c a9f3           427 	inc	fp,#~L2
024e 9e08           428 	ret
                    429 	~L1 := 0
                    430 	~L2 := 4
                    431 	}	/* _rcmd */
                    432 
                    433 .data
                    434 
                    435 /*~~dcmd:*/
                    436 
                    437 .code
                    438 
                    439 _dcmd::
                    440 {
0250 dea5           441 	callr	_init
0252 de74           442 	callr	_getaf
0254 8d24           443 	test	r2
0256 e618           444 	jpr	eq,L135
0258 dddc           445 	callr	_noar
025a e816           446 	jpr	L135
                    447 L20010:
025c 2107  0064     448 	ld	r7,#100
0260 db35           449 	callr	_mesg
0262 6107  0000*    450 	ld	r7,_af
0266 2106  ffff     451 	ld	r6,#-1
026a bd53           452 	ldk	r5,#3
                    453 L20011:
026c dc4f           454 	callr	_copyfil
026e e80c           455 	jpr	L135
                    456 L20013:
0270 dbc7           457 	callr	_match
0272 8d24           458 	test	r2
0274 eef3           459 	jpr	ne,L20010
0276 2107  0063     460 	ld	r7,#99
027a db42           461 	callr	_mesg
027c 6107  0000*    462 	ld	r7,_af
0280 6106  0000*    463 	ld	r6,_tf
0284 bd5e           464 	ldk	r5,#14
0286 e8f2           465 	jpr	L20011
                    466 L135:
0288 dc00           467 	callr	_getdir
028a 8d24           468 	test	r2
028c e6f1           469 	jpr	eq,L20013
028e dd5b           470 	callr	_install
0290 9e08           471 	ret
                    472 	~L1 := 0
                    473 	~L2 := 0
                    474 	}	/* _dcmd */
                    475 
                    476 .data
                    477 
                    478 L146:
012c 25  73  3a     479 .byte	%25,%73,%3a,%20,%25,%73,%20,%63
012f 20  25  73  
0132 20  63  
0134 61  6e  6e     480 .byte	%61,%6e,%6e,%6f,%74,%20,%63,%72
0137 6f  74  20  
013a 63  72  
013c 65  61  74     481 .byte	%65,%61,%74,%65,%a,%0
013f 65  0a  00  
                    482 .even
                    483 /*~~xcmd:*/
                    484 
                    485 .code
                    486 
                    487 _xcmd::
                    488 {
0292 abf1           489 	dec	fp,#~L2
0294 2ffe           490 	ld	@fp,r14
0296 de96           491 	callr	_getaf
0298 8d24           492 	test	r2
029a e63c           493 	jpr	eq,L141
029c ddfe           494 	callr	_noar
029e e83a           495 	jpr	L141
                    496 L20015:
02a0 2107  0010*    497 	ld	r7,#__iob+16
02a4 2106  012c'    498 	ld	r6,#L146
02a8 6105  0000*    499 	ld	r5,_av0
02ac 6104  0000*    500 	ld	r4,_file
02b0 5f00  0000*    501 	callr	_fprintf
                    502 L147:
02b4 2107  0063     503 	ld	r7,#99
02b8 db61           504 	callr	_mesg
02ba 6107  0000*    505 	ld	r7,_af
02be 2106  ffff     506 	ld	r6,#-1
02c2 bd53           507 	ldk	r5,#3
02c4 dc7b           508 	callr	_copyfil
02c6 4d01  0000*    509 	cp	_namc,#0
02ca 0000  
02cc e223           510 	jpr	le,L141
02ce ddc1           511 	callr	_morefil
02d0 8d24           512 	test	r2
02d2 ee20           513 	jpr	ne,L141
02d4 8377           514 	sub	r7,r7
02d6 de0a           515 	callr	_done
02d8 e81d           516 	jpr	L141
                    517 L20017:
02da 4d04  0000*    518 	test	_namc
02de e603           519 	jpr	eq,L10017
02e0 dbff           520 	callr	_match
02e2 8d24           521 	test	r2
02e4 e6e7           522 	jpr	eq,L147
                    523 L10017:
02e6 6106  0014*    524 	ld	r6,_arbuf+20
02ea 0706  01ff     525 	and	r6,#511
02ee 6107  0000*    526 	ld	r7,_file
02f2 5f00  0000*    527 	callr	_creat
02f6 a12e           528 	ld	r14,r2
02f8 0b02  0000     529 	cp	r2,#0
02fc e1d1           530 	jpr	lt,L20015
02fe 2107  0078     531 	ld	r7,#120
0302 db86           532 	callr	_mesg
0304 6107  0000*    533 	ld	r7,_af
0308 a1e6           534 	ld	r6,r14
030a bd52           535 	ldk	r5,#2
030c dc9f           536 	callr	_copyfil
030e a1e7           537 	ld	r7,r14
0310 5f00  0000*    538 	callr	_close
                    539 L141:
0314 dc46           540 	callr	_getdir
0316 8d24           541 	test	r2
0318 e6e0           542 	jpr	eq,L20017
031a 21fe           543 	ld	r14,@fp
031c a9f1           544 	inc	fp,#~L2
031e 9e08           545 	ret
                    546 	~L1 := 0
                    547 	~L2 := 2
                    548 	}	/* _xcmd */
                    549 
                    550 .data
                    551 
                    552 L157:
0142 0a  3c  25     553 .byte	%a,%3c,%25,%73,%3e,%a,%a,%0
0145 73  3e  0a  
0148 0a  00  
                    554 .even
                    555 /*~~pcmd:*/
                    556 
                    557 .code
                    558 
                    559 _pcmd::
                    560 {
0320 dedb           561 	callr	_getaf
0322 8d24           562 	test	r2
0324 e621           563 	jpr	eq,L152
0326 de43           564 	callr	_noar
0328 e81f           565 	jpr	L152
                    566 L10021:
032a 4c04  0015*    567 	testb	_flg+21
032e e60a           568 	jpr	eq,L155
0330 2107  0142'    569 	ld	r7,#L157
0334 6106  0000*    570 	ld	r6,_file
0338 5f00  0000*    571 	callr	_printf
033c 2107  0008*    572 	ld	r7,#__iob+8
0340 5f00  0000*    573 	callr	_fflush
                    574 L155:
0344 6107  0000*    575 	ld	r7,_af
0348 bd61           576 	ldk	r6,#1
034a bd52           577 	ldk	r5,#2
                    578 L20019:
034c dcbf           579 	callr	_copyfil
034e e80c           580 	jpr	L152
                    581 L20021:
0350 4d04  0000*    582 	test	_namc
0354 e6ea           583 	jpr	eq,L10021
0356 dc3a           584 	callr	_match
0358 8d24           585 	test	r2
035a eee7           586 	jpr	ne,L10021
035c 6107  0000*    587 	ld	r7,_af
0360 2106  ffff     588 	ld	r6,#-1
0364 bd53           589 	ldk	r5,#3
0366 e8f2           590 	jpr	L20019
                    591 L152:
0368 dc70           592 	callr	_getdir
036a 8d24           593 	test	r2
036c e6f1           594 	jpr	eq,L20021
036e 9e08           595 	ret
                    596 	~L1 := 0
                    597 	~L2 := 0
                    598 	}	/* _pcmd */
                    599 
                    600 .data
                    601 
                    602 L163:
014a 25  73  3a     603 .byte	%25,%73,%3a,%20,%63,%61,%6e,%6e
014d 20  63  61  
0150 6e  6e  
0152 6f  74  20     604 .byte	%6f,%74,%20,%63,%72,%65,%61,%74
0155 63  72  65  
0158 61  74  
015a 65  20  74     605 .byte	%65,%20,%74,%68,%69,%72,%64,%20
015d 68  69  72  
0160 64  20  
0162 74  65  6d     606 .byte	%74,%65,%6d,%70,%a,%0
0165 70  0a  00  
                    607 .even
                    608 /*~~mcmd:*/
                    609 
                    610 .code
                    611 
                    612 _mcmd::
                    613 {
0370 abf1           614 	dec	fp,#~L2
0372 df36           615 	callr	_init
0374 df05           616 	callr	_getaf
0376 8d24           617 	test	r2
0378 e601           618 	jpr	eq,L160
037a de6d           619 	callr	_noar
                    620 L160:
037c 6107  0068'    621 	ld	r7,_tmp2
0380 5f00  0000*    622 	callr	_mktemp
0384 6f02  0000*    623 	ld	_tf2nam,r2
0388 a127           624 	ld	r7,r2
038a 2106  0180     625 	ld	r6,#384
038e 5f00  0000*    626 	callr	_creat
0392 6ff2  0000     627 	ld	~L1(fp),r2
0396 a127           628 	ld	r7,r2
0398 5f00  0000*    629 	callr	_close
039c 6107  0000*    630 	ld	r7,_tf2nam
03a0 bd62           631 	ldk	r6,#2
03a2 5f00  0000*    632 	callr	_open
03a6 6f02  0000*    633 	ld	_tf2,r2
03aa 0b02  0000     634 	cp	r2,#0
03ae e921           635 	jpr	ge,L164
03b0 2107  0010*    636 	ld	r7,#__iob+16
03b4 2106  014a'    637 	ld	r6,#L163
03b8 6105  0000*    638 	ld	r5,_av0
03bc 5f00  0000*    639 	callr	_fprintf
03c0 bd71           640 	ldk	r7,#1
03c2 de80           641 	callr	_done
03c4 e816           642 	jpr	L164
                    643 L20023:
03c6 2107  006d     644 	ld	r7,#109
03ca dbea           645 	callr	_mesg
03cc 6107  0000*    646 	ld	r7,_af
03d0 6106  0000*    647 	ld	r6,_tf2
                    648 L20025:
03d4 bd5e           649 	ldk	r5,#14
03d6 dd04           650 	callr	_copyfil
03d8 e80c           651 	jpr	L164
                    652 L20027:
03da dc46           653 	callr	_bamatch
03dc dc7d           654 	callr	_match
03de 8d24           655 	test	r2
03e0 eef2           656 	jpr	ne,L20023
03e2 2107  0063     657 	ld	r7,#99
03e6 dbf8           658 	callr	_mesg
03e8 6107  0000*    659 	ld	r7,_af
03ec 6106  0000*    660 	ld	r6,_tf
03f0 e8f1           661 	jpr	L20025
                    662 L164:
03f2 dcb5           663 	callr	_getdir
03f4 8d24           664 	test	r2
03f6 e6f1           665 	jpr	eq,L20027
03f8 de10           666 	callr	_install
03fa a9f1           667 	inc	fp,#~L2
03fc 9e08           668 	ret
                    669 	~L1 := 0
                    670 	~L2 := 2
                    671 	}	/* _mcmd */
                    672 
                    673 .data
                    674 
                    675 L174:
0168 25  73  0a     676 .byte	%25,%73,%a,%0
016b 00  
                    677 .even
                    678 /*~~tcmd:*/
                    679 
                    680 .code
                    681 
                    682 _tcmd::
                    683 {
03fe abf1           684 	dec	fp,#~L2
0400 df4b           685 	callr	_getaf
0402 8d24           686 	test	r2
0404 e61c           687 	jpr	eq,L169
0406 deb3           688 	callr	_noar
0408 e81a           689 	jpr	L169
                    690 L20029:
040a 4d04  0000*    691 	test	_namc
040e e603           692 	jpr	eq,L10028
0410 dc97           693 	callr	_match
0412 8d24           694 	test	r2
0414 e60e           695 	jpr	eq,L171
                    696 L10028:
0416 4c04  0015*    697 	testb	_flg+21
041a e601           698 	jpr	eq,L172
041c dbdc           699 	callr	_longt
                    700 L172:
041e 6107  0000*    701 	ld	r7,_file
0422 dc00           702 	callr	_trim
0424 6ff2  0000     703 	ld	~L1(fp),r2
0428 2107  0168'    704 	ld	r7,#L174
042c a126           705 	ld	r6,r2
042e 5f00  0000*    706 	callr	_printf
                    707 L171:
0432 6107  0000*    708 	ld	r7,_af
0436 2106  ffff     709 	ld	r6,#-1
043a bd53           710 	ldk	r5,#3
043c dd37           711 	callr	_copyfil
                    712 L169:
043e dcdb           713 	callr	_getdir
0440 8d24           714 	test	r2
0442 e6e3           715 	jpr	eq,L20029
0444 a9f1           716 	inc	fp,#~L2
0446 9e08           717 	ret
                    718 	~L1 := 0
                    719 	~L2 := 2
                    720 	}	/* _tcmd */
                    721 
                    722 .data
                    723 
                    724 L177:
016c 25  73  3a     725 .byte	%25,%73,%3a,%20,%61,%62,%69,%20
016f 20  61  62  
0172 69  20  
0174 6e  6f  74     726 .byte	%6e,%6f,%74,%20,%61,%6c,%6c,%6f
0177 20  61  6c  
017a 6c  6f  
017c 77  65  64     727 .byte	%77,%65,%64,%20,%77,%69,%74,%68
017f 20  77  69  
0182 74  68  
0184 20  71  0a     728 .byte	%20,%71,%a,%0
0187 00  
                    729 .even
                    730 L188:
0188 25  73  3a     731 .byte	%25,%73,%3a,%20,%25,%73,%20,%63
018b 20  25  73  
018e 20  63  
0190 61  6e  6e     732 .byte	%61,%6e,%6e,%6f,%74,%20,%6f,%70
0193 6f  74  20  
0196 6f  70  
0198 65  6e  0a     733 .byte	%65,%6e,%a,%0
019b 00  
                    734 .even
                    735 /*~~qcmd:*/
                    736 
                    737 .code
                    738 
                    739 _qcmd::
                    740 {
0448 abf3           741 	dec	fp,#~L2
044a 1cf9  0d01     742 	ldm	@fp,r13,#2
044e 4c04  0000*    743 	testb	_flg
0452 ee03           744 	jpr	ne,L10032
0454 4c04  0001*    745 	testb	_flg+1
0458 e60a           746 	jpr	eq,L176
                    747 L10032:
045a 2107  0010*    748 	ld	r7,#__iob+16
045e 2106  016c'    749 	ld	r6,#L177
0462 6105  0000*    750 	ld	r5,_av0
0466 5f00  0000*    751 	callr	_fprintf
046a bd71           752 	ldk	r7,#1
046c ded5           753 	callr	_done
                    754 L176:
046e df58           755 	callr	_getqf
0470 bde0           756 	ldk	r14,#0
0472 e808           757 	jpr	L181
                    758 L20031:
0474 a1e2           759 	ld	r2,r14
0476 81e2           760 	add	r2,r14
0478 6127  0042'    761 	ld	r7,_signum(r2)
047c bd61           762 	ldk	r6,#1
047e 5f00  0000*    763 	callr	_signal
0482 a9e0           764 	inc	r14,#1
                    765 L181:
0484 a1e2           766 	ld	r2,r14
0486 81e2           767 	add	r2,r14
0488 4d24  0042'    768 	test	_signum(r2)
048c eef3           769 	jpr	ne,L20031
048e 6107  0000*    770 	ld	r7,_qf
0492 9244           771 	subl	rr4,rr4
0494 bd32           772 	ldk	r3,#2
0496 5f00  0000*    773 	callr	_lseek
049a bde0           774 	ldk	r14,#0
049c e82e           775 	jpr	L185
                    776 L20033:
049e a1e2           777 	ld	r2,r14
04a0 81e2           778 	add	r2,r14
04a2 6103  0000*    779 	ld	r3,_namv
04a6 8123           780 	add	r3,r2
04a8 2132           781 	ld	r2,@r3
04aa 6f02  0000*    782 	ld	_file,r2
04ae 8d24           783 	test	r2
04b0 e623           784 	jpr	eq,L183
04b2 a1e2           785 	ld	r2,r14
04b4 81e2           786 	add	r2,r14
04b6 6103  0000*    787 	ld	r3,_namv
04ba 8123           788 	add	r3,r2
04bc 0d38           789 	clr	@r3
04be 2107  0071     790 	ld	r7,#113
04c2 dc66           791 	callr	_mesg
04c4 dd9a           792 	callr	_stats
04c6 a12d           793 	ld	r13,r2
04c8 0b02  0000     794 	cp	r2,#0
04cc e90b           795 	jpr	ge,L187
04ce 2107  0010*    796 	ld	r7,#__iob+16
04d2 2106  0188'    797 	ld	r6,#L188
04d6 6105  0000*    798 	ld	r5,_av0
04da 6104  0000*    799 	ld	r4,_file
04de 5f00  0000*    800 	callr	_fprintf
04e2 e80a           801 	jpr	L183
                    802 L187:
04e4 6102  0000*    803 	ld	r2,_qf
04e8 6f02  0000*    804 	ld	_tf,r2
04ec a1d7           805 	ld	r7,r13
04ee ddeb           806 	callr	_movefil
04f0 6102  0000*    807 	ld	r2,_tf
04f4 6f02  0000*    808 	ld	_qf,r2
                    809 L183:
04f8 a9e0           810 	inc	r14,#1
                    811 L185:
04fa 4b0e  0000*    812 	cp	r14,_namc
04fe e1cf           813 	jpr	lt,L20033
0500 1cf1  0d01     814 	ldm	r13,@fp,#2
0504 a9f3           815 	inc	fp,#~L2
0506 9e08           816 	ret
                    817 	~L1 := 0
                    818 	~L2 := 4
                    819 	}	/* _qcmd */
                    820 
                    821 .data
                    822 
                    823 .even
                    824 L190:
019c ff65           825 .word	%ff65
                    826 L192:
019e 25  73  3a     827 .byte	%25,%73,%3a,%20,%63,%61,%6e,%6e
01a1 20  63  61  
01a4 6e  6e  
01a6 6f  74  20     828 .byte	%6f,%74,%20,%63,%72,%65,%61,%74
01a9 63  72  65  
01ac 61  74  
01ae 65  20  74     829 .byte	%65,%20,%74,%65,%6d,%70,%20,%66
01b1 65  6d  70  
01b4 20  66  
01b6 69  6c  65     830 .byte	%69,%6c,%65,%a,%0
01b9 0a  00  
                    831 .even
                    832 /*~~init:*/
                    833 
                    834 .code
                    835 
                    836 _init::
                    837 {
0508 abf1           838 	dec	fp,#~L2
050a 6107  004a'    839 	ld	r7,_tmp0
050e 5f00  0000*    840 	callr	_mktemp
0512 6f02  0000*    841 	ld	_tfnam,r2
0516 a127           842 	ld	r7,r2
0518 2106  0180     843 	ld	r6,#384
051c 5f00  0000*    844 	callr	_creat
0520 6ff2  0000     845 	ld	~L1(fp),r2
0524 a127           846 	ld	r7,r2
0526 5f00  0000*    847 	callr	_close
052a 6107  0000*    848 	ld	r7,_tfnam
052e bd62           849 	ldk	r6,#2
0530 5f00  0000*    850 	callr	_open
0534 6f02  0000*    851 	ld	_tf,r2
0538 0b02  0000     852 	cp	r2,#0
053c e90a           853 	jpr	ge,L191
053e 2107  0010*    854 	ld	r7,#__iob+16
0542 2106  019e'    855 	ld	r6,#L192
0546 6105  0000*    856 	ld	r5,_av0
054a 5f00  0000*    857 	callr	_fprintf
054e bd71           858 	ldk	r7,#1
0550 df47           859 	callr	_done
                    860 L191:
0552 6107  0000*    861 	ld	r7,_tf
0556 2106  019c'    862 	ld	r6,#L190
055a bd52           863 	ldk	r5,#2
055c 5f00  0000*    864 	callr	_write
0560 0b02  0002     865 	cp	r2,#2
0564 e601           866 	jpr	eq,L10033
0566 dc1e           867 	callr	_wrerr
                    868 L10033:
0568 a9f1           869 	inc	fp,#~L2
056a 9e08           870 	ret
                    871 	~L1 := 0
                    872 	~L2 := 2
                    873 	}	/* _init */
                    874 
                    875 .data
                    876 
                    877 L200:
01bc 25  73  3a     878 .byte	%25,%73,%3a,%20,%25,%73,%20,%6e
01bf 20  25  73  
01c2 20  6e  
01c4 6f  74  20     879 .byte	%6f,%74,%20,%69,%6e,%20,%61,%72
01c7 69  6e  20  
01ca 61  72  
01cc 63  68  69     880 .byte	%63,%68,%69,%76,%65,%20,%66,%6f
01cf 76  65  20  
01d2 66  6f  
01d4 72  6d  61     881 .byte	%72,%6d,%61,%74,%a,%0
01d7 74  0a  00  
                    882 .even
                    883 /*~~getaf:*/
                    884 
                    885 .code
                    886 
                    887 _getaf::
                    888 {
056c abf3           889 	dec	fp,#~L2
056e 6107  0000*    890 	ld	r7,_arnam
0572 8366           891 	sub	r6,r6
0574 5f00  0000*    892 	callr	_open
0578 6f02  0000*    893 	ld	_af,r2
057c 0b02  0000     894 	cp	r2,#0
0580 e902           895 	jpr	ge,L197
0582 bd21           896 	ldk	r2,#1
0584 e81b           897 	jpr	L10036
                    898 L197:
0586 76f6  0000     899 	lda	r6,~L1(fp)
058a 6107  0000*    900 	ld	r7,_af
058e bd52           901 	ldk	r5,#2
0590 5f00  0000*    902 	callr	_read
0594 0b02  0002     903 	cp	r2,#2
0598 ee04           904 	jpr	ne,L10039
059a 4df1  0000     905 	cp	~L1(fp),#65381
059e ff65  
05a0 e60c           906 	jpr	eq,L199
                    907 L10039:
05a2 2107  0010*    908 	ld	r7,#__iob+16
05a6 2106  01bc'    909 	ld	r6,#L200
05aa 6105  0000*    910 	ld	r5,_av0
05ae 6104  0000*    911 	ld	r4,_arnam
05b2 5f00  0000*    912 	callr	_fprintf
05b6 bd71           913 	ldk	r7,#1
05b8 df7b           914 	callr	_done
                    915 L199:
05ba 8322           916 	sub	r2,r2
                    917 L10036:
05bc a9f3           918 	inc	fp,#~L2
05be 9e08           919 	ret
                    920 	~L1 := 0
                    921 	~L2 := 4
                    922 	}	/* _getaf */
                    923 
                    924 .data
                    925 
                    926 L204:
01da 25  73  3a     927 .byte	%25,%73,%3a,%20,%63,%72,%65,%61
01dd 20  63  72  
01e0 65  61  
01e2 74  69  6e     928 .byte	%74,%69,%6e,%67,%20,%25,%73,%a
01e5 67  20  25  
01e8 73  0a  
01ea 00             929 .byte	%0
                    930 .even
                    931 L206:
01ec 25  73  3a     932 .byte	%25,%73,%3a,%20,%63,%61,%6e,%6e
01ef 20  63  61  
01f2 6e  6e  
01f4 6f  74  20     933 .byte	%6f,%74,%20,%63,%72,%65,%61,%74
01f7 63  72  65  
01fa 61  74  
01fc 65  20  25     934 .byte	%65,%20,%25,%73,%a,%0
01ff 73  0a  00  
                    935 .even
                    936 L210:
0202 25  73  3a     937 .byte	%25,%73,%3a,%20,%25,%73,%20,%6e
0205 20  25  73  
0208 20  6e  
020a 6f  74  20     938 .byte	%6f,%74,%20,%69,%6e,%20,%61,%72
020d 69  6e  20  
0210 61  72  
0212 63  68  69     939 .byte	%63,%68,%69,%76,%65,%20,%66,%6f
0215 76  65  20  
0218 66  6f  
021a 72  6d  61     940 .byte	%72,%6d,%61,%74,%a,%0
021d 74  0a  00  
                    941 .even
                    942 /*~~getqf:*/
                    943 
                    944 .code
                    945 
                    946 _getqf::
                    947 {
05c0 abf3           948 	dec	fp,#~L2
05c2 6107  0000*    949 	ld	r7,_arnam
05c6 bd62           950 	ldk	r6,#2
05c8 5f00  0000*    951 	callr	_open
05cc 6f02  0000*    952 	ld	_qf,r2
05d0 0b02  0000     953 	cp	r2,#0
05d4 e93d           954 	jpr	ge,L202
05d6 4c04  0002*    955 	testb	_flg+2
05da ee0a           956 	jpr	ne,L203
05dc 2107  0010*    957 	ld	r7,#__iob+16
05e0 2106  01da'    958 	ld	r6,#L204
05e4 6105  0000*    959 	ld	r5,_av0
05e8 6104  0000*    960 	ld	r4,_arnam
05ec 5f00  0000*    961 	callr	_fprintf
                    962 L203:
05f0 6107  0000*    963 	ld	r7,_arnam
05f4 2106  01b6     964 	ld	r6,#438
05f8 5f00  0000*    965 	callr	_creat
05fc 6ff2  0002     966 	ld	~L1+2(fp),r2
0600 a127           967 	ld	r7,r2
0602 5f00  0000*    968 	callr	_close
0606 6107  0000*    969 	ld	r7,_arnam
060a bd62           970 	ldk	r6,#2
060c 5f00  0000*    971 	callr	_open
0610 6f02  0000*    972 	ld	_qf,r2
0614 0b02  0000     973 	cp	r2,#0
0618 e90c           974 	jpr	ge,L205
061a 2107  0010*    975 	ld	r7,#__iob+16
061e 2106  01ec'    976 	ld	r6,#L206
0622 6105  0000*    977 	ld	r5,_av0
0626 6104  0000*    978 	ld	r4,_arnam
062a 5f00  0000*    979 	callr	_fprintf
062e bd71           980 	ldk	r7,#1
0630 dfb7           981 	callr	_done
                    982 L205:
0632 4df5  0000     983 	ld	~L1(fp),#65381
0636 ff65  
0638 76f6  0000     984 	lda	r6,~L1(fp)
063c 6107  0000*    985 	ld	r7,_qf
0640 bd52           986 	ldk	r5,#2
0642 5f00  0000*    987 	callr	_write
0646 0b02  0002     988 	cp	r2,#2
064a e61c           989 	jpr	eq,L10040
064c dc91           990 	callr	_wrerr
064e e81a           991 	jpr	L10040
                    992 L202:
0650 76f6  0000     993 	lda	r6,~L1(fp)
0654 6107  0000*    994 	ld	r7,_qf
0658 bd52           995 	ldk	r5,#2
065a 5f00  0000*    996 	callr	_read
065e 0b02  0002     997 	cp	r2,#2
0662 ee04           998 	jpr	ne,L10043
0664 4df1  0000     999 	cp	~L1(fp),#65381
0668 ff65  
066a e60c          1000 	jpr	eq,L10040
                   1001 L10043:
066c 2107  0010*   1002 	ld	r7,#__iob+16
0670 2106  0202'   1003 	ld	r6,#L210
0674 6105  0000*   1004 	ld	r5,_av0
0678 6104  0000*   1005 	ld	r4,_arnam
067c 5f00  0000*   1006 	callr	_fprintf
0680 bd71          1007 	ldk	r7,#1
0682 dfe0          1008 	callr	_done
                   1009 L10040:
0684 a9f3          1010 	inc	fp,#~L2
0686 9e08          1011 	ret
                   1012 	~L1 := 0
                   1013 	~L2 := 4
                   1014 	}	/* _getqf */
                   1015 
                   1016 .data
                   1017 
                   1018 L212:
0220 75  73  61    1019 .byte	%75,%73,%61,%67,%65,%3a,%20,%25
0223 67  65  3a  
0226 20  25  
0228 73  20  5b    1020 .byte	%73,%20,%5b,%25,%73,%5d,%5b,%25
022b 25  73  5d  
022e 5b  25  
0230 73  5d  20    1021 .byte	%73,%5d,%20,%61,%72,%63,%68,%69
0233 61  72  63  
0236 68  69  
0238 76  65  20    1022 .byte	%76,%65,%20,%66,%69,%6c,%65,%73
023b 66  69  6c  
023e 65  73  
0240 20  2e  2e    1023 .byte	%20,%2e,%2e,%2e,%a,%0
0243 2e  0a  00  
                   1024 .even
                   1025 /*~~usage:*/
                   1026 
                   1027 .code
                   1028 
                   1029 _usage::
                   1030 {
0688 2107  0220'   1031 	ld	r7,#L212
068c 6106  0000*   1032 	ld	r6,_av0
0690 6105  0036'   1033 	ld	r5,_opt
0694 6104  002c'   1034 	ld	r4,_man
0698 5f00  0000*   1035 	callr	_printf
069c bd71          1036 	ldk	r7,#1
069e dfee          1037 	callr	_done
06a0 9e08          1038 	ret
                   1039 	~L1 := 0
                   1040 	~L2 := 0
                   1041 	}	/* _usage */
                   1042 
                   1043 .data
                   1044 
                   1045 L214:
0246 25  73  3a    1046 .byte	%25,%73,%3a,%20,%25,%73,%20,%64
0249 20  25  73  
024c 20  64  
024e 6f  65  73    1047 .byte	%6f,%65,%73,%20,%6e,%6f,%74,%20
0251 20  6e  6f  
0254 74  20  
0256 65  78  69    1048 .byte	%65,%78,%69,%73,%74,%a,%0
0259 73  74  0a  
025c 00  
                   1049 .even
                   1050 /*~~noar:*/
                   1051 
                   1052 .code
                   1053 
                   1054 _noar::
                   1055 {
06a2 2107  0010*   1056 	ld	r7,#__iob+16
06a6 2106  0246'   1057 	ld	r6,#L214
06aa 6105  0000*   1058 	ld	r5,_av0
06ae 6104  0000*   1059 	ld	r4,_arnam
06b2 5f00  0000*   1060 	callr	_fprintf
06b6 bd71          1061 	ldk	r7,#1
06b8 dffb          1062 	callr	_done
06ba 9e08          1063 	ret
                   1064 	~L1 := 0
                   1065 	~L2 := 0
                   1066 	}	/* _noar */
                   1067 
                   1068 .data
                   1069 
                   1070 /*~~sigdone:*/
                   1071 
                   1072 .code
                   1073 
                   1074 _sigdone::
                   1075 {
06bc 2107  0064    1076 	ld	r7,#100
06c0 dfff          1077 	callr	_done
06c2 9e08          1078 	ret
                   1079 	~L1 := 0
                   1080 	~L2 := 0
                   1081 	}	/* _sigdone */
                   1082 
                   1083 .data
                   1084 
                   1085 /*~~done:*/
                   1086 
                   1087 .code
                   1088 
                   1089 _done::
                   1090 {
06c4 abf1          1091 	dec	fp,#~L2
06c6 2ff7          1092 	ld	@fp,r7
06c8 4d04  0000*   1093 	test	_tfnam
06cc e604          1094 	jpr	eq,L217
06ce 6107  0000*   1095 	ld	r7,_tfnam
06d2 5f00  0000*   1096 	callr	_unlink
                   1097 L217:
06d6 4d04  0000*   1098 	test	_tf1nam
06da e604          1099 	jpr	eq,L219
06dc 6107  0000*   1100 	ld	r7,_tf1nam
06e0 5f00  0000*   1101 	callr	_unlink
                   1102 L219:
06e4 4d04  0000*   1103 	test	_tf2nam
06e8 e604          1104 	jpr	eq,L220
06ea 6107  0000*   1105 	ld	r7,_tf2nam
06ee 5f00  0000*   1106 	callr	_unlink
                   1107 L220:
06f2 61f7  0000    1108 	ld	r7,~L1(fp)
06f6 5f00  0000*   1109 	callr	_exit
06fa a9f1          1110 	inc	fp,#~L2
06fc 9e08          1111 	ret
                   1112 	~L1 := 0
                   1113 	~L2 := 2
                   1114 	}	/* _done */
                   1115 
                   1116 .data
                   1117 
                   1118 L227:
025e 25  73  3a    1119 .byte	%25,%73,%3a,%20,%25,%73,%20,%6e
0261 20  25  73  
0264 20  6e  
0266 6f  74  20    1120 .byte	%6f,%74,%20,%66,%6f,%75,%6e,%64
0269 66  6f  75  
026c 6e  64  
026e 0a  00        1121 .byte	%a,%0
                   1122 .even
                   1123 /*~~notfound:*/
                   1124 
                   1125 .code
                   1126 
                   1127 _notfound::
                   1128 {
06fe abf7          1129 	dec	fp,#~L2
0700 5cf9  0d01    1130 	ldm	4(fp),r13,#2
0704 0004  
0706 bdd0          1131 	ldk	r13,#0
0708 a1de          1132 	ld	r14,r13
070a e818          1133 	jpr	L225
                   1134 L20035:
070c a1e2          1135 	ld	r2,r14
070e 81e2          1136 	add	r2,r14
0710 6103  0000*   1137 	ld	r3,_namv
0714 8123          1138 	add	r3,r2
0716 0d34          1139 	test	@r3
0718 e610          1140 	jpr	eq,L223
071a a1e2          1141 	ld	r2,r14
071c 81e2          1142 	add	r2,r14
071e 6104  0000*   1143 	ld	r4,_namv
0722 8124          1144 	add	r4,r2
0724 2145          1145 	ld	r5,@r4
0726 a154          1146 	ld	r4,r5
0728 2106  025e'   1147 	ld	r6,#L227
072c 6105  0000*   1148 	ld	r5,_av0
0730 2107  0010*   1149 	ld	r7,#__iob+16
0734 5f00  0000*   1150 	callr	_fprintf
0738 a9d0          1151 	inc	r13,#1
                   1152 L223:
073a a9e0          1153 	inc	r14,#1
                   1154 L225:
073c 4b0e  0000*   1155 	cp	r14,_namc
0740 e1e5          1156 	jpr	lt,L20035
0742 a1d2          1157 	ld	r2,r13
0744 5cf1  0d01    1158 	ldm	r13,4(fp),#2
0748 0004  
074a a9f7          1159 	inc	fp,#~L2
074c 9e08          1160 	ret
                   1161 	~L1 := 0
                   1162 	~L2 := 8
                   1163 	}	/* _notfound */
                   1164 
                   1165 .data
                   1166 
                   1167 /*~~morefil:*/
                   1168 
                   1169 .code
                   1170 
                   1171 _morefil::
                   1172 {
074e abf5          1173 	dec	fp,#~L2
0750 bd40          1174 	ldk	r4,#0
0752 a141          1175 	ld	r1,r4
0754 e809          1176 	jpr	L231
                   1177 L20037:
0756 a112          1178 	ld	r2,r1
0758 8112          1179 	add	r2,r1
075a 6103  0000*   1180 	ld	r3,_namv
075e 8123          1181 	add	r3,r2
0760 0d34          1182 	test	@r3
0762 e601          1183 	jpr	eq,L229
0764 a940          1184 	inc	r4,#1
                   1185 L229:
0766 a910          1186 	inc	r1,#1
                   1187 L231:
0768 4b01  0000*   1188 	cp	r1,_namc
076c e1f4          1189 	jpr	lt,L20037
076e a142          1190 	ld	r2,r4
0770 a9f5          1191 	inc	fp,#~L2
0772 9e08          1192 	ret
                   1193 	~L1 := 0
                   1194 	~L2 := 6
                   1195 	}	/* _morefil */
                   1196 
                   1197 .data
                   1198 
                   1199 L239:
0270 25  73  3a    1200 .byte	%25,%73,%3a,%20,%25,%73,%20,%63
0273 20  25  73  
0276 20  63  
0278 61  6e  6e    1201 .byte	%61,%6e,%6e,%6f,%74,%20,%6f,%70
027b 6f  74  20  
027e 6f  70  
0280 65  6e  0a    1202 .byte	%65,%6e,%a,%0
0283 00  
                   1203 .even
                   1204 /*~~cleanup:*/
                   1205 
                   1206 .code
                   1207 
                   1208 _cleanup::
                   1209 {
0774 abf3          1210 	dec	fp,#~L2
0776 1cf9  0d01    1211 	ldm	@fp,r13,#2
077a bde0          1212 	ldk	r14,#0
077c e826          1213 	jpr	L236
                   1214 L20039:
077e a1e2          1215 	ld	r2,r14
0780 81e2          1216 	add	r2,r14
0782 6103  0000*   1217 	ld	r3,_namv
0786 8123          1218 	add	r3,r2
0788 2132          1219 	ld	r2,@r3
078a 6f02  0000*   1220 	ld	_file,r2
078e 8d24          1221 	test	r2
0790 e61b          1222 	jpr	eq,L234
0792 a1e2          1223 	ld	r2,r14
0794 81e2          1224 	add	r2,r14
0796 6103  0000*   1225 	ld	r3,_namv
079a 8123          1226 	add	r3,r2
079c 0d38          1227 	clr	@r3
079e 2107  0061    1228 	ld	r7,#97
07a2 ddd6          1229 	callr	_mesg
07a4 df0a          1230 	callr	_stats
07a6 a12d          1231 	ld	r13,r2
07a8 0b02  0000    1232 	cp	r2,#0
07ac e90b          1233 	jpr	ge,L238
07ae 2107  0010*   1234 	ld	r7,#__iob+16
07b2 2106  0270'   1235 	ld	r6,#L239
07b6 6105  0000*   1236 	ld	r5,_av0
07ba 6104  0000*   1237 	ld	r4,_file
07be 5f00  0000*   1238 	callr	_fprintf
07c2 e802          1239 	jpr	L234
                   1240 L238:
07c4 a1d7          1241 	ld	r7,r13
07c6 df57          1242 	callr	_movefil
                   1243 L234:
07c8 a9e0          1244 	inc	r14,#1
                   1245 L236:
07ca 4b0e  0000*   1246 	cp	r14,_namc
07ce e1d7          1247 	jpr	lt,L20039
07d0 dffc          1248 	callr	_install
07d2 1cf1  0d01    1249 	ldm	r13,@fp,#2
07d6 a9f3          1250 	inc	fp,#~L2
07d8 9e08          1251 	ret
                   1252 	~L1 := 0
                   1253 	~L2 := 4
                   1254 	}	/* _cleanup */
                   1255 
                   1256 .data
                   1257 
                   1258 L246:
0284 25  73  3a    1259 .byte	%25,%73,%3a,%20,%63,%72,%65,%61
0287 20  63  72  
028a 65  61  
028c 74  69  6e    1260 .byte	%74,%69,%6e,%67,%20,%25,%73,%a
028f 67  20  25  
0292 73  0a  
0294 00            1261 .byte	%0
                   1262 .even
                   1263 L248:
0296 25  73  3a    1264 .byte	%25,%73,%3a,%20,%63,%61,%6e,%6e
0299 20  63  61  
029c 6e  6e  
029e 6f  74  20    1265 .byte	%6f,%74,%20,%63,%72,%65,%61,%74
02a1 63  72  65  
02a4 61  74  
02a6 65  20  25    1266 .byte	%65,%20,%25,%73,%a,%0
02a9 73  0a  00  
                   1267 .even
                   1268 /*~~install:*/
                   1269 
                   1270 .code
                   1271 
                   1272 _install::
                   1273 {
07da abf1          1274 	dec	fp,#~L2
07dc 2ffe          1275 	ld	@fp,r14
07de bde0          1276 	ldk	r14,#0
07e0 e808          1277 	jpr	L243
                   1278 L20041:
07e2 a1e2          1279 	ld	r2,r14
07e4 81e2          1280 	add	r2,r14
07e6 6127  0042'   1281 	ld	r7,_signum(r2)
07ea bd61          1282 	ldk	r6,#1
07ec 5f00  0000*   1283 	callr	_signal
07f0 a9e0          1284 	inc	r14,#1
                   1285 L243:
07f2 a1e2          1286 	ld	r2,r14
07f4 81e2          1287 	add	r2,r14
07f6 4d24  0042'   1288 	test	_signum(r2)
07fa eef3          1289 	jpr	ne,L20041
07fc 4d01  0000*   1290 	cp	_af,#0
0800 0000  
0802 e90d          1291 	jpr	ge,L244
0804 4c04  0002*   1292 	testb	_flg+2
0808 ee0a          1293 	jpr	ne,L244
080a 2107  0010*   1294 	ld	r7,#__iob+16
080e 2106  0284'   1295 	ld	r6,#L246
0812 6105  0000*   1296 	ld	r5,_av0
0816 6104  0000*   1297 	ld	r4,_arnam
081a 5f00  0000*   1298 	callr	_fprintf
                   1299 L244:
081e 6107  0000*   1300 	ld	r7,_af
0822 5f00  0000*   1301 	callr	_close
0826 6107  0000*   1302 	ld	r7,_arnam
082a 2106  01b6    1303 	ld	r6,#438
082e 5f00  0000*   1304 	callr	_creat
0832 6f02  0000*   1305 	ld	_af,r2
0836 0b02  0000    1306 	cp	r2,#0
083a e90c          1307 	jpr	ge,L247
083c 2107  0010*   1308 	ld	r7,#__iob+16
0840 2106  0296'   1309 	ld	r6,#L248
0844 6105  0000*   1310 	ld	r5,_av0
0848 6104  0000*   1311 	ld	r4,_arnam
084c 5f00  0000*   1312 	callr	_fprintf
0850 bd71          1313 	ldk	r7,#1
0852 d0c8          1314 	callr	_done
                   1315 L247:
0854 4d04  0000*   1316 	test	_tfnam
0858 e61d          1317 	jpr	eq,L249
085a 6107  0000*   1318 	ld	r7,_tf
085e 9244          1319 	subl	rr4,rr4
0860 8333          1320 	sub	r3,r3
0862 5f00  0000*   1321 	callr	_lseek
0866 e80a          1322 	jpr	L250
                   1323 L20043:
0868 6107  0000*   1324 	ld	r7,_af
086c 2106  0000*   1325 	ld	r6,#_buf
0870 a1e5          1326 	ld	r5,r14
0872 5f00  0000*   1327 	callr	_write
0876 8b2e          1328 	cp	r14,r2
0878 e601          1329 	jpr	eq,L250
087a dda8          1330 	callr	_wrerr
                   1331 L250:
087c 6107  0000*   1332 	ld	r7,_tf
0880 2106  0000*   1333 	ld	r6,#_buf
0884 2105  0200    1334 	ld	r5,#512
0888 5f00  0000*   1335 	callr	_read
088c a12e          1336 	ld	r14,r2
088e 0b02  0000    1337 	cp	r2,#0
0892 eaea          1338 	jpr	gt,L20043
                   1339 L249:
0894 4d04  0000*   1340 	test	_tf2nam
0898 e61d          1341 	jpr	eq,L253
089a 6107  0000*   1342 	ld	r7,_tf2
089e 9244          1343 	subl	rr4,rr4
08a0 8333          1344 	sub	r3,r3
08a2 5f00  0000*   1345 	callr	_lseek
08a6 e80a          1346 	jpr	L254
                   1347 L20045:
08a8 6107  0000*   1348 	ld	r7,_af
08ac 2106  0000*   1349 	ld	r6,#_buf
08b0 a1e5          1350 	ld	r5,r14
08b2 5f00  0000*   1351 	callr	_write
08b6 8b2e          1352 	cp	r14,r2
08b8 e601          1353 	jpr	eq,L254
08ba ddc8          1354 	callr	_wrerr
                   1355 L254:
08bc 6107  0000*   1356 	ld	r7,_tf2
08c0 2106  0000*   1357 	ld	r6,#_buf
08c4 2105  0200    1358 	ld	r5,#512
08c8 5f00  0000*   1359 	callr	_read
08cc a12e          1360 	ld	r14,r2
08ce 0b02  0000    1361 	cp	r2,#0
08d2 eaea          1362 	jpr	gt,L20045
                   1363 L253:
08d4 4d04  0000*   1364 	test	_tf1nam
08d8 e61d          1365 	jpr	eq,L10065
08da 6107  0000*   1366 	ld	r7,_tf1
08de 9244          1367 	subl	rr4,rr4
08e0 8333          1368 	sub	r3,r3
08e2 5f00  0000*   1369 	callr	_lseek
08e6 e80a          1370 	jpr	L258
                   1371 L20047:
08e8 6107  0000*   1372 	ld	r7,_af
08ec 2106  0000*   1373 	ld	r6,#_buf
08f0 a1e5          1374 	ld	r5,r14
08f2 5f00  0000*   1375 	callr	_write
08f6 8b2e          1376 	cp	r14,r2
08f8 e601          1377 	jpr	eq,L258
08fa dde8          1378 	callr	_wrerr
                   1379 L258:
08fc 6107  0000*   1380 	ld	r7,_tf1
0900 2106  0000*   1381 	ld	r6,#_buf
0904 2105  0200    1382 	ld	r5,#512
0908 5f00  0000*   1383 	callr	_read
090c a12e          1384 	ld	r14,r2
090e 0b02  0000    1385 	cp	r2,#0
0912 eaea          1386 	jpr	gt,L20047
                   1387 L10065:
0914 21fe          1388 	ld	r14,@fp
0916 a9f1          1389 	inc	fp,#~L2
0918 9e08          1390 	ret
                   1391 	~L1 := 0
                   1392 	~L2 := 2
                   1393 	}	/* _install */
                   1394 
                   1395 .data
                   1396 
                   1397 /*~~movefil:*/
                   1398 
                   1399 .code
                   1400 
                   1401 _movefil::
                   1402 {
091a abf5          1403 	dec	fp,#~L2
091c 5cf9  0d01    1404 	ldm	2(fp),r13,#2
0920 0002  
0922 2ff7          1405 	ld	@fp,r7
0924 6107  0000*   1406 	ld	r7,_file
0928 de83          1407 	callr	_trim
092a a12e          1408 	ld	r14,r2
092c bdd0          1409 	ldk	r13,#0
                   1410 L20049:
092e 20ea          1411 	ldb	rl2,@r14
0930 6eda  0000*   1412 	ldb	_arbuf(r13),rl2
0934 8ca4          1413 	testb	rl2
0936 e601          1414 	jpr	eq,L262
0938 a9e0          1415 	inc	r14,#1
                   1416 L262:
093a a9d0          1417 	inc	r13,#1
093c 0b0d  000d    1418 	cp	r13,#13
0940 e1f6          1419 	jpr	lt,L20049
0942 0b0d  000d    1420 	cp	r13,#13
0946 ee02          1421 	jpr	ne,L266
0948 4cd8  0000*   1422 	clrb	_arbuf(r13)
                   1423 L266:
094c 5402  000e*   1424 	ldl	rr2,_stbuf+14
0950 5d02  0016*   1425 	ldl	_arbuf+22,rr2
0954 5404  0016*   1426 	ldl	rr4,_stbuf+22
0958 5d04  000e*   1427 	ldl	_arbuf+14,rr4
095c 600a  0009*   1428 	ldb	rl2,_stbuf+9
0960 6e0a  0012*   1429 	ldb	_arbuf+18,rl2
0964 600b  000b*   1430 	ldb	rl3,_stbuf+11
0968 6e0b  0013*   1431 	ldb	_arbuf+19,rl3
096c 6102  0004*   1432 	ld	r2,_stbuf+4
0970 6f02  0014*   1433 	ld	_arbuf+20,r2
0974 61f7  0000    1434 	ld	r7,~L1(fp)
0978 6106  0000*   1435 	ld	r6,_tf
097c bd5c          1436 	ldk	r5,#12
097e dfd8          1437 	callr	_copyfil
0980 61f7  0000    1438 	ld	r7,~L1(fp)
0984 5f00  0000*   1439 	callr	_close
0988 5cf1  0d01    1440 	ldm	r13,2(fp),#2
098c 0002  
098e a9f5          1441 	inc	fp,#~L2
0990 9e08          1442 	ret
                   1443 	~L1 := 0
                   1444 	~L2 := 6
                   1445 	}	/* _movefil */
                   1446 
                   1447 .data
                   1448 
                   1449 /*~~stats:*/
                   1450 
                   1451 .code
                   1452 
                   1453 _stats::
                   1454 {
0992 abf3          1455 	dec	fp,#~L2
0994 6ffe  0002    1456 	ld	2(fp),r14
0998 6107  0000*   1457 	ld	r7,_file
099c 8366          1458 	sub	r6,r6
099e 5f00  0000*   1459 	callr	_open
09a2 a12e          1460 	ld	r14,r2
09a4 0b02  0000    1461 	cp	r2,#0
09a8 e10e          1462 	jpr	lt,L270
09aa a127          1463 	ld	r7,r2
09ac 2106  0000*   1464 	ld	r6,#_stbuf
09b0 5f00  0000*   1465 	callr	_fstat
09b4 0b02  0000    1466 	cp	r2,#0
09b8 e906          1467 	jpr	ge,L270
09ba a1e7          1468 	ld	r7,r14
09bc 5f00  0000*   1469 	callr	_close
09c0 2102  ffff    1470 	ld	r2,#-1
09c4 e801          1471 	jpr	L10071
                   1472 L270:
09c6 a1e2          1473 	ld	r2,r14
                   1474 L10071:
09c8 61fe  0002    1475 	ld	r14,2(fp)
09cc a9f3          1476 	inc	fp,#~L2
09ce 9e08          1477 	ret
                   1478 	~L1 := 0
                   1479 	~L2 := 4
                   1480 	}	/* _stats */
                   1481 
                   1482 .data
                   1483 
                   1484 /*~~copyfil:*/
                   1485 
                   1486 .code
                   1487 
                   1488 _copyfil::
                   1489 {
09d0 abfb          1490 	dec	fp,#~L2
09d2 5cf9  0d01    1491 	ldm	8(fp),r13,#2
09d6 0008  
09d8 1cf9  0502    1492 	ldm	@fp,r5,#3
09dc a753          1493 	bit	r5,#3
09de e60c          1494 	jpr	eq,L272
09e0 61f7  0002    1495 	ld	r7,~L1+2(fp)
09e4 2106  0000*   1496 	ld	r6,#_arbuf
09e8 2105  001a    1497 	ld	r5,#26
09ec 5f00  0000*   1498 	callr	_write
09f0 0b02  001a    1499 	cp	r2,#26
09f4 e601          1500 	jpr	eq,L272
09f6 de66          1501 	callr	_wrerr
                   1502 L272:
09f8 4df8  0006    1503 	clr	~L1+6(fp)
09fc e837          1504 	jpr	L274
                   1505 L20051:
09fe 210d  0200    1506 	ld	r13,#512
0a02 a1de          1507 	ld	r14,r13
0a04 a1d5          1508 	ld	r5,r13
0a06 b14a          1509 	exts	rr4
0a08 5004  0016*   1510 	cpl	rr4,_arbuf+22
0a0c e20d          1511 	jpr	le,L276
0a0e 610d  0018*   1512 	ld	r13,_arbuf+24
0a12 a1de          1513 	ld	r14,r13
0a14 a7d0          1514 	bit	r13,#0
0a16 e608          1515 	jpr	eq,L276
0a18 67f1  0000    1516 	bit	~L1(fp),#1
0a1c e601          1517 	jpr	eq,L278
0a1e a9e0          1518 	inc	r14,#1
                   1519 L278:
0a20 67f2  0000    1520 	bit	~L1(fp),#2
0a24 e601          1521 	jpr	eq,L276
0a26 a9d0          1522 	inc	r13,#1
                   1523 L276:
0a28 61f7  0004    1524 	ld	r7,~L1+4(fp)
0a2c 2106  0000*   1525 	ld	r6,#_buf
0a30 a1e5          1526 	ld	r5,r14
0a32 5f00  0000*   1527 	callr	_read
0a36 8b2e          1528 	cp	r14,r2
0a38 e602          1529 	jpr	eq,L280
0a3a 69f0  0006    1530 	inc	~L1+6(fp),#1
                   1531 L280:
0a3e 61f2  0000    1532 	ld	r2,~L1(fp)
0a42 0702  0001    1533 	and	r2,#1
0a46 8d24          1534 	test	r2
0a48 ee0a          1535 	jpr	ne,L281
0a4a 61f7  0002    1536 	ld	r7,~L1+2(fp)
0a4e 2106  0000*   1537 	ld	r6,#_buf
0a52 a1d5          1538 	ld	r5,r13
0a54 5f00  0000*   1539 	callr	_write
0a58 8b2d          1540 	cp	r13,r2
0a5a e601          1541 	jpr	eq,L281
0a5c de99          1542 	callr	_wrerr
                   1543 L281:
0a5e 5402  0016*   1544 	ldl	rr2,_arbuf+22
0a62 1202  0000    1545 	subl	rr2,#512
0a66 0200  
0a68 5d02  0016*   1546 	ldl	_arbuf+22,rr2
                   1547 L274:
0a6c 5402  0016*   1548 	ldl	rr2,_arbuf+22
0a70 1002  0000    1549 	cpl	rr2,#0
0a74 0000  
0a76 eac3          1550 	jpr	gt,L20051
0a78 4df4  0006    1551 	test	~L1+6(fp)
0a7c e601          1552 	jpr	eq,L10074
0a7e df4f          1553 	callr	_phserr
                   1554 L10074:
0a80 5cf1  0d01    1555 	ldm	r13,8(fp),#2
0a84 0008  
0a86 a9fb          1556 	inc	fp,#~L2
0a88 9e08          1557 	ret
                   1558 	~L1 := 0
                   1559 	~L2 := 12
                   1560 	}	/* _copyfil */
                   1561 
                   1562 .data
                   1563 
                   1564 /*~~getdir:*/
                   1565 
                   1566 .code
                   1567 
                   1568 _getdir::
                   1569 {
0a8a abf3          1570 	dec	fp,#~L2
0a8c 6ffe  0002    1571 	ld	2(fp),r14
0a90 6107  0000*   1572 	ld	r7,_af
0a94 2106  0000*   1573 	ld	r6,#_arbuf
0a98 2105  001a    1574 	ld	r5,#26
0a9c 5f00  0000*   1575 	callr	_read
0aa0 a12e          1576 	ld	r14,r2
0aa2 0b02  001a    1577 	cp	r2,#26
0aa6 e60d          1578 	jpr	eq,L286
0aa8 4d04  0000*   1579 	test	_tf1nam
0aac e608          1580 	jpr	eq,L287
0aae 610e  0000*   1581 	ld	r14,_tf
0ab2 6102  0000*   1582 	ld	r2,_tf1
0ab6 6f02  0000*   1583 	ld	_tf,r2
0aba 6f0e  0000*   1584 	ld	_tf1,r14
                   1585 L287:
0abe bd21          1586 	ldk	r2,#1
0ac0 e80d          1587 	jpr	L10077
                   1588 L286:
0ac2 bde0          1589 	ldk	r14,#0
                   1590 L20053:
0ac4 60ea  0000*   1591 	ldb	rl2,_arbuf(r14)
0ac8 6eea  0000*   1592 	ldb	_name(r14),rl2
0acc a9e0          1593 	inc	r14,#1
0ace 0b0e  000e    1594 	cp	r14,#14
0ad2 e1f8          1595 	jpr	lt,L20053
0ad4 4d05  0000*   1596 	ld	_file,#_name
0ad8 0000* 
0ada 8322          1597 	sub	r2,r2
                   1598 L10077:
0adc 61fe  0002    1599 	ld	r14,2(fp)
0ae0 a9f3          1600 	inc	fp,#~L2
0ae2 9e08          1601 	ret
                   1602 	~L1 := 0
                   1603 	~L2 := 4
                   1604 	}	/* _getdir */
                   1605 
                   1606 .data
                   1607 
                   1608 /*~~match:*/
                   1609 
                   1610 .code
                   1611 
                   1612 _match::
                   1613 {
0ae4 abf3          1614 	dec	fp,#~L2
0ae6 6ffe  0002    1615 	ld	2(fp),r14
0aea bde0          1616 	ldk	r14,#0
0aec e829          1617 	jpr	L294
                   1618 L20055:
0aee a1e2          1619 	ld	r2,r14
0af0 81e2          1620 	add	r2,r14
0af2 6103  0000*   1621 	ld	r3,_namv
0af6 8123          1622 	add	r3,r2
0af8 0d34          1623 	test	@r3
0afa e621          1624 	jpr	eq,L292
0afc a1e2          1625 	ld	r2,r14
0afe 81e2          1626 	add	r2,r14
0b00 6107  0000*   1627 	ld	r7,_namv
0b04 8127          1628 	add	r7,r2
0b06 2176          1629 	ld	r6,@r7
0b08 a167          1630 	ld	r7,r6
0b0a df74          1631 	callr	_trim
0b0c 6ff2  0000    1632 	ld	~L1(fp),r2
0b10 a127          1633 	ld	r7,r2
0b12 6106  0000*   1634 	ld	r6,_file
0b16 5f00  0000*   1635 	callr	_strcmp
0b1a 8d24          1636 	test	r2
0b1c ee10          1637 	jpr	ne,L292
0b1e a1e2          1638 	ld	r2,r14
0b20 81e2          1639 	add	r2,r14
0b22 6103  0000*   1640 	ld	r3,_namv
0b26 8123          1641 	add	r3,r2
0b28 2132          1642 	ld	r2,@r3
0b2a 6f02  0000*   1643 	ld	_file,r2
0b2e a1e3          1644 	ld	r3,r14
0b30 81e3          1645 	add	r3,r14
0b32 6104  0000*   1646 	ld	r4,_namv
0b36 8134          1647 	add	r4,r3
0b38 0d48          1648 	clr	@r4
0b3a bd21          1649 	ldk	r2,#1
0b3c e805          1650 	jpr	L10080
                   1651 L292:
0b3e a9e0          1652 	inc	r14,#1
                   1653 L294:
0b40 4b0e  0000*   1654 	cp	r14,_namc
0b44 e1d4          1655 	jpr	lt,L20055
0b46 8322          1656 	sub	r2,r2
                   1657 L10080:
0b48 61fe  0002    1658 	ld	r14,2(fp)
0b4c a9f3          1659 	inc	fp,#~L2
0b4e 9e08          1660 	ret
                   1661 	~L1 := 0
                   1662 	~L2 := 4
                   1663 	}	/* _match */
                   1664 
                   1665 .data
                   1666 
                   1667 L306:
02ac 25  73  3a    1668 .byte	%25,%73,%3a,%20,%63,%61,%6e,%6e
02af 20  63  61  
02b2 6e  6e  
02b4 6f  74  20    1669 .byte	%6f,%74,%20,%63,%72,%65,%61,%74
02b7 63  72  65  
02ba 61  74  
02bc 65  20  73    1670 .byte	%65,%20,%73,%65,%63,%6f,%6e,%64
02bf 65  63  6f  
02c2 6e  64  
02c4 20  74  65    1671 .byte	%20,%74,%65,%6d,%70,%a,%0
02c7 6d  70  0a  
02ca 00  
                   1672 .even
                   1673 /*~~bamatch:*/
                   1674 
                   1675 .code
                   1676 
                   1677 _bamatch::
                   1678 {
0b50 abf3          1679 	dec	fp,#~L2
0b52 6ffe  0002    1680 	ld	2(fp),r14
0b56 6102  0000*   1681 	ld	r2,_bastate
0b5a e839          1682 	jpr	L300
                   1683 L301:
0b5c 6107  0000*   1684 	ld	r7,_file
0b60 6106  0000*   1685 	ld	r6,_ponam
0b64 5f00  0000*   1686 	callr	_strcmp
0b68 8d24          1687 	test	r2
0b6a ee37          1688 	jpr	ne,L10083
0b6c 4d05  0000*   1689 	ld	_bastate,#2
0b70 0002  
0b72 4c04  0000*   1690 	testb	_flg
0b76 ee31          1691 	jpr	ne,L10083
                   1692 L304:
0b78 4d08  0000*   1693 	clr	_bastate
0b7c 6107  0058'   1694 	ld	r7,_tmp1
0b80 5f00  0000*   1695 	callr	_mktemp
0b84 6f02  0000*   1696 	ld	_tf1nam,r2
0b88 a127          1697 	ld	r7,r2
0b8a 2106  0180    1698 	ld	r6,#384
0b8e 5f00  0000*   1699 	callr	_creat
0b92 6ff2  0000    1700 	ld	~L1(fp),r2
0b96 a127          1701 	ld	r7,r2
0b98 5f00  0000*   1702 	callr	_close
0b9c 6107  0000*   1703 	ld	r7,_tf1nam
0ba0 bd62          1704 	ldk	r6,#2
0ba2 5f00  0000*   1705 	callr	_open
0ba6 a12e          1706 	ld	r14,r2
0ba8 0b02  0000    1707 	cp	r2,#0
0bac e909          1708 	jpr	ge,L305
0bae 2107  0010*   1709 	ld	r7,#__iob+16
0bb2 2106  02ac'   1710 	ld	r6,#L306
0bb6 6105  0000*   1711 	ld	r5,_av0
0bba 5f00  0000*   1712 	callr	_fprintf
0bbe e80d          1713 	jpr	L10083
                   1714 L305:
0bc0 6102  0000*   1715 	ld	r2,_tf
0bc4 6f02  0000*   1716 	ld	_tf1,r2
0bc8 6f0e  0000*   1717 	ld	_tf,r14
0bcc e806          1718 	jpr	L10083
                   1719 L300:
0bce 0b02  0001    1720 	cp	r2,#1
0bd2 e6c4          1721 	jpr	eq,L301
0bd4 0b02  0002    1722 	cp	r2,#2
0bd8 e6cf          1723 	jpr	eq,L304
                   1724 L10083:
0bda 61fe  0002    1725 	ld	r14,2(fp)
0bde a9f3          1726 	inc	fp,#~L2
0be0 9e08          1727 	ret
                   1728 	~L1 := 0
                   1729 	~L2 := 4
                   1730 	}	/* _bamatch */
                   1731 
                   1732 .data
                   1733 
                   1734 L308:
02cc 25  73  3a    1735 .byte	%25,%73,%3a,%20,%70,%68,%61,%73
02cf 20  70  68  
02d2 61  73  
02d4 65  20  65    1736 .byte	%65,%20,%65,%72,%72,%6f,%72,%20
02d7 72  72  6f  
02da 72  20  
02dc 6f  6e  20    1737 .byte	%6f,%6e,%20,%25,%73,%a,%0
02df 25  73  0a  
02e2 00  
                   1738 .even
                   1739 /*~~phserr:*/
                   1740 
                   1741 .code
                   1742 
                   1743 _phserr::
                   1744 {
0be2 2107  0010*   1745 	ld	r7,#__iob+16
0be6 2106  02cc'   1746 	ld	r6,#L308
0bea 6105  0000*   1747 	ld	r5,_av0
0bee 6104  0000*   1748 	ld	r4,_file
0bf2 5f00  0000*   1749 	callr	_fprintf
0bf6 9e08          1750 	ret
                   1751 	~L1 := 0
                   1752 	~L2 := 0
                   1753 	}	/* _phserr */
                   1754 
                   1755 .data
                   1756 
                   1757 L312:
02e4 25  63  20    1758 .byte	%25,%63,%20,%2d,%20,%25,%73,%a
02e7 2d  20  25  
02ea 73  0a  
02ec 00            1759 .byte	%0
                   1760 .even
                   1761 /*~~mesg:*/
                   1762 
                   1763 .code
                   1764 
                   1765 _mesg::
                   1766 {
0bf8 abf1          1767 	dec	fp,#~L2
0bfa 2ff7          1768 	ld	@fp,r7
0bfc 4c04  0015*   1769 	testb	_flg+21
0c00 e60f          1770 	jpr	eq,L10089
0c02 0b07  0063    1771 	cp	r7,#99
0c06 ee04          1772 	jpr	ne,L10092
0c08 4c01  0015*   1773 	cpb	_flg+21,#1
0c0c 0101  
0c0e e208          1774 	jpr	le,L10089
                   1775 L10092:
0c10 2107  02e4'   1776 	ld	r7,#L312
0c14 61f6  0000    1777 	ld	r6,~L1(fp)
0c18 6105  0000*   1778 	ld	r5,_file
0c1c 5f00  0000*   1779 	callr	_printf
                   1780 L10089:
0c20 a9f1          1781 	inc	fp,#~L2
0c22 9e08          1782 	ret
                   1783 	~L1 := 0
                   1784 	~L2 := 2
                   1785 	}	/* _mesg */
                   1786 
                   1787 .data
                   1788 
                   1789 /*~~trim:*/
                   1790 
                   1791 .code
                   1792 
                   1793 _trim::
                   1794 {
0c24 abf7          1795 	dec	fp,#~L2
0c26 2ff7          1796 	ld	@fp,r7
0c28 a171          1797 	ld	r1,r7
0c2a e801          1798 	jpr	L316
                   1799 L20057:
0c2c a910          1800 	inc	r1,#1
                   1801 L316:
0c2e 0c14          1802 	testb	@r1
0c30 eefd          1803 	jpr	ne,L20057
0c32 e806          1804 	jpr	L317
                   1805 L20059:
0c34 ab10          1806 	dec	r1,#1
0c36 201a          1807 	ldb	rl2,@r1
0c38 0a0a  2f2f    1808 	cpb	rl2,#47
0c3c ee04          1809 	jpr	ne,L318
0c3e 0c18          1810 	clrb	@r1
                   1811 L317:
0c40 4bf1  0000    1812 	cp	r1,~L1(fp)
0c44 ebf7          1813 	jpr	ugt,L20059
                   1814 L318:
0c46 61f3  0000    1815 	ld	r3,~L1(fp)
0c4a a131          1816 	ld	r1,r3
0c4c e807          1817 	jpr	L322
                   1818 L20061:
0c4e 0c11  2f2f    1819 	cpb	@r1,#47
0c52 ee03          1820 	jpr	ne,L320
0c54 7612  0001    1821 	lda	r2,1(r1)
0c58 a123          1822 	ld	r3,r2
                   1823 L320:
0c5a a910          1824 	inc	r1,#1
                   1825 L322:
0c5c 0c14          1826 	testb	@r1
0c5e eef7          1827 	jpr	ne,L20061
0c60 a132          1828 	ld	r2,r3
0c62 a9f7          1829 	inc	fp,#~L2
0c64 9e08          1830 	ret
                   1831 	~L1 := 0
                   1832 	~L2 := 8
                   1833 	}	/* _trim */
                   1834 
                   1835 .data
                   1836 
                   1837 L326:
02ee 25  33  64    1838 .byte	%25,%33,%64,%2f,%25,%31,%64,%0
02f1 2f  25  31  
02f4 64  00  
                   1839 .even
                   1840 L327:
02f6 25  37  6c    1841 .byte	%25,%37,%6c,%64,%0
02f9 64  00  
                   1842 .even
                   1843 L328:
02fc 20  25  2d    1844 .byte	%20,%25,%2d,%31,%32,%2e,%31,%32
02ff 31  32  2e  
0302 31  32  
0304 73  20  25    1845 .byte	%73,%20,%25,%2d,%34,%2e,%34,%73
0307 2d  34  2e  
030a 34  73  
030c 20  00        1846 .byte	%20,%0
                   1847 .even
                   1848 /*~~longt:*/
                   1849 
                   1850 .code
                   1851 
                   1852 _longt::
                   1853 {
0c66 abf3          1854 	dec	fp,#~L2
0c68 6ffe  0002    1855 	ld	2(fp),r14
0c6c dfdf          1856 	callr	_pmode
0c6e 2107  02ee'   1857 	ld	r7,#L326
0c72 600e  0012*   1858 	ldb	rl6,_arbuf+18
0c76 b160          1859 	extsb	r6
0c78 600d  0013*   1860 	ldb	rl5,_arbuf+19
0c7c b150          1861 	extsb	r5
0c7e 5f00  0000*   1862 	callr	_printf
0c82 2107  02f6'   1863 	ld	r7,#L327
0c86 5404  0016*   1864 	ldl	rr4,_arbuf+22
0c8a 5f00  0000*   1865 	callr	_printf
0c8e 2107  000e*   1866 	ld	r7,#_arbuf+14
0c92 5f00  0000*   1867 	callr	_ctime
0c96 a12e          1868 	ld	r14,r2
0c98 76e6  0004    1869 	lda	r6,4(r14)
0c9c 76e5  0014    1870 	lda	r5,20(r14)
0ca0 2107  02fc'   1871 	ld	r7,#L328
0ca4 5f00  0000*   1872 	callr	_printf
0ca8 61fe  0002    1873 	ld	r14,2(fp)
0cac a9f3          1874 	inc	fp,#~L2
0cae 9e08          1875 	ret
                   1876 	~L1 := 0
                   1877 	~L2 := 4
                   1878 	}	/* _longt */
                   1879 
                   1880 .data
                   1881 
                   1882 .even
                   1883 _m1::
030e 0001          1884 .word	%1
0310 0100          1885 .word	%100
0312 0072          1886 .word	%72
0314 002d          1887 .word	%2d
                   1888 .even
                   1889 _m2::
0316 0001          1890 .word	%1
0318 0080          1891 .word	%80
031a 0077          1892 .word	%77
031c 002d          1893 .word	%2d
                   1894 .even
                   1895 _m3::
031e 0002          1896 .word	%2
0320 0800          1897 .word	%800
0322 0073          1898 .word	%73
0324 0040          1899 .word	%40
0326 0078          1900 .word	%78
0328 002d          1901 .word	%2d
                   1902 .even
                   1903 _m4::
032a 0001          1904 .word	%1
032c 0020          1905 .word	%20
032e 0072          1906 .word	%72
0330 002d          1907 .word	%2d
                   1908 .even
                   1909 _m5::
0332 0001          1910 .word	%1
0334 0010          1911 .word	%10
0336 0077          1912 .word	%77
0338 002d          1913 .word	%2d
                   1914 .even
                   1915 _m6::
033a 0002          1916 .word	%2
033c 0400          1917 .word	%400
033e 0073          1918 .word	%73
0340 0008          1919 .word	%8
0342 0078          1920 .word	%78
0344 002d          1921 .word	%2d
                   1922 .even
                   1923 _m7::
0346 0001          1924 .word	%1
0348 0004          1925 .word	%4
034a 0072          1926 .word	%72
034c 002d          1927 .word	%2d
                   1928 .even
                   1929 _m8::
034e 0001          1930 .word	%1
0350 0002          1931 .word	%2
0352 0077          1932 .word	%77
0354 002d          1933 .word	%2d
                   1934 .even
                   1935 _m9::
0356 0002          1936 .word	%2
0358 0200          1937 .word	%200
035a 0074          1938 .word	%74
035c 0001          1939 .word	%1
035e 0078          1940 .word	%78
0360 002d          1941 .word	%2d
                   1942 .even
                   1943 _m::
0362 030e'         1944 .addr	_m1
0364 0316'         1945 .addr	_m2
0366 031e'         1946 .addr	_m3
0368 032a'         1947 .addr	_m4
036a 0332'         1948 .addr	_m5
036c 033a'         1949 .addr	_m6
036e 0346'         1950 .addr	_m7
0370 034e'         1951 .addr	_m8
0372 0356'         1952 .addr	_m9
                   1953 /*~~pmode:*/
                   1954 
                   1955 .code
                   1956 
                   1957 _pmode::
                   1958 {
0cb0 abf1          1959 	dec	fp,#~L2
0cb2 2ffe          1960 	ld	@fp,r14
0cb4 210e  0362'   1961 	ld	r14,#_m
0cb8 e805          1962 	jpr	L342
                   1963 L20063:
0cba a1e7          1964 	ld	r7,r14
0cbc a9e1          1965 	inc	r14,#2
0cbe 2176          1966 	ld	r6,@r7
0cc0 a167          1967 	ld	r7,r6
0cc2 dffa          1968 	callr	_select
                   1969 L342:
0cc4 0b0e  0374'   1970 	cp	r14,#_m+18
0cc8 e7f8          1971 	jpr	ult,L20063
0cca 21fe          1972 	ld	r14,@fp
0ccc a9f1          1973 	inc	fp,#~L2
0cce 9e08          1974 	ret
                   1975 	~L1 := 0
                   1976 	~L2 := 2
                   1977 	}	/* _pmode */
                   1978 
                   1979 .data
                   1980 
                   1981 /*~~select:*/
                   1982 
                   1983 .code
                   1984 
                   1985 _select::
                   1986 {
0cd0 abf5          1987 	dec	fp,#~L2
0cd2 5cf9  0d01    1988 	ldm	2(fp),r13,#2
0cd6 0002  
0cd8 2ff7          1989 	ld	@fp,r7
0cda a17d          1990 	ld	r13,r7
0cdc 21de          1991 	ld	r14,@r13
0cde e80d          1992 	jpr	L20064
                   1993 L20068:
0ce0 21d7          1994 	ld	r7,@r13
0ce2 2106  0008*   1995 	ld	r6,#__iob+8
0ce6 5f00  0000*   1996 	callr	__flsbuf
0cea e81b          1997 	jpr	L10102
                   1998 L20066:
0cec a1d2          1999 	ld	r2,r13
0cee a9d1          2000 	inc	r13,#2
0cf0 2123          2001 	ld	r3,@r2
0cf2 4703  0014*   2002 	and	r3,_arbuf+20
0cf6 8d34          2003 	test	r3
0cf8 ee05          2004 	jpr	ne,L346
                   2005 L20064:
0cfa a9d1          2006 	inc	r13,#2
0cfc abe0          2007 	dec	r14,#1
0cfe 0b0e  0000    2008 	cp	r14,#0
0d02 e9f4          2009 	jpr	ge,L20066
                   2010 L346:
0d04 6b00  000a*   2011 	dec	__iob+10,#1
0d08 4d01  000a*   2012 	cp	__iob+10,#0
0d0c 0000  
0d0e e1e8          2013 	jpr	lt,L20068
0d10 6102  0008*   2014 	ld	r2,__iob+8
0d14 6900  0008*   2015 	inc	__iob+8,#1
0d18 60db  0001    2016 	ldb	rl3,1(r13)
0d1c 2e2b          2017 	ldb	@r2,rl3
0d1e 8233          2018 	subb	rh3,rh3
0d20 a132          2019 	ld	r2,r3
                   2020 L10102:
0d22 5cf1  0d01    2021 	ldm	r13,2(fp),#2
0d26 0002  
0d28 a9f5          2022 	inc	fp,#~L2
0d2a 9e08          2023 	ret
                   2024 	~L1 := 0
                   2025 	~L2 := 6
                   2026 	}	/* _select */
                   2027 
                   2028 .data
                   2029 
                   2030 L350:
0374 61  72  20    2031 .byte	%61,%72,%20,%77,%72,%69,%74,%65
0377 77  72  69  
037a 74  65  
037c 20  65  72    2032 .byte	%20,%65,%72,%72,%6f,%72,%0
037f 72  6f  72  
0382 00  
                   2033 .even
                   2034 /*~~wrerr:*/
                   2035 
                   2036 .code
                   2037 
                   2038 _wrerr::
                   2039 {
0d2c 2107  0374'   2040 	ld	r7,#L350
0d30 5f00  0000*   2041 	callr	_perror
0d34 bd71          2042 	ldk	r7,#1
0d36 d33a          2043 	callr	_done
0d38 9e08          2044 	ret
                   2045 	~L1 := 0
                   2046 	~L2 := 0
                   2047 	}	/* _wrerr */
                   2048 
                   2049 .data
                   2050 
