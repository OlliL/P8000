LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~stty:*/
                      5 
                      6 .code
                      7 
                      8 _stty::
                      9 {
0000 abf5            10 	dec	fp,#~L2
0002 1df6            11 	ldl	@fp,rr6
0004 a172            12 	ld	r2,r7
0006 2106  7409      13 	ld	r6,#29705
000a 61f5  0000      14 	ld	r5,~L1(fp)
000e 5f00  0000*     15 	callr	_ioctl
0012 a9f5            16 	inc	fp,#~L2
0014 9e08            17 	ret
                     18 	~L1 := 0
                     19 	~L2 := 6
                     20 	}	/* _stty */
                     21 
                     22 .data
                     23 
                     24 /*~~gtty:*/
                     25 
                     26 .code
                     27 
                     28 _gtty::
                     29 {
0016 abf5            30 	dec	fp,#~L2
0018 1df6            31 	ldl	@fp,rr6
001a a172            32 	ld	r2,r7
001c 2106  7408      33 	ld	r6,#29704
0020 61f5  0000      34 	ld	r5,~L1(fp)
0024 5f00  0000*     35 	callr	_ioctl
0028 a9f5            36 	inc	fp,#~L2
002a 9e08            37 	ret
                     38 	~L1 := 0
                     39 	~L2 := 6
                     40 	}	/* _gtty */
                     41 
                     42 .data
                     43 
