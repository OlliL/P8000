LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 _shell:
0000 2f               5 .byte	%2f
0001 62               6 .byte	%62
0002 69               7 .byte	%69
0003 6e               8 .byte	%6e
0004 2f               9 .byte	%2f
0005 73              10 .byte	%73
0006 68              11 .byte	%68
0007 00              12 .byte	%0
                     13 _cshell:
0008 2f              14 .byte	%2f
0009 62              15 .byte	%62
000a 69              16 .byte	%69
000b 6e              17 .byte	%6e
000c 2f              18 .byte	%2f
000d 63              19 .byte	%63
000e 73              20 .byte	%73
000f 68              21 .byte	%68
0010 00              22 .byte	%0
0011 00              23 .byte	0
                     24 L19:
0012 50  41  54      25 .byte	%50,%41,%54,%48,%0
0015 48  00  
                     26 .even
                     27 L21:
0018 3a  2f  62      28 .byte	%3a,%2f,%62,%69,%6e,%3a,%2f,%75
001b 69  6e  3a  
001e 2f  75  
0020 73  72  2f      29 .byte	%73,%72,%2f,%62,%69,%6e,%0
0023 62  69  6e  
0026 00  
                     30 .even
                     31 L22:
0028 00              32 .byte	%0
                     33 .even
                     34 L32:
002a 73  68  00      35 .byte	%73,%68,%0
                     36 .even
                     37 L34:
002e 63  73  68      38 .byte	%63,%73,%68,%0
0031 00  
                     39 .even
                     40 .even
                     41 L46:
0032 0007            42 .word	7
0034 0008            43 .word	8
0036 000c            44 .word	12
0038 000d            45 .word	13
003a 001a            46 .word	26
                     47 .even
                     48 /*~~execvp:*/
                     49 
                     50 .code
                     51 
                     52 _execvp::
                     53 {
                     54 	.data
                     55 	.even
                     56 L47:
003c 010a'           57 	.word	L44
003e 0078'           58 	.word	L30
0040 010a'           59 	.word	L44
0042 00fa'           60 	.word	L42
0044 00ea'           61 	.word	L39
                     62 	.code
0000 030f  0292      63 	sub	fp,#~L2
0004 5cf9  0c02      64 	ldm	652(fp),r12,#3
0008 028c  
000a 1df6            65 	ldl	@fp,rr6
000c bdd1            66 	ldk	r13,#1
000e bdc0            67 	ldk	r12,#0
0010 2107  0012'     68 	ld	r7,#L19
0014 5f00  0000*     69 	callr	_getenv
0018 6ff2  0004      70 	ld	~L1+4(fp),r2
001c 8d24            71 	test	r2
001e ee03            72 	jpr	ne,L20
0020 4df5  0004      73 	ld	~L1+4(fp),#L21
0024 0018' 
                     74 L20:
0026 61f7  0002      75 	ld	r7,~L1+2(fp)
002a 2106  002f      76 	ld	r6,#47
002e 5f00  0000*     77 	callr	_index
0032 8d24            78 	test	r2
0034 e603            79 	jpr	eq,L10003
0036 2102  0028'     80 	ld	r2,#L22
003a e802            81 	jpr	L10004
                     82 L10003:
003c 61f2  0004      83 	ld	r2,~L1+4(fp)
                     84 L10004:
0040 a12e            85 	ld	r14,r2
                     86 L25:
0042 76f5  0008      87 	lda	r5,~L1+8(fp)
0046 61f6  0002      88 	ld	r6,~L1+2(fp)
004a a1e7            89 	ld	r7,r14
004c df9a            90 	callr	_execat
004e a12e            91 	ld	r14,r2
                     92 L26:
0050 76f7  0008      93 	lda	r7,~L1+8(fp)
0054 61f6  0000      94 	ld	r6,~L1(fp)
0058 5f00  0000*     95 	callr	_execv
005c 6102  0000*     96 	ld	r2,_errno
0060 2103  0032'     97 	ld	r3,#L46
0064 2104  0005      98 	ld	r4,#5
0068 bb34  0426      99 	cpir	r2,@r3,r4,eq
006c ee47           100 	jpr	ne,L24
006e 8144           101 	add	r4,r4
0070 8d42           102 	neg	r4
0072 6142  0044'    103 	ld	r2,L47+8(r4)
0076 1e28           104 	jp	@r2
                    105 L30:
0078 76f7  0008     106 	lda	r7,~L1+8(fp)
007c df8b           107 	callr	_whichshell
007e 6ff2  0006     108 	ld	~L1+6(fp),r2
0082 0b02  0000'    109 	cp	r2,#_shell
0086 ee04           110 	jpr	ne,L31
0088 4df5  0088     111 	ld	~L1+136(fp),#L32
008c 002a' 
008e e803           112 	jpr	L33
                    113 L31:
0090 4df5  0088     114 	ld	~L1+136(fp),#L34
0094 002e' 
                    115 L33:
0096 76f2  0008     116 	lda	r2,~L1+8(fp)
009a 6ff2  008a     117 	ld	~L1+138(fp),r2
009e 4df5  0288     118 	ld	~L1+648(fp),#1
00a2 0001  
00a4 e80a           119 	jpr	L37
                    120 L20001:
00a6 4df1  0288     121 	cp	~L1+648(fp),#254
00aa 00fe  
00ac e104           122 	jpr	lt,L35
00ae 4d05  0000*    123 	ld	_errno,#7
00b2 0007  
00b4 e82a           124 	jpr	L44
                    125 L35:
00b6 69f0  0288     126 	inc	~L1+648(fp),#1
                    127 L37:
00ba 61f2  0288     128 	ld	r2,~L1+648(fp)
00be a920           129 	inc	r2,#1
00c0 8122           130 	add	r2,r2
00c2 76f3  0088     131 	lda	r3,~L1+136(fp)
00c6 61f4  0288     132 	ld	r4,~L1+648(fp)
00ca 8144           133 	add	r4,r4
00cc 61f5  0000     134 	ld	r5,~L1(fp)
00d0 8145           135 	add	r5,r4
00d2 8123           136 	add	r3,r2
00d4 2152           137 	ld	r2,@r5
00d6 2f32           138 	ld	@r3,r2
00d8 8d24           139 	test	r2
00da eee5           140 	jpr	ne,L20001
00dc 76f6  0088     141 	lda	r6,~L1+136(fp)
00e0 61f7  0006     142 	ld	r7,~L1+6(fp)
00e4 5f00  0000*    143 	callr	_execv
00e8 e810           144 	jpr	L44
                    145 L39:
00ea a9d0           146 	inc	r13,#1
00ec 0b0d  0005     147 	cp	r13,#5
00f0 eb0c           148 	jpr	ugt,L44
00f2 a1d7           149 	ld	r7,r13
00f4 5f00  0000*    150 	callr	_sleep
00f8 e8ab           151 	jpr	L26
                    152 L42:
00fa a9c0           153 	inc	r12,#1
                    154 L24:
00fc 8de4           155 	test	r14
00fe eea1           156 	jpr	ne,L25
0100 8dc4           157 	test	r12
0102 e603           158 	jpr	eq,L44
0104 4d05  0000*    159 	ld	_errno,#13
0108 000d  
                    160 L44:
010a 2102  ffff     161 	ld	r2,#-1
010e 5cf1  0c02     162 	ldm	r12,652(fp),#3
0112 028c  
0114 010f  0292     163 	add	fp,#~L2
0118 9e08           164 	ret
                    165 	~L1 := 0
                    166 	~L2 := 658
                    167 	}	/* _execvp */
                    168 
                    169 .data
                    170 
                    171 
                    172 .code
                    173 
                    174 _execat:
                    175 {
011a abfd           176 	dec	fp,#~L2
011c 2ff5           177 	ld	@fp,r5
011e a163           178 	ld	r3,r6
0120 a171           179 	ld	r1,r7
0122 a154           180 	ld	r4,r5
0124 e80a           181 	jpr	L50
                    182 L20003:
0126 0c11  3a3a     183 	cpb	@r1,#58
012a e609           184 	jpr	eq,L51
012c 0c11  2d2d     185 	cpb	@r1,#45
0130 e606           186 	jpr	eq,L51
0132 201a           187 	ldb	rl2,@r1
0134 2e4a           188 	ldb	@r4,rl2
0136 a910           189 	inc	r1,#1
0138 a940           190 	inc	r4,#1
                    191 L50:
013a 0c14           192 	testb	@r1
013c eef4           193 	jpr	ne,L20003
                    194 L51:
013e 4bf4  0000     195 	cp	r4,~L1(fp)
0142 e60a           196 	jpr	eq,L53
0144 0c45  2f2f     197 	ldb	@r4,#47
0148 e806           198 	jpr	L20004
                    199 L20008:
014a a910           200 	inc	r1,#1
014c a112           201 	ld	r2,r1
014e e80a           202 	jpr	L10005
                    203 L20006:
0150 203a           204 	ldb	rl2,@r3
0152 2e4a           205 	ldb	@r4,rl2
0154 a930           206 	inc	r3,#1
                    207 L20004:
0156 a940           208 	inc	r4,#1
                    209 L53:
0158 0c34           210 	testb	@r3
015a eefa           211 	jpr	ne,L20006
015c 0c48           212 	clrb	@r4
015e 0c14           213 	testb	@r1
0160 eef4           214 	jpr	ne,L20008
0162 8322           215 	sub	r2,r2
                    216 L10005:
0164 a9fd           217 	inc	fp,#~L2
0166 9e08           218 	ret
                    219 	~L1 := 0
                    220 	~L2 := 14
                    221 	}	/* _execat */
                    222 
                    223 .data
                    224 
                    225 
                    226 .code
                    227 
                    228 _whichshell:
                    229 {
0168 abf9           230 	dec	fp,#~L2
016a 5cf9  0d01     231 	ldm	6(fp),r13,#2
016e 0006  
0170 a17e           232 	ld	r14,r7
0172 8366           233 	sub	r6,r6
0174 5f00  0000*    234 	callr	_open
0178 a12d           235 	ld	r13,r2
017a 0b02  0000     236 	cp	r2,#0
017e e904           237 	jpr	ge,L57
0180 a127           238 	ld	r7,r2
0182 5f00  0000*    239 	callr	_close
0186 e810           240 	jpr	L60
                    241 L57:
0188 76f6  0003     242 	lda	r6,~L1+3(fp)
018c a1d7           243 	ld	r7,r13
018e bd51           244 	ldk	r5,#1
0190 5f00  0000*    245 	callr	_read
0194 a1d7           246 	ld	r7,r13
0196 5f00  0000*    247 	callr	_close
019a 4cf1  0003     248 	cpb	~L1+3(fp),#35
019e 2323  
01a0 ee03           249 	jpr	ne,L60
01a2 2102  0008'    250 	ld	r2,#_cshell
01a6 e802           251 	jpr	L10010
                    252 L60:
01a8 2102  0000'    253 	ld	r2,#_shell
                    254 L10010:
01ac 5cf1  0d01     255 	ldm	r13,6(fp),#2
01b0 0006  
01b2 a9f9           256 	inc	fp,#~L2
01b4 9e08           257 	ret
                    258 	~L1 := 0
                    259 	~L2 := 10
                    260 	}	/* _whichshell */
                    261 
                    262 .data
                    263 
