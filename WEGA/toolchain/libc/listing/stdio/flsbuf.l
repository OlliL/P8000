LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~_flsbuf:*/
                      5 
                      6 .code
                      7 
                      8 __flsbuf::
                      9 {
0000 abff            10 	dec	fp,#~L2
0002 5cf9  0b03      11 	ldm	8(fp),r11,#4
0006 0008  
0008 6ff7  0002      12 	ld	~L1+2(fp),r7
000c a16e            13 	ld	r14,r6
000e 60ee  0006      14 	ldb	rl6,6(r14)
0012 b160            15 	extsb	r6
0014 a767            16 	bit	r6,#7
0016 e614            17 	jpr	eq,L23
0018 64e1  0006      18 	setb	6(r14),#1
001c 62e4  0006      19 	resb	6(r14),#4
0020 e80f            20 	jpr	L23
                     21 L20001:
0022 600f  000f*     22 	ldb	rl7,__iob+15
0026 b170            23 	extsb	r7
0028 5f00  0000*     24 	callr	_isatty
002c 8d24            25 	test	r2
002e e603            26 	jpr	eq,L30
                     27 L20002:
0030 64e2  0006      28 	setb	6(r14),#2
0034 e805            29 	jpr	L23
                     30 L30:
0036 4de5  0004      31 	ld	4(r14),#__sobuf
003a 0000* 
003c 0de5  0000*     32 	ld	@r14,#__sobuf
                     33 L23:
0040 60ea  0006      34 	ldb	rl2,6(r14)
0044 b120            35 	extsb	r2
0046 a722            36 	bit	r2,#2
0048 e611            37 	jpr	eq,L24
004a 60fa  0003      38 	ldb	rl2,~L1+3(fp)
004e 6efa  0005      39 	ldb	~L1+5(fp),rl2
0052 bdb1            40 	ldk	r11,#1
0054 76f6  0005      41 	lda	r6,~L1+5(fp)
0058 60ef  0007      42 	ldb	rl7,7(r14)
005c b170            43 	extsb	r7
005e a1b5            44 	ld	r5,r11
0060 5f00  0000*     45 	callr	_write
0064 a12c            46 	ld	r12,r2
0066 4de8  0002      47 	clr	2(r14)
006a e82d            48 	jpr	L26
                     49 L24:
006c 61ed  0004      50 	ld	r13,4(r14)
0070 8dd4            51 	test	r13
0072 ee11            52 	jpr	ne,L27
0074 0b0e  0008*     53 	cp	r14,#__iob+8
0078 e6d4            54 	jpr	eq,L20001
007a 2107  0200      55 	ld	r7,#512
007e 5f00  0000*     56 	callr	_malloc
0082 a12d            57 	ld	r13,r2
0084 6fe2  0004      58 	ld	4(r14),r2
0088 8d24            59 	test	r2
008a e6d2            60 	jpr	eq,L20002
008c 64e3  0006      61 	setb	6(r14),#3
0090 bdc0            62 	ldk	r12,#0
0092 a1cb            63 	ld	r11,r12
0094 e810            64 	jpr	L32
                     65 L27:
0096 21e2            66 	ld	r2,@r14
0098 83d2            67 	sub	r2,r13
009a a12c            68 	ld	r12,r2
009c a12b            69 	ld	r11,r2
009e 0b02  0000      70 	cp	r2,#0
00a2 e209            71 	jpr	le,L32
00a4 2fed            72 	ld	@r14,r13
00a6 60ef  0007      73 	ldb	rl7,7(r14)
00aa b170            74 	extsb	r7
00ac a1d6            75 	ld	r6,r13
00ae a1c5            76 	ld	r5,r12
00b0 5f00  0000*     77 	callr	_write
00b4 a12c            78 	ld	r12,r2
                     79 L32:
00b6 4de5  0002      80 	ld	2(r14),#511
00ba 01ff  
00bc 60fa  0003      81 	ldb	rl2,~L1+3(fp)
00c0 2eda            82 	ldb	@r13,rl2
00c2 a9d0            83 	inc	r13,#1
00c4 2fed            84 	ld	@r14,r13
                     85 L26:
00c6 8bcb            86 	cp	r11,r12
00c8 e605            87 	jpr	eq,L34
00ca 64e5  0006      88 	setb	6(r14),#5
00ce 2102  ffff      89 	ld	r2,#-1
00d2 e802            90 	jpr	L10000
                     91 L34:
00d4 61f2  0002      92 	ld	r2,~L1+2(fp)
                     93 L10000:
00d8 5cf1  0b03      94 	ldm	r11,8(fp),#4
00dc 0008  
00de a9ff            95 	inc	fp,#~L2
00e0 9e08            96 	ret
                     97 	~L1 := 0
                     98 	~L2 := 16
                     99 	}	/* __flsbuf */
                    100 
                    101 .data
                    102 
                    103 /*~~fflush:*/
                    104 
                    105 .code
                    106 
                    107 _fflush::
                    108 {
00e2 abf9           109 	dec	fp,#~L2
00e4 5cf9  0c02     110 	ldm	4(fp),r12,#3
00e8 0004  
00ea a17e           111 	ld	r14,r7
00ec 60ea  0006     112 	ldb	rl2,6(r14)
00f0 b120           113 	extsb	r2
00f2 0702  0006     114 	and	r2,#6
00f6 0b02  0002     115 	cp	r2,#2
00fa ee1c           116 	jpr	ne,L37
00fc 61ed  0004     117 	ld	r13,4(r14)
0100 8dd4           118 	test	r13
0102 e618           119 	jpr	eq,L37
0104 21e2           120 	ld	r2,@r14
0106 83d2           121 	sub	r2,r13
0108 a12c           122 	ld	r12,r2
010a 0b02  0000     123 	cp	r2,#0
010e e212           124 	jpr	le,L37
0110 2fed           125 	ld	@r14,r13
0112 4de5  0002     126 	ld	2(r14),#512
0116 0200  
0118 60ef  0007     127 	ldb	rl7,7(r14)
011c b170           128 	extsb	r7
011e a1d6           129 	ld	r6,r13
0120 a1c5           130 	ld	r5,r12
0122 5f00  0000*    131 	callr	_write
0126 8b2c           132 	cp	r12,r2
0128 e605           133 	jpr	eq,L37
012a 64e5  0006     134 	setb	6(r14),#5
012e 2102  ffff     135 	ld	r2,#-1
0132 e801           136 	jpr	L10003
                    137 L37:
0134 8322           138 	sub	r2,r2
                    139 L10003:
0136 5cf1  0c02     140 	ldm	r12,4(fp),#3
013a 0004  
013c a9f9           141 	inc	fp,#~L2
013e 9e08           142 	ret
                    143 	~L1 := 0
                    144 	~L2 := 10
                    145 	}	/* _fflush */
                    146 
                    147 .data
                    148 
                    149 /*~~_cleanup:*/
                    150 
                    151 .code
                    152 
                    153 __cleanup::
                    154 {
0140 abf1           155 	dec	fp,#~L2
0142 2ffe           156 	ld	@fp,r14
0144 210e  0000*    157 	ld	r14,#__iob
0148 e803           158 	jpr	L44
                    159 L20004:
014a a1e7           160 	ld	r7,r14
014c dff9           161 	callr	_fclose
014e a9e7           162 	inc	r14,#8
                    163 L44:
0150 4b0e  0000*    164 	cp	r14,__lastbuf
0154 e7fa           165 	jpr	ult,L20004
0156 21fe           166 	ld	r14,@fp
0158 a9f1           167 	inc	fp,#~L2
015a 9e08           168 	ret
                    169 	~L1 := 0
                    170 	~L2 := 2
                    171 	}	/* __cleanup */
                    172 
                    173 .data
                    174 
                    175 /*~~fclose:*/
                    176 
                    177 .code
                    178 
                    179 _fclose::
                    180 {
015c abf7           181 	dec	fp,#~L2
015e 5cf9  0d01     182 	ldm	4(fp),r13,#2
0162 0004  
0164 a17e           183 	ld	r14,r7
0166 210d  ffff     184 	ld	r13,#-1
016a 60ea  0006     185 	ldb	rl2,6(r14)
016e b120           186 	extsb	r2
0170 0702  0083     187 	and	r2,#131
0174 e625           188 	jpr	eq,L47
0176 60ea  0006     189 	ldb	rl2,6(r14)
017a b120           190 	extsb	r2
017c 0702  0040     191 	and	r2,#64
0180 8d24           192 	test	r2
0182 ee1e           193 	jpr	ne,L47
0184 d052           194 	callr	_fflush
0186 a12d           195 	ld	r13,r2
0188 60ef  0007     196 	ldb	rl7,7(r14)
018c b170           197 	extsb	r7
018e 5f00  0000*    198 	callr	_close
0192 0b02  0000     199 	cp	r2,#0
0196 e903           200 	jpr	ge,L49
0198 2102  ffff     201 	ld	r2,#-1
019c e81a           202 	jpr	L10009
                    203 L49:
019e 60ea  0006     204 	ldb	rl2,6(r14)
01a2 b120           205 	extsb	r2
01a4 a723           206 	bit	r2,#3
01a6 e604           207 	jpr	eq,L50
01a8 61e7  0004     208 	ld	r7,4(r14)
01ac 5f00  0000*    209 	callr	_free
                    210 L50:
01b0 60ea  0006     211 	ldb	rl2,6(r14)
01b4 b120           212 	extsb	r2
01b6 0702  000c     213 	and	r2,#12
01ba e602           214 	jpr	eq,L47
01bc 4de8  0004     215 	clr	4(r14)
                    216 L47:
01c0 60ea  0006     217 	ldb	rl2,6(r14)
01c4 060a  0000     218 	andb	rl2,#0
01c8 6eea  0006     219 	ldb	6(r14),rl2
01cc 4de8  0002     220 	clr	2(r14)
01d0 a1d2           221 	ld	r2,r13
                    222 L10009:
01d2 5cf1  0d01     223 	ldm	r13,4(fp),#2
01d6 0004  
01d8 a9f7           224 	inc	fp,#~L2
01da 9e08           225 	ret
                    226 	~L1 := 0
                    227 	~L2 := 8
                    228 	}	/* _fclose */
                    229 
                    230 .data
                    231 
