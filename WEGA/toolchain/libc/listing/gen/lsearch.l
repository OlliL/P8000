LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~lsearch:*/
                      5 
                      6 .code
                      7 
                      8 _lsearch::
                      9 {
0000 abff            10 	dec	fp,#~L2
0002 1cf9  0304      11 	ldm	@fp,r3,#5
0006 6ff6  000a      12 	ld	~L1+10(fp),r6
000a a152            13 	ld	r2,r5
000c 2121            14 	ld	r1,@r2
000e 9940            15 	mult	rr0,r4
0010 8116            16 	add	r6,r1
0012 6ff6  000c      17 	ld	~L1+12(fp),r6
0016 a167            18 	ld	r7,r6
0018 61f6  0008      19 	ld	r6,~L1+8(fp)
001c 61f5  0002      20 	ld	r5,~L1+2(fp)
0020 5f00  0000*     21 	callr	_strncpy
0024 e806            22 	jpr	L13
                     23 L20001:
0026 61f2  000a      24 	ld	r2,~L1+10(fp)
002a 41f2  0002      25 	add	r2,~L1+2(fp)
002e 6ff2  000a      26 	ld	~L1+10(fp),r2
                     27 L13:
0032 61f2  0000      28 	ld	r2,~L1(fp)
0036 6ff2  000e      29 	ld	~L1+14(fp),r2
003a 61f7  0008      30 	ld	r7,~L1+8(fp)
003e 61f6  000a      31 	ld	r6,~L1+10(fp)
0042 a121            32 	ld	r1,r2
0044 1f10            33 	call	@r1
0046 8d24            34 	test	r2
0048 eeee            35 	jpr	ne,L20001
004a 61f2  000a      36 	ld	r2,~L1+10(fp)
004e 4bf2  000c      37 	cp	r2,~L1+12(fp)
0052 ee03            38 	jpr	ne,L15
0054 61f3  0004      39 	ld	r3,~L1+4(fp)
0058 2930            40 	inc	@r3,#1
                     41 L15:
005a 61f2  000a      42 	ld	r2,~L1+10(fp)
005e a9ff            43 	inc	fp,#~L2
0060 9e08            44 	ret
                     45 	~L1 := 0
                     46 	~L2 := 16
                     47 	}	/* _lsearch */
                     48 
                     49 .data
                     50 
