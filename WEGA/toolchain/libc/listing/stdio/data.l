LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 .comm	512,	__sibuf;
                      5 .comm	512,	__sobuf;
                      6 .even
                      7 __iob::
0000 0000*            8 .addr	__sibuf
0002 0000             9 .word	%0
0004 0000*           10 .addr	__sibuf
0006 01              11 .byte	%1
0007 00              12 .byte	%0
0008 0000            13 .word	%0
000a 0000            14 .word	%0
000c 0000            15 .word	%0
000e 02              16 .byte	%2
000f 01              17 .byte	%1
0010 0000            18 .word	%0
0012 0000            19 .word	%0
0014 0000            20 .word	%0
0016 06              21 .byte	%6
0017 02              22 .byte	%2
                     23 .=.+136
                     24 .even
                     25 __lastbuf::
00a0 00a0'           26 .addr	__iob+160
