LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~_fltuse:*/
                      5 
                      6 .code
                      7 
                      8 __fltuse::
                      9 {
0000 9e08            10 	ret
                     11 	~L1 := 0
                     12 	~L2 := 0
                     13 	}	/* __fltuse */
                     14 
                     15 .data
                     16 
                     17 /*~~_fegcvt:*/
                     18 
                     19 .code
                     20 
                     21 __fegcvt::
                     22 {
0002 9e08            23 	ret
                     24 	~L1 := 0
                     25 	~L2 := 0
                     26 	}	/* __fegcvt */
                     27 
                     28 .data
                     29 
                     30 /*~~_inflt:*/
                     31 
                     32 .code
                     33 
                     34 __inflt::
                     35 {
0004 9e08            36 	ret
                     37 	~L1 := 0
                     38 	~L2 := 0
                     39 	}	/* __inflt */
                     40 
                     41 .data
                     42 
