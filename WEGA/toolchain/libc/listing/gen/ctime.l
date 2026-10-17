LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 .comm	26,	_cbuf;
                      5 .even
                      6 _dmsize::
0000 001f             7 .word	%1f
0002 001c             8 .word	%1c
0004 001f             9 .word	%1f
0006 001e            10 .word	%1e
0008 001f            11 .word	%1f
000a 001e            12 .word	%1e
000c 001f            13 .word	%1f
000e 001f            14 .word	%1f
0010 001e            15 .word	%1e
0012 001f            16 .word	%1f
0014 001e            17 .word	%1e
0016 001f            18 .word	%1f
                     19 .even
                     20 _timezone::
0018 0000  4650      21 .long	%4650
                     22 .even
                     23 _tzname::
001c 0020'           24 .addr	L14
001e 0024'           25 .addr	L15
                     26 L14:
0020 45  53  54      27 .byte	%45,%53,%54,%0
0023 00  
                     28 L15:
0024 45  44  54      29 .byte	%45,%44,%54,%0
0027 00  
                     30 .even
                     31 .even
                     32 _daylight::
0028 0001            33 .word	%1
                     34 .even
                     35 _daytab::
002a 0005            36 .word	%5
002c 014d            37 .word	%14d
002e 003a            38 .word	%3a
0030 012f            39 .word	%12f
                     40 /*~~ctime:*/
                     41 
                     42 .code
                     43 
                     44 _ctime::
                     45 {
0000 abf5            46 	dec	fp,#~L2
0002 2ff7            47 	ld	@fp,r7
0004 a176            48 	ld	r6,r7
0006 dffa            49 	callr	_localtime
0008 6ff2  0004      50 	ld	~L1+4(fp),r2
000c a127            51 	ld	r7,r2
000e ded2            52 	callr	_asctime
0010 a9f5            53 	inc	fp,#~L2
0012 9e08            54 	ret
                     55 	~L1 := 0
                     56 	~L2 := 6
                     57 	}	/* _ctime */
                     58 
                     59 .data
                     60 
                     61 /*~~localtime:*/
                     62 
                     63 .code
                     64 
                     65 _localtime::
                     66 {
0014 abff            67 	dec	fp,#~L2
0016 5cf9  0b03      68 	ldm	8(fp),r11,#4
001a 0008  
001c 2ff7            69 	ld	@fp,r7
001e de31            70 	callr	_tzset
0020 61f2  0000      71 	ld	r2,~L1(fp)
0024 1424            72 	ldl	rr4,@r2
0026 5204  0018'     73 	subl	rr4,_timezone
002a 5df4  0002      74 	ldl	~L1+2(fp),rr4
002e 76f7  0002      75 	lda	r7,~L1+2(fp)
0032 df92            76 	callr	_gmtime
0034 a12d            77 	ld	r13,r2
0036 61de  000e      78 	ld	r14,14(r13)
003a 210c  0077      79 	ld	r12,#119
003e 210b  012f      80 	ld	r11,#303
0042 4dd1  000a      81 	cp	10(r13),#74
0046 004a  
0048 e604            82 	jpr	eq,L10006
004a 4dd1  000a      83 	cp	10(r13),#75
004e 004b  
0050 ee10            84 	jpr	ne,L27
                     85 L10006:
0052 61d2  000a      86 	ld	r2,10(r13)
0056 0302  004a      87 	sub	r2,#74
005a b329  0002      88 	sla	r2,#2
005e 612c  002a'     89 	ld	r12,_daytab(r2)
0062 61d2  000a      90 	ld	r2,10(r13)
0066 0302  004a      91 	sub	r2,#74
006a b329  0002      92 	sla	r2,#2
006e 612b  002c'     93 	ld	r11,_daytab+2(r2)
                     94 L27:
0072 a1d7            95 	ld	r7,r13
0074 a1c6            96 	ld	r6,r12
0076 dfd5            97 	callr	_sunday
0078 a12c            98 	ld	r12,r2
007a a1d7            99 	ld	r7,r13
007c a1b6           100 	ld	r6,r11
007e dfd9           101 	callr	_sunday
0080 a12b           102 	ld	r11,r2
0082 4d04  0028'    103 	test	_daylight
0086 e61d           104 	jpr	eq,L29
0088 8bce           105 	cp	r14,r12
008a ea06           106 	jpr	gt,L10007
008c 8bce           107 	cp	r14,r12
008e ee19           108 	jpr	ne,L29
0090 4dd1  0004     109 	cp	4(r13),#2
0094 0002  
0096 e115           110 	jpr	lt,L29
                    111 L10007:
0098 8bbe           112 	cp	r14,r11
009a e106           113 	jpr	lt,L10008
009c 8bbe           114 	cp	r14,r11
009e ee11           115 	jpr	ne,L29
00a0 4dd1  0004     116 	cp	4(r13),#1
00a4 0001  
00a6 e90d           117 	jpr	ge,L29
                    118 L10008:
00a8 54f2  0002     119 	ldl	rr2,~L1+2(fp)
00ac 1602  0000     120 	addl	rr2,#3600
00b0 0e10  
00b2 5df2  0002     121 	ldl	~L1+2(fp),rr2
00b6 76f7  0002     122 	lda	r7,~L1+2(fp)
00ba dfd6           123 	callr	_gmtime
00bc a12d           124 	ld	r13,r2
00be 69d0  0010     125 	inc	16(r13),#1
                    126 L29:
00c2 a1d2           127 	ld	r2,r13
00c4 5cf1  0b03     128 	ldm	r11,8(fp),#4
00c8 0008  
00ca a9ff           129 	inc	fp,#~L2
00cc 9e08           130 	ret
                    131 	~L1 := 0
                    132 	~L2 := 16
                    133 	}	/* _localtime */
                    134 
                    135 .data
                    136 
                    137 /*~~sunday:*/
                    138 
                    139 .code
                    140 
                    141 _sunday::
                    142 {
00ce abf9           143 	dec	fp,#~L2
00d0 5cf9  0d01     144 	ldm	6(fp),r13,#2
00d4 0006  
00d6 a16d           145 	ld	r13,r6
00d8 a17e           146 	ld	r14,r7
00da 0b06  003a     147 	cp	r6,#58
00de e106           148 	jpr	lt,L31
00e0 61e7  000a     149 	ld	r7,10(r14)
00e4 dec9           150 	callr	_dysize
00e6 0302  016d     151 	sub	r2,#365
00ea 812d           152 	add	r13,r2
                    153 L31:
00ec a1d2           154 	ld	r2,r13
00ee 43e2  000e     155 	sub	r2,14(r14)
00f2 41e2  000c     156 	add	r2,12(r14)
00f6 0102  02bc     157 	add	r2,#700
00fa a125           158 	ld	r5,r2
00fc b14a           159 	exts	rr4
00fe 1b04  0007     160 	div	rr4,#7
0102 a1d2           161 	ld	r2,r13
0104 8342           162 	sub	r2,r4
0106 5cf1  0d01     163 	ldm	r13,6(fp),#2
010a 0006  
010c a9f9           164 	inc	fp,#~L2
010e 9e08           165 	ret
                    166 	~L1 := 0
                    167 	~L2 := 10
                    168 	}	/* _sunday */
                    169 
                    170 .data
                    171 
                    172 .even
                    173 L34:
                    174 .=.+18
                    175 /*~~gmtime:*/
                    176 
                    177 .code
                    178 
                    179 _gmtime::
                    180 {
0110 030f  0016     181 	sub	fp,#~L2
0114 5cf9  0c02     182 	ldm	16(fp),r12,#3
0118 0010  
011a 2ff7           183 	ld	@fp,r7
011c a176           184 	ld	r6,r7
011e 1462           185 	ldl	rr2,@r6
0120 9426           186 	ldl	rr6,rr2
0122 b147           187 	extsl	rq4
0124 1a04  0001     188 	divl	rq4,#86400
0128 5180  
012a 5df4  0004     189 	ldl	~L1+4(fp),rr4
012e 61f6  0000     190 	ld	r6,~L1(fp)
0132 1462           191 	ldl	rr2,@r6
0134 9426           192 	ldl	rr6,rr2
0136 b147           193 	extsl	rq4
0138 1a04  0001     194 	divl	rq4,#86400
013c 5180  
013e 5df6  0008     195 	ldl	~L1+8(fp),rr6
0142 54f2  0004     196 	ldl	rr2,~L1+4(fp)
0146 1002  0000     197 	cpl	rr2,#0
014a 0000  
014c e90c           198 	jpr	ge,L35
014e 9424           199 	ldl	rr4,rr2
0150 1604  0001     200 	addl	rr4,#86400
0154 5180  
0156 5df4  0004     201 	ldl	~L1+4(fp),rr4
015a 9464           202 	ldl	rr4,rr6
015c 1204  0000     203 	subl	rr4,#1
0160 0001  
0162 5df4  0008     204 	ldl	~L1+8(fp),rr4
                    205 L35:
0166 210c  0032'    206 	ld	r12,#L34
016a 54f6  0004     207 	ldl	rr6,~L1+4(fp)
016e b147           208 	extsl	rq4
0170 1a04  0000     209 	divl	rq4,#60
0174 003c  
0176 2fc5           210 	ld	@r12,r5
0178 a9c1           211 	inc	r12,#2
017a 54f6  0004     212 	ldl	rr6,~L1+4(fp)
017e b147           213 	extsl	rq4
0180 1a04  0000     214 	divl	rq4,#60
0184 003c  
0186 a17d           215 	ld	r13,r7
0188 a173           216 	ld	r3,r7
018a b12a           217 	exts	rr2
018c 1b02  003c     218 	div	rr2,#60
0190 2fc2           219 	ld	@r12,r2
0192 a9c1           220 	inc	r12,#2
0194 a1d3           221 	ld	r3,r13
0196 b12a           222 	exts	rr2
0198 1b02  003c     223 	div	rr2,#60
019c a13d           224 	ld	r13,r3
019e 2fc3           225 	ld	@r12,r3
01a0 a9c1           226 	inc	r12,#2
01a2 54f6  0008     227 	ldl	rr6,~L1+8(fp)
01a6 1606  0070     228 	addl	rr6,#7340036
01aa 0004  
01ac b147           229 	extsl	rq4
01ae 1a04  0000     230 	divl	rq4,#7
01b2 0007  
01b4 6f05  003e'    231 	ld	L34+12,r5
01b8 54f2  0008     232 	ldl	rr2,~L1+8(fp)
01bc 1002  0000     233 	cpl	rr2,#0
01c0 0000  
01c2 e114           234 	jpr	lt,L36
01c4 210d  0046     235 	ld	r13,#70
                    236 L39:
01c8 a1d7           237 	ld	r7,r13
01ca df3c           238 	callr	_dysize
01cc a125           239 	ld	r5,r2
01ce b14a           240 	exts	rr4
01d0 50f4  0008     241 	cpl	rr4,~L1+8(fp)
01d4 ea1e           242 	jpr	gt,L40
01d6 a1d7           243 	ld	r7,r13
01d8 df43           244 	callr	_dysize
01da a125           245 	ld	r5,r2
01dc b14a           246 	exts	rr4
01de 54f2  0008     247 	ldl	rr2,~L1+8(fp)
01e2 9242           248 	subl	rr2,rr4
01e4 5df2  0008     249 	ldl	~L1+8(fp),rr2
01e8 a9d0           250 	inc	r13,#1
01ea e8ee           251 	jpr	L39
                    252 L36:
01ec 210d  0046     253 	ld	r13,#70
01f0 e80a           254 	jpr	L43
                    255 L20001:
01f2 a1d7           256 	ld	r7,r13
01f4 ab70           257 	dec	r7,#1
01f6 df52           258 	callr	_dysize
01f8 a125           259 	ld	r5,r2
01fa b14a           260 	exts	rr4
01fc 56f4  0008     261 	addl	rr4,~L1+8(fp)
0200 5df4  0008     262 	ldl	~L1+8(fp),rr4
0204 abd0           263 	dec	r13,#1
                    264 L43:
0206 54f2  0008     265 	ldl	rr2,~L1+8(fp)
020a 1002  0000     266 	cpl	rr2,#0
020e 0000  
0210 e1f0           267 	jpr	lt,L20001
                    268 L40:
0212 6f0d  003c'    269 	ld	L34+10,r13
0216 61fe  000a     270 	ld	r14,~L1+10(fp)
021a 6f0e  0040'    271 	ld	L34+14,r14
021e a1d7           272 	ld	r7,r13
0220 df67           273 	callr	_dysize
0222 0b02  016e     274 	cp	r2,#366
0226 ee03           275 	jpr	ne,L44
0228 4d05  0002'    276 	ld	_dmsize+2,#29
022c 001d  
                    277 L44:
022e bdd0           278 	ldk	r13,#0
0230 e805           279 	jpr	L47
                    280 L20003:
0232 a1d2           281 	ld	r2,r13
0234 81d2           282 	add	r2,r13
0236 432e  0000'    283 	sub	r14,_dmsize(r2)
023a a9d0           284 	inc	r13,#1
                    285 L47:
023c a1d2           286 	ld	r2,r13
023e 81d2           287 	add	r2,r13
0240 4b2e  0000'    288 	cp	r14,_dmsize(r2)
0244 e9f6           289 	jpr	ge,L20003
0246 4d05  0002'    290 	ld	_dmsize+2,#28
024a 001c  
024c 76e2  0001     291 	lda	r2,1(r14)
0250 2fc2           292 	ld	@r12,r2
0252 a9c1           293 	inc	r12,#2
0254 2fcd           294 	ld	@r12,r13
0256 a9c1           295 	inc	r12,#2
0258 4d08  0042'    296 	clr	L34+16
025c 2102  0032'    297 	ld	r2,#L34
0260 5cf1  0c02     298 	ldm	r12,16(fp),#3
0264 0010  
0266 010f  0016     299 	add	fp,#~L2
026a 9e08           300 	ret
                    301 	~L1 := 0
                    302 	~L2 := 22
                    303 	}	/* _gmtime */
                    304 
                    305 .data
                    306 
                    307 L49:
0044 44  61  79     308 .byte	%44,%61,%79,%20,%4d,%6f,%6e,%20
0047 20  4d  6f  
004a 6e  20  
004c 30  30  20     309 .byte	%30,%30,%20,%30,%30,%3a,%30,%30
004f 30  30  3a  
0052 30  30  
0054 3a  30  30     310 .byte	%3a,%30,%30,%20,%31,%39,%30,%30
0057 20  31  39  
005a 30  30  
005c 0a  00         311 .byte	%a,%0
                    312 .even
                    313 L53:
005e 53  75  6e     314 .byte	%53,%75,%6e,%4d,%6f,%6e,%54,%75
0061 4d  6f  6e  
0064 54  75  
0066 65  57  65     315 .byte	%65,%57,%65,%64,%54,%68,%75,%46
0069 64  54  68  
006c 75  46  
006e 72  69  53     316 .byte	%72,%69,%53,%61,%74,%0
0071 61  74  00  
                    317 .even
                    318 L54:
0074 4a  61  6e     319 .byte	%4a,%61,%6e,%46,%65,%62,%4d,%61
0077 46  65  62  
007a 4d  61  
007c 72  41  70     320 .byte	%72,%41,%70,%72,%4d,%61,%79,%4a
007f 72  4d  61  
0082 79  4a  
0084 75  6e  4a     321 .byte	%75,%6e,%4a,%75,%6c,%41,%75,%67
0087 75  6c  41  
008a 75  67  
008c 53  65  70     322 .byte	%53,%65,%70,%4f,%63,%74,%4e,%6f
008f 4f  63  74  
0092 4e  6f  
0094 76  44  65     323 .byte	%76,%44,%65,%63,%0
0097 63  00  
                    324 .even
                    325 /*~~asctime:*/
                    326 
                    327 .code
                    328 
                    329 _asctime::
                    330 {
026c abfb           331 	dec	fp,#~L2
026e 5cf9  0c02     332 	ldm	6(fp),r12,#3
0272 0006  
0274 2ff7           333 	ld	@fp,r7
0276 210e  0000*    334 	ld	r14,#_cbuf
027a 210d  0044'    335 	ld	r13,#L49
                    336 L52:
027e a1d2           337 	ld	r2,r13
0280 a9d0           338 	inc	r13,#1
0282 a1e3           339 	ld	r3,r14
0284 a9e0           340 	inc	r14,#1
0286 202c           341 	ldb	rl4,@r2
0288 2e3c           342 	ldb	@r3,rl4
028a 8cc4           343 	testb	rl4
028c eef8           344 	jpr	ne,L52
028e 61f2  0000     345 	ld	r2,~L1(fp)
0292 6125  000c     346 	ld	r5,12(r2)
0296 1904  0003     347 	mult	rr4,#3
029a 0105  005e'    348 	add	r5,#L53
029e a15d           349 	ld	r13,r5
02a0 210e  0000*    350 	ld	r14,#_cbuf
02a4 20db           351 	ldb	rl3,@r13
02a6 2eeb           352 	ldb	@r14,rl3
02a8 a9d0           353 	inc	r13,#1
02aa a9e0           354 	inc	r14,#1
02ac 20db           355 	ldb	rl3,@r13
02ae 2eeb           356 	ldb	@r14,rl3
02b0 a9d0           357 	inc	r13,#1
02b2 a9e0           358 	inc	r14,#1
02b4 20db           359 	ldb	rl3,@r13
02b6 2eeb           360 	ldb	@r14,rl3
02b8 a9d0           361 	inc	r13,#1
02ba a9e0           362 	inc	r14,#1
02bc a9e0           363 	inc	r14,#1
02be 61f2  0000     364 	ld	r2,~L1(fp)
02c2 a927           365 	inc	r2,#8
02c4 a12c           366 	ld	r12,r2
02c6 21c3           367 	ld	r3,@r12
02c8 1902  0003     368 	mult	rr2,#3
02cc 0103  0074'    369 	add	r3,#L54
02d0 a13d           370 	ld	r13,r3
02d2 20da           371 	ldb	rl2,@r13
02d4 2eea           372 	ldb	@r14,rl2
02d6 a9d0           373 	inc	r13,#1
02d8 a9e0           374 	inc	r14,#1
02da 20da           375 	ldb	rl2,@r13
02dc 2eea           376 	ldb	@r14,rl2
02de a9d0           377 	inc	r13,#1
02e0 a9e0           378 	inc	r14,#1
02e2 20da           379 	ldb	rl2,@r13
02e4 2eea           380 	ldb	@r14,rl2
02e6 a9d0           381 	inc	r13,#1
02e8 a9e0           382 	inc	r14,#1
02ea abc1           383 	dec	r12,#2
02ec 21c6           384 	ld	r6,@r12
02ee a1e7           385 	ld	r7,r14
02f0 dfc0           386 	callr	_ct_numb
02f2 abc1           387 	dec	r12,#2
02f4 21c6           388 	ld	r6,@r12
02f6 0106  0064     389 	add	r6,#100
02fa a127           390 	ld	r7,r2
02fc dfc6           391 	callr	_ct_numb
02fe abc1           392 	dec	r12,#2
0300 21c6           393 	ld	r6,@r12
0302 0106  0064     394 	add	r6,#100
0306 a127           395 	ld	r7,r2
0308 dfcc           396 	callr	_ct_numb
030a abc1           397 	dec	r12,#2
030c 21c6           398 	ld	r6,@r12
030e 0106  0064     399 	add	r6,#100
0312 a127           400 	ld	r7,r2
0314 dfd2           401 	callr	_ct_numb
0316 a12e           402 	ld	r14,r2
0318 61f2  0000     403 	ld	r2,~L1(fp)
031c 4d21  000a     404 	cp	10(r2),#100
0320 0064  
0322 e106           405 	jpr	lt,L55
0324 4ce5  0001     406 	ldb	1(r14),#50
0328 3232  
032a 4ce5  0002     407 	ldb	2(r14),#48
032e 3030  
                    408 L55:
0330 a9e1           409 	inc	r14,#2
0332 61f6  0000     410 	ld	r6,~L1(fp)
0336 6167  000a     411 	ld	r7,10(r6)
033a 0107  0064     412 	add	r7,#100
033e a176           413 	ld	r6,r7
0340 a1e7           414 	ld	r7,r14
0342 dfe9           415 	callr	_ct_numb
0344 a12e           416 	ld	r14,r2
0346 2102  0000*    417 	ld	r2,#_cbuf
034a 5cf1  0c02     418 	ldm	r12,6(fp),#3
034e 0006  
0350 a9fb           419 	inc	fp,#~L2
0352 9e08           420 	ret
                    421 	~L1 := 0
                    422 	~L2 := 12
                    423 	}	/* _asctime */
                    424 
                    425 .data
                    426 
                    427 /*~~dysize:*/
                    428 
                    429 .code
                    430 
                    431 _dysize::
                    432 {
0354 abf3           433 	dec	fp,#~L2
0356 2ff7           434 	ld	@fp,r7
0358 a173           435 	ld	r3,r7
035a b12a           436 	exts	rr2
035c 1b02  0004     437 	div	rr2,#4
0360 8d24           438 	test	r2
0362 ee03           439 	jpr	ne,L57
0364 2102  016e     440 	ld	r2,#366
0368 e802           441 	jpr	L10018
                    442 L57:
036a 2102  016d     443 	ld	r2,#365
                    444 L10018:
036e a9f3           445 	inc	fp,#~L2
0370 9e08           446 	ret
                    447 	~L1 := 0
                    448 	~L2 := 4
                    449 	}	/* _dysize */
                    450 
                    451 .data
                    452 
                    453 /*~~ct_numb:*/
                    454 
                    455 .code
                    456 
                    457 _ct_numb::
                    458 {
0372 abf7           459 	dec	fp,#~L2
0374 6ffe  0006     460 	ld	6(fp),r14
0378 2ff6           461 	ld	@fp,r6
037a a17e           462 	ld	r14,r7
037c a9e0           463 	inc	r14,#1
037e 0b06  000a     464 	cp	r6,#10
0382 e10c           465 	jpr	lt,L59
0384 a163           466 	ld	r3,r6
0386 b12a           467 	exts	rr2
0388 1b02  000a     468 	div	rr2,#10
038c a135           469 	ld	r5,r3
038e bd40           470 	ldk	r4,#0
0390 1b04  000a     471 	div	rr4,#10
0394 0104  0030     472 	add	r4,#48
0398 2eec           473 	ldb	@r14,rl4
039a e802           474 	jpr	L20004
                    475 L59:
039c 0ce5  2020     476 	ldb	@r14,#32
                    477 L20004:
03a0 a9e0           478 	inc	r14,#1
03a2 61f3  0000     479 	ld	r3,~L1(fp)
03a6 b12a           480 	exts	rr2
03a8 1b02  000a     481 	div	rr2,#10
03ac 0102  0030     482 	add	r2,#48
03b0 2eea           483 	ldb	@r14,rl2
03b2 a9e0           484 	inc	r14,#1
03b4 a1e2           485 	ld	r2,r14
03b6 61fe  0006     486 	ld	r14,6(fp)
03ba a9f7           487 	inc	fp,#~L2
03bc 9e08           488 	ret
                    489 	~L1 := 0
                    490 	~L2 := 8
                    491 	}	/* _ct_numb */
                    492 
                    493 .data
                    494 
                    495 L63:
009a 54  5a  00     496 .byte	%54,%5a,%0
                    497 .even
                    498 /*~~tzset:*/
                    499 
                    500 .code
                    501 
                    502 _tzset::
                    503 {
03be abf9           504 	dec	fp,#~L2
03c0 5cf9  0c02     505 	ldm	4(fp),r12,#3
03c4 0004  
03c6 2107  009a'    506 	ld	r7,#L63
03ca 5f00  0000*    507 	callr	_getenv
03ce a12e           508 	ld	r14,r2
03d0 8d24           509 	test	r2
03d2 e658           510 	jpr	eq,L10024
03d4 0ce4           511 	testb	@r14
03d6 e656           512 	jpr	eq,L10024
03d8 bdc3           513 	ldk	r12,#3
03da 610d  001c'    514 	ld	r13,_tzname
                    515 L67:
03de 0ce4           516 	testb	@r14
03e0 e605           517 	jpr	eq,L10027
03e2 a1e2           518 	ld	r2,r14
03e4 a9e0           519 	inc	r14,#1
03e6 202b           520 	ldb	rl3,@r2
03e8 b130           521 	extsb	r3
03ea e802           522 	jpr	L10028
                    523 L10027:
03ec 2103  0020     524 	ld	r3,#32
                    525 L10028:
03f0 2edb           526 	ldb	@r13,rl3
03f2 a9d0           527 	inc	r13,#1
03f4 abc0           528 	dec	r12,#1
03f6 eef3           529 	jpr	ne,L67
03f8 0ce1  2d2d     530 	cpb	@r14,#45
03fc bd20           531 	ldk	r2,#0
03fe af26           532 	tcc	eq,r2
0400 b120           533 	extsb	r2
0402 6ff2  0002     534 	ld	~L1+2(fp),r2
0406 8d24           535 	test	r2
0408 e601           536 	jpr	eq,L68
040a a9e0           537 	inc	r14,#1
                    538 L68:
040c bdc0           539 	ldk	r12,#0
040e e80d           540 	jpr	L69
                    541 L20006:
0410 0ce1  3939     542 	cpb	@r14,#57
0414 ea0d           543 	jpr	gt,L70
0416 a1c3           544 	ld	r3,r12
0418 1902  000a     545 	mult	rr2,#10
041c 20ea           546 	ldb	rl2,@r14
041e b120           547 	extsb	r2
0420 8123           548 	add	r3,r2
0422 0303  0030     549 	sub	r3,#48
0426 a13c           550 	ld	r12,r3
0428 a9e0           551 	inc	r14,#1
                    552 L69:
042a 0ce1  3030     553 	cpb	@r14,#48
042e e9f0           554 	jpr	ge,L20006
                    555 L70:
0430 4df4  0002     556 	test	~L1+2(fp)
0434 e603           557 	jpr	eq,L71
0436 a1c2           558 	ld	r2,r12
0438 8d22           559 	neg	r2
043a a12c           560 	ld	r12,r2
                    561 L71:
043c a1c7           562 	ld	r7,r12
043e 1906  003c     563 	mult	rr6,#60
0442 a173           564 	ld	r3,r7
0444 b12a           565 	exts	rr2
0446 9426           566 	ldl	rr6,rr2
0448 1804  0000     567 	multl	rq4,#60
044c 003c  
044e 5d06  0018'    568 	ldl	_timezone,rr6
0452 0ce1  0000     569 	cpb	@r14,#0
0456 bd20           570 	ldk	r2,#0
0458 af2e           571 	tcc	ne,r2
045a b120           572 	extsb	r2
045c 6f02  0028'    573 	ld	_daylight,r2
0460 8d24           574 	test	r2
0462 e610           575 	jpr	eq,L10024
0464 610d  001e'    576 	ld	r13,_tzname+2
0468 bdc3           577 	ldk	r12,#3
                    578 L75:
046a 0ce4           579 	testb	@r14
046c e605           580 	jpr	eq,L10031
046e a1e2           581 	ld	r2,r14
0470 a9e0           582 	inc	r14,#1
0472 202b           583 	ldb	rl3,@r2
0474 b130           584 	extsb	r3
0476 e802           585 	jpr	L10032
                    586 L10031:
0478 2103  0020     587 	ld	r3,#32
                    588 L10032:
047c 2edb           589 	ldb	@r13,rl3
047e a9d0           590 	inc	r13,#1
0480 abc0           591 	dec	r12,#1
0482 eef3           592 	jpr	ne,L75
                    593 L10024:
0484 5cf1  0c02     594 	ldm	r12,4(fp),#3
0488 0004  
048a a9f9           595 	inc	fp,#~L2
048c 9e08           596 	ret
                    597 	~L1 := 0
                    598 	~L2 := 10
                    599 	}	/* _tzset */
                    600 
                    601 .data
                    602 
