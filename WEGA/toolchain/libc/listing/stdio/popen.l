LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 .even
                      5 _popen_pid:
                      6 .=.+40
                      7 L28:
0028 2f  62  69       8 .byte	%2f,%62,%69,%6e,%2f,%73,%68,%0
002b 6e  2f  73  
002e 68  00  
                      9 .even
                     10 L29:
0030 73  68  00      11 .byte	%73,%68,%0
                     12 .even
                     13 L30:
0034 2d  63  00      14 .byte	%2d,%63,%0
                     15 .even
                     16 /*~~popen:*/
                     17 
                     18 .code
                     19 
                     20 _popen::
                     21 {
0000 abff            22 	dec	fp,#~L2
0002 5cf9  0c02      23 	ldm	10(fp),r12,#3
0006 000a  
0008 1df6            24 	ldl	@fp,rr6
000a 76f7  0004      25 	lda	r7,~L1+4(fp)
000e 5f00  0000*     26 	callr	_pipe
0012 0b02  0000      27 	cp	r2,#0
0016 e902            28 	jpr	ge,L22
                     29 L20000:
0018 8322            30 	sub	r2,r2
001a e849            31 	jpr	L10000
                     32 L22:
001c 61f2  0000      33 	ld	r2,~L1(fp)
0020 0c21  7272      34 	cpb	@r2,#114
0024 ee03            35 	jpr	ne,L10003
0026 61f3  0004      36 	ld	r3,~L1+4(fp)
002a e802            37 	jpr	L10004
                     38 L10003:
002c 61f3  0006      39 	ld	r3,~L1+6(fp)
                     40 L10004:
0030 a13e            41 	ld	r14,r3
0032 61f2  0000      42 	ld	r2,~L1(fp)
0036 0c21  7272      43 	cpb	@r2,#114
003a ee03            44 	jpr	ne,L10005
003c 61f3  0006      45 	ld	r3,~L1+6(fp)
0040 e802            46 	jpr	L10006
                     47 L10005:
0042 61f3  0004      48 	ld	r3,~L1+4(fp)
                     49 L10006:
0046 a13d            50 	ld	r13,r3
0048 5f00  0000*     51 	callr	_fork
004c a12c            52 	ld	r12,r2
004e 8d24            53 	test	r2
0050 ee1f            54 	jpr	ne,L24
0052 a1e7            55 	ld	r7,r14
0054 5f00  0000*     56 	callr	_close
0058 61f2  0000      57 	ld	r2,~L1(fp)
005c 0c21  7272      58 	cpb	@r2,#114
0060 ee02            59 	jpr	ne,L10007
0062 bd61            60 	ldk	r6,#1
0064 e801            61 	jpr	L10008
                     62 L10007:
0066 8366            63 	sub	r6,r6
                     64 L10008:
0068 a1d7            65 	ld	r7,r13
006a 5f00  0000*     66 	callr	_dup2
006e a1d7            67 	ld	r7,r13
0070 5f00  0000*     68 	callr	_close
0074 2107  0028'     69 	ld	r7,#L28
0078 2106  0030'     70 	ld	r6,#L29
007c 2105  0034'     71 	ld	r5,#L30
0080 61f4  0002      72 	ld	r4,~L1+2(fp)
0084 8333            73 	sub	r3,r3
0086 5f00  0000*     74 	callr	_execl
008a bd71            75 	ldk	r7,#1
008c 5f00  0000*     76 	callr	__exit
                     77 L24:
0090 0b0c  ffff      78 	cp	r12,#-1
0094 e6c1            79 	jpr	eq,L20000
0096 a1e2            80 	ld	r2,r14
0098 81e2            81 	add	r2,r14
009a 6f2c  0000'     82 	ld	_popen_pid(r2),r12
009e a1d7            83 	ld	r7,r13
00a0 5f00  0000*     84 	callr	_close
00a4 a1e7            85 	ld	r7,r14
00a6 61f6  0000      86 	ld	r6,~L1(fp)
00aa 5f00  0000*     87 	callr	_fdopen
                     88 L10000:
00ae 5cf1  0c02      89 	ldm	r12,10(fp),#3
00b2 000a  
00b4 a9ff            90 	inc	fp,#~L2
00b6 9e08            91 	ret
                     92 	~L1 := 0
                     93 	~L2 := 16
                     94 	}	/* _popen */
                     95 
                     96 .data
                     97 
                     98 /*~~pclose:*/
                     99 
                    100 .code
                    101 
                    102 _pclose::
                    103 {
00b8 abff           104 	dec	fp,#~L2
00ba 5cf9  0a04     105 	ldm	6(fp),r10,#5
00be 0006  
00c0 2ff7           106 	ld	@fp,r7
00c2 a176           107 	ld	r6,r7
00c4 606a  0007     108 	ldb	rl2,7(r6)
00c8 b120           109 	extsb	r2
00ca a12e           110 	ld	r14,r2
00cc 5f00  0000*    111 	callr	_fclose
00d0 bd72           112 	ldk	r7,#2
00d2 bd61           113 	ldk	r6,#1
00d4 5f00  0000*    114 	callr	_signal
00d8 a12b           115 	ld	r11,r2
00da bd73           116 	ldk	r7,#3
00dc bd61           117 	ldk	r6,#1
00de 5f00  0000*    118 	callr	_signal
00e2 a12a           119 	ld	r10,r2
00e4 bd71           120 	ldk	r7,#1
00e6 bd61           121 	ldk	r6,#1
00e8 5f00  0000*    122 	callr	_signal
00ec a12c           123 	ld	r12,r2
                    124 L37:
00ee 76f7  0002     125 	lda	r7,~L1+2(fp)
00f2 5f00  0000*    126 	callr	_wait
00f6 a12d           127 	ld	r13,r2
00f8 a1e3           128 	ld	r3,r14
00fa 81e3           129 	add	r3,r14
00fc 4b32  0000'    130 	cp	r2,_popen_pid(r3)
0100 e603           131 	jpr	eq,L38
0102 0b02  ffff     132 	cp	r2,#-1
0106 eef3           133 	jpr	ne,L37
                    134 L38:
0108 0b0d  ffff     135 	cp	r13,#-1
010c ee03           136 	jpr	ne,L39
010e 4df5  0002     137 	ld	~L1+2(fp),#-1
0112 ffff  
                    138 L39:
0114 bd72           139 	ldk	r7,#2
0116 a1b6           140 	ld	r6,r11
0118 5f00  0000*    141 	callr	_signal
011c bd73           142 	ldk	r7,#3
011e a1a6           143 	ld	r6,r10
0120 5f00  0000*    144 	callr	_signal
0124 bd71           145 	ldk	r7,#1
0126 a1c6           146 	ld	r6,r12
0128 5f00  0000*    147 	callr	_signal
012c 61f2  0002     148 	ld	r2,~L1+2(fp)
0130 5cf1  0a04     149 	ldm	r10,6(fp),#5
0134 0006  
0136 a9ff           150 	inc	fp,#~L2
0138 9e08           151 	ret
                    152 	~L1 := 0
                    153 	~L2 := 16
                    154 	}	/* _pclose */
                    155 
                    156 .data
                    157 
