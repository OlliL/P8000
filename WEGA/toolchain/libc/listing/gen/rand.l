LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 .even
                      5 _randx:
0000 0000  0001       6 .long	%1
                      7 /*~~srand:*/
                      8 
                      9 .code
                     10 
                     11 _srand::
                     12 {
0000 abf1            13 	dec	fp,#~L2
0002 2ff7            14 	ld	@fp,r7
0004 bd20            15 	ldk	r2,#0
0006 a173            16 	ld	r3,r7
0008 5d02  0000'     17 	ldl	_randx,rr2
000c a9f1            18 	inc	fp,#~L2
000e 9e08            19 	ret
                     20 	~L1 := 0
                     21 	~L2 := 2
                     22 	}	/* _srand */
                     23 
                     24 .data
                     25 
                     26 /*~~rand:*/
                     27 
                     28 .code
                     29 
                     30 _rand::
                     31 {
0010 abf1            32 	dec	fp,#~L2
0012 5406  0000'     33 	ldl	rr6,_randx
0016 1804  41c6      34 	multl	rq4,#1103515245
001a 4e6d  
001c 1606  0000      35 	addl	rr6,#12345
0020 3039  
0022 5d06  0000'     36 	ldl	_randx,rr6
0026 b36d  fff0      37 	sral	rr6,#16
002a bd60            38 	ldk	r6,#0
002c 0707  7fff      39 	and	r7,#32767
0030 a172            40 	ld	r2,r7
0032 a9f1            41 	inc	fp,#~L2
0034 9e08            42 	ret
                     43 	~L1 := 0
                     44 	~L2 := 2
                     45 	}	/* _rand */
                     46 
                     47 .data
                     48 
