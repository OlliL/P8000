LOC    OBJ CODE    STMT SOURCE STATEMENT
                      1 
                      2 fp		:= r15;
                      3 sp		:= r15;
                      4 .even
                      5 L14:
0000 0000             6 .word	0
                      7 /*~~strtok:*/
                      8 
                      9 .code
                     10 
                     11 _strtok::
                     12 {
0000 abfb            13 	dec	fp,#~L2
0002 5cf9  0c02      14 	ldm	6(fp),r12,#3
0006 0006  
0008 1df6            15 	ldl	@fp,rr6
000a 8d74            16 	test	r7
000c ee03            17 	jpr	ne,L15
000e 610e  0000'     18 	ld	r14,L14
0012 e802            19 	jpr	L16
                     20 L15:
0014 61fe  0002      21 	ld	r14,~L1+2(fp)
                     22 L16:
0018 8de4            23 	test	r14
001a ee02            24 	jpr	ne,L17
                     25 L20000:
001c 8322            26 	sub	r2,r2
001e e826            27 	jpr	L10000
                     28 L17:
0020 a1e7            29 	ld	r7,r14
0022 61f6  0000      30 	ld	r6,~L1(fp)
0026 5f00  0000*     31 	callr	_strspn
002a 81e2            32 	add	r2,r14
002c a12d            33 	ld	r13,r2
002e 0cd4            34 	testb	@r13
0030 e6f5            35 	jpr	eq,L20000
0032 a127            36 	ld	r7,r2
0034 61f6  0000      37 	ld	r6,~L1(fp)
0038 5f00  0000*     38 	callr	_strpbrk
003c a12c            39 	ld	r12,r2
003e 8d24            40 	test	r2
0040 ee03            41 	jpr	ne,L19
0042 4d08  0000'     42 	clr	L14
0046 e811            43 	jpr	L20
                     44 L19:
0048 0cc8            45 	clrb	@r12
004a a1c7            46 	ld	r7,r12
004c 61f6  0000      47 	ld	r6,~L1(fp)
0050 5f00  0000*     48 	callr	_strspn
0054 81c2            49 	add	r2,r12
0056 a12e            50 	ld	r14,r2
0058 8bc2            51 	cp	r2,r12
005a e303            52 	jpr	ule,L10003
005c a9e0            53 	inc	r14,#1
005e a1e2            54 	ld	r2,r14
0060 e802            55 	jpr	L10004
                     56 L10003:
0062 a9c0            57 	inc	r12,#1
0064 a1c2            58 	ld	r2,r12
                     59 L10004:
0066 6f02  0000'     60 	ld	L14,r2
                     61 L20:
006a a1d2            62 	ld	r2,r13
                     63 L10000:
006c 5cf1  0c02      64 	ldm	r12,6(fp),#3
0070 0006  
0072 a9fb            65 	inc	fp,#~L2
0074 9e08            66 	ret
                     67 	~L1 := 0
                     68 	~L2 := 12
                     69 	}	/* _strtok */
                     70 
                     71 .data
                     72 
