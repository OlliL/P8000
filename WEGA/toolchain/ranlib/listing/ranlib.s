
fp		:= r15;
sp		:= r15;
_sccsid:
.byte	%40
.byte	%5b
.byte	%24
.byte	%5d
.byte	%72
.byte	%61
.byte	%6e
.byte	%6c
.byte	%69
.byte	%62
.byte	%2e
.byte	%63
.byte	%9
.byte	%31
.byte	%2e
.byte	%31
.byte	%20
.byte	%20
.byte	%30
.byte	%37
.byte	%2f
.byte	%32
.byte	%34
.byte	%2f
.byte	%38
.byte	%32
.byte	%20
.byte	%30
.byte	%32
.byte	%3a
.byte	%34
.byte	%39
.byte	%3a
.byte	%32
.byte	%33
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
.comm	26,	_archdr;
.comm	24,	_exp;
.comm	4,	_arsize;
.comm	2,	_fi;
.comm	2,	_fo;
.comm	2,	_new;
.comm	2,	_longnm;
.comm	4,	_off;
.comm	4,	_oldoff;
.comm	4,	_offdelta;
.comm	8400,	_tab;
.comm	2,	_tnum;
_tempnm::
.byte	%5f
.byte	%5f
.byte	%2e
.byte	%53
.byte	%59
.byte	%4d
.byte	%44
.byte	%45
.byte	%46
.byte	%0
.comm	18,	_firstname;
.comm	2,	_av0;
/*~~trim:*/

.code

_trim::
{
	dec	fp,#~L2
	ld	@fp,r7
	ld	r1,r7
	ld	r4,r7
	jpr	L38
L20001:
	ld	r2,r4
	inc	r4,#1
	ldb	rl3,@r2
	cpb	rl3,#47
	jpr	ne,L38
	testb	@r4
	jpr	eq,L38
	ld	r1,r4
L38:
	testb	@r4
	jpr	ne,L20001
	ld	r2,r1
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 8
	}	/* _trim */

.data

L46:
.byte	%72,%0
.even
L49:
.byte	%25,%73,%3a,%20,%63,%61,%6e,%6e
.byte	%6f,%74,%20,%6f,%70,%65,%6e,%20
.byte	%25,%73,%a,%0
.even
L52:
.byte	%6e,%6f,%74,%20,%61,%72,%63,%68
.byte	%69,%76,%65,%3a,%20,%25,%73,%a
.byte	%0
.even
L66:
.byte	%25,%73,%3a,%20,%25,%73,%2d,%2d
.byte	%20,%6e,%6f,%20,%6e,%61,%6d,%65
.byte	%20,%6c,%69,%73,%74,%a,%0
.even
L74:
.byte	%77,%0
.even
L76:
.byte	%63,%61,%6e,%27,%74,%20,%63,%72
.byte	%65,%61,%74,%65,%20,%74,%65,%6d
.byte	%70,%6f,%72,%61,%72,%79,%a,%0
.even
L81:
.byte	%61,%72,%20,%72,%6c,%62,%20,%25
.byte	%73,%20,%25,%73,%20,%25,%73,%a
.byte	%0
.even
L83:
.byte	%61,%72,%20,%72,%6c,%20,%25,%73
.byte	%20,%25,%73,%a,%0
.even
L86:
.byte	%63,%61,%6e,%27,%74,%20,%65,%78
.byte	%65,%63,%75,%74,%65,%20,%25,%73
.byte	%a,%0
.even
/*~~main:*/

.code

_main::
{
	sub	fp,#~L2
	ld	406(fp),r14
	ldl	@fp,rr6
	ld	r7,r6
	ld	r2,@r7
	ld	r7,r2
	callr	_trim
	ld	_av0,r2
	dec	~L1+2(fp),#1
	jpr	L44
L20003:
	ld	r4,~L1(fp)
	ld	r5,@r4
	ld	r4,r5
	ld	r6,#L49
	ld	r5,_av0
L20005:
	ld	r7,#__iob+16
	callr	_fprintf
	jpr	L44
L20007:
	ld	r5,~L1(fp)
	ld	r4,@r5
	ld	r5,r4
	ld	r6,#L52
	jpr	L20005
L59:
	ld	r7,#_exp
	ldk	r6,#1
	ld	r5,#24
	ld	r4,_fi
	callr	_fread
	ld	r7,#_exp
	callr	_goodmagic
	test	r2
	jpr	eq,L58
	test	_swap_flg
	jpr	eq,L62
	ld	r7,#_exp
	ld	r6,r7
	ld	r5,#24
	callr	_swab
L62:
	ldl	rr2,_exp+2
	ldl	~L1+260(fp),rr2
	bit	_exp+18,#0
	jpr	ne,L64
	ldl	rr6,rr2
	multl	rq4,#2
	ldl	~L1+260(fp),rr6
L64:
	ldk	r2,#0
	ld	r3,_exp+10
	addl	rr2,~L1+260(fp)
	ldl	~L1+260(fp),rr2
	ld	r7,_fi
	ldl	rr4,rr2
	ldk	r3,#1
	callr	_fseek
	ld	r3,_exp+12
	ldk	r2,#0
	div	rr2,#14
	ld	r14,r3
	test	r3
	jpr	ne,L67
	ld	r7,#__iob+16
	ld	r6,#L66
	ld	r5,_av0
	ld	r4,#_archdr
	callr	_fprintf
L58:
	ld	r7,_fi
	callr	_nextel
	test	r2
	jpr	ne,L59
	callr	_fnew
	ld	_new,r2
	ld	r7,_fi
	callr	_fclose
	ld	r7,#_tempnm
	ld	r6,#L74
	callr	_fopen
	ld	_fo,r2
	test	r2
	jpr	ne,L75
	ld	r7,#__iob+16
	ld	r6,#L76
	callr	_fprintf
	ldk	r7,#1
	callr	_exit
L75:
	ld	r7,#_tab
	ld	r6,_tnum
	ldk	r5,#12
	ld	r4,_fo
	callr	_fwrite
	ld	r7,_fo
	callr	_fclose
	test	_new
	jpr	eq,L79
	lda	r7,~L1+4(fp)
	ld	r4,~L1(fp)
	ld	r5,@r4
	ld	r4,r5
	ld	r5,#_firstname
	ld	r6,#L81
	ld	r3,#_tempnm
	jpr	L20008
L67:
	dec	r14,#1
	cp	r14,#0
	jpr	lt,L58
	lda	r7,~L1+264(fp)
	ld	r6,_fi
	callr	_readsym
	ld	_longnm,r2
	sub	r14,r2
	ldb	rl2,~L1+268(fp)
	extsb	r2
	bit	r2,#5
	jpr	eq,L67
	ldb	rl2,~L1+268(fp)
	extsb	r2
	and	r2,#31
	jpr	eq,L67
	lda	r7,~L1+264(fp)
	callr	_stash
	jpr	L67
L79:
	lda	r7,~L1+4(fp)
	ld	r4,~L1(fp)
	ld	r5,@r4
	ld	r4,r5
	ld	r5,#_tempnm
	ld	r6,#L83
L20008:
	callr	_sprintf
	lda	r7,~L1+4(fp)
	callr	_system
	test	r2
	jpr	eq,L85
	lda	r5,~L1+4(fp)
	ld	r6,#L86
	ld	r7,#__iob+16
	callr	_fprintf
	jpr	L87
L85:
	ld	r7,~L1(fp)
	ld	r6,@r7
	ld	r7,r6
	callr	_fixdate
L87:
	ld	r7,#_tempnm
	callr	_unlink
	jpr	L44
L20010:
	inc	~L1(fp),#2
	ld	r7,~L1(fp)
	ld	r6,@r7
	ld	r7,r6
	ld	r6,#L46
	callr	_fopen
	ld	_fi,r2
	test	r2
	jpr	eq,L20003
	ldl	rr2,#2
	ldl	_off,rr2
	ld	r7,#_exp
	ldk	r6,#1
	ldk	r5,#2
	ld	r4,_fi
	callr	_fread
	cp	_exp,#65381
	jpr	ne,L20007
	ld	r7,_fi
	subl	rr4,rr4
	sub	r3,r3
	callr	_fseek
	clr	_tnum
	clr	_new
	ld	r7,_fi
	callr	_nextel
	test	r2
	jpr	ne,L59
	ld	r7,_fi
	callr	_fclose
L44:
	ld	r2,~L1+2(fp)
	dec	~L1+2(fp),#1
	test	r2
	jpr	ne,L20010
	sub	r7,r7
	callr	_exit
	ld	r14,406(fp)
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 408
	}	/* _main */

.data

/*~~nextel:*/

.code

_nextel::
{
	dec	fp,#~L2
	ld	4(fp),r14
	ld	@fp,r7
	ldl	rr2,_off
	ldl	_oldoff,rr2
	ld	r6,r7
	ldl	rr4,_off
	sub	r3,r3
	callr	_fseek
	ld	r7,#_archdr
	ldk	r6,#1
	ld	r5,#26
	ld	r4,~L1(fp)
	callr	_fread
	ld	r14,r2
	cp	r2,#0
	jpr	gt,L91
	sub	r2,r2
	jpr	L10006
L91:
	ldl	rr2,_archdr+22
	ldl	rr4,rr2
	ldk	r4,#0
	and	r5,#1
	ldl	rr2,rr4
	testl	rr4
	jpr	eq,L92
	ldl	rr4,_archdr+22
	addl	rr4,#1
	ldl	_archdr+22,rr4
L92:
	ld	r7,~L1(fp)
	callr	_ftell
	addl	rr2,_archdr+22
	ldl	_off,rr2
	ldk	r2,#1
L10006:
	ld	r14,4(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 6
	}	/* _nextel */

.data

L95:
.byte	%73,%79,%6d,%62,%6f,%6c,%20,%74
.byte	%61,%62,%6c,%65,%20,%6f,%76,%65
.byte	%72,%66,%6c,%6f,%77,%a,%0
.even
/*~~stash:*/

.code

_stash::
{
	dec	fp,#~L2
	ld	@fp,r7
	cp	_tnum,#700
	jpr	lt,L94
	ld	r7,#__iob+16
	ld	r6,#L95
	callr	_fprintf
	ldk	r7,#1
	callr	_exit
L94:
	test	_longnm
	jpr	ne,L96
	ld	~L1+4(fp),#8
	jpr	L97
L96:
	ld	r2,~L1(fp)
	ldb	rl3,6(r2)
	extsb	r3
	and	r3,#127
	ld	~L1+4(fp),r3
L97:
	clr	~L1+2(fp)
	jpr	L100
L20012:
	ld	r2,~L1+2(fp)
	ld	r3,~L1(fp)
	inc	r3,#6
	add	r3,r2
	ld	r5,_tnum
	mult	rr4,#12
	add	r5,#_tab+4
	add	r5,r2
	ldb	rl4,@r3
	ldb	@r5,rl4
	inc	~L1+2(fp),#1
L100:
	ld	r2,~L1+2(fp)
	cp	r2,~L1+4(fp)
	jpr	lt,L20012
	ld	r3,_tnum
	mult	rr2,#12
	ldl	rr4,_oldoff
	ldl	_tab(r3),rr4
	inc	_tnum,#1
	test	_longnm
	jpr	eq,L10009
	dec	~L1+4(fp),#8
	ld	r2,~L1+4(fp)
	inc	r2,#11
	ld	r7,r2
	exts	rr6
	div	rr6,#12
	add	r7,_tnum
	ld	_tnum,r7
L10009:
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 6
	}	/* _stash */

.data

/*~~fnew:*/

.code

_fnew::
{
	dec	fp,#~L2
	ld	r3,_tnum
	mult	rr2,#12
	add	r3,#26
	ld	r5,r3
	exts	rr4
	ldl	_offdelta,rr4
	ldl	rr2,#2
	ldl	_off,rr2
	ld	r7,_fi
	callr	_nextel
	ld	r7,#_archdr
	ld	r6,#_tempnm
	ldk	r5,#14
	callr	_strncmp
	test	r2
	jpr	ne,L104
	clr	_new
	ldl	rr2,_archdr+22
	addl	rr2,#26
	ldl	rr4,_offdelta
	subl	rr4,rr2
	ldl	_offdelta,rr4
	jpr	L105
L104:
	ld	_new,#1
	ld	r7,#_firstname
	ld	r6,#_archdr
	ldk	r5,#14
	callr	_strncpy
L105:
	clr	~L1(fp)
	jpr	L109
L20014:
	ld	r5,~L1(fp)
	mult	rr4,#12
	ldl	rr6,_tab(r5)
	addl	rr6,_offdelta
	ldl	_tab(r5),rr6
	test	_swap_flg
	jpr	eq,L110
	ld	r5,~L1(fp)
	mult	rr4,#12
	add	r5,#_tab
	ld	r7,r5
	callr	_longswap
L110:
	ld	r3,~L1(fp)
	mult	rr2,#12
	ldb	rl2,_tab+4(r3)
	extsb	r2
	bit	r2,#7
	jpr	eq,L107
	ld	r3,~L1(fp)
	mult	rr2,#12
	ldb	rl2,_tab+4(r3)
	extsb	r2
	and	r2,#127
	ld	~L1+2(fp),r2
	dec	~L1+2(fp),#8
	ld	r2,~L1+2(fp)
	inc	r2,#11
	ld	r5,r2
	exts	rr4
	div	rr4,#12
	add	r5,~L1(fp)
	ld	~L1(fp),r5
L107:
	inc	~L1(fp),#1
L109:
	ld	r2,~L1(fp)
	cp	r2,_tnum
	jpr	lt,L20014
	ld	r2,_new
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 6
	}	/* _fnew */

.data

L115:
.byte	%63,%61,%6e,%27,%74,%20,%72,%65
.byte	%6f,%70,%65,%6e,%20,%25,%73,%a
.byte	%0
.even
/*~~fixdate:*/

.code

_fixdate::
{
	dec	fp,#~L2
	ld	6(fp),r14
	ld	@fp,r7
	ldk	r6,#1
	callr	_open
	ld	r14,r2
	cp	r2,#0
	jpr	ge,L114
	ld	r7,#__iob+16
	ld	r6,#L115
	ld	r5,~L1(fp)
	callr	_fprintf
	jpr	L10015
L114:
	sub	r7,r7
	callr	_time
	inc	r2,#5
	ld	r5,r2
	exts	rr4
	ldl	~L1+2(fp),rr4
	ld	r4,#_archdr+14
	sub	r4,#_archdr
	ld	r3,r4
	exts	rr2
	addl	rr2,#2
	ldl	rr4,rr2
	ld	r7,r14
	sub	r3,r3
	callr	_lseek
	lda	r6,~L1+2(fp)
	ld	r7,r14
	ldk	r5,#4
	callr	_write
	ld	r7,r14
	callr	_close
L10015:
	ld	r14,6(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 8
	}	/* _fixdate */

.data

