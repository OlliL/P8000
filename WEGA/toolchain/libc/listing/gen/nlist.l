LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 .even
                      5 _a_magic::
0000 e607             6 .word	%e607
0002 e611             7 .word	%e611
0004 e707             8 .word	%e707
0006 e711             9 .word	%e711
0008 0000            10 .word	%0
                     11 /*~~nlist:*/
                     12 
                     13 .code
                     14 
                     15 _nlist::
                     16 {
0000 030f  063e      17 	sub	fp,#~L2
0004 5cf9  0d01      18 	ldm	1594(fp),r13,#2
0008 063a  
000a 1df6            19 	ldl	@fp,rr6
000c 4df8  062e      20 	clr	~L1+1582(fp)
0010 a16d            21 	ld	r13,r6
0012 e808            22 	jpr	L15
                     23 L20001:
0014 4cd8  0006      24 	clrb	6(r13)
0018 9222            25 	subl	rr2,rr2
001a 5dd2  0002      26 	ldl	2(r13),rr2
001e 69f0  062e      27 	inc	~L1+1582(fp),#1
0022 a9d7            28 	inc	r13,#8
                     29 L15:
0024 21d2            30 	ld	r2,@r13
0026 0c24            31 	testb	@r2
0028 eef5            32 	jpr	ne,L20001
002a 61f7  0002      33 	ld	r7,~L1+2(fp)
002e 8366            34 	sub	r6,r6
0030 5f00  0000*     35 	callr	_open
0034 6ff2  0004      36 	ld	~L1+4(fp),r2
0038 0b02  0000      37 	cp	r2,#0
003c e904            38 	jpr	ge,L17
                     39 L20006:
003e 2102  ffff      40 	ld	r2,#-1
0042 5e08  024e'     41 	jpr	L10000
                     42 L17:
0046 76f6  0012      43 	lda	r6,~L1+18(fp)
004a 61f7  0004      44 	ld	r7,~L1+4(fp)
004e 2105  0018      45 	ld	r5,#24
0052 5f00  0000*     46 	callr	_read
0056 4df8  0006      47 	clr	~L1+6(fp)
005a e80a            48 	jpr	L21
                     49 L20003:
005c 61f2  0006      50 	ld	r2,~L1+6(fp)
0060 8122            51 	add	r2,r2
0062 6123  0000'     52 	ld	r3,_a_magic(r2)
0066 4bf3  0012      53 	cp	r3,~L1+18(fp)
006a e608            54 	jpr	eq,L20
006c 69f0  0006      55 	inc	~L1+6(fp),#1
                     56 L21:
0070 61f2  0006      57 	ld	r2,~L1+6(fp)
0074 8122            58 	add	r2,r2
0076 4d24  0000'     59 	test	_a_magic(r2)
007a eef0            60 	jpr	ne,L20003
                     61 L20:
007c 61f2  0006      62 	ld	r2,~L1+6(fp)
0080 8122            63 	add	r2,r2
0082 4d24  0000'     64 	test	_a_magic(r2)
0086 ee05            65 	jpr	ne,L23
0088 61f7  0004      66 	ld	r7,~L1+4(fp)
008c 5f00  0000*     67 	callr	_close
0090 e8d6            68 	jpr	L20006
                     69 L23:
0092 54f2  0014      70 	ldl	rr2,~L1+20(fp)
0096 5df2  000e      71 	ldl	~L1+14(fp),rr2
009a 61f4  0024      72 	ld	r4,~L1+36(fp)
009e 0704  0001      73 	and	r4,#1
00a2 0b04  0001      74 	cp	r4,#1
00a6 e606            75 	jpr	eq,L25
00a8 9426            76 	ldl	rr6,rr2
00aa 1804  0000      77 	multl	rq4,#2
00ae 0002  
00b0 5df6  000e      78 	ldl	~L1+14(fp),rr6
                     79 L25:
00b4 61f2  001c      80 	ld	r2,~L1+28(fp)
00b8 0102  0018      81 	add	r2,#24
00bc bd40            82 	ldk	r4,#0
00be a125            83 	ld	r5,r2
00c0 56f4  000e      84 	addl	rr4,~L1+14(fp)
00c4 5df4  000e      85 	ldl	~L1+14(fp),rr4
00c8 61f7  0004      86 	ld	r7,~L1+4(fp)
00cc 8333            87 	sub	r3,r3
00ce 5f00  0000*     88 	callr	_lseek
00d2 61f2  001e      89 	ld	r2,~L1+30(fp)
00d6 6ff2  000a      90 	ld	~L1+10(fp),r2
                     91 L27:
00da 4df4  000a      92 	test	~L1+10(fp)
00de 5e06  0244'     93 	jpr	eq,L28
00e2 4df5  000c      94 	ld	~L1+12(fp),#1400
00e6 0578  
00e8 4df1  000a      95 	cp	~L1+10(fp),#1400
00ec 0578  
00ee ef04            96 	jpr	uge,L29
00f0 61f2  000a      97 	ld	r2,~L1+10(fp)
00f4 6ff2  000c      98 	ld	~L1+12(fp),r2
                     99 L29:
00f8 76f6  002a     100 	lda	r6,~L1+42(fp)
00fc 61f7  0004     101 	ld	r7,~L1+4(fp)
0100 61f5  000c     102 	ld	r5,~L1+12(fp)
0104 5f00  0000*    103 	callr	_read
0108 61f2  000a     104 	ld	r2,~L1+10(fp)
010c 43f2  000c     105 	sub	r2,~L1+12(fp)
0110 6ff2  000a     106 	ld	~L1+10(fp),r2
0114 76f2  002a     107 	lda	r2,~L1+42(fp)
0118 a12e           108 	ld	r14,r2
                    109 L30:
011a 4df4  000c     110 	test	~L1+12(fp)
011e e6dd           111 	jpr	eq,L27
0120 61fd  0000     112 	ld	r13,~L1(fp)
                    113 L34:
0124 21d2           114 	ld	r2,@r13
0126 0c24           115 	testb	@r2
0128 e64a           116 	jpr	eq,L33
012a 21d2           117 	ld	r2,@r13
012c 6ff2  0632     118 	ld	~L1+1586(fp),r2
0130 76e3  0006     119 	lda	r3,6(r14)
0134 6ff3  0634     120 	ld	~L1+1588(fp),r3
0138 a134           121 	ld	r4,r3
013a 204d           122 	ldb	rl5,@r4
013c b150           123 	extsb	r5
013e a757           124 	bit	r5,#7
0140 e60b           125 	jpr	eq,L35
0142 204d           126 	ldb	rl5,@r4
0144 b150           127 	extsb	r5
0146 0705  007f     128 	and	r5,#127
014a 41f5  0634     129 	add	r5,~L1+1588(fp)
014e 6ff5  0630     130 	ld	~L1+1584(fp),r5
0152 69f0  0634     131 	inc	~L1+1588(fp),#1
0156 e805           132 	jpr	L36
                    133 L35:
0158 61f2  0634     134 	ld	r2,~L1+1588(fp)
015c a927           135 	inc	r2,#8
015e 6ff2  0630     136 	ld	~L1+1584(fp),r2
                    137 L36:
0162 4df8  0636     138 	clr	~L1+1590(fp)
                    139 L37:
0166 61f2  0632     140 	ld	r2,~L1+1586(fp)
016a 0c24           141 	testb	@r2
016c e611           142 	jpr	eq,L38
016e 61f3  0634     143 	ld	r3,~L1+1588(fp)
0172 4bf3  0630     144 	cp	r3,~L1+1584(fp)
0176 ef0c           145 	jpr	uge,L38
0178 69f0  0632     146 	inc	~L1+1586(fp),#1
017c 202b           147 	ldb	rl3,@r2
017e 61f2  0634     148 	ld	r2,~L1+1588(fp)
0182 69f0  0634     149 	inc	~L1+1588(fp),#1
0186 202c           150 	ldb	rl4,@r2
0188 8acb           151 	cpb	rl3,rl4
018a e6ed           152 	jpr	eq,L37
018c 69f0  0636     153 	inc	~L1+1590(fp),#1
                    154 L38:
0190 61f2  0634     155 	ld	r2,~L1+1588(fp)
0194 4bf2  0630     156 	cp	r2,~L1+1584(fp)
0198 ef04           157 	jpr	uge,L40
019a 0c24           158 	testb	@r2
019c e602           159 	jpr	eq,L40
019e 69f0  0636     160 	inc	~L1+1590(fp),#1
                    161 L40:
01a2 4df4  0636     162 	test	~L1+1590(fp)
01a6 ee0f           163 	jpr	ne,L32
01a8 14e2           164 	ldl	rr2,@r14
01aa 5dd2  0002     165 	ldl	2(r13),rr2
01ae 60ea  0004     166 	ldb	rl2,4(r14)
01b2 6eda  0006     167 	ldb	6(r13),rl2
01b6 8ca4           168 	testb	rl2
01b8 e602           169 	jpr	eq,L33
01ba 6bf0  062e     170 	dec	~L1+1582(fp),#1
                    171 L33:
01be 4df4  062e     172 	test	~L1+1582(fp)
01c2 ee03           173 	jpr	ne,L43
01c4 e83f           174 	jpr	L28
                    175 L32:
01c6 a9d7           176 	inc	r13,#8
01c8 e8ad           177 	jpr	L34
                    178 L43:
01ca a1e7           179 	ld	r7,r14
01cc dfba           180 	callr	_readby
01ce 6ff2  0006     181 	ld	~L1+6(fp),r2
01d2 a125           182 	ld	r5,r2
01d4 1904  000e     183 	mult	rr4,#14
01d8 61f4  000c     184 	ld	r4,~L1+12(fp)
01dc 8354           185 	sub	r4,r5
01de 6ff4  000c     186 	ld	~L1+12(fp),r4
01e2 8d44           187 	test	r4
01e4 e69a           188 	jpr	eq,L30
01e6 61f3  0006     189 	ld	r3,~L1+6(fp)
01ea 1902  000e     190 	mult	rr2,#14
01ee 813e           191 	add	r14,r3
01f0 a1e7           192 	ld	r7,r14
01f2 dfcd           193 	callr	_readby
01f4 6ff2  0006     194 	ld	~L1+6(fp),r2
01f8 61f5  000c     195 	ld	r5,~L1+12(fp)
01fc bd40           196 	ldk	r4,#0
01fe 1b04  000e     197 	div	rr4,#14
0202 6ff5  0008     198 	ld	~L1+8(fp),r5
0206 8b52           199 	cp	r2,r5
0208 e288           200 	jpr	le,L30
020a a125           201 	ld	r5,r2
020c 43f5  0008     202 	sub	r5,~L1+8(fp)
0210 a153           203 	ld	r3,r5
0212 1902  000e     204 	mult	rr2,#14
0216 6ff3  0006     205 	ld	~L1+6(fp),r3
021a a135           206 	ld	r5,r3
021c 76f6  05a2     207 	lda	r6,~L1+1442(fp)
0220 61f7  0004     208 	ld	r7,~L1+4(fp)
0224 5f00  0000*    209 	callr	_read
0228 61f2  000a     210 	ld	r2,~L1+10(fp)
022c 43f2  0006     211 	sub	r2,~L1+6(fp)
0230 6ff2  000a     212 	ld	~L1+10(fp),r2
0234 61f2  000c     213 	ld	r2,~L1+12(fp)
0238 41f2  0006     214 	add	r2,~L1+6(fp)
023c 6ff2  000c     215 	ld	~L1+12(fp),r2
0240 5e08  011a'    216 	jpr	L30
                    217 L28:
0244 61f7  0004     218 	ld	r7,~L1+4(fp)
0248 5f00  0000*    219 	callr	_close
024c 8322           220 	sub	r2,r2
                    221 L10000:
024e 5cf1  0d01     222 	ldm	r13,1594(fp),#2
0252 063a  
0254 010f  063e     223 	add	fp,#~L2
0258 9e08           224 	ret
                    225 	~L1 := 0
                    226 	~L2 := 1598
                    227 	}	/* _nlist */
                    228 
                    229 .data
                    230 
                    231 /*~~readby:*/
                    232 
                    233 .code
                    234 
                    235 _readby::
                    236 {
025a abf5           237 	dec	fp,#~L2
025c 2ff7           238 	ld	@fp,r7
025e a176           239 	ld	r6,r7
0260 606a  0006     240 	ldb	rl2,6(r6)
0264 b120           241 	extsb	r2
0266 a727           242 	bit	r2,#7
0268 e615           243 	jpr	eq,L48
026a 607a  0006     244 	ldb	rl2,6(r7)
026e b120           245 	extsb	r2
0270 0702  007f     246 	and	r2,#127
0274 ab26           247 	dec	r2,#7
0276 6ff2  0002     248 	ld	~L1+2(fp),r2
027a ab26           249 	dec	r2,#7
027c a125           250 	ld	r5,r2
027e b14a           251 	exts	rr4
0280 1b04  000e     252 	div	rr4,#14
0284 a950           253 	inc	r5,#1
0286 6ff5  0002     254 	ld	~L1+2(fp),r5
028a 69f0  0002     255 	inc	~L1+2(fp),#1
028e 61f2  0002     256 	ld	r2,~L1+2(fp)
0292 e801           257 	jpr	L10003
                    258 L48:
0294 bd21           259 	ldk	r2,#1
                    260 L10003:
0296 a9f5           261 	inc	fp,#~L2
0298 9e08           262 	ret
                    263 	~L1 := 0
                    264 	~L2 := 6
                    265 	}	/* _readby */
                    266 
                    267 .data
                    268 
