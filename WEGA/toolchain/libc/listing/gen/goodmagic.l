LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 .comm	2,	_swap_flg;
                      5 .even
                      6 _magics::
0000 0000             7 .word	%0
0002 e707             8 .word	%e707
0004 e711             9 .word	%e711
0006 e705            10 .word	%e705
0008 e607            11 .word	%e607
000a e611            12 .word	%e611
000c e605            13 .word	%e605
000e e507            14 .word	%e507
0010 e511            15 .word	%e511
0012 e505            16 .word	%e505
                     17 /*~~goodmagic:*/
                     18 
                     19 .code
                     20 
                     21 _goodmagic::
                     22 {
0000 abf5            23 	dec	fp,#~L2
0002 6ffe  0004      24 	ld	4(fp),r14
0006 2ff7            25 	ld	@fp,r7
0008 4d08  0000*     26 	clr	_swap_flg
                     27 L16:
000c 61f2  0000      28 	ld	r2,~L1(fp)
0010 6f02  0000'     29 	ld	_magics,r2
0014 bdea            30 	ldk	r14,#10
                     31 L19:
0016 abe0            32 	dec	r14,#1
0018 a1e2            33 	ld	r2,r14
001a 81e2            34 	add	r2,r14
001c 61f3  0000      35 	ld	r3,~L1(fp)
0020 4b23  0000'     36 	cp	r3,_magics(r2)
0024 eef8            37 	jpr	ne,L19
0026 8de4            38 	test	r14
0028 ee0b            39 	jpr	ne,L14
002a a137            40 	ld	r7,r3
002c 5f00  0000*     41 	callr	_swap
0030 6ff2  0000      42 	ld	~L1(fp),r2
0034 6102  0000*     43 	ld	r2,_swap_flg
0038 6900  0000*     44 	inc	_swap_flg,#1
003c 8d24            45 	test	r2
003e e6e6            46 	jpr	eq,L16
                     47 L14:
0040 8de4            48 	test	r14
0042 ee02            49 	jpr	ne,L22
0044 4d08  0000*     50 	clr	_swap_flg
                     51 L22:
0048 a1e2            52 	ld	r2,r14
004a 61fe  0004      53 	ld	r14,4(fp)
004e a9f5            54 	inc	fp,#~L2
0050 9e08            55 	ret
                     56 	~L1 := 0
                     57 	~L2 := 6
                     58 	}	/* _goodmagic */
                     59 
                     60 .data
                     61 
