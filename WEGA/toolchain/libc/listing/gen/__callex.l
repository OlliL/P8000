LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~_callex:*/
                      5 
                      6 .code
                      7 
                      8 __callex::
                      9 {
0000 5f00  0000*     10 	callr	_exit
0004 9e08            11 	ret
                     12 	~L1 := 0
                     13 	~L2 := 0
                     14 	}	/* __callex */
                     15 
                     16 .data
                     17 
