
fp		:= r15;
sp		:= r15;
_sccsid:
.byte	%40
.byte	%5b
.byte	%24
.byte	%5d
.byte	%73
.byte	%74
.byte	%72
.byte	%69
.byte	%70
.byte	%2e
.byte	%63
.byte	%9
.byte	%31
.byte	%2e
.byte	%32
.byte	%20
.byte	%20
.byte	%31
.byte	%31
.byte	%2f
.byte	%31
.byte	%39
.byte	%2f
.byte	%38
.byte	%32
.byte	%20
.byte	%31
.byte	%34
.byte	%3a
.byte	%30
.byte	%38
.byte	%3a
.byte	%33
.byte	%39
.byte	%20
.byte	%2d
.byte	%20
.byte	%38
.byte	%37
.byte	%77
.byte	%65
.byte	%67
.byte	%61
.byte	%33
.byte	%2e
.byte	%32
.byte	%0
.byte	0
.comm	24,	_head;
.comm	4096,	_segt;
.comm	2,	_tf;
.comm	2,	_rf;
.comm	4,	_imsize;
.comm	2,	_segtsz;
L31:
.byte	%55,%73,%61,%67,%65,%3a,%20,%73
.byte	%74,%72,%69,%70,%20,%5b,%2d,%68
.byte	%5d,%20,%66,%69,%6c,%65,%2d,%6e
.byte	%61,%6d,%65,%20,%2e,%2e,%2e,%20
.byte	%a,%0
.even
L35:
.byte	%2d,%68,%0
.even
L37:
.byte	%73,%74,%72,%69,%70,%3a,%20,%69
.byte	%6c,%6c,%65,%67,%61,%6c,%20,%6f
.byte	%70,%74,%69,%6f,%6e,%3a,%20,%25
.byte	%73,%a,%0
.even
L38:
.byte	%2f,%74,%6d,%70,%2f,%73,%58,%58
.byte	%58,%58,%58,%58,%0
.even
L39:
.byte	%77,%2b,%0
.even
L41:
.byte	%73,%74,%72,%69,%70,%3a,%20,%63
.byte	%61,%6e,%6e,%6f,%74,%20,%63,%72
.byte	%65,%61,%74,%65,%20,%74,%65,%6d
.byte	%70,%6f,%72,%61,%72,%79,%20,%66
.byte	%69,%6c,%65,%a,%0
.even
/*~~main:*/

.code

_main::
{
	dec	fp,#~L2
	ld	12(fp),r14
	ldl	@fp,rr6
	ld	r2,r6
	ld	r3,2(r2)
	cpb	@r3,#45
	jpr	ne,L10003
	ldk	r3,#3
	jpr	L10004
L10003:
	ldk	r3,#2
L10004:
	cp	r3,~L1+2(fp)
	jpr	le,L29
	ld	r7,#__iob+16
	ld	r6,#L31
	callr	_fprintf
	ldk	r7,#2
	callr	_exit
L29:
	ld	r2,~L1(fp)
	ld	r3,2(r2)
	cpb	@r3,#45
	jpr	ne,L33
	ld	r7,r2
	ld	r3,2(r7)
	ld	r7,r3
	ld	r6,#L35
	callr	_strcmp
	cp	r2,#0
	ldk	r2,#0
	tcc	eq,r2
	ld	~L1+8(fp),r2
	test	r2
	jpr	ne,L33
	ld	r5,~L1(fp)
	ld	r4,2(r5)
	ld	r5,r4
	ld	r6,#L37
	ld	r7,#__iob+16
	callr	_fprintf
	ldk	r7,#2
	callr	_exit
L33:
	ld	r7,#L38
	callr	_mktemp
	ld	~L1+4(fp),r2
	ld	r7,r2
	ld	r6,#L39
	callr	_fopen
	ld	_tf,r2
	test	r2
	jpr	ne,L40
	ld	r7,#__iob+16
	ld	r6,#L41
	callr	_fprintf
	ldk	r7,#2
	callr	_exit
L40:
	ld	r7,~L1+4(fp)
	callr	_unlink
	ld	r2,~L1(fp)
	ld	r3,2(r2)
	cpb	@r3,#45
	jpr	ne,L10006
	ldk	r3,#2
	jpr	L10007
L10006:
	ldk	r3,#1
L10007:
	ld	r14,r3
	jpr	L45
L20001:
	ld	r2,r14
	add	r2,r14
	ld	r7,~L1(fp)
	add	r7,r2
	ld	r6,@r7
	ld	r7,r6
	ld	r6,~L1+8(fp)
	callr	_strip
	ld	~L1+6(fp),r2
	cp	r2,#1
	jpr	gt,L44
	inc	r14,#1
L45:
	cp	r14,~L1+2(fp)
	jpr	lt,L20001
L44:
	ld	r7,_tf
	callr	_fclose
	ld	r7,~L1+6(fp)
	callr	_exit
	ld	r14,12(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 14
	}	/* _main */

.data

L50:
.byte	%72,%0
.even
L52:
.byte	%73,%74,%72,%69,%70,%3a,%20,%75
.byte	%6e,%61,%62,%6c,%65,%20,%74,%6f
.byte	%20,%6f,%70,%65,%6e,%20,%25,%73
.byte	%a,%0
.even
L57:
.byte	%73,%74,%72,%69,%70,%3a,%20,%25
.byte	%73,%20,%6e,%6f,%74,%20,%69,%6e
.byte	%20,%63,%6f,%72,%72,%65,%63,%74
.byte	%20,%6f,%62,%6a,%65,%63,%74,%20
.byte	%66,%6f,%72,%6d,%61,%74,%a,%0
.even
L62:
.byte	%73,%74,%72,%69,%70,%3a,%20,%25
.byte	%73,%20,%61,%6c,%72,%65,%61,%64
.byte	%79,%20,%73,%74,%72,%69,%70,%70
.byte	%65,%64,%a,%0
.even
L71:
.byte	%73,%74,%72,%69,%70,%3a,%20,%77
.byte	%72,%69,%74,%65,%20,%65,%72,%72
.byte	%6f,%72,%a,%0
.even
L76:
.byte	%73,%74,%72,%69,%70,%3a,%20,%77
.byte	%72,%69,%74,%65,%20,%65,%72,%72
.byte	%6f,%72,%a,%0
.even
L84:
.byte	%77,%0
.even
L86:
.byte	%73,%74,%72,%69,%70,%3a,%20,%63
.byte	%61,%6e,%6e,%6f,%74,%20,%72,%65
.byte	%63,%72,%65,%61,%74,%65,%20,%25
.byte	%73,%a,%0
.even
/*~~strip:*/

.code

_strip::
{
	dec	fp,#~L2
	ldl	@fp,rr6
	ld	r2,r7
	ld	r6,#L50
	callr	_fopen
	ld	~L1+4(fp),r2
	test	r2
	jpr	ne,L51
	ld	r7,#__iob+16
	ld	r6,#L52
L20004:
	ld	r5,~L1+2(fp)
	callr	_fprintf
	jpr	L20002
L20016:
	ld	r7,#__iob+16
	ld	r6,#L57
L20008:
	ld	r5,~L1+2(fp)
	callr	_fprintf
	ld	r7,~L1+4(fp)
	callr	_fclose
L20002:
	ldk	r2,#1
	jpr	L10008
L51:
	lda	r7,~L1+6(fp)
	ldk	r6,#2
	ldk	r5,#1
	ld	r4,~L1+4(fp)
	callr	_fread
	ld	r7,~L1+4(fp)
	subl	rr4,rr4
	sub	r3,r3
	callr	_fseek
	ld	r7,~L1+6(fp)
	callr	_goodmagic
	test	r2
	jpr	eq,L20016
	test	_swap_flg
	jpr	eq,L58
	lda	r7,~L1+6(fp)
	lda	r6,~L1+6(fp)
	ldk	r5,#2
	callr	_swab
L58:
	ld	r7,#_head
	ld	r6,#24
	ldk	r5,#1
	ld	r4,~L1+4(fp)
	callr	_fread
	ld	~L1+8(fp),#1
	test	_swap_flg
	jpr	eq,L60
	lda	r7,~L1+8(fp)
	lda	r6,~L1+8(fp)
	ldk	r5,#2
	callr	_swab
L60:
	test	_head+12
	jpr	ne,L61
	ld	r2,_head+18
	and	r2,~L1+8(fp)
	cp	r2,~L1+8(fp)
	jpr	ne,L61
	test	~L1(fp)
	jpr	ne,L61
	ld	r7,#__iob+16
	ld	r6,#L62
	jpr	L20008
L61:
	ld	r7,_tf
	subl	rr4,rr4
	sub	r3,r3
	callr	_fseek
	ldl	rr2,_head+2
	ldl	_imsize,rr2
	ld	r4,_head+10
	ld	_segtsz,r4
	test	_swap_flg
	jpr	eq,L63
	ld	r7,#_imsize
	callr	_longswap
	ld	r7,#_segtsz
	ld	r6,r7
	ldk	r5,#2
	callr	_swab
L63:
	ld	r5,_segtsz
	srl	r5,#4
	ld	r6,#16
	ld	r7,#_segt
	ld	r4,~L1+4(fp)
	callr	_fread
	test	_swap_flg
	jpr	eq,L65
	ld	r7,#_segt
	ld	r6,r7
	ld	r5,_segtsz
	callr	_swab
L65:
	test	~L1(fp)
	jpr	ne,L66
	clr	_head+12
	ld	r2,_head+18
	or	r2,~L1+8(fp)
	ld	_head+18,r2
	cp	~L1+6(fp),#58631
	jpr	eq,L10011
	cp	~L1+6(fp),#58641
	jpr	eq,L10011
	cp	~L1+6(fp),#58629
	jpr	ne,L67
L10011:
	ldk	r2,#0
	ld	r3,_segt+4
	ld	r4,r2
	ld	r5,_segt+6
	addl	rr2,rr4
	ldl	_head+2,rr2
	ld	r2,_segt+4
	ld	_head+20,r2
	test	_swap_flg
	jpr	eq,L67
	ld	r7,#_head+2
	callr	_longswap
L67:
	ld	r7,#_head
	ld	r6,#24
	ldk	r5,#1
	ld	r4,_tf
	callr	_fwrite
	cp	r2,#1
	jpr	eq,L70
	ld	r7,#__iob+16
	ld	r6,#L71
L20010:
	callr	_fprintf
L20011:
	ldk	r2,#2
	jpr	L10008
L70:
	cp	~L1+6(fp),#58631
	jpr	eq,L10012
	cp	~L1+6(fp),#58641
	jpr	eq,L10012
	cp	~L1+6(fp),#58629
	jpr	ne,L72
L10012:
	ld	r2,_segt+4
	add	r2,_segt+6
	ldk	r4,#0
	ld	r5,r2
	ld	r2,r4
	ld	r3,_segtsz
	addl	rr4,rr2
	addl	rr4,#24
	ldl	~L1+10(fp),rr4
	jpr	L73
L72:
	ldk	r2,#0
	ld	r3,_segtsz
	addl	rr2,_imsize
	addl	rr2,#24
	ldl	~L1+10(fp),rr2
L73:
	test	_swap_flg
	jpr	eq,L74
	ld	r7,#_segt
	ld	r6,r7
	ld	r5,_segtsz
	callr	_swab
L74:
	ld	r5,_segtsz
	srl	r5,#4
	ld	r6,#16
	ld	r7,#_segt
	ld	r4,_tf
	callr	_fwrite
	ld	r3,_segtsz
	srl	r3,#4
	cp	r2,r3
	jpr	eq,L77
	ld	r7,#__iob+16
	ld	r6,#L76
	jpr	L20010
L66:
	ldl	rr2,_imsize
	ldl	~L1+10(fp),rr2
L77:
	cp	~L1+6(fp),#58631
	jpr	eq,L10013
	cp	~L1+6(fp),#58641
	jpr	eq,L10013
	cp	~L1+6(fp),#58629
	jpr	ne,L78
L10013:
	test	~L1(fp)
	jpr	ne,L78
	ld	r7,~L1+2(fp)
	ld	r6,~L1+4(fp)
	ld	r5,_tf
	ldl	rr2,_imsize
	callr	_rcopy
	test	r2
	jpr	eq,L81
L20013:
	ld	r7,~L1+4(fp)
	callr	_fclose
	jpr	L20011
L78:
	ld	r7,~L1+2(fp)
	ld	r6,~L1+4(fp)
	ld	r5,_tf
	ldl	rr2,_imsize
	callr	_copy
	test	r2
	jpr	ne,L20013
L81:
	ld	r7,~L1+4(fp)
	callr	_fclose
	ld	r7,~L1+2(fp)
	ld	r6,#L84
	callr	_fopen
	ld	~L1+4(fp),r2
	test	r2
	jpr	ne,L85
	ld	r7,#__iob+16
	ld	r6,#L86
	jpr	L20004
L85:
	ld	r7,_tf
	subl	rr4,rr4
	sub	r3,r3
	callr	_fseek
	ld	r7,~L1+2(fp)
	ld	r6,_tf
	ld	r5,~L1+4(fp)
	ldl	rr2,~L1+10(fp)
	callr	_copy
	test	r2
	jpr	ne,L20013
	sub	r2,r2
L10008:
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 16
	}	/* _strip */

.data

L93:
.byte	%73,%74,%72,%69,%70,%3a,%20,%75
.byte	%6e,%65,%78,%70,%65,%63,%74,%65
.byte	%64,%20,%65,%6f,%66,%20,%6f,%6e
.byte	%20,%25,%73,%a,%0
.even
L95:
.byte	%73,%74,%72,%69,%70,%3a,%20,%77
.byte	%72,%69,%74,%65,%20,%65,%72,%72
.byte	%6f,%72,%20,%6f,%6e,%20,%25,%73
.byte	%a,%0
.even
/*~~copy:*/

.code

_copy::
{
	sub	fp,#~L2
	ldm	526(fp),r13,#2
	ldm	@fp,r2,#6
	ldk	r7,#1
	ldk	r6,#1
	callr	_signal
	ldk	r7,#2
	ldk	r6,#1
	callr	_signal
	ldk	r7,#3
	ldk	r6,#1
	callr	_signal
	jpr	L90
L20018:
	ldl	rr2,~L1(fp)
	cpl	rr2,#512
	jpr	le,L10017
	ldl	rr4,#512
	jpr	L10018
L10017:
	ldl	rr4,~L1(fp)
L10018:
	ld	r14,r5
	lda	r7,~L1+12(fp)
	ldk	r6,#1
	ld	r4,~L1+8(fp)
	callr	_fread
	ld	r13,r2
	cp	r14,r2
	jpr	eq,L92
	ld	r7,#__iob+16
	ld	r6,#L93
	jpr	L20021
L20023:
	ld	r7,#__iob+16
	ld	r6,#L95
L20021:
	ld	r5,~L1+10(fp)
	callr	_fprintf
	ldk	r2,#1
	jpr	L10014
L92:
	lda	r7,~L1+12(fp)
	ldk	r6,#1
	ld	r5,r13
	ld	r4,~L1+6(fp)
	callr	_fwrite
	cp	r13,r2
	jpr	ne,L20023
	ld	r3,r13
	exts	rr2
	ldl	rr4,~L1(fp)
	subl	rr4,rr2
	ldl	~L1(fp),rr4
L90:
	ldl	rr2,~L1(fp)
	testl	rr2
	jpr	ne,L20018
	ldk	r7,#1
	sub	r6,r6
	callr	_signal
	ldk	r7,#2
	sub	r6,r6
	callr	_signal
	ldk	r7,#3
	sub	r6,r6
	callr	_signal
	sub	r2,r2
L10014:
	ldm	r13,526(fp),#2
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 530
	}	/* _copy */

.data

L97:
.byte	%72,%0
.even
L99:
.byte	%73,%74,%72,%69,%70,%3a,%20,%75
.byte	%6e,%61,%62,%6c,%65,%20,%74,%6f
.byte	%20,%6f,%70,%65,%6e,%20,%25,%73
.byte	%a,%0
.even
L104:
.byte	%73,%74,%72,%69,%70,%3a,%20,%77
.byte	%72,%69,%74,%65,%20,%65,%72,%72
.byte	%6f,%72,%a,%0
.even
L108:
.byte	%73,%74,%72,%69,%70,%3a,%20,%77
.byte	%72,%69,%74,%65,%20,%65,%72,%72
.byte	%6f,%72,%a,%0
.even
L111:
.byte	%73,%74,%72,%69,%70,%3a,%20,%77
.byte	%72,%69,%74,%65,%20,%65,%72,%72
.byte	%6f,%72,%a,%0
.even
/*~~rcopy:*/

.code

_rcopy::
{
	sub	fp,#~L2
	ldm	@fp,r2,#6
	ld	r4,r7
	ld	r6,#L97
	callr	_fopen
	ld	_rf,r2
	test	r2
	jpr	ne,L98
	ld	r7,#__iob+16
	ld	r6,#L99
	ld	r5,~L1+10(fp)
L20027:
	callr	_fprintf
	ldk	r2,#1
	jpr	L10019
L98:
	ldk	r2,#0
	ld	r3,_segtsz
	addl	rr2,#24
	addl	rr2,_imsize
	ldl	~L1+12(fp),rr2
	ld	r7,_rf
	ldl	rr4,rr2
	sub	r3,r3
	callr	_fseek
	ldk	r7,#1
	ldk	r6,#1
	callr	_signal
	ldk	r7,#2
	ldk	r6,#1
	callr	_signal
	ldk	r7,#3
	ldk	r6,#1
	callr	_signal
	jpr	L100
L20025:
	lda	r7,~L1+17(fp)
	ldk	r6,#1
	ld	r5,r6
	ld	r4,~L1+8(fp)
	callr	_fread
	lda	r7,~L1+18(fp)
	ldk	r6,#1
	ld	r5,r6
	ld	r4,_rf
	callr	_fread
	ld	r2,~L1+18(fp)
	and	r2,#15
	cp	r2,#9
	jpr	eq,L10022
	ld	r2,~L1+18(fp)
	and	r2,#248
	cp	r2,#16
	jpr	eq,L10022
	ld	r2,~L1+18(fp)
	and	r2,#248
	cp	r2,#48
	jpr	eq,L10022
	ld	r2,~L1+18(fp)
	and	r2,#15
	cp	r2,#11
	jpr	ne,L102
L10022:
	lda	r7,~L1+17(fp)
	ldk	r6,#1
	ld	r5,r6
	ld	r4,~L1+6(fp)
	callr	_fwrite
	cp	r2,#1
	jpr	eq,L105
	ld	r7,#__iob+16
	ld	r6,#L104
	jpr	L20027
L102:
	ld	r2,~L1+18(fp)
	and	r2,#15
	cp	r2,#10
	jpr	eq,L10023
	ld	r2,~L1+18(fp)
	and	r2,#248
	cp	r2,#32
	jpr	ne,L106
L10023:
	ld	r2,~L1+18(fp)
	ldb	rl2,rh2
	clrb	rh2
	ld	~L1+18(fp),r2
	lda	r7,~L1+18(fp)
	ldk	r6,#1
	ld	r5,r6
	ld	r4,~L1+6(fp)
	callr	_fwrite
	cp	r2,#1
	jpr	eq,L105
	ld	r7,#__iob+16
	ld	r6,#L108
	jpr	L20027
L106:
	lda	r7,~L1+17(fp)
	ldk	r6,#2
	ldk	r5,#1
	ld	r4,~L1+6(fp)
	callr	_fwrite
	cp	r2,#1
	jpr	eq,L105
	ld	r7,#__iob+16
	ld	r6,#L111
	jpr	L20027
L105:
	ldl	rr2,~L1(fp)
	subl	rr2,#2
	ldl	~L1(fp),rr2
L100:
	ldl	rr2,~L1(fp)
	testl	rr2
	jpr	ne,L20025
	ldk	r7,#1
	sub	r6,r6
	callr	_signal
	ldk	r7,#2
	sub	r6,r6
	callr	_signal
	ldk	r7,#3
	sub	r6,r6
	callr	_signal
	ld	r7,_rf
	callr	_fclose
	sub	r2,r2
L10019:
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 22
	}	/* _rcopy */

.data

