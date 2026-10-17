LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~abs:*/
                      5 
                      6 .code
                      7 
                      8 _abs::
                      9 {
0000 abf3            10 	dec	fp,#~L2
0002 2ff7            11 	ld	@fp,r7
0004 0b07  0000      12 	cp	r7,#0
0008 e903            13 	jpr	ge,L12
000a 8d72            14 	neg	r7
000c 6ff7  0000      15 	ld	~L1(fp),r7
                     16 L12:
0010 61f2  0000      17 	ld	r2,~L1(fp)
0014 a9f3            18 	inc	fp,#~L2
0016 9e08            19 	ret
                     20 	~L1 := 0
                     21 	~L2 := 4
                     22 	}	/* _abs */
                     23 
                     24 .data
                     25 
