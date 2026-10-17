LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 .even
                      5 _vec:
                      6 .=.+10
                      7 /*~~mpx:*/
                      8 
                      9 .code
                     10 
                     11 _mpx::
                     12 {
0000 abf5            13 	dec	fp,#~L2
0002 1df6            14 	ldl	@fp,rr6
0004 a172            15 	ld	r2,r7
0006 0c24            16 	testb	@r2
0008 e608            17 	jpr	eq,L21
000a 6f07  0000'     18 	ld	_vec,r7
000e 6f06  0004'     19 	ld	_vec+4,r6
0012 bd75            20 	ldk	r7,#5
0014 2106  0000'     21 	ld	r6,#_vec
0018 e802            22 	jpr	L20000
                     23 L21:
001a bd76            24 	ldk	r7,#6
001c 8366            25 	sub	r6,r6
                     26 L20000:
001e 5f00  0000*     27 	callr	_mpxcall
0022 a9f5            28 	inc	fp,#~L2
0024 9e08            29 	ret
                     30 	~L1 := 0
                     31 	~L2 := 6
                     32 	}	/* _mpx */
                     33 
                     34 .data
                     35 
                     36 /*~~chan:*/
                     37 
                     38 .code
                     39 
                     40 _chan::
                     41 {
0026 abf3            42 	dec	fp,#~L2
0028 2ff7            43 	ld	@fp,r7
002a 6f07  0006'     44 	ld	_vec+6,r7
002e bd71            45 	ldk	r7,#1
0030 2106  0000'     46 	ld	r6,#_vec
0034 5f00  0000*     47 	callr	_mpxcall
0038 a9f3            48 	inc	fp,#~L2
003a 9e08            49 	ret
                     50 	~L1 := 0
                     51 	~L2 := 4
                     52 	}	/* _chan */
                     53 
                     54 .data
                     55 
                     56 /*~~join:*/
                     57 
                     58 .code
                     59 
                     60 _join::
                     61 {
003c abf5            62 	dec	fp,#~L2
003e 1df6            63 	ldl	@fp,rr6
0040 6f07  0004'     64 	ld	_vec+4,r7
0044 6f06  0006'     65 	ld	_vec+6,r6
0048 bd72            66 	ldk	r7,#2
004a 2106  0000'     67 	ld	r6,#_vec
004e 5f00  0000*     68 	callr	_mpxcall
0052 a9f5            69 	inc	fp,#~L2
0054 9e08            70 	ret
                     71 	~L1 := 0
                     72 	~L2 := 6
                     73 	}	/* _join */
                     74 
                     75 .data
                     76 
                     77 /*~~connect:*/
                     78 
                     79 .code
                     80 
                     81 _connect::
                     82 {
0056 abf7            83 	dec	fp,#~L2
0058 1cf9  0502      84 	ldm	@fp,r5,#3
005c 6f07  0004'     85 	ld	_vec+4,r7
0060 6f06  0006'     86 	ld	_vec+6,r6
0064 6f05  0008'     87 	ld	_vec+8,r5
0068 bd77            88 	ldk	r7,#7
006a 2106  0000'     89 	ld	r6,#_vec
006e 5f00  0000*     90 	callr	_mpxcall
0072 a9f7            91 	inc	fp,#~L2
0074 9e08            92 	ret
                     93 	~L1 := 0
                     94 	~L2 := 8
                     95 	}	/* _connect */
                     96 
                     97 .data
                     98 
                     99 /*~~attach:*/
                    100 
                    101 .code
                    102 
                    103 _attach::
                    104 {
0076 abf5           105 	dec	fp,#~L2
0078 1df6           106 	ldl	@fp,rr6
007a 6f07  0004'    107 	ld	_vec+4,r7
007e 6f06  0006'    108 	ld	_vec+6,r6
0082 bd74           109 	ldk	r7,#4
0084 2106  0000'    110 	ld	r6,#_vec
0088 5f00  0000*    111 	callr	_mpxcall
008c a9f5           112 	inc	fp,#~L2
008e 9e08           113 	ret
                    114 	~L1 := 0
                    115 	~L2 := 6
                    116 	}	/* _attach */
                    117 
                    118 .data
                    119 
                    120 /*~~detach:*/
                    121 
                    122 .code
                    123 
                    124 _detach::
                    125 {
0090 abf5           126 	dec	fp,#~L2
0092 1df6           127 	ldl	@fp,rr6
0094 6f07  0004'    128 	ld	_vec+4,r7
0098 6f06  0006'    129 	ld	_vec+6,r6
009c bd78           130 	ldk	r7,#8
009e 2106  0000'    131 	ld	r6,#_vec
00a2 5f00  0000*    132 	callr	_mpxcall
00a6 a9f5           133 	inc	fp,#~L2
00a8 9e08           134 	ret
                    135 	~L1 := 0
                    136 	~L2 := 6
                    137 	}	/* _detach */
                    138 
                    139 .data
                    140 
                    141 /*~~extract:*/
                    142 
                    143 .code
                    144 
                    145 _extract::
                    146 {
00aa abf7           147 	dec	fp,#~L2
00ac 1cf9  0502     148 	ldm	@fp,r5,#3
00b0 6f07  0004'    149 	ld	_vec+4,r7
00b4 6f06  0006'    150 	ld	_vec+6,r6
00b8 6f05  0008'    151 	ld	_vec+8,r5
00bc bd73           152 	ldk	r7,#3
00be 2106  0000'    153 	ld	r6,#_vec
00c2 5f00  0000*    154 	callr	_mpxcall
00c6 a9f7           155 	inc	fp,#~L2
00c8 9e08           156 	ret
                    157 	~L1 := 0
                    158 	~L2 := 8
                    159 	}	/* _extract */
                    160 
                    161 .data
                    162 
                    163 /*~~debug:*/
                    164 
                    165 .code
                    166 
                    167 _debug::
                    168 {
00ca abf5           169 	dec	fp,#~L2
00cc 1df6           170 	ldl	@fp,rr6
00ce 6f07  0004'    171 	ld	_vec+4,r7
00d2 6f06  0006'    172 	ld	_vec+6,r6
00d6 bd7a           173 	ldk	r7,#10
00d8 2106  0000'    174 	ld	r6,#_vec
00dc 5f00  0000*    175 	callr	_mpxcall
00e0 a9f5           176 	inc	fp,#~L2
00e2 9e08           177 	ret
                    178 	~L1 := 0
                    179 	~L2 := 6
                    180 	}	/* _debug */
                    181 
                    182 .data
                    183 
                    184 /*~~npgrp:*/
                    185 
                    186 .code
                    187 
                    188 _npgrp::
                    189 {
00e4 abf7           190 	dec	fp,#~L2
00e6 1cf9  0502     191 	ldm	@fp,r5,#3
00ea 6f07  0004'    192 	ld	_vec+4,r7
00ee 6f06  0006'    193 	ld	_vec+6,r6
00f2 6f05  0008'    194 	ld	_vec+8,r5
00f6 bd7b           195 	ldk	r7,#11
00f8 2106  0000'    196 	ld	r6,#_vec
00fc 5f00  0000*    197 	callr	_mpxcall
0100 a9f7           198 	inc	fp,#~L2
0102 9e08           199 	ret
                    200 	~L1 := 0
                    201 	~L2 := 8
                    202 	}	/* _npgrp */
                    203 
                    204 .data
                    205 
                    206 /*~~ckill:*/
                    207 
                    208 .code
                    209 
                    210 _ckill::
                    211 {
0104 abf7           212 	dec	fp,#~L2
0106 1cf9  0502     213 	ldm	@fp,r5,#3
010a 6f07  0004'    214 	ld	_vec+4,r7
010e 6f06  0006'    215 	ld	_vec+6,r6
0112 6f05  0008'    216 	ld	_vec+8,r5
0116 bd7c           217 	ldk	r7,#12
0118 2106  0000'    218 	ld	r6,#_vec
011c 5f00  0000*    219 	callr	_mpxcall
0120 a9f7           220 	inc	fp,#~L2
0122 9e08           221 	ret
                    222 	~L1 := 0
                    223 	~L2 := 8
                    224 	}	/* _ckill */
                    225 
                    226 .data
                    227 
