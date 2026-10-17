LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 
                      5 .code
                      6 
                      7 _fix:
                      8 {
0000 abf7             9 	dec	fp,#~L2
0002 1cf9  0502      10 	ldm	@fp,r5,#3
0006 a174            11 	ld	r4,r7
0008 0d44            12 	test	@r4
000a e603            13 	jpr	eq,L10003
000c 0d71  0002      14 	cp	@r7,#2
0010 ee07            15 	jpr	ne,L19
                     16 L10003:
0012 61f2  0004      17 	ld	r2,~L1+4(fp)
0016 2920            18 	inc	@r2,#1
0018 61f2  0002      19 	ld	r2,~L1+2(fp)
001c 2921            20 	inc	@r2,#2
001e e81e            21 	jpr	L10000
                     22 L19:
0020 61f2  0004      23 	ld	r2,~L1+4(fp)
0024 0d21  0004      24 	cp	@r2,#4
0028 ee07            25 	jpr	ne,L21
002a 2921            26 	inc	@r2,#2
002c 61f2  0002      27 	ld	r2,~L1+2(fp)
0030 61f3  0000      28 	ld	r3,~L1(fp)
0034 2f23            29 	ld	@r2,r3
0036 e812            30 	jpr	L10000
                     31 L21:
0038 61f2  0002      32 	ld	r2,~L1+2(fp)
003c 2123            33 	ld	r3,@r2
003e 2134            34 	ld	r4,@r3
0040 6ff4  0006      35 	ld	~L1+6(fp),r4
0044 2123            36 	ld	r3,@r2
0046 2125            37 	ld	r5,@r2
0048 6136  0002      38 	ld	r6,2(r3)
004c 2f56            39 	ld	@r5,r6
004e 61f2  0002      40 	ld	r2,~L1+2(fp)
0052 2123            41 	ld	r3,@r2
0054 61f5  0006      42 	ld	r5,~L1+6(fp)
0058 6f35  0002      43 	ld	2(r3),r5
                     44 L10000:
005c a9f7            45 	inc	fp,#~L2
005e 9e08            46 	ret
                     47 	~L1 := 0
                     48 	~L2 := 8
                     49 	}	/* _fix */
                     50 
                     51 .data
                     52 
                     53 .even
                     54 L85:
0000 28  6e  75      55 .byte	%28,%6e,%75,%6c,%6c,%29,%0
0003 6c  6c  29  
0006 00  
                     56 .even
                     57 .even
                     58 L100:
0008 0044            59 .word	68
000a 004f            60 .word	79
000c 0055            61 .word	85
000e 0058            62 .word	88
0010 0063            63 .word	99
0012 0064            64 .word	100
0014 0065            65 .word	101
0016 0066            66 .word	102
0018 0067            67 .word	103
001a 006c            68 .word	108
001c 006f            69 .word	111
001e 0073            70 .word	115
0020 0075            71 .word	117
0022 0078            72 .word	120
                     73 .even
                     74 /*~~_doprnt:*/
                     75 
                     76 .code
                     77 
                     78 __doprnt::
                     79 {
                     80 	.data
                     81 	.even
                     82 L54:
0024 01da'           83 	.word	L51
0026 01da'           84 	.word	L51
0028 01da'           85 	.word	L51
002a 01e0'           86 	.word	L46
002c 01e0'           87 	.word	L46
002e 01e0'           88 	.word	L46
0030 01e0'           89 	.word	L46
0032 01e0'           90 	.word	L46
0034 01e0'           91 	.word	L46
0036 01e0'           92 	.word	L46
0038 01e0'           93 	.word	L46
003a 01e0'           94 	.word	L46
003c 01e0'           95 	.word	L46
003e 01e0'           96 	.word	L46
0040 0200'           97 	.word	L52
                     98 	.even
                     99 L101:
0042 02a2'          100 	.word	L20027
0044 0286'          101 	.word	L20025
0046 02b0'          102 	.word	L80
0048 0294'          103 	.word	L20026
004a 03d4'          104 	.word	L87
004c 0248'          105 	.word	L63
004e 0402'          106 	.word	L91
0050 0402'          107 	.word	L91
0052 0402'          108 	.word	L91
0054 026e'          109 	.word	L65
0056 0206'          110 	.word	L57
0058 0396'          111 	.word	L83
005a 0216'          112 	.word	L60
005c 020e'          113 	.word	L59
                    114 	.code
0060 030f  0074     115 	sub	fp,#~L2
0064 5cf9  0b03     116 	ldm	108(fp),r11,#4
0068 006c  
006a 1cf9  0204     117 	ldm	@fp,r2,#5
006e a17e           118 	ld	r14,r7
0070 4df8  0012     119 	clr	~L1+18(fp)
0074 6ff6  0014     120 	ld	~L1+20(fp),r6
0078 5f00  0000*    121 	callr	_strlen
007c a12c           122 	ld	r12,r2
007e e812           123 	jpr	L27
                    124 L20003:
0080 61f2  0000     125 	ld	r2,~L1(fp)
0084 6b20  0002     126 	dec	2(r2),#1
0088 6123  0002     127 	ld	r3,2(r2)
008c 0b03  0000     128 	cp	r3,#0
0090 5e01  0498'    129 	jpr	lt,L20001
                    130 L20010:
0094 a1d2           131 	ld	r2,r13
0096 61f3  0000     132 	ld	r3,~L1(fp)
009a 2930           133 	inc	@r3,#1
009c 2134           134 	ld	r4,@r3
009e ab40           135 	dec	r4,#1
00a0 2e4a           136 	ldb	@r4,rl2
00a2 8222           137 	subb	rh2,rh2
                    138 L27:
00a4 a1c2           139 	ld	r2,r12
00a6 abc0           140 	dec	r12,#1
00a8 8d24           141 	test	r2
00aa 5e06  058a'    142 	jpr	eq,L10004
00ae 20ea           143 	ldb	rl2,@r14
00b0 b120           144 	extsb	r2
00b2 0702  00ff     145 	and	r2,#255
00b6 a12d           146 	ld	r13,r2
00b8 a9e0           147 	inc	r14,#1
00ba 0b02  0025     148 	cp	r2,#37
00be eee0           149 	jpr	ne,L20003
00c0 8dc4           150 	test	r12
00c2 5e06  058a'    151 	jpr	eq,L10004
00c6 abc0           152 	dec	r12,#1
00c8 20ea           153 	ldb	rl2,@r14
00ca b120           154 	extsb	r2
00cc 0702  00ff     155 	and	r2,#255
00d0 a12d           156 	ld	r13,r2
00d2 a9e0           157 	inc	r14,#1
00d4 4df8  0018     158 	clr	~L1+24(fp)
00d8 4df5  001a     159 	ld	~L1+26(fp),#-1
00dc ffff  
00de 4cf5  0069     160 	ldb	~L1+105(fp),#32
00e2 2020  
00e4 4df8  0064     161 	clr	~L1+100(fp)
00e8 0b02  002d     162 	cp	r2,#45
00ec ee0d           163 	jpr	ne,L32
00ee 69f0  0064     164 	inc	~L1+100(fp),#1
00f2 a1c2           165 	ld	r2,r12
00f4 abc0           166 	dec	r12,#1
00f6 8d24           167 	test	r2
00f8 5e06  058a'    168 	jpr	eq,L10004
00fc 20ea           169 	ldb	rl2,@r14
00fe b120           170 	extsb	r2
0100 0702  00ff     171 	and	r2,#255
0104 a12d           172 	ld	r13,r2
0106 a9e0           173 	inc	r14,#1
                    174 L32:
0108 0b0d  0061     175 	cp	r13,#97
010c e958           176 	jpr	ge,L34
010e 0b0d  002a     177 	cp	r13,#42
0112 ee12           178 	jpr	ne,L35
0114 61f2  0014     179 	ld	r2,~L1+20(fp)
0118 2123           180 	ld	r3,@r2
011a 6ff3  0018     181 	ld	~L1+24(fp),r3
011e 69f1  0014     182 	inc	~L1+20(fp),#2
0122 69f0  0002     183 	inc	~L1+2(fp),#1
0126 4df1  0002     184 	cp	~L1+2(fp),#5
012a 0005  
012c ee15           185 	jpr	ne,L37
012e 61f4  0004     186 	ld	r4,~L1+4(fp)
0132 6ff4  0014     187 	ld	~L1+20(fp),r4
0136 e810           188 	jpr	L37
                    189 L35:
0138 0b0d  0030     190 	cp	r13,#48
013c ee03           191 	jpr	ne,L38
013e 4cf5  0069     192 	ldb	~L1+105(fp),#48
0142 3030  
                    193 L38:
0144 76f5  0018     194 	lda	r5,~L1+24(fp)
0148 a9c0           195 	inc	r12,#1
014a a1c6           196 	ld	r6,r12
014c abe0           197 	dec	r14,#1
014e a1e7           198 	ld	r7,r14
0150 ddde           199 	callr	_getnum
0152 a12b           200 	ld	r11,r2
0154 812e           201 	add	r14,r2
0156 832c           202 	sub	r12,r2
                    203 L37:
0158 8dc4           204 	test	r12
015a 5e06  058a'    205 	jpr	eq,L10004
015e 0ce1  2e2e     206 	cpb	@r14,#46
0162 ee23           207 	jpr	ne,L40
0164 a9e0           208 	inc	r14,#1
0166 abc0           209 	dec	r12,#1
0168 8dc4           210 	test	r12
016a 5e06  058a'    211 	jpr	eq,L10004
016e 0ce1  2a2a     212 	cpb	@r14,#42
0172 ee13           213 	jpr	ne,L42
0174 61f2  0014     214 	ld	r2,~L1+20(fp)
0178 2123           215 	ld	r3,@r2
017a 6ff3  001a     216 	ld	~L1+26(fp),r3
017e 69f1  0014     217 	inc	~L1+20(fp),#2
0182 bdb1           218 	ldk	r11,#1
0184 69f0  0002     219 	inc	~L1+2(fp),#1
0188 4df1  0002     220 	cp	~L1+2(fp),#5
018c 0005  
018e ee0b           221 	jpr	ne,L44
0190 61f4  0004     222 	ld	r4,~L1+4(fp)
0194 6ff4  0014     223 	ld	~L1+20(fp),r4
0198 e806           224 	jpr	L44
                    225 L42:
019a 76f5  001a     226 	lda	r5,~L1+26(fp)
019e a1c6           227 	ld	r6,r12
01a0 a1e7           228 	ld	r7,r14
01a2 de07           229 	callr	_getnum
01a4 a12b           230 	ld	r11,r2
                    231 L44:
01a6 83bc           232 	sub	r12,r11
01a8 81be           233 	add	r14,r11
                    234 L40:
01aa 8dc4           235 	test	r12
01ac 5e06  058a'    236 	jpr	eq,L10004
01b0 abc0           237 	dec	r12,#1
01b2 20ea           238 	ldb	rl2,@r14
01b4 b120           239 	extsb	r2
01b6 0702  00ff     240 	and	r2,#255
01ba a12d           241 	ld	r13,r2
01bc a9e0           242 	inc	r14,#1
                    243 L34:
01be 4df1  001a     244 	cp	~L1+26(fp),#0
01c2 0000  
01c4 e90d           245 	jpr	ge,L46
01c6 a1d2           246 	ld	r2,r13
01c8 0302  0065     247 	sub	r2,#101
01cc 0b02  000e     248 	cp	r2,#14
01d0 eb07           249 	jpr	ugt,L46
01d2 b320           250 	rl	r2,#1
01d4 6122  0024'    251 	ld	r2,L54(r2)
01d8 1e28           252 	jp	@r2
                    253 L51:
01da 4df5  001a     254 	ld	~L1+26(fp),#6
01de 0006  
                    255 L46:
01e0 6ffd  006a     256 	ld	~L1+106(fp),r13
01e4 a1d2           257 	ld	r2,r13
01e6 2103  0008'    258 	ld	r3,#L100
01ea 2104  000e     259 	ld	r4,#14
01ee bb34  0426     260 	cpir	r2,@r3,r4,eq
01f2 5e0e  0484'    261 	jpr	ne,L99
01f6 8144           262 	add	r4,r4
01f8 8d42           263 	neg	r4
01fa 6142  005c'    264 	ld	r2,L101+26(r4)
01fe 1e28           265 	jp	@r2
                    266 L52:
0200 4df8  001a     267 	clr	~L1+26(fp)
0204 e8ed           268 	jpr	L46
                    269 L57:
0206 4df5  001c     270 	ld	~L1+28(fp),#8
020a 0008  
020c e807           271 	jpr	L58
                    272 L59:
020e 4df5  001c     273 	ld	~L1+28(fp),#16
0212 0010  
0214 e803           274 	jpr	L58
                    275 L60:
0216 4df5  001c     276 	ld	~L1+28(fp),#10
021a 000a  
                    277 L58:
021c 61f3  0014     278 	ld	r3,~L1+20(fp)
0220 2134           279 	ld	r4,@r3
0222 a147           280 	ld	r7,r4
0224 b16a           281 	exts	rr6
0226 5df6  001e     282 	ldl	~L1+30(fp),rr6
022a 69f1  0014     283 	inc	~L1+20(fp),#2
022e 4df8  001e     284 	clr	~L1+30(fp)
0232 69f0  0002     285 	inc	~L1+2(fp),#1
0236 4df1  0002     286 	cp	~L1+2(fp),#5
023a 0005  
023c ee58           287 	jpr	ne,L62
                    288 L20024:
023e 61f4  0004     289 	ld	r4,~L1+4(fp)
0242 6ff4  0014     290 	ld	~L1+20(fp),r4
0246 e853           291 	jpr	L62
                    292 L63:
0248 4df5  001c     293 	ld	~L1+28(fp),#-10
024c fff6  
024e 61f3  0014     294 	ld	r3,~L1+20(fp)
0252 2134           295 	ld	r4,@r3
0254 a147           296 	ld	r7,r4
0256 b16a           297 	exts	rr6
0258 5df6  001e     298 	ldl	~L1+30(fp),rr6
025c 69f1  0014     299 	inc	~L1+20(fp),#2
0260 69f0  0002     300 	inc	~L1+2(fp),#1
0264 4df1  0002     301 	cp	~L1+2(fp),#5
0268 0005  
026a e6e9           302 	jpr	eq,L20024
026c e840           303 	jpr	L62
                    304 L65:
026e 8dc4           305 	test	r12
0270 e672           306 	jpr	eq,L67
0272 abc0           307 	dec	r12,#1
0274 20eb           308 	ldb	rl3,@r14
0276 b130           309 	extsb	r3
0278 0703  00ff     310 	and	r3,#255
027c a13d           311 	ld	r13,r3
027e a9e0           312 	inc	r14,#1
0280 0b03  006f     313 	cp	r3,#111
0284 ee04           314 	jpr	ne,L68
                    315 L20025:
0286 4df5  001c     316 	ld	~L1+28(fp),#8
028a 0008  
028c e814           317 	jpr	L76
                    318 L68:
028e 0b0d  0078     319 	cp	r13,#120
0292 ee04           320 	jpr	ne,L70
                    321 L20026:
0294 4df5  001c     322 	ld	~L1+28(fp),#16
0298 0010  
029a e80d           323 	jpr	L76
                    324 L70:
029c 0b0d  0064     325 	cp	r13,#100
02a0 ee04           326 	jpr	ne,L72
                    327 L20027:
02a2 4df5  001c     328 	ld	~L1+28(fp),#-10
02a6 fff6  
02a8 e806           329 	jpr	L76
                    330 L72:
02aa 0b0d  0075     331 	cp	r13,#117
02ae ee2c           332 	jpr	ne,L74
                    333 L80:
02b0 4df5  001c     334 	ld	~L1+28(fp),#10
02b4 000a  
                    335 L76:
02b6 4df1  0002     336 	cp	~L1+2(fp),#5
02ba 0005  
02bc e907           337 	jpr	ge,L81
02be 76f7  0002     338 	lda	r7,~L1+2(fp)
02c2 76f6  0014     339 	lda	r6,~L1+20(fp)
02c6 61f5  0004     340 	ld	r5,~L1+4(fp)
02ca d166           341 	callr	_fix
                    342 L81:
02cc 61f2  0014     343 	ld	r2,~L1+20(fp)
02d0 1424           344 	ldl	rr4,@r2
02d2 5df4  001e     345 	ldl	~L1+30(fp),rr4
02d6 69f3  0014     346 	inc	~L1+20(fp),#4
02da 69f1  0002     347 	inc	~L1+2(fp),#2
02de 4df1  0002     348 	cp	~L1+2(fp),#5
02e2 0005  
02e4 ee04           349 	jpr	ne,L62
02e6 61f3  0004     350 	ld	r3,~L1+4(fp)
02ea 6ff3  0014     351 	ld	~L1+20(fp),r3
                    352 L62:
02ee 76f2  0024     353 	lda	r2,~L1+36(fp)
02f2 6ff2  0022     354 	ld	~L1+34(fp),r2
02f6 a127           355 	ld	r7,r2
02f8 61f6  001c     356 	ld	r6,~L1+28(fp)
02fc 54f4  001e     357 	ldl	rr4,~L1+30(fp)
0300 5f00  0000*    358 	callr	__ltostr
                    359 L20028:
0304 a12b           360 	ld	r11,r2
0306 e827           361 	jpr	L67
                    362 L74:
0308 61f3  0000     363 	ld	r3,~L1(fp)
030c 6b30  0002     364 	dec	2(r3),#1
0310 6134  0002     365 	ld	r4,2(r3)
0314 0b04  0000     366 	cp	r4,#0
0318 e907           367 	jpr	ge,L10009
031a 2107  006c     368 	ld	r7,#108
031e 61f6  0000     369 	ld	r6,~L1(fp)
0322 5f00  0000*    370 	callr	__flsbuf
0326 e809           371 	jpr	L10010
                    372 L10009:
0328 61f2  0000     373 	ld	r2,~L1(fp)
032c 2920           374 	inc	@r2,#1
032e 2123           375 	ld	r3,@r2
0330 ab30           376 	dec	r3,#1
0332 0c35  6c6c     377 	ldb	@r3,#108
0336 ca6c           378 	ldb	rl2,#108
0338 8222           379 	subb	rh2,rh2
                    380 L10010:
033a 61f2  0000     381 	ld	r2,~L1(fp)
033e 6b20  0002     382 	dec	2(r2),#1
0342 6123  0002     383 	ld	r3,2(r2)
0346 0b03  0000     384 	cp	r3,#0
034a e91c           385 	jpr	ge,L10011
034c a1d7           386 	ld	r7,r13
034e 61f6  0000     387 	ld	r6,~L1(fp)
0352 5f00  0000*    388 	callr	__flsbuf
                    389 L67:
0356 4df4  001a     390 	test	~L1+26(fp)
035a e609           391 	jpr	eq,L102
035c 4df1  006a     392 	cp	~L1+106(fp),#115
0360 0073  
0362 ee05           393 	jpr	ne,L102
0364 4bfb  001a     394 	cp	r11,~L1+26(fp)
0368 e202           395 	jpr	le,L102
036a 61fb  001a     396 	ld	r11,~L1+26(fp)
                    397 L102:
036e 4bfb  0018     398 	cp	r11,~L1+24(fp)
0372 5e09  04a6'    399 	jpr	ge,L103
0376 61f2  0018     400 	ld	r2,~L1+24(fp)
037a 83b2           401 	sub	r2,r11
037c 6ff2  0066     402 	ld	~L1+102(fp),r2
0380 5e08  04aa'    403 	jpr	L104
                    404 L10011:
0384 a1d2           405 	ld	r2,r13
0386 61f3  0000     406 	ld	r3,~L1(fp)
038a 2930           407 	inc	@r3,#1
038c 2134           408 	ld	r4,@r3
038e ab40           409 	dec	r4,#1
0390 2e4a           410 	ldb	@r4,rl2
0392 8222           411 	subb	rh2,rh2
0394 e8e0           412 	jpr	L67
                    413 L83:
0396 4cf5  0069     414 	ldb	~L1+105(fp),#32
039a 2020  
039c 61f2  0014     415 	ld	r2,~L1+20(fp)
03a0 2123           416 	ld	r3,@r2
03a2 6ff3  0022     417 	ld	~L1+34(fp),r3
03a6 69f1  0014     418 	inc	~L1+20(fp),#2
03aa 8d34           419 	test	r3
03ac ee03           420 	jpr	ne,L84
03ae 4df5  0022     421 	ld	~L1+34(fp),#L85
03b2 0000' 
                    422 L84:
03b4 61f7  0022     423 	ld	r7,~L1+34(fp)
03b8 5f00  0000*    424 	callr	_strlen
03bc a12b           425 	ld	r11,r2
03be 69f0  0002     426 	inc	~L1+2(fp),#1
03c2 4df1  0002     427 	cp	~L1+2(fp),#5
03c6 0005  
03c8 eec6           428 	jpr	ne,L67
03ca 61f2  0004     429 	ld	r2,~L1+4(fp)
03ce 6ff2  0014     430 	ld	~L1+20(fp),r2
03d2 e8c1           431 	jpr	L67
                    432 L87:
03d4 4cf5  0069     433 	ldb	~L1+105(fp),#32
03d8 2020  
03da bdb1           434 	ldk	r11,#1
03dc 61f2  0014     435 	ld	r2,~L1+20(fp)
03e0 6ff2  0022     436 	ld	~L1+34(fp),r2
03e4 69f1  0014     437 	inc	~L1+20(fp),#2
03e8 69f0  0022     438 	inc	~L1+34(fp),#1
03ec 69f0  0002     439 	inc	~L1+2(fp),#1
03f0 4df1  0002     440 	cp	~L1+2(fp),#5
03f4 0005  
03f6 eeaf           441 	jpr	ne,L67
03f8 61f3  0004     442 	ld	r3,~L1+4(fp)
03fc 6ff3  0014     443 	ld	~L1+20(fp),r3
0400 e8aa           444 	jpr	L67
                    445 L91:
0402 4df1  0002     446 	cp	~L1+2(fp),#5
0406 0005  
0408 e91e           447 	jpr	ge,L92
040a 4df1  0012     448 	cp	~L1+18(fp),#4
040e 0004  
0410 e90d           449 	jpr	ge,L93
0412 61f2  0014     450 	ld	r2,~L1+20(fp)
0416 6ff2  0016     451 	ld	~L1+22(fp),r2
041a 61f3  0006     452 	ld	r3,~L1+6(fp)
041e 6ff3  0014     453 	ld	~L1+20(fp),r3
0422 69f7  0006     454 	inc	~L1+6(fp),#8
0426 69f0  0012     455 	inc	~L1+18(fp),#1
042a e80f           456 	jpr	L96
                    457 L93:
042c 4df1  0012     458 	cp	~L1+18(fp),#4
0430 0004  
0432 ee0b           459 	jpr	ne,L96
0434 4df8  0016     460 	clr	~L1+22(fp)
0438 61f2  0004     461 	ld	r2,~L1+4(fp)
043c 6ff2  0014     462 	ld	~L1+20(fp),r2
0440 69f4  0002     463 	inc	~L1+2(fp),#5
0444 e802           464 	jpr	L96
                    465 L92:
0446 4df8  0016     466 	clr	~L1+22(fp)
                    467 L96:
044a 76f5  0024     468 	lda	r5,~L1+36(fp)
044e 61f7  001a     469 	ld	r7,~L1+26(fp)
0452 61f6  006a     470 	ld	r6,~L1+106(fp)
0456 61f4  0014     471 	ld	r4,~L1+20(fp)
045a 0f44  8303     472 	fldd	f3,@r4
045e df28           473 	callr	_fcv
0460 6ff2  0022     474 	ld	~L1+34(fp),r2
0464 4df4  0016     475 	test	~L1+22(fp)
0468 e605           476 	jpr	eq,L97
046a 61f2  0016     477 	ld	r2,~L1+22(fp)
046e 6ff2  0014     478 	ld	~L1+20(fp),r2
0472 e802           479 	jpr	L98
                    480 L97:
0474 69f7  0014     481 	inc	~L1+20(fp),#8
                    482 L98:
0478 61f7  0022     483 	ld	r7,~L1+34(fp)
047c 5f00  0000*    484 	callr	_strlen
0480 5e08  0304'    485 	jpr	L20028
                    486 L99:
0484 61f2  0000     487 	ld	r2,~L1(fp)
0488 6b20  0002     488 	dec	2(r2),#1
048c 6123  0002     489 	ld	r3,2(r2)
0490 0b03  0000     490 	cp	r3,#0
0494 5e09  0094'    491 	jpr	ge,L20010
                    492 L20001:
0498 a1d7           493 	ld	r7,r13
049a 61f6  0000     494 	ld	r6,~L1(fp)
049e 5f00  0000*    495 	callr	__flsbuf
04a2 5e08  00a4'    496 	jpr	L27
                    497 L103:
04a6 4df8  0066     498 	clr	~L1+102(fp)
                    499 L104:
04aa 4df4  0064     500 	test	~L1+100(fp)
04ae e61b           501 	jpr	eq,L106
04b0 e842           502 	jpr	L108
                    503 L20012:
04b2 60ff  0069     504 	ldb	rl7,~L1+105(fp)
04b6 b170           505 	extsb	r7
04b8 61f6  0000     506 	ld	r6,~L1(fp)
04bc 5f00  0000*    507 	callr	__flsbuf
04c0 e812           508 	jpr	L106
                    509 L20014:
04c2 61f2  0000     510 	ld	r2,~L1(fp)
04c6 6b20  0002     511 	dec	2(r2),#1
04ca 6123  0002     512 	ld	r3,2(r2)
04ce 0b03  0000     513 	cp	r3,#0
04d2 e1ef           514 	jpr	lt,L20012
04d4 61f2  0000     515 	ld	r2,~L1(fp)
04d8 2920           516 	inc	@r2,#1
04da 2123           517 	ld	r3,@r2
04dc ab30           518 	dec	r3,#1
04de 60fa  0069     519 	ldb	rl2,~L1+105(fp)
04e2 2e3a           520 	ldb	@r3,rl2
04e4 8222           521 	subb	rh2,rh2
                    522 L106:
04e6 61f2  0066     523 	ld	r2,~L1+102(fp)
04ea 6bf0  0066     524 	dec	~L1+102(fp),#1
04ee 8d24           525 	test	r2
04f0 eee8           526 	jpr	ne,L20014
04f2 e821           527 	jpr	L108
                    528 L20016:
04f4 61f7  0022     529 	ld	r7,~L1+34(fp)
04f8 69f0  0022     530 	inc	~L1+34(fp),#1
04fc 207e           531 	ldb	rl6,@r7
04fe b160           532 	extsb	r6
0500 a167           533 	ld	r7,r6
0502 61f6  0000     534 	ld	r6,~L1(fp)
0506 5f00  0000*    535 	callr	__flsbuf
050a e815           536 	jpr	L108
                    537 L20018:
050c 61f2  0000     538 	ld	r2,~L1(fp)
0510 6b20  0002     539 	dec	2(r2),#1
0514 6123  0002     540 	ld	r3,2(r2)
0518 0b03  0000     541 	cp	r3,#0
051c e1eb           542 	jpr	lt,L20016
051e 61f2  0022     543 	ld	r2,~L1+34(fp)
0522 69f0  0022     544 	inc	~L1+34(fp),#1
0526 61f3  0000     545 	ld	r3,~L1(fp)
052a 2930           546 	inc	@r3,#1
052c 2134           547 	ld	r4,@r3
052e ab40           548 	dec	r4,#1
0530 202b           549 	ldb	rl3,@r2
0532 2e4b           550 	ldb	@r4,rl3
0534 8233           551 	subb	rh3,rh3
                    552 L108:
0536 a1b2           553 	ld	r2,r11
0538 abb0           554 	dec	r11,#1
053a 8d24           555 	test	r2
053c eee7           556 	jpr	ne,L20018
053e 4df4  0064     557 	test	~L1+100(fp)
0542 ee09           558 	jpr	ne,L111
0544 5e08  00a4'    559 	jpr	L27
                    560 L20020:
0548 60ff  0069     561 	ldb	rl7,~L1+105(fp)
054c b170           562 	extsb	r7
054e 61f6  0000     563 	ld	r6,~L1(fp)
0552 5f00  0000*    564 	callr	__flsbuf
                    565 L111:
0556 61f2  0066     566 	ld	r2,~L1+102(fp)
055a 6bf0  0066     567 	dec	~L1+102(fp),#1
055e 8d24           568 	test	r2
0560 5e06  00a4'    569 	jpr	eq,L27
0564 61f2  0000     570 	ld	r2,~L1(fp)
0568 6b20  0002     571 	dec	2(r2),#1
056c 6123  0002     572 	ld	r3,2(r2)
0570 0b03  0000     573 	cp	r3,#0
0574 e1e9           574 	jpr	lt,L20020
0576 61f2  0000     575 	ld	r2,~L1(fp)
057a 2920           576 	inc	@r2,#1
057c 2123           577 	ld	r3,@r2
057e ab30           578 	dec	r3,#1
0580 60fa  0069     579 	ldb	rl2,~L1+105(fp)
0584 2e3a           580 	ldb	@r3,rl2
0586 8222           581 	subb	rh2,rh2
0588 e8e6           582 	jpr	L111
                    583 L10004:
058a 5cf1  0b03     584 	ldm	r11,108(fp),#4
058e 006c  
0590 010f  0074     585 	add	fp,#~L2
0594 9e08           586 	ret
                    587 	.extern	__fltuse
                    588 	~L1 := 0
                    589 	~L2 := 116
                    590 	}	/* __doprnt */
                    591 
                    592 .data
                    593 
                    594 
                    595 .code
                    596 
                    597 _getnum:
                    598 {
0596 030f  0012     599 	sub	fp,#~L2
059a 1cf9  0501     600 	ldm	@fp,r5,#2
059e a171           601 	ld	r1,r7
05a0 a164           602 	ld	r4,r6
05a2 4df8  0006     603 	clr	~L1+6(fp)
05a6 8d64           604 	test	r6
05a8 e609           605 	jpr	eq,L114
05aa 0c11  2d2d     606 	cpb	@r1,#45
05ae ee06           607 	jpr	ne,L114
05b0 6bf0  0002     608 	dec	~L1+2(fp),#1
05b4 a910           609 	inc	r1,#1
05b6 4df5  0006     610 	ld	~L1+6(fp),#1
05ba 0001  
                    611 L114:
05bc bd50           612 	ldk	r5,#0
05be e815           613 	jpr	L115
                    614 L20030:
05c0 201a           615 	ldb	rl2,@r1
05c2 6efa  0009     616 	ldb	~L1+9(fp),rl2
05c6 a910           617 	inc	r1,#1
05c8 0a0a  3030     618 	cpb	rl2,#48
05cc e110           619 	jpr	lt,L116
05ce 0a0a  3939     620 	cpb	rl2,#57
05d2 ea0d           621 	jpr	gt,L116
05d4 a153           622 	ld	r3,r5
05d6 1902  000a     623 	mult	rr2,#10
05da 60fa  0009     624 	ldb	rl2,~L1+9(fp)
05de b120           625 	extsb	r2
05e0 0302  0030     626 	sub	r2,#48
05e4 8123           627 	add	r3,r2
05e6 a135           628 	ld	r5,r3
05e8 ab40           629 	dec	r4,#1
                    630 L115:
05ea 8d44           631 	test	r4
05ec eee9           632 	jpr	ne,L20030
                    633 L116:
05ee 4df4  0006     634 	test	~L1+6(fp)
05f2 e603           635 	jpr	eq,L10025
05f4 a152           636 	ld	r2,r5
05f6 8d22           637 	neg	r2
05f8 e801           638 	jpr	L10026
                    639 L10025:
05fa a152           640 	ld	r2,r5
                    641 L10026:
05fc 61f3  0000     642 	ld	r3,~L1(fp)
0600 2f32           643 	ld	@r3,r2
0602 61f2  0002     644 	ld	r2,~L1+2(fp)
0606 8342           645 	sub	r2,r4
0608 010f  0012     646 	add	fp,#~L2
060c 9e08           647 	ret
                    648 	~L1 := 0
                    649 	~L2 := 18
                    650 	}	/* _getnum */
                    651 
                    652 .data
                    653 
                    654 
                    655 .code
                    656 
                    657 _ecv:
                    658 {
060e 9e08           659 	ret
                    660 	~L1 := 0
                    661 	~L2 := 0
                    662 	}	/* _ecv */
                    663 
                    664 .data
                    665 
                    666 
                    667 .code
                    668 
                    669 _fcv:
                    670 {
0610 9e08           671 	ret
                    672 	~L1 := 0
                    673 	~L2 := 0
                    674 	}	/* _fcv */
                    675 
                    676 .data
                    677 
                    678 
                    679 .code
                    680 
                    681 _gcv:
                    682 {
0612 9e08           683 	ret
                    684 	~L1 := 0
                    685 	~L2 := 0
                    686 	}	/* _gcv */
                    687 
                    688 .data
                    689 
