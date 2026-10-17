LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 __sctab::
0000 00               5 .byte	%0
0001 00               6 .byte	%0
0002 00               7 .byte	%0
0003 00               8 .byte	%0
0004 00               9 .byte	%0
0005 00              10 .byte	%0
0006 00              11 .byte	%0
0007 00              12 .byte	%0
0008 00              13 .byte	%0
0009 01              14 .byte	%1
000a 01              15 .byte	%1
000b 00              16 .byte	%0
000c 00              17 .byte	%0
000d 00              18 .byte	%0
000e 00              19 .byte	%0
000f 00              20 .byte	%0
0010 00              21 .byte	%0
0011 00              22 .byte	%0
0012 00              23 .byte	%0
0013 00              24 .byte	%0
0014 00              25 .byte	%0
0015 00              26 .byte	%0
0016 00              27 .byte	%0
0017 00              28 .byte	%0
0018 00              29 .byte	%0
0019 00              30 .byte	%0
001a 00              31 .byte	%0
001b 00              32 .byte	%0
001c 00              33 .byte	%0
001d 00              34 .byte	%0
001e 00              35 .byte	%0
001f 00              36 .byte	%0
0020 01              37 .byte	%1
0021 00              38 .byte	%0
0022 00              39 .byte	%0
0023 00              40 .byte	%0
0024 00              41 .byte	%0
0025 00              42 .byte	%0
0026 00              43 .byte	%0
0027 00              44 .byte	%0
0028 00              45 .byte	%0
0029 00              46 .byte	%0
002a 00              47 .byte	%0
002b 00              48 .byte	%0
002c 00              49 .byte	%0
002d 00              50 .byte	%0
002e 00              51 .byte	%0
002f 00              52 .byte	%0
0030 00              53 .byte	%0
0031 00              54 .byte	%0
0032 00              55 .byte	%0
0033 00              56 .byte	%0
0034 00              57 .byte	%0
0035 00              58 .byte	%0
0036 00              59 .byte	%0
0037 00              60 .byte	%0
0038 00              61 .byte	%0
0039 00              62 .byte	%0
003a 00              63 .byte	%0
003b 00              64 .byte	%0
003c 00              65 .byte	%0
003d 00              66 .byte	%0
003e 00              67 .byte	%0
003f 00              68 .byte	%0
                     69 .=.+64
                     70 .even
                     71 L59:
0080 0000            72 .word	0
0082 0009            73 .word	9
0084 000a            74 .word	10
0086 0020            75 .word	32
0088 0025            76 .word	37
                     77 .even
                     78 /*~~_doscan:*/
                     79 
                     80 .code
                     81 
                     82 __doscan::
                     83 {
                     84 	.data
                     85 	.even
                     86 L60:
008a 01ca'           87 	.word	L20004
008c 011c'           88 	.word	L52
008e 011c'           89 	.word	L52
0090 011c'           90 	.word	L52
0092 0028'           91 	.word	L29
                     92 	.code
0000 030f  001a      93 	sub	fp,#~L2
0004 5cf9  0c02      94 	ldm	20(fp),r12,#3
0008 0014  
000a 6ff7  0004      95 	ld	~L1+4(fp),r7
000e a15d            96 	ld	r13,r5
0010 a16e            97 	ld	r14,r6
0012 4df8  0006      98 	clr	~L1+6(fp)
0016 4df8  000e      99 	clr	~L1+14(fp)
001a 5e08  016c'    100 	jpr	L25
                    101 L20003:
001e 8144           102 	add	r4,r4
0020 8d42           103 	neg	r4
0022 6142  0092'    104 	ld	r2,L60+8(r4)
0026 1e28           105 	jp	@r2
                    106 L29:
0028 a1e2           107 	ld	r2,r14
002a a9e0           108 	inc	r14,#1
002c 202b           109 	ldb	rl3,@r2
002e b130           110 	extsb	r3
0030 a13c           111 	ld	r12,r3
0032 0b03  0025     112 	cp	r3,#37
0036 5e06  0186'    113 	jpr	eq,L31
003a 4df8  000c     114 	clr	~L1+12(fp)
003e 0b03  002a     115 	cp	r3,#42
0042 e604           116 	jpr	eq,L32
0044 6ffd  000c     117 	ld	~L1+12(fp),r13
0048 a9d1           118 	inc	r13,#2
004a e804           119 	jpr	L33
                    120 L32:
004c 20ea           121 	ldb	rl2,@r14
004e b120           122 	extsb	r2
0050 a12c           123 	ld	r12,r2
0052 a9e0           124 	inc	r14,#1
                    125 L33:
0054 4df8  0008     126 	clr	~L1+8(fp)
0058 4df5  0010     127 	ld	~L1+16(fp),#1
005c 0001  
005e e80d           128 	jpr	L34
                    129 L20001:
0060 61f3  0008     130 	ld	r3,~L1+8(fp)
0064 1902  000a     131 	mult	rr2,#10
0068 81c3           132 	add	r3,r12
006a 0303  0030     133 	sub	r3,#48
006e 6ff3  0008     134 	ld	~L1+8(fp),r3
0072 20ea           135 	ldb	rl2,@r14
0074 b120           136 	extsb	r2
0076 a12c           137 	ld	r12,r2
0078 a9e0           138 	inc	r14,#1
                    139 L34:
007a 60ca  0001*    140 	ldb	rl2,__ctype+1(r12)
007e b120           141 	extsb	r2
0080 a722           142 	bit	r2,#2
0082 eeee           143 	jpr	ne,L20001
0084 4df4  0008     144 	test	~L1+8(fp)
0088 ee03           145 	jpr	ne,L36
008a 4df5  0008     146 	ld	~L1+8(fp),#30000
008e 7530  
                    147 L36:
0090 0b0c  006c     148 	cp	r12,#108
0094 ee08           149 	jpr	ne,L37
0096 20ea           150 	ldb	rl2,@r14
0098 b120           151 	extsb	r2
009a a12c           152 	ld	r12,r2
009c a9e0           153 	inc	r14,#1
009e 4df5  0010     154 	ld	~L1+16(fp),#2
00a2 0002  
00a4 e810           155 	jpr	L38
                    156 L37:
00a6 0b0c  0068     157 	cp	r12,#104
00aa ee07           158 	jpr	ne,L39
00ac 4df8  0010     159 	clr	~L1+16(fp)
00b0 20ea           160 	ldb	rl2,@r14
00b2 b120           161 	extsb	r2
00b4 a12c           162 	ld	r12,r2
00b6 a9e0           163 	inc	r14,#1
00b8 e806           164 	jpr	L38
                    165 L39:
00ba 0b0c  005b     166 	cp	r12,#91
00be ee03           167 	jpr	ne,L38
00c0 a1e7           168 	ld	r7,r14
00c2 dd3e           169 	callr	__getccl
00c4 a12e           170 	ld	r14,r2
                    171 L38:
00c6 60ca  0001*    172 	ldb	rl2,__ctype+1(r12)
00ca b120           173 	extsb	r2
00cc a720           174 	bit	r2,#0
00ce e607           175 	jpr	eq,L42
00d0 a1c7           176 	ld	r7,r12
00d2 5f00  0000*    177 	callr	_tolower
00d6 a12c           178 	ld	r12,r2
00d8 4df5  0010     179 	ld	~L1+16(fp),#2
00dc 0002  
                    180 L42:
00de 8dc4           181 	test	r12
00e0 ee03           182 	jpr	ne,L44
                    183 L20005:
00e2 2102  ffff     184 	ld	r2,#-1
00e6 e873           185 	jpr	L10000
                    186 L44:
00e8 76f2  000e     187 	lda	r2,~L1+14(fp)
00ec a1c6           188 	ld	r6,r12
00ee 61f5  0008     189 	ld	r5,~L1+8(fp)
00f2 61f4  0010     190 	ld	r4,~L1+16(fp)
00f6 61f3  0004     191 	ld	r3,~L1+4(fp)
00fa 61f7  000c     192 	ld	r7,~L1+12(fp)
00fe df93           193 	callr	__innum
0100 8d24           194 	test	r2
0102 e605           195 	jpr	eq,L46
0104 4df4  000c     196 	test	~L1+12(fp)
0108 e602           197 	jpr	eq,L46
010a 69f0  0006     198 	inc	~L1+6(fp),#1
                    199 L46:
010e 4df4  000e     200 	test	~L1+14(fp)
0112 e62c           201 	jpr	eq,L25
0114 4df4  0006     202 	test	~L1+6(fp)
0118 e6e4           203 	jpr	eq,L20005
011a e857           204 	jpr	L20004
                    205 L52:
011c 61f2  0004     206 	ld	r2,~L1+4(fp)
0120 6b20  0002     207 	dec	2(r2),#1
0124 6123  0002     208 	ld	r3,2(r2)
0128 0b03  0000     209 	cp	r3,#0
012c e905           210 	jpr	ge,L10006
012e 61f7  0004     211 	ld	r7,~L1+4(fp)
0132 5f00  0000*    212 	callr	__filbuf
0136 e807           213 	jpr	L10007
                    214 L10006:
0138 61f2  0004     215 	ld	r2,~L1+4(fp)
013c 2920           216 	inc	@r2,#1
013e 2123           217 	ld	r3,@r2
0140 ab30           218 	dec	r3,#1
0142 203a           219 	ldb	rl2,@r3
0144 8222           220 	subb	rh2,rh2
                    221 L10007:
0146 6ff2  000a     222 	ld	~L1+10(fp),r2
014a 0b02  0020     223 	cp	r2,#32
014e e6e6           224 	jpr	eq,L52
0150 0b02  0009     225 	cp	r2,#9
0154 e6e3           226 	jpr	eq,L52
0156 0b02  000a     227 	cp	r2,#10
015a e6e0           228 	jpr	eq,L52
015c 0b02  ffff     229 	cp	r2,#-1
0160 e605           230 	jpr	eq,L25
0162 a127           231 	ld	r7,r2
0164 61f6  0004     232 	ld	r6,~L1+4(fp)
0168 5f00  0000*    233 	callr	_ungetc
                    234 L25:
016c 20eb           235 	ldb	rl3,@r14
016e b130           236 	extsb	r3
0170 a13c           237 	ld	r12,r3
0172 a132           238 	ld	r2,r3
0174 a9e0           239 	inc	r14,#1
0176 2103  0080'    240 	ld	r3,#L59
017a 2104  0005     241 	ld	r4,#5
017e bb34  0426     242 	cpir	r2,@r3,r4,eq
0182 5e06  001e'    243 	jpr	eq,L20003
                    244 L31:
0186 61f2  0004     245 	ld	r2,~L1+4(fp)
018a 6b20  0002     246 	dec	2(r2),#1
018e 6123  0002     247 	ld	r3,2(r2)
0192 0b03  0000     248 	cp	r3,#0
0196 e905           249 	jpr	ge,L10008
0198 61f7  0004     250 	ld	r7,~L1+4(fp)
019c 5f00  0000*    251 	callr	__filbuf
01a0 e807           252 	jpr	L10009
                    253 L10008:
01a2 61f2  0004     254 	ld	r2,~L1+4(fp)
01a6 2920           255 	inc	@r2,#1
01a8 2123           256 	ld	r3,@r2
01aa ab30           257 	dec	r3,#1
01ac 203a           258 	ldb	rl2,@r3
01ae 8222           259 	subb	rh2,rh2
                    260 L10009:
01b0 6ff2  000a     261 	ld	~L1+10(fp),r2
01b4 4bfc  000a     262 	cp	r12,~L1+10(fp)
01b8 e6d9           263 	jpr	eq,L25
01ba 0b02  ffff     264 	cp	r2,#-1
01be e691           265 	jpr	eq,L20005
01c0 a127           266 	ld	r7,r2
01c2 61f6  0004     267 	ld	r6,~L1+4(fp)
01c6 5f00  0000*    268 	callr	_ungetc
                    269 L20004:
01ca 61f2  0006     270 	ld	r2,~L1+6(fp)
                    271 L10000:
01ce 5cf1  0c02     272 	ldm	r12,20(fp),#3
01d2 0014  
01d4 010f  001a     273 	add	fp,#~L2
01d8 9e08           274 	ret
                    275 	~L1 := 0
                    276 	~L2 := 26
                    277 	}	/* __doscan */
                    278 
                    279 .data
                    280 
                    281 .even
                    282 /*~~_innum:*/
                    283 
                    284 .code
                    285 
                    286 __innum::
                    287 {
                    288 	.data
                    289 	.even
                    290 L105:
0094 04f8'          291 	.word	L101
0096 04f8'          292 	.word	L101
0098 0506'          293 	.word	L103
009a 04e0'          294 	.word	L96
009c 04e0'          295 	.word	L96
009e 04e0'          296 	.word	L96
00a0 04e0'          297 	.word	L96
00a2 04e0'          298 	.word	L96
00a4 04e0'          299 	.word	L96
00a6 04e0'          300 	.word	L96
00a8 04e0'          301 	.word	L96
00aa 04e0'          302 	.word	L96
00ac 04e0'          303 	.word	L96
00ae 04e0'          304 	.word	L96
00b0 04e0'          305 	.word	L96
00b2 04e0'          306 	.word	L96
00b4 04ce'          307 	.word	L99
00b6 04ce'          308 	.word	L99
00b8 04e4'          309 	.word	L100
                    310 	.code
01da 030f  0062     311 	sub	fp,#~L2
01de 5cf9  0c02     312 	ldm	92(fp),r12,#3
01e2 005c  
01e4 1cf9  0205     313 	ldm	@fp,r2,#6
01e8 0b06  0063     314 	cp	r6,#99
01ec e607           315 	jpr	eq,L10013
01ee 0b06  0073     316 	cp	r6,#115
01f2 e604           317 	jpr	eq,L10013
01f4 4df1  0008     318 	cp	~L1+8(fp),#91
01f8 005b  
01fa ee14           319 	jpr	ne,L63
                    320 L10013:
01fc 4df4  000a     321 	test	~L1+10(fp)
0200 e605           322 	jpr	eq,L10014
0202 61f7  000a     323 	ld	r7,~L1+10(fp)
0206 2176           324 	ld	r6,@r7
0208 a167           325 	ld	r7,r6
020a e801           326 	jpr	L10015
                    327 L10014:
020c 8377           328 	sub	r7,r7
                    329 L10015:
020e 61f6  0008     330 	ld	r6,~L1+8(fp)
0212 61f5  0006     331 	ld	r5,~L1+6(fp)
0216 61f4  0002     332 	ld	r4,~L1+2(fp)
021a 61f3  0000     333 	ld	r3,~L1(fp)
021e de80           334 	callr	__instr
0220 5e08  0514'    335 	jpr	L10010
                    336 L63:
0224 9222           337 	subl	rr2,rr2
0226 5df2  0056     338 	ldl	~L1+86(fp),rr2
022a 4df8  0054     339 	clr	~L1+84(fp)
022e 4df8  004e     340 	clr	~L1+78(fp)
0232 4df1  0008     341 	cp	~L1+8(fp),#101
0236 0065  
0238 e604           342 	jpr	eq,L10016
023a 4df1  0008     343 	cp	~L1+8(fp),#102
023e 0066  
0240 ee03           344 	jpr	ne,L65
                    345 L10016:
0242 4df5  004e     346 	ld	~L1+78(fp),#1
0246 0001  
                    347 L65:
0248 bdca           348 	ldk	r12,#10
024a 4df1  0008     349 	cp	~L1+8(fp),#111
024e 006f  
0250 ee02           350 	jpr	ne,L66
0252 bdc8           351 	ldk	r12,#8
0254 e806           352 	jpr	L67
                    353 L66:
0256 4df1  0008     354 	cp	~L1+8(fp),#120
025a 0078  
025c ee02           355 	jpr	ne,L67
025e 210c  0010     356 	ld	r12,#16
                    357 L67:
0262 76fe  000c     358 	lda	r14,~L1+12(fp)
0266 4df8  004c     359 	clr	~L1+76(fp)
026a 4df8  0050     360 	clr	~L1+80(fp)
                    361 L69:
026e 61f2  0002     362 	ld	r2,~L1+2(fp)
0272 6b20  0002     363 	dec	2(r2),#1
0276 6123  0002     364 	ld	r3,2(r2)
027a 0b03  0000     365 	cp	r3,#0
027e e905           366 	jpr	ge,L10018
0280 61f7  0002     367 	ld	r7,~L1+2(fp)
0284 5f00  0000*    368 	callr	__filbuf
0288 e807           369 	jpr	L10019
                    370 L10018:
028a 61f2  0002     371 	ld	r2,~L1+2(fp)
028e 2920           372 	inc	@r2,#1
0290 2123           373 	ld	r3,@r2
0292 ab30           374 	dec	r3,#1
0294 203a           375 	ldb	rl2,@r3
0296 8222           376 	subb	rh2,rh2
                    377 L10019:
0298 a12d           378 	ld	r13,r2
029a 0b02  0020     379 	cp	r2,#32
029e e6e7           380 	jpr	eq,L69
02a0 0b02  0009     381 	cp	r2,#9
02a4 e6e4           382 	jpr	eq,L69
02a6 0b02  000a     383 	cp	r2,#10
02aa e6e1           384 	jpr	eq,L69
02ac 0b02  002d     385 	cp	r2,#45
02b0 ee1d           386 	jpr	ne,L71
02b2 69f0  0050     387 	inc	~L1+80(fp),#1
02b6 2eea           388 	ldb	@r14,rl2
02b8 a9e0           389 	inc	r14,#1
02ba 61f2  0002     390 	ld	r2,~L1+2(fp)
02be 6b20  0002     391 	dec	2(r2),#1
02c2 6123  0002     392 	ld	r3,2(r2)
02c6 0b03  0000     393 	cp	r3,#0
02ca e905           394 	jpr	ge,L10020
02cc 61f7  0002     395 	ld	r7,~L1+2(fp)
02d0 5f00  0000*    396 	callr	__filbuf
02d4 e807           397 	jpr	L10021
                    398 L10020:
02d6 61f2  0002     399 	ld	r2,~L1+2(fp)
02da 2920           400 	inc	@r2,#1
02dc 2123           401 	ld	r3,@r2
02de ab30           402 	dec	r3,#1
02e0 203a           403 	ldb	rl2,@r3
02e2 8222           404 	subb	rh2,rh2
                    405 L10021:
02e4 a12d           406 	ld	r13,r2
02e6 6bf0  0006     407 	dec	~L1+6(fp),#1
02ea e876           408 	jpr	L76
                    409 L71:
02ec 0b0d  002b     410 	cp	r13,#43
02f0 ee73           411 	jpr	ne,L76
02f2 6bf0  0006     412 	dec	~L1+6(fp),#1
02f6 61f2  0002     413 	ld	r2,~L1+2(fp)
02fa 6b20  0002     414 	dec	2(r2),#1
02fe 6123  0002     415 	ld	r3,2(r2)
0302 0b03  0000     416 	cp	r3,#0
0306 e163           417 	jpr	lt,L20017
                    418 L10022:
0308 61f2  0002     419 	ld	r2,~L1+2(fp)
030c 2920           420 	inc	@r2,#1
030e 2123           421 	ld	r3,@r2
0310 ab30           422 	dec	r3,#1
0312 203a           423 	ldb	rl2,@r3
0314 8222           424 	subb	rh2,rh2
0316 e85f           425 	jpr	L10023
                    426 L20015:
0318 0b0d  0061     427 	cp	r13,#97
031c e103           428 	jpr	lt,L10026
031e 0b0d  0066     429 	cp	r13,#102
0322 e206           430 	jpr	le,L10024
                    431 L10026:
0324 0b0d  0041     432 	cp	r13,#65
0328 e165           433 	jpr	lt,L77
032a 0b0d  0046     434 	cp	r13,#70
032e ea62           435 	jpr	gt,L77
                    436 L10024:
0330 69f0  0054     437 	inc	~L1+84(fp),#1
0334 0b0c  0008     438 	cp	r12,#8
0338 ee05           439 	jpr	ne,L78
033a 54f2  0056     440 	ldl	rr2,~L1+86(fp)
033e b32d  0003     441 	slal	rr2,#3
0342 e810           442 	jpr	L20006
                    443 L78:
0344 0b0c  000a     444 	cp	r12,#10
0348 ee09           445 	jpr	ne,L80
034a 54f2  0056     446 	ldl	rr2,~L1+86(fp)
034e b32d  0002     447 	slal	rr2,#2
0352 56f2  0056     448 	addl	rr2,~L1+86(fp)
0356 b32d  0001     449 	slal	rr2,#1
035a e804           450 	jpr	L20006
                    451 L80:
035c 54f2  0056     452 	ldl	rr2,~L1+86(fp)
0360 b32d  0004     453 	slal	rr2,#4
                    454 L20006:
0364 5df2  0056     455 	ldl	~L1+86(fp),rr2
0368 6ffd  0052     456 	ld	~L1+82(fp),r13
036c 0b0d  0030     457 	cp	r13,#48
0370 e106           458 	jpr	lt,L82
0372 0b0d  0039     459 	cp	r13,#57
0376 ea03           460 	jpr	gt,L82
0378 030d  0030     461 	sub	r13,#48
037c e80b           462 	jpr	L83
                    463 L82:
037e 0b0d  0061     464 	cp	r13,#97
0382 e106           465 	jpr	lt,L84
0384 0b0d  0066     466 	cp	r13,#102
0388 ea03           467 	jpr	gt,L84
038a 030d  0057     468 	sub	r13,#87
038e e802           469 	jpr	L83
                    470 L84:
0390 030d  0037     471 	sub	r13,#55
                    472 L83:
0394 a1d3           473 	ld	r3,r13
0396 b12a           474 	exts	rr2
0398 56f2  0056     475 	addl	rr2,~L1+86(fp)
039c 5df2  0056     476 	ldl	~L1+86(fp),rr2
03a0 61fd  0052     477 	ld	r13,~L1+82(fp)
03a4 e808           478 	jpr	L74
                    479 L20008:
03a6 0b0c  000a     480 	cp	r12,#10
03aa ee60           481 	jpr	ne,L75
03ac 4df4  004e     482 	test	~L1+78(fp)
03b0 e65d           483 	jpr	eq,L75
03b2 69f0  0054     484 	inc	~L1+84(fp),#1
                    485 L74:
03b6 a1d2           486 	ld	r2,r13
03b8 2eea           487 	ldb	@r14,rl2
03ba a9e0           488 	inc	r14,#1
03bc 61f2  0002     489 	ld	r2,~L1+2(fp)
03c0 6b20  0002     490 	dec	2(r2),#1
03c4 6123  0002     491 	ld	r3,2(r2)
03c8 0b03  0000     492 	cp	r3,#0
03cc e99d           493 	jpr	ge,L10022
                    494 L20017:
03ce 61f7  0002     495 	ld	r7,~L1+2(fp)
03d2 5f00  0000*    496 	callr	__filbuf
                    497 L10023:
03d6 a12d           498 	ld	r13,r2
                    499 L76:
03d8 6bf0  0006     500 	dec	~L1+6(fp),#1
03dc 4df1  0006     501 	cp	~L1+6(fp),#0
03e0 0000  
03e2 e144           502 	jpr	lt,L75
03e4 60da  0001*    503 	ldb	rl2,__ctype+1(r13)
03e8 b120           504 	extsb	r2
03ea a722           505 	bit	r2,#2
03ec eea1           506 	jpr	ne,L10024
03ee 0b0c  0010     507 	cp	r12,#16
03f2 e692           508 	jpr	eq,L20015
                    509 L77:
03f4 0b0d  002e     510 	cp	r13,#46
03f8 e6d6           511 	jpr	eq,L20008
03fa 0b0d  0065     512 	cp	r13,#101
03fe e603           513 	jpr	eq,L10028
0400 0b0d  0045     514 	cp	r13,#69
0404 ee33           515 	jpr	ne,L75
                    516 L10028:
0406 4df4  004c     517 	test	~L1+76(fp)
040a ee30           518 	jpr	ne,L75
040c 0b0c  000a     519 	cp	r12,#10
0410 ee2d           520 	jpr	ne,L75
0412 4df4  004e     521 	test	~L1+78(fp)
0416 e62a           522 	jpr	eq,L75
0418 4df4  0054     523 	test	~L1+84(fp)
041c e627           524 	jpr	eq,L75
041e 69f0  004c     525 	inc	~L1+76(fp),#1
0422 a1d2           526 	ld	r2,r13
0424 2eea           527 	ldb	@r14,rl2
0426 a9e0           528 	inc	r14,#1
0428 61f2  0002     529 	ld	r2,~L1+2(fp)
042c 6b20  0002     530 	dec	2(r2),#1
0430 6123  0002     531 	ld	r3,2(r2)
0434 0b03  0000     532 	cp	r3,#0
0438 e905           533 	jpr	ge,L10030
043a 61f7  0002     534 	ld	r7,~L1+2(fp)
043e 5f00  0000*    535 	callr	__filbuf
0442 e807           536 	jpr	L10031
                    537 L10030:
0444 61f2  0002     538 	ld	r2,~L1+2(fp)
0448 2920           539 	inc	@r2,#1
044a 2123           540 	ld	r3,@r2
044c ab30           541 	dec	r3,#1
044e 203a           542 	ldb	rl2,@r3
0450 8222           543 	subb	rh2,rh2
                    544 L10031:
0452 a12d           545 	ld	r13,r2
0454 0b02  002b     546 	cp	r2,#43
0458 e6ae           547 	jpr	eq,L74
045a 0b02  002d     548 	cp	r2,#45
045e e6ab           549 	jpr	eq,L74
0460 0b02  0030     550 	cp	r2,#48
0464 e103           551 	jpr	lt,L75
0466 0b02  0039     552 	cp	r2,#57
046a e2a5           553 	jpr	le,L74
                    554 L75:
046c 4df4  0050     555 	test	~L1+80(fp)
0470 e605           556 	jpr	eq,L92
0472 9222           557 	subl	rr2,rr2
0474 52f2  0056     558 	subl	rr2,~L1+86(fp)
0478 5df2  0056     559 	ldl	~L1+86(fp),rr2
                    560 L92:
047c 0b0d  ffff     561 	cp	r13,#-1
0480 e609           562 	jpr	eq,L93
0482 a1d7           563 	ld	r7,r13
0484 61f6  0002     564 	ld	r6,~L1+2(fp)
0488 5f00  0000*    565 	callr	_ungetc
048c 61f2  0000     566 	ld	r2,~L1(fp)
0490 0d28           567 	clr	@r2
0492 e804           568 	jpr	L94
                    569 L93:
0494 61f2  0000     570 	ld	r2,~L1(fp)
0498 0d25  0001     571 	ld	@r2,#1
                    572 L94:
049c 4df4  000a     573 	test	~L1+10(fp)
04a0 e604           574 	jpr	eq,L10035
04a2 76f2  000c     575 	lda	r2,~L1+12(fp)
04a6 8b2e           576 	cp	r14,r2
04a8 ee02           577 	jpr	ne,L95
                    578 L10035:
04aa 8322           579 	sub	r2,r2
04ac e833           580 	jpr	L10010
                    581 L95:
04ae 0ce8           582 	clrb	@r14
04b0 a9e0           583 	inc	r14,#1
04b2 61f3  004e     584 	ld	r3,~L1+78(fp)
04b6 b339  0004     585 	sla	r3,#4
04ba 45f3  0004     586 	or	r3,~L1+4(fp)
04be a132           587 	ld	r2,r3
04c0 0b03  0012     588 	cp	r3,#18
04c4 eb0d           589 	jpr	ugt,L96
04c6 b320           590 	rl	r2,#1
04c8 6122  0094'    591 	ld	r2,L105(r2)
04cc 1e28           592 	jp	@r2
                    593 L99:
04ce 76f7  000c     594 	lda	r7,~L1+12(fp)
04d2 5f00  0000*    595 	callr	_atof
04d6 61f2  000a     596 	ld	r2,~L1+10(fp)
04da 2123           597 	ld	r3,@r2
04dc 0f3c  8001     598 	flds	@r3,f0
                    599 L96:
04e0 bd21           600 	ldk	r2,#1
04e2 e818           601 	jpr	L10010
                    602 L100:
04e4 76f7  000c     603 	lda	r7,~L1+12(fp)
04e8 5f00  0000*    604 	callr	_atof
04ec 61f2  000a     605 	ld	r2,~L1+10(fp)
04f0 2123           606 	ld	r3,@r2
04f2 0f3c  8003     607 	fldd	@r3,f0
04f6 e8f4           608 	jpr	L96
                    609 L101:
04f8 61f2  000a     610 	ld	r2,~L1+10(fp)
04fc 2123           611 	ld	r3,@r2
04fe 61f4  0058     612 	ld	r4,~L1+88(fp)
0502 2f34           613 	ld	@r3,r4
0504 e8ed           614 	jpr	L96
                    615 L103:
0506 61f2  000a     616 	ld	r2,~L1+10(fp)
050a 2123           617 	ld	r3,@r2
050c 54f4  0056     618 	ldl	rr4,~L1+86(fp)
0510 1d34           619 	ldl	@r3,rr4
0512 e8e6           620 	jpr	L96
                    621 L10010:
0514 5cf1  0c02     622 	ldm	r12,92(fp),#3
0518 005c  
051a 010f  0062     623 	add	fp,#~L2
051e 9e08           624 	ret
                    625 	.extern	__fltuse
                    626 	~L1 := 0
                    627 	~L2 := 98
                    628 	}	/* __innum */
                    629 
                    630 .data
                    631 
                    632 /*~~_instr:*/
                    633 
                    634 .code
                    635 
                    636 __instr::
                    637 {
0520 030f  0016     638 	sub	fp,#~L2
0524 5cf9  0b03     639 	ldm	14(fp),r11,#4
0528 000e  
052a 1cf9  0303     640 	ldm	@fp,r3,#4
052e a14d           641 	ld	r13,r4
0530 a17e           642 	ld	r14,r7
0532 a132           643 	ld	r2,r3
0534 0d28           644 	clr	@r2
0536 a1eb           645 	ld	r11,r14
0538 0b06  0063     646 	cp	r6,#99
053c ee06           647 	jpr	ne,L107
053e 0b05  7530     648 	cp	r5,#30000
0542 ee03           649 	jpr	ne,L107
0544 4df5  0004     650 	ld	~L1+4(fp),#1
0548 0001  
                    651 L107:
054a 4df8  000a     652 	clr	~L1+10(fp)
054e 4df1  0006     653 	cp	~L1+6(fp),#115
0552 0073  
0554 ee03           654 	jpr	ne,L109
0556 4df5  000a     655 	ld	~L1+10(fp),#1
055a 0001  
                    656 L109:
055c 6bd0  0002     657 	dec	2(r13),#1
0560 4dd1  0002     658 	cp	2(r13),#0
0564 0000  
0566 e904           659 	jpr	ge,L10039
0568 a1d7           660 	ld	r7,r13
056a 5f00  0000*    661 	callr	__filbuf
056e e805           662 	jpr	L10040
                    663 L10039:
0570 21d2           664 	ld	r2,@r13
0572 29d0           665 	inc	@r13,#1
0574 202b           666 	ldb	rl3,@r2
0576 8233           667 	subb	rh3,rh3
0578 a132           668 	ld	r2,r3
                    669 L10040:
057a a12c           670 	ld	r12,r2
057c 602b  0000'    671 	ldb	rl3,__sctab(r2)
0580 b130           672 	extsb	r3
0582 47f3  000a     673 	and	r3,~L1+10(fp)
0586 e603           674 	jpr	eq,L110
0588 0b0c  ffff     675 	cp	r12,#-1
058c eee7           676 	jpr	ne,L109
                    677 L110:
058e 4df5  000a     678 	ld	~L1+10(fp),#1
0592 0001  
0594 4df1  0006     679 	cp	~L1+6(fp),#99
0598 0063  
059a ee03           680 	jpr	ne,L112
059c 4df8  000a     681 	clr	~L1+10(fp)
05a0 e82a           682 	jpr	L115
                    683 L112:
05a2 4df1  0006     684 	cp	~L1+6(fp),#91
05a6 005b  
05a8 ee26           685 	jpr	ne,L115
05aa 4df5  000a     686 	ld	~L1+10(fp),#2
05ae 0002  
05b0 e822           687 	jpr	L115
                    688 L20022:
05b2 60ca  0000'    689 	ldb	rl2,__sctab(r12)
05b6 b120           690 	extsb	r2
05b8 47f2  000a     691 	and	r2,~L1+10(fp)
05bc 8d24           692 	test	r2
05be ee1e           693 	jpr	ne,L116
05c0 8de4           694 	test	r14
05c2 e603           695 	jpr	eq,L117
05c4 a1c2           696 	ld	r2,r12
05c6 2eea           697 	ldb	@r14,rl2
05c8 a9e0           698 	inc	r14,#1
                    699 L117:
05ca 6bf0  0004     700 	dec	~L1+4(fp),#1
05ce 4df1  0004     701 	cp	~L1+4(fp),#0
05d2 0000  
05d4 e213           702 	jpr	le,L116
05d6 6bd0  0002     703 	dec	2(r13),#1
05da 4dd1  0002     704 	cp	2(r13),#0
05de 0000  
05e0 e904           705 	jpr	ge,L10041
05e2 a1d7           706 	ld	r7,r13
05e4 5f00  0000*    707 	callr	__filbuf
05e8 e805           708 	jpr	L10042
                    709 L10041:
05ea 21d2           710 	ld	r2,@r13
05ec 29d0           711 	inc	@r13,#1
05ee 202b           712 	ldb	rl3,@r2
05f0 8233           713 	subb	rh3,rh3
05f2 a132           714 	ld	r2,r3
                    715 L10042:
05f4 a12c           716 	ld	r12,r2
                    717 L115:
05f6 0b0c  ffff     718 	cp	r12,#-1
05fa eedb           719 	jpr	ne,L20022
                    720 L116:
05fc 0b0c  ffff     721 	cp	r12,#-1
0600 e60c           722 	jpr	eq,L119
0602 4df1  0004     723 	cp	~L1+4(fp),#0
0606 0000  
0608 e204           724 	jpr	le,L120
060a a1c7           725 	ld	r7,r12
060c a1d6           726 	ld	r6,r13
060e 5f00  0000*    727 	callr	_ungetc
                    728 L120:
0612 61f2  0000     729 	ld	r2,~L1(fp)
0616 0d28           730 	clr	@r2
0618 e804           731 	jpr	L121
                    732 L119:
061a 61f2  0000     733 	ld	r2,~L1(fp)
061e 0d25  0001     734 	ld	@r2,#1
                    735 L121:
0622 8de4           736 	test	r14
0624 e60a           737 	jpr	eq,L122
0626 8bbe           738 	cp	r14,r11
0628 e608           739 	jpr	eq,L122
062a 4df1  0006     740 	cp	~L1+6(fp),#99
062e 0063  
0630 e602           741 	jpr	eq,L123
0632 0ce8           742 	clrb	@r14
0634 a9e0           743 	inc	r14,#1
                    744 L123:
0636 bd21           745 	ldk	r2,#1
0638 e801           746 	jpr	L10036
                    747 L122:
063a 8322           748 	sub	r2,r2
                    749 L10036:
063c 5cf1  0b03     750 	ldm	r11,14(fp),#4
0640 000e  
0642 010f  0016     751 	add	fp,#~L2
0646 9e08           752 	ret
                    753 	~L1 := 0
                    754 	~L2 := 22
                    755 	}	/* __instr */
                    756 
                    757 .data
                    758 
                    759 /*~~_getccl:*/
                    760 
                    761 .code
                    762 
                    763 __getccl::
                    764 {
0648 abf9           765 	dec	fp,#~L2
064a a171           766 	ld	r1,r7
064c bd50           767 	ldk	r5,#0
064e 0c11  5e5e     768 	cpb	@r1,#94
0652 ee02           769 	jpr	ne,L125
0654 a950           770 	inc	r5,#1
0656 a910           771 	inc	r1,#1
                    772 L125:
0658 bd40           773 	ldk	r4,#0
                    774 L20024:
065a 8d54           775 	test	r5
065c e603           776 	jpr	eq,L129
065e 6241  0000'    777 	resb	__sctab(r4),#1
0662 e802           778 	jpr	L126
                    779 L129:
0664 6441  0000'    780 	setb	__sctab(r4),#1
                    781 L126:
0668 a940           782 	inc	r4,#1
066a 0b04  0080     783 	cp	r4,#128
066e e1f5           784 	jpr	lt,L20024
                    785 L131:
0670 a112           786 	ld	r2,r1
0672 a910           787 	inc	r1,#1
0674 202b           788 	ldb	rl3,@r2
0676 b130           789 	extsb	r3
0678 a134           790 	ld	r4,r3
067a 0703  007f     791 	and	r3,#127
067e 0b03  005d     792 	cp	r3,#93
0682 e60b           793 	jpr	eq,L132
0684 8d54           794 	test	r5
0686 e603           795 	jpr	eq,L133
0688 6441  0000'    796 	setb	__sctab(r4),#1
068c e802           797 	jpr	L20025
                    798 L133:
068e 6241  0000'    799 	resb	__sctab(r4),#1
                    800 L20025:
0692 a940           801 	inc	r4,#1
0694 8d44           802 	test	r4
0696 eeec           803 	jpr	ne,L131
0698 ab10           804 	dec	r1,#1
                    805 L132:
069a a112           806 	ld	r2,r1
069c a9f9           807 	inc	fp,#~L2
069e 9e08           808 	ret
                    809 	~L1 := 0
                    810 	~L2 := 10
                    811 	}	/* __getccl */
                    812 
                    813 .data
                    814 
