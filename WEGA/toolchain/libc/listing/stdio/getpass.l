LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 L21:
                      5 .=.+8
0008 00               6 .byte	0
0009 00               7 .byte	0
                      8 L22:
000a 2f  64  65       9 .byte	%2f,%64,%65,%76,%2f,%74,%74,%79
000d 76  2f  74  
0010 74  79  
0012 00              10 .byte	%0
                     11 .even
                     12 L23:
0014 72  00          13 .byte	%72,%0
                     14 .even
                     15 L33:
0016 0a  00          16 .byte	%a,%0
                     17 .even
                     18 /*~~getpass:*/
                     19 
                     20 .code
                     21 
                     22 _getpass::
                     23 {
0000 030f  0020      24 	sub	fp,#~L2
0004 5cf9  0d01      25 	ldm	28(fp),r13,#2
0008 001c  
000a 2ff7            26 	ld	@fp,r7
000c 2107  000a'     27 	ld	r7,#L22
0010 2106  0014'     28 	ld	r6,#L23
0014 5f00  0000*     29 	callr	_fopen
0018 6ff2  0016      30 	ld	~L1+22(fp),r2
001c 8d24            31 	test	r2
001e ee02            32 	jpr	ne,L24
0020 8322            33 	sub	r2,r2
0022 e87e            34 	jpr	L10000
                     35 L24:
0024 61f7  0016      36 	ld	r7,~L1+22(fp)
0028 8366            37 	sub	r6,r6
002a 5f00  0000*     38 	callr	_setbuf
002e bd72            39 	ldk	r7,#2
0030 bd61            40 	ldk	r6,#1
0032 5f00  0000*     41 	callr	_signal
0036 6ff2  0018      42 	ld	~L1+24(fp),r2
003a 61f7  0016      43 	ld	r7,~L1+22(fp)
003e 607e  0007      44 	ldb	rl6,7(r7)
0042 a0ef            45 	ldb	rl7,rl6
0044 b170            46 	extsb	r7
0046 76f5  0002      47 	lda	r5,~L1+2(fp)
004a 2106  5401      48 	ld	r6,#21505
004e 5f00  0000*     49 	callr	_ioctl
0052 61f2  0008      50 	ld	r2,~L1+8(fp)
0056 6ff2  0014      51 	ld	~L1+20(fp),r2
005a 61f3  0008      52 	ld	r3,~L1+8(fp)
005e 0703  ff87      53 	and	r3,#65415
0062 6ff3  0008      54 	ld	~L1+8(fp),r3
0066 61f7  0016      55 	ld	r7,~L1+22(fp)
006a 607e  0007      56 	ldb	rl6,7(r7)
006e a0ef            57 	ldb	rl7,rl6
0070 b170            58 	extsb	r7
0072 76f5  0002      59 	lda	r5,~L1+2(fp)
0076 2106  5404      60 	ld	r6,#21508
007a 5f00  0000*     61 	callr	_ioctl
007e 2107  0010*     62 	ld	r7,#__iob+16
0082 61f6  0000      63 	ld	r6,~L1(fp)
0086 5f00  0000*     64 	callr	_fprintf
008a 210e  0000'     65 	ld	r14,#L21
008e e809            66 	jpr	L31
                     67 L20001:
0090 0b0d  ffff      68 	cp	r13,#-1
0094 e61f            69 	jpr	eq,L30
0096 0b0e  0008'     70 	cp	r14,#L21+8
009a ef03            71 	jpr	uge,L31
009c a1d2            72 	ld	r2,r13
009e 2eea            73 	ldb	@r14,rl2
00a0 a9e0            74 	inc	r14,#1
                     75 L31:
00a2 61f2  0016      76 	ld	r2,~L1+22(fp)
00a6 6b20  0002      77 	dec	2(r2),#1
00aa 6123  0002      78 	ld	r3,2(r2)
00ae 0b03  0000      79 	cp	r3,#0
00b2 e905            80 	jpr	ge,L10003
00b4 61f7  0016      81 	ld	r7,~L1+22(fp)
00b8 5f00  0000*     82 	callr	__filbuf
00bc e807            83 	jpr	L10004
                     84 L10003:
00be 61f2  0016      85 	ld	r2,~L1+22(fp)
00c2 2920            86 	inc	@r2,#1
00c4 2123            87 	ld	r3,@r2
00c6 ab30            88 	dec	r3,#1
00c8 203a            89 	ldb	rl2,@r3
00ca 8222            90 	subb	rh2,rh2
                     91 L10004:
00cc a12d            92 	ld	r13,r2
00ce 0b02  000a      93 	cp	r2,#10
00d2 eede            94 	jpr	ne,L20001
                     95 L30:
00d4 0ce8            96 	clrb	@r14
00d6 2107  0010*     97 	ld	r7,#__iob+16
00da 2106  0016'     98 	ld	r6,#L33
00de 5f00  0000*     99 	callr	_fprintf
00e2 61f2  0014     100 	ld	r2,~L1+20(fp)
00e6 6ff2  0008     101 	ld	~L1+8(fp),r2
00ea 61f7  0016     102 	ld	r7,~L1+22(fp)
00ee 607b  0007     103 	ldb	rl3,7(r7)
00f2 a0bf           104 	ldb	rl7,rl3
00f4 b170           105 	extsb	r7
00f6 76f5  0002     106 	lda	r5,~L1+2(fp)
00fa 2106  5403     107 	ld	r6,#21507
00fe 5f00  0000*    108 	callr	_ioctl
0102 bd72           109 	ldk	r7,#2
0104 61f6  0018     110 	ld	r6,~L1+24(fp)
0108 5f00  0000*    111 	callr	_signal
010c 4df1  0016     112 	cp	~L1+22(fp),#__iob
0110 0000* 
0112 e604           113 	jpr	eq,L34
0114 61f7  0016     114 	ld	r7,~L1+22(fp)
0118 5f00  0000*    115 	callr	_fclose
                    116 L34:
011c 2102  0000'    117 	ld	r2,#L21
                    118 L10000:
0120 5cf1  0d01     119 	ldm	r13,28(fp),#2
0124 001c  
0126 010f  0020     120 	add	fp,#~L2
012a 9e08           121 	ret
                    122 	~L1 := 0
                    123 	~L2 := 32
                    124 	}	/* _getpass */
                    125 
                    126 .data
                    127 
