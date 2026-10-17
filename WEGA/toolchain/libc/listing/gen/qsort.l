LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 .even
                      5 _qscmp:
0000 0000             6 .word	0
                      7 .even
                      8 _qses:
0002 0000             9 .word	0
                     10 /*~~qsort:*/
                     11 
                     12 .code
                     13 
                     14 _qsort::
                     15 {
0000 abf7            16 	dec	fp,#~L2
0002 1cf9  0403      17 	ldm	@fp,r4,#4
0006 6f04  0000'     18 	ld	_qscmp,r4
000a 6f05  0002'     19 	ld	_qses,r5
000e a163            20 	ld	r3,r6
0010 9952            21 	mult	rr2,r5
0012 8137            22 	add	r7,r3
0014 a176            23 	ld	r6,r7
0016 61f7  0006      24 	ld	r7,~L1+6(fp)
001a dffe            25 	callr	_qs1
001c a9f7            26 	inc	fp,#~L2
001e 9e08            27 	ret
                     28 	~L1 := 0
                     29 	~L2 := 8
                     30 	}	/* _qsort */
                     31 
                     32 .data
                     33 
                     34 
                     35 .code
                     36 
                     37 _qs1:
                     38 {
0020 030f  0018      39 	sub	fp,#~L2
0024 5cf9  0c02      40 	ldm	18(fp),r12,#3
0028 0012  
002a 1df6            41 	ldl	@fp,rr6
002c 610c  0002'     42 	ld	r12,_qses
                     43 L16:
0030 61f2  0000      44 	ld	r2,~L1(fp)
0034 43f2  0002      45 	sub	r2,~L1+2(fp)
0038 6ff2  000e      46 	ld	~L1+14(fp),r2
003c 8b2c            47 	cp	r12,r2
003e 5e0f  017a'     48 	jpr	uge,L10003
0042 a1c2            49 	ld	r2,r12
0044 81c2            50 	add	r2,r12
0046 61f5  000e      51 	ld	r5,~L1+14(fp)
004a bd40            52 	ldk	r4,#0
004c 9b24            53 	div	rr4,r2
004e a1c3            54 	ld	r3,r12
0050 9952            55 	mult	rr2,r5
0052 6ff3  000e      56 	ld	~L1+14(fp),r3
0056 61f2  0002      57 	ld	r2,~L1+2(fp)
005a 8132            58 	add	r2,r3
005c 6ff2  0008      59 	ld	~L1+8(fp),r2
0060 6ff2  000a      60 	ld	~L1+10(fp),r2
0064 61fe  0002      61 	ld	r14,~L1+2(fp)
0068 61f2  0000      62 	ld	r2,~L1(fp)
006c 83c2            63 	sub	r2,r12
                     64 L20011:
006e a12d            65 	ld	r13,r2
0070 e819            66 	jpr	L20
                     67 L20001:
0072 61f6  0008      68 	ld	r6,~L1+8(fp)
0076 83c6            69 	sub	r6,r12
0078 6ff6  0008      70 	ld	~L1+8(fp),r6
007c a1e7            71 	ld	r7,r14
007e df7d            72 	callr	_qsexc
0080 e811            73 	jpr	L20
                     74 L20003:
0082 6102  0000'     75 	ld	r2,_qscmp
0086 6ff2  0010      76 	ld	~L1+16(fp),r2
008a a1e7            77 	ld	r7,r14
008c 61f6  0008      78 	ld	r6,~L1+8(fp)
0090 a121            79 	ld	r1,r2
0092 1f10            80 	call	@r1
0094 6ff2  000c      81 	ld	~L1+12(fp),r2
0098 8d24            82 	test	r2
009a e6eb            83 	jpr	eq,L20001
009c 0b02  0000      84 	cp	r2,#0
00a0 e935            85 	jpr	ge,L25
                     86 L20008:
00a2 81ce            87 	add	r14,r12
                     88 L20:
00a4 4bfe  0008      89 	cp	r14,~L1+8(fp)
00a8 e7ec            90 	jpr	ult,L20003
00aa e830            91 	jpr	L25
                     92 L20005:
00ac 61f7  000a      93 	ld	r7,~L1+10(fp)
00b0 81c7            94 	add	r7,r12
00b2 6ff7  000a      95 	ld	~L1+10(fp),r7
00b6 a1d6            96 	ld	r6,r13
00b8 df9a            97 	callr	_qsexc
00ba e828            98 	jpr	L25
                     99 L20007:
00bc 61f6  000a     100 	ld	r6,~L1+10(fp)
00c0 81c6           101 	add	r6,r12
00c2 6ff6  000a     102 	ld	~L1+10(fp),r6
00c6 a1e7           103 	ld	r7,r14
00c8 a1d5           104 	ld	r5,r13
00ca df8c           105 	callr	_qstexc
00cc 61f2  0008     106 	ld	r2,~L1+8(fp)
00d0 81c2           107 	add	r2,r12
00d2 6ff2  0008     108 	ld	~L1+8(fp),r2
00d6 a12e           109 	ld	r14,r2
00d8 e819           110 	jpr	L25
                    111 L20010:
00da 6102  0000'    112 	ld	r2,_qscmp
00de 6ff2  0010     113 	ld	~L1+16(fp),r2
00e2 61f7  000a     114 	ld	r7,~L1+10(fp)
00e6 a1d6           115 	ld	r6,r13
00e8 a121           116 	ld	r1,r2
00ea 1f10           117 	call	@r1
00ec 6ff2  000c     118 	ld	~L1+12(fp),r2
00f0 8d24           119 	test	r2
00f2 e6dc           120 	jpr	eq,L20005
00f4 0b02  0000     121 	cp	r2,#0
00f8 e208           122 	jpr	le,L28
00fa 4bfe  0008     123 	cp	r14,~L1+8(fp)
00fe e6de           124 	jpr	eq,L20007
0100 a1e7           125 	ld	r7,r14
0102 a1d6           126 	ld	r6,r13
0104 dfc0           127 	callr	_qsexc
0106 83cd           128 	sub	r13,r12
0108 e8cc           129 	jpr	L20008
                    130 L28:
010a 83cd           131 	sub	r13,r12
                    132 L25:
010c 4bfd  000a     133 	cp	r13,~L1+10(fp)
0110 ebe4           134 	jpr	ugt,L20010
0112 4bfe  0008     135 	cp	r14,~L1+8(fp)
0116 ee22           136 	jpr	ne,L31
0118 61f2  0008     137 	ld	r2,~L1+8(fp)
011c 43f2  0002     138 	sub	r2,~L1+2(fp)
0120 61f3  0000     139 	ld	r3,~L1(fp)
0124 43f3  000a     140 	sub	r3,~L1+10(fp)
0128 8b32           141 	cp	r2,r3
012a e10c           142 	jpr	lt,L32
012c 61f7  000a     143 	ld	r7,~L1+10(fp)
0130 81c7           144 	add	r7,r12
0132 61f6  0000     145 	ld	r6,~L1(fp)
0136 d08c           146 	callr	_qs1
0138 61f2  0008     147 	ld	r2,~L1+8(fp)
013c 6ff2  0000     148 	ld	~L1(fp),r2
0140 5e08  0030'    149 	jpr	L16
                    150 L32:
0144 61f7  0002     151 	ld	r7,~L1+2(fp)
0148 61f6  0008     152 	ld	r6,~L1+8(fp)
014c d097           153 	callr	_qs1
014e 61f2  000a     154 	ld	r2,~L1+10(fp)
0152 81c2           155 	add	r2,r12
0154 6ff2  0002     156 	ld	~L1+2(fp),r2
0158 5e08  0030'    157 	jpr	L16
                    158 L31:
015c 61f6  0008     159 	ld	r6,~L1+8(fp)
0160 83c6           160 	sub	r6,r12
0162 6ff6  0008     161 	ld	~L1+8(fp),r6
0166 a1d7           162 	ld	r7,r13
0168 a1e5           163 	ld	r5,r14
016a dfdc           164 	callr	_qstexc
016c 61f2  000a     165 	ld	r2,~L1+10(fp)
0170 83c2           166 	sub	r2,r12
0172 6ff2  000a     167 	ld	~L1+10(fp),r2
0176 5e08  006e'    168 	jpr	L20011
                    169 L10003:
017a 5cf1  0c02     170 	ldm	r12,18(fp),#3
017e 0012  
0180 010f  0018     171 	add	fp,#~L2
0184 9e08           172 	ret
                    173 	~L1 := 0
                    174 	~L2 := 24
                    175 	}	/* _qs1 */
                    176 
                    177 .data
                    178 
                    179 
                    180 .code
                    181 
                    182 _qsexc:
                    183 {
0186 abfb           184 	dec	fp,#~L2
0188 1df6           185 	ldl	@fp,rr6
018a 6102  0002'    186 	ld	r2,_qses
018e 6ff2  0006     187 	ld	~L1+6(fp),r2
0192 a171           188 	ld	r1,r7
0194 a164           189 	ld	r4,r6
                    190 L37:
0196 201a           191 	ldb	rl2,@r1
0198 6efa  0005     192 	ldb	~L1+5(fp),rl2
019c 204b           193 	ldb	rl3,@r4
019e 2e1b           194 	ldb	@r1,rl3
01a0 a910           195 	inc	r1,#1
01a2 60fb  0005     196 	ldb	rl3,~L1+5(fp)
01a6 2e4b           197 	ldb	@r4,rl3
01a8 a940           198 	inc	r4,#1
01aa 6bf0  0006     199 	dec	~L1+6(fp),#1
01ae eef3           200 	jpr	ne,L37
01b0 a9fb           201 	inc	fp,#~L2
01b2 9e08           202 	ret
                    203 	~L1 := 0
                    204 	~L2 := 12
                    205 	}	/* _qsexc */
                    206 
                    207 .data
                    208 
                    209 
                    210 .code
                    211 
                    212 _qstexc:
                    213 {
01b4 abff           214 	dec	fp,#~L2
01b6 6ffc  000a     215 	ld	10(fp),r12
01ba 1cf9  0502     216 	ldm	@fp,r5,#3
01be 6104  0002'    217 	ld	r4,_qses
01c2 6ff4  0008     218 	ld	~L1+8(fp),r4
01c6 a171           219 	ld	r1,r7
01c8 a163           220 	ld	r3,r6
01ca a15c           221 	ld	r12,r5
                    222 L41:
01cc 201a           223 	ldb	rl2,@r1
01ce b120           224 	extsb	r2
01d0 6ff2  0006     225 	ld	~L1+6(fp),r2
01d4 20ca           226 	ldb	rl2,@r12
01d6 2e1a           227 	ldb	@r1,rl2
01d8 a910           228 	inc	r1,#1
01da 203a           229 	ldb	rl2,@r3
01dc 2eca           230 	ldb	@r12,rl2
01de a9c0           231 	inc	r12,#1
01e0 60fa  0007     232 	ldb	rl2,~L1+7(fp)
01e4 2e3a           233 	ldb	@r3,rl2
01e6 a930           234 	inc	r3,#1
01e8 6bf0  0008     235 	dec	~L1+8(fp),#1
01ec eeef           236 	jpr	ne,L41
01ee 61fc  000a     237 	ld	r12,10(fp)
01f2 a9ff           238 	inc	fp,#~L2
01f4 9e08           239 	ret
                    240 	~L1 := 0
                    241 	~L2 := 16
                    242 	}	/* _qstexc */
                    243 
                    244 .data
                    245 
