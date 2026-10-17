LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 /*~~_findiop:*/
                      5 
                      6 .code
                      7 
                      8 __findiop::
                      9 {
0000 abf3            10 	dec	fp,#~L2
0002 6ffe  0002      11 	ld	2(fp),r14
0006 210e  0000*     12 	ld	r14,#__iob
000a e806            13 	jpr	L23
                     14 L20001:
000c 4b0e  0000*     15 	cp	r14,__lastbuf
0010 e702            16 	jpr	ult,L21
0012 8322            17 	sub	r2,r2
0014 e808            18 	jpr	L10000
                     19 L21:
0016 a9e7            20 	inc	r14,#8
                     21 L23:
0018 60ea  0006      22 	ldb	rl2,6(r14)
001c b120            23 	extsb	r2
001e 0702  0083      24 	and	r2,#131
0022 eef4            25 	jpr	ne,L20001
0024 a1e2            26 	ld	r2,r14
                     27 L10000:
0026 61fe  0002      28 	ld	r14,2(fp)
002a a9f3            29 	inc	fp,#~L2
002c 9e08            30 	ret
                     31 	~L1 := 0
                     32 	~L2 := 4
                     33 	}	/* __findiop */
                     34 
                     35 .data
                     36 
