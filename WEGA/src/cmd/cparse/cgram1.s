// cgram1: the parser of /lib/cparse (cgram1.y 2.5) cut from the original by tools/cutgram.py
// (fallback until the grammar is rebuilt; do not edit, make it again)
	.comm	2, _curftn
	.comm	200, _asavbc
	.comm	2, _instruct
	.comm	6500, _ntab
	.comm	2, _bssgap1
	.comm	2, _brklab
	.comm	4, _yyval
	.comm	2, _stwart
	.comm	600, _yyv
	.comm	2, _contlab
	.comm	2, _paramno
	.comm	2, _Ffile
	.comm	2, _flostat
	.comm	4, _lastcon
	.comm	2, _curdim
	.comm	2, _idname
	.comm	4, _strucoff
	.comm	8, _dcon
	.comm	2, _blevel
	.comm	2, _regvar
	.comm	2, _curclass
	.comm	2, _retstat
	.comm	4, _yylval
	.comm	1800, _swtab
	.comm	2, _swp
	.comm	2, _swx
	.comm	4, _autooff
	.comm	2, _lregvar
	.comm	4000, _dimtab
	.comm	2, _dfile
	.comm	5600, _node
	.comm	2, _dpath
	.comm	2, _sfile
	.comm	2, _outfile
	.comm	2, _minrvar
	.comm	4, _ftnpos
	.comm	50, _dfname
	.comm	2, _fminrvar
	.comm	4, _inoff
	.comm	50, _ifname
	.comm	2, _ipath
	.comm	2, _fregvar
	.comm	50, _sfname
	.comm	2, _spath
	.comm	2, _strflg
	.comm	4, _maxoff
	.comm	258, _lxmask
	.comm	2, _lxmatch
	.comm	126, _yytext
	.comm	2, _lxgcp
	.comm	258, _lxcp
	.comm	2, _pregvar
	.comm	300, _paramstk
	.comm	4, _dpos
	.comm	2, _pfregvar
	.comm	258, _dope
	.comm	2, _afull
	.comm	258, _opst
	.comm	512, _dbuf
	.comm	512, _ibuf
	.comm	512, _sbuf
	.comm	2, _lastfree
	.comm	220, _instack
	.comm	2, _iclass
	.comm	2, _pstk
	.comm	128, _hashtab
	.comm	2, _bssgap2
	.comm	4, _shdrpos
	.comm	3306, _reftab
	.comm	2, _commflag
	.comm	14, _shdr
	.comm	1102, _refsym
	.comm	1000, _taglist
	.extern	_Fflag, _bccode, _bcon, _beginit, _bfend, _block, _branch, _bstruct
	.extern	_buildtree, _cerror, _clearst, _ctype, _dclargs, _dclstruct, _defid, _deflab
	.extern	_doinit, _doszof, _ecomp, _endinit, _f3970, _falloc, _fprintf, _ftnarg
	.extern	_ftnend, _genswitch, _getlab, _getstr, _icons, _ilbrace, _irbrace, _lcons
	.extern	_lineno, _locctr, _lookup, _makety, _moedef, _ncopy, _nidcl, _notlval
	.extern	_optim, _printf, _psavbc, _reached, _refid, _retlab, _rstruct, _sprintf
	.extern	_talloc, _tempnode, _tfree, _tymerge, _types, _uclass, _uerror, _werror
	.extern	_yyerror, _yylex
	.psec
	.code
_mkty::
	dec	r15,#6
	ldm	%0000(r15),r5,#3
	ld	r7,#%0021
	sub	r6,r6
	sub	r5,r5
	ld	r4,%0004(r15)
	ld	r3,%0002(r15)
	ld	r2,%0000(r15)
	call	_block
	inc	r15,#6
	ret
_bdty::
	dec	r15,#8
	ld	%0006(r15),r14
	ldm	%0000(r15),r5,#3
	ld	r7,%0004(r15)
	ld	r6,%0002(r15)
	sub	r5,r5
	ldk	r4,#4
	sub	r3,r3
	ld	r2,r4
	call	_block
	ld	r14,r2
	ld	r2,%0004(r15)
	ld	r3,#%00BE
	ld	r4,#%0004
	cpir	r2,@r3,r4,eq
	jr	ne,L0078
	ld	r2,%0006(r3)
	jp	@r2
	ld	r7,%0000(r15)
	call	_bcon
	ld	%000A(r14),r2
L006a:
	ld	r2,r14
	jr	L0082
	ld	r2,%0000(r15)
	ld	%000C(r14),r2
	jr	L006a
L0078:
	ld	r7,#D1630
	call	_cerror
	jr	L006a
L0082:
	ld	r14,%0006(r15)
	inc	r15,#8
	ret
_dstash::
	dec	r15,#4
	ldl	%0000(r15),rr6
	cp	_curdim,#%03E7
	jr	lt,L00a0
	ld	r7,#D1639
	call	_cerror
L00a0:
	ld	r2,_curdim
	sla	r2,#2
	ldl	rr0,%0000(r15)
	ldl	_dimtab(r2),rr0
	inc	_curdim,#1
	inc	r15,#4
	ret
_savebc::
	cp	_psavbc,#_asavbc+192
	jr	ule,L00c8
	ld	r7,#D1652
	call	_cerror
L00c8:
	ld	r2,_psavbc
	ld	r3,_brklab
	ld	@r2,r3
	inc	_psavbc,#2
	ld	r2,_psavbc
	ld	r3,_contlab
	ld	@r2,r3
	inc	_psavbc,#2
	ld	r2,_psavbc
	ld	r3,_flostat
	ld	@r2,r3
	inc	_psavbc,#2
	ld	r2,_psavbc
	ld	r3,_swx
	ld	@r2,r3
	inc	_psavbc,#2
	clr	_flostat
	ret
_resetbc::
	dec	r15,#2
	dec	_psavbc,#2
	ld	r2,_psavbc
	ld	r3,@r2
	ld	_swx,r3
	ld	r2,_flostat
	and	r2,r7
	dec	_psavbc,#2
	ld	r3,_psavbc
	or	r2,@r3
	ld	_flostat,r2
	dec	_psavbc,#2
	ld	r2,_psavbc
	ld	r3,@r2
	ld	_contlab,r3
	dec	_psavbc,#2
	ld	r2,_psavbc
	ld	r3,@r2
	ld	_brklab,r3
	inc	r15,#2
	ret
_addcase::
	dec	r15,#4
	ld	%0002(r15),r14
	call	_optim
	ld	r14,r2
	cp	@r14,#%0004
	jr	eq,L016c
	ld	r7,#D1677
	jr	L0166
L0162:
	ld	r7,#D1694
L0166:
	call	_uerror
	jr	L01ae
L016c:
	cp	_swp,#_swtab
	jr	eq,L0162
	cp	_swp,#_swp
	jr	c,L0184
	ld	r7,#D16a7
	call	_cerror
L0184:
	ld	r2,_swp
	ldl	rr0,%0008(r14)
	ldl	@r2,rr0
	call	_getlab
	ld	r3,_swp
	ld	%0004(r3),r2
	ld	%0000(r15),r2
	ld	r7,r2
	call	_deflab
	inc	_swp,#6
	ld	r7,r14
	call	_tfree
L01ae:
	ld	r14,%0002(r15)
	inc	r15,#4
	ret
_adddef::
	dec	r15,#2
	ld	r2,_swx
	inc	r2,#1
	ld	r3,r2
	mult	rr2,#%0006
	cp	_swtab+4(r3),#%0000
	jr	lt,L01dc
	ld	r7,#D16bd
	jr	L01d6
L01d2:
	ld	r7,#D16d9
L01d6:
	call	_uerror
	jr	L0202
L01dc:
	cp	_swp,#_swtab
	jr	eq,L01d2
	call	_getlab
	ld	r3,_swx
	inc	r3,#1
	ld	r1,r3
	mult	rr0,#%0006
	ld	_swtab+4(r1),r2
	ld	%0000(r15),r2
	ld	r7,r2
	call	_deflab
L0202:
	inc	r15,#2
	ret
_swstart::
	cp	_swp,#_swp
	jr	c,L0216
	ld	r7,#D16f3
	call	_cerror
L0216:
	ld	r2,_swp
	ld	%0004(r2),#%FFFF
	inc	_swp,#6
	ret
_swend::
	dec	r15,#16
	ldm	%0006(r15),r10,#5
	ld	r2,_swx
	inc	r2,#2
	ld	r3,r2
	mult	rr2,#%0006
	add	r3,#_swtab
	ld	r14,r3
	ld	r10,r3
	ld	r2,_swp
	dec	r2,#6
	ld	r11,r2
	jr	L0290
L024c:
	ld	r12,r14
	jr	L0288
L0250:
	ldl	rr2,@r12
	cpl	rr2,%0006(r12)
	jr	le,L0286
	lda	r2,%0006(r12)
	ld	r10,r2
	ldl	rr2,@r12
	ldl	%0000(r15),rr2
	ldl	rr2,@r10
	ldl	@r12,rr2
	ldl	rr2,%0000(r15)
	ldl	@r10,rr2
	ld	r2,%0004(r12)
	ld	%0004(r15),r2
	ld	r2,%0004(r10)
	ld	%0004(r12),r2
	ld	r2,%0004(r15)
	ld	%0004(r10),r2
L0286:
	inc	r12,#6
L0288:
	cp	r12,r11
	jr	c,L0250
	ld	r11,r10
	ld	r10,r14
L0290:
	cp	r14,r11
	jr	c,L024c
	lda	r2,%0006(r14)
	ld	r13,r2
	jr	L02b8
L029c:
	ldl	rr2,@r13
	cpl	rr2,%FFFA(r13)
	jr	ne,L02b6
	ld	r6,%0002(r13)
	ld	%0004(r15),r6
	ld	r7,#D1709
	call	_uerror
	jr	L02de
L02b6:
	inc	r13,#6
L02b8:
	cp	r13,_swp
	jr	c,L029c
	ld	r6,_swp
	sub	r6,r14
	ld	r3,r6
	exts	rr2
	div	rr2,#%0006
	ld	r6,r3
	ld	r7,r14
	dec	r7,#12
	call	_genswitch
	ld	r2,r14
	dec	r2,#12
	ld	_swp,r2
L02de:
	ldm	r10,%0006(r15),#5
	inc	r15,#16
	ret
_yyparse::
	sub	r15,#%0144
	ldm	%0136(r15),r8,#7
	ldk	r13,#0
	ld	D14a2,#%FFFF
	clr	D14a4
	clr	D14a6
	lda	r2,%FFFE(r15)
	ld	r12,r2
	ld	r10,#_yyv-4
L030c:
	test	D14a0
	jr	eq,L0320
	ld	r7,#D1726
	ld	r6,r13
	ld	r5,D14a2
	call	_printf
L0320:
	lda	r2,%012C(r15)
	inc	r12,#2
	cp	r2,r12
	jr	nc,L0338
	ld	r7,#D173a
	call	_yyerror
L0332:
	ldk	r2,#1
	jp	L133e
L0338:
	ld	@r12,r13
	inc	r10,#4
	ldl	rr2,_yyval
	ldl	@r10,rr2
L0342:
	ld	r2,r13
	add	r2,r13
	ld	r11,D0a22(r2)
	cp	r11,#%FC18
	jr	le,L03ae
	cp	D14a2,#%0000
	jr	ge,L036a
	call	_yylex
	ld	D14a2,r2
	cp	r2,#%0000
	jr	ge,L036a
	clr	D14a2
L036a:
	add	r11,D14a2
	cp	r11,#%0000
	jr	lt,L03ae
	cp	r11,#%0481
	jr	ge,L03ae
	ld	r2,r11
	add	r2,r11
	ld	r11,D0120(r2)
	ld	r2,r11
	add	r2,r11
	ld	r2,D0fd4(r2)
	cp	r2,D14a2
	jr	ne,L03ae
	ld	D14a2,#%FFFF
	ldl	rr2,_yylval
	ldl	_yyval,rr2
	ld	r13,r11
	cp	D14a6,#%0000
	jr	le,L030c
	dec	D14a6,#1
	jr	L030c
L03ae:
	ld	r2,r13
	add	r2,r13
	ld	r11,D123a(r2)
	cp	r11,#%FFFE
	jr	ne,L040a
	cp	D14a2,#%0000
	jr	ge,L03d6
	call	_yylex
	ld	D14a2,r2
	cp	r2,#%0000
	jr	ge,L03d6
	clr	D14a2
L03d6:
	ld	r9,#%00CE
	jr	L03de
L03dc:
	inc	r9,#4
L03de:
	cp	@r9,#%FFFF
	jr	ne,L03dc
	cp	r13,%0002(r9)
	jr	ne,L03dc
L03ea:
	inc	r9,#4
	cp	@r9,#%0000
	jr	lt,L03fa
	ld	r2,@r9
	cp	r2,D14a2
	jr	ne,L03ea
L03fa:
	ld	r11,%0002(r9)
	cp	r11,#%0000
	jr	ge,L040a
	sub	r2,r2
	jp	L133e
L040a:
	test	r11
	jr	ne,L04ba
	ld	r2,D14a6
	cp	r2,#%0003
	jr	ugt,L04ba
	rl	r2,#1
	ld	r2,D14a8(r2)
	jp	@r2
	ld	r7,#D174e
	call	_yyerror
	inc	D14a4,#1
	ld	D14a6,#%0003
	jr	L048a
L0434:
	ld	r2,@r12
	add	r2,r2
	ld	r2,D0a22(r2)
	add	r2,#%0100
	ld	r11,r2
	cp	r2,#%0000
	jr	lt,L046a
	cp	r2,#%0481
	jr	ge,L046a
	add	r2,r2
	ld	r2,D0120(r2)
	add	r2,r2
	cp	D0fd4(r2),#%0100
	jr	ne,L046a
	ld	r2,r11
	add	r2,r11
	ld	r13,D0120(r2)
	jp	L030c
L046a:
	ld	r2,@r12
	add	r2,r2
	ld	r11,D0a22(r2)
	test	D14a0
	jr	eq,L0486
	ld	r7,#D175b
	ld	r6,@r12
	ld	r5,%FFFE(r12)
	call	_printf
L0486:
	dec	r12,#2
	dec	r10,#4
L048a:
	lda	r2,%0000(r15)
	cp	r12,r2
	jr	nc,L0434
	jp	L0332
	test	D14a0
	jr	eq,L04a8
	ld	r7,#D1786
	ld	r6,D14a2
	call	_printf
L04a8:
	test	D14a2
	jp	eq,L0332
	ld	D14a2,#%FFFF
	jp	L0342
L04ba:
	test	D14a0
	jr	eq,L04ca
	ld	r7,#D17a7
	ld	r6,r11
	call	_printf
L04ca:
	ld	r2,r11
	add	r2,r11
	ld	r2,D0e6a(r2)
	add	r2,r2
	sub	r12,r2
	ld	r14,r10
	ld	r2,r11
	add	r2,r11
	ld	r2,D0e6a(r2)
	sla	r2,#2
	sub	r10,r2
	lda	r2,%0004(r10)
	ldl	rr0,@r2
	ldl	_yyval,rr0
	ld	%012E(r15),r11
	ld	r2,r11
	add	r2,r11
	ld	r11,D0d00(r2)
	ld	r2,r11
	add	r2,r11
	ld	r2,D0c88(r2)
	add	r2,@r12
	inc	r2,#1
	ld	%012C(r15),r2
	cp	r2,#%0481
	jr	ge,L0526
	add	r2,r2
	ld	r13,D0120(r2)
	ld	r2,r13
	add	r2,r13
	ld	r3,r11
	neg	r3
	cp	r3,D0fd4(r2)
	jr	eq,L0534
L0526:
	ld	r2,r11
	add	r2,r11
	ld	r2,D0c88(r2)
	add	r2,r2
	ld	r13,D0120(r2)
L0534:
	ld	r2,%012E(r15)
	dec	r2,#2
	cp	r2,#%00B1
	jp	ugt,L030c
	rl	r2,#1
	ld	r2,D14cc(r2)
	jp	@r2
	clr	_curclass
	clr	_blevel
	jp	L030c
	ld	r2,%FFF8(r14)
	ld	@r2,#%0064
	test	_paramno
	jp	eq,L030c
	ld	r7,#D17b2
	call	_uerror
	jp	L030c
	ld	r7,%FFFC(r14)
	ld	r6,@r14
	call	_tymerge
	ld	%0130(r15),r2
	cp	_curclass,#%0003
	jr	ne,L058c
	ldk	r6,#3
	jr	L058e
L058c:
	ldk	r6,#5
L058e:
	ld	r7,%0130(r15)
	call	_defid
	ld	r7,@r14
	ld	r7,%000C(r7)
	call	_refid
	subl	rr2,rr2
	ldl	_autooff,rr2
	jp	L030c
	test	_blevel
	jr	eq,L05b8
	ld	r7,#D17cc
	call	_cerror
L05b8:
	test	_reached
	jr	eq,L05c2
	set	_retstat,#1
L05c2:
	ld	r2,%FFF4(r14)
	ld	@r2,#%0064
	call	_ftnend
	jp	L030c
	ld	_blevel,#%0001
	jp	L030c
	call	_bccode
	sub	r7,r7
	call	_locctr
	jp	L030c
	clr	_curclass
	ld	r2,%FFF8(r14)
	jr	L05fc
	clr	_curclass
L05f8:
	ld	r2,%FFFC(r14)
L05fc:
	ld	@r2,#%0064
	jp	L030c
	ldk	r7,#4
	sub	r6,r6
	ld	r5,r7
	calr	_mkty
	ld	_yyval,r2
	clr	_curclass
	jp	L030c
	ldk	r7,#4
	sub	r6,r6
	ld	r5,r7
	calr	_mkty
	jp	L1094
	ld	r2,@r14
	ld	_curclass,r2
	jp	L030c
	ld	r7,%FFFC(r14)
	ld	r7,%0002(r7)
	ld	r6,@r14
	ld	r6,%0002(r6)
	sub	r5,r5
	call	_types
	ld	r3,%FFFC(r14)
	ld	%0002(r3),r2
L064a:
	ld	r2,@r14
	jr	L05fc
	ld	r7,%FFF8(r14)
	ld	r7,%0002(r7)
	ld	r6,%FFFC(r14)
	ld	r6,%0002(r6)
	ld	r5,@r14
	ld	r5,%0002(r5)
	call	_types
	ld	r3,%FFF8(r14)
	ld	%0002(r3),r2
	ld	r2,@r14
	ld	@r2,#%0064
	jr	L05f8
	ld	r7,%FFF0(r14)
	call	_dclstruct
	jp	L1094
	ld	r7,@r14
	sub	r6,r6
	call	_rstruct
	ld	_yyval,r2
L0690:
	ld	r2,_instruct
	ld	_stwart,r2
	jp	L030c
	sub	r7,r7
L069e:
	sub	r6,r6
	call	_bstruct
	jp	L1094
	ld	r7,@r14
	jr	L069e
	ld	r7,@r14
L06ae:
	call	_moedef
	jp	L030c
	ld	r3,@r14
	exts	rr2
	ldl	_strucoff,rr2
	ld	r7,%FFF8(r14)
	jr	L06ae
	ld	r2,@r14
	cpb	%0006(r2),#%0A
	jr	eq,L06ee
	ld	r2,@r14
	cpb	%0006(r2),#%0C
	jr	eq,L06ee
	ld	r2,@r14
	ld	r7,#_ntab
	add	r7,%0002(r2)
	ldk	r6,#1
	ld	r5,r6
	ldk	r4,#1
	call	_lookup
	ld	@r14,r2
L06ee:
	ld	r7,@r14
	ld	r6,%FFFC(r14)
	call	_rstruct
	jp	L1094
	sub	r7,r7
	ld	r6,@r14
L0700:
	call	_bstruct
L0704:
	ld	_yyval,r2
L0708:
	clr	_stwart
	jp	L030c
	ld	r7,@r14
	ld	r6,%FFFC(r14)
	jr	L0700
	cp	_curclass,#%000B
	jr	eq,L0724
	clr	_curclass
L0724:
	clr	_stwart
	jp	L05f8
	cp	_curclass,#%000B
	jr	eq,L073a
	clr	_curclass
	jr	L078a
L073a:
	ld	r5,D003a
	inc	D003a,#1
	ld	r6,#D17dc
	ld	r7,#%003E
	call	_sprintf
	ld	r7,#%003E
	ldk	r6,#1
	ld	r5,r6
	sub	r4,r4
	call	_lookup
	ld	%0130(r15),r2
	ldk	r7,#2
	sub	r6,r6
	ld	r5,r2
	calr	_bdty
	ld	%0132(r15),r2
	ld	r7,@r14
	ld	r6,r2
	call	_tymerge
	ld	%0134(r15),r2
	ld	r7,r2
	ld	r6,_curclass
	call	_defid
	ld	r7,#D17e4
	call	_werror
L078a:
	clr	_stwart
	jp	L064a
	ld	r7,%FFFC(r14)
L0796:
	ld	r6,@r14
	call	_tymerge
	ld	%0130(r15),r2
	ld	r7,r2
	ld	r6,_curclass
	call	_defid
	jp	L0690
	ld	r2,%FFF8(r14)
	jp	L1094
	ld	r7,%FFF0(r14)
	jr	L0796
	bit	_instruct,#1
	jr	ne,L07ca
	ld	r7,#D180f
	call	_uerror
L07ca:
	cp	@r14,#%0000
	jr	lt,L07d6
	cp	@r14,#%0040
	jr	lt,L07e2
L07d6:
	ld	r7,#D1824
	call	_uerror
	ld	@r14,#%0001
L07e2:
	ld	r7,%FFF4(r14)
	ld	r6,%FFF8(r14)
	call	_tymerge
	ld	%0130(r15),r2
	ld	r6,@r14
	or	r6,#%0040
	ld	r7,r2
	call	_defid
L07fe:
	clr	_yyval
	jp	L030c
	bit	_instruct,#1
	jr	ne,L0814
	ld	r7,#D1837
	call	_uerror
L0814:
	sub	r7,r7
	ld	r6,@r14
	ld	r5,#%FFFF
	ld	r4,%FFF8(r14)
	call	_falloc
	jr	L07fe
	ldk	r7,#13
	ld	r6,@r14
L082a:
	sub	r5,r5
L082c:
	calr	_bdty
	jp	L1094
	ld	r7,#%0048
L0836:
	ld	r6,%FFF8(r14)
	jr	L082a
	ld	r2,%FFF8(r14)
	cp	@r2,#%0036
	jr	ne,L084e
	ld	r7,#D184c
	call	_uerror
L084e:
	ld	r7,#%0036
	jr	L0836
	ldl	rr2,%FFFC(r14)
	cpl	rr2,#%00000000
	jr	gt,L0866
	ld	r7,#D1863
	jr	L0876
L0866:
	ldl	rr2,%FFFC(r14)
	cpl	rr2,#%00010000
	jr	le,L087a
	ld	r7,#D187e
L0876:
	call	_uerror
L087a:
	ld	r7,#%0036
	ld	r6,%FFF4(r14)
	sub	r5,r5
	ldk	r4,#4
	sub	r3,r3
	ld	r2,r4
	call	_block
	ld	_yyval,r2
	sub	r7,r7
	call	_bcon
	ld	r3,_yyval
	ld	%000A(r3),r2
	ld	r2,_yyval
	ld	r2,%000A(r2)
	ldl	rr0,%FFFC(r14)
	ldl	%0008(r2),rr0
	ldk	r7,#5
	call	_ctype
	ld	r3,_yyval
	ld	r3,%000A(r3)
	ld	%0002(r3),r2
	ld	r3,_yyval
	ld	r3,%000A(r3)
L08ca:
	ld	%0006(r3),r2
	jp	L030c
	ldk	r7,#2
	sub	r6,r6
	ld	r5,@r14
	jr	L082c
	ld	r2,%FFFC(r14)
	jp	L1094
	test	_blevel
	jr	eq,L08f0
	ld	r7,#D1892
	call	_uerror
L08f0:
	ldk	r7,#2
	sub	r6,r6
	ld	r5,%FFF8(r14)
L08f8:
	calr	_bdty
	ld	%0130(r15),r2
	ld	r7,#%0048
	ld	r6,r2
	sub	r5,r5
	calr	_bdty
	jp	L0704
	ldk	r7,#2
	sub	r6,r6
	ld	r5,%FFFC(r14)
	jr	L08f8
	ld	r7,@r14
	call	_ftnarg
	ld	_stwart,#%0001
	jp	L030c
	ld	r7,%FFFC(r14)
	ld	r6,@r14
	call	_tymerge
	ld	@r14,r2
	ld	%0130(r15),r2
	ld	r7,r2
	ld	r6,_curclass
	call	_defid
	ld	r7,@r14
	ld	r7,%000C(r7)
	call	_beginit
	jp	L030c
	ld	r7,%FFFC(r14)
	ld	r6,@r14
	call	_tymerge
	ld	%0130(r15),r2
	ld	r7,r2
	call	_nidcl
	jp	L030c
	ld	r7,_curclass
	call	_uclass
	ld	%0130(r15),r2
	ld	r7,%FFFC(r14)
	ld	r6,@r14
	call	_tymerge
	ld	%0132(r15),r2
	ld	r7,r2
	ld	r6,%0130(r15)
	call	_defid
	jp	L030c
	ld	r7,@r14
	call	_doinit
	call	_endinit
	jp	L030c
	ld	r7,@r14
	call	_doinit
	jp	L030c
	call	_irbrace
	jp	L030c
	ld	r7,#D18a5
	call	_werror
	jp	L030c
	call	_ilbrace
	jp	L030c
	dec	_blevel,#1
	cp	_blevel,#%0001
	jr	ne,L09d2
	clr	_blevel
L09d2:
	ld	r7,_blevel
	call	_clearst
	dec	_psavbc,#4
	ld	r2,_psavbc
	ldl	rr0,@r2
	ldl	_autooff,rr0
	dec	_psavbc,#2
	ld	r2,_psavbc
	ld	r3,@r2
	ld	_regvar,r3
	dec	_psavbc,#2
	ld	r2,_psavbc
	ld	r3,@r2
	ld	_lregvar,r3
	call	_bfend
	jp	L030c
	cp	_blevel,#%0001
	jr	ne,L0a18
	call	_dclargs
L0a18:
	inc	_blevel,#1
	cp	_psavbc,#_asavbc+196
	jr	ule,L0a2c
	ld	r7,#D18c2
	call	_cerror
L0a2c:
	ld	r2,_psavbc
	ld	r3,_lregvar
	ld	@r2,r3
	inc	_psavbc,#2
	ld	r2,_psavbc
	ld	r3,_regvar
	ld	@r2,r3
	inc	_psavbc,#2
	ld	r2,_psavbc
	ldl	rr0,_autooff
	ldl	@r2,rr0
	inc	_psavbc,#4
	jp	L030c
	ld	r7,%FFFC(r14)
L0a5e:
	call	_ecomp
	jp	L030c
	cp	%FFFC(r14),#%FFFF
	jp	eq,L030c
	ld	r7,%FFFC(r14)
L0a74:
	call	_deflab
L0a78:
	ld	_reached,#%0001
	jp	L030c
	ld	r7,_contlab
	call	_branch
	ld	r7,_brklab
	call	_deflab
	bit	_flostat,#1
	jr	ne,L0a9e
	bit	_flostat,#4
	jr	ne,L0aa6
L0a9e:
	ld	_reached,#%0001
	jr	L0aaa
L0aa6:
	clr	_reached
L0aaa:
	sub	r7,r7
L0aac:
	calr	_resetbc
	jp	L030c
	ld	r7,_contlab
	call	_deflab
	bit	_flostat,#2
	jr	eq,L0ac6
	ld	_reached,#%0001
L0ac6:
	ld	r7,%FFE8(r14)
	call	_bcon
	ld	%0130(r15),r2
	ld	r7,#%004C
	ld	r6,%FFF8(r14)
	sub	r5,r5
	call	_buildtree
	ld	%0132(r15),r2
	ld	r7,#%0073
	ld	r6,r2
	ld	r5,%0130(r15)
	call	_buildtree
	ld	%0134(r15),r2
	ld	r7,r2
	call	_ecomp
	ld	r7,_brklab
	call	_deflab
	jr	L0a9e
	ld	r7,_contlab
	call	_deflab
	bit	_flostat,#2
	jr	eq,L0b1a
	ld	_reached,#%0001
L0b1a:
	test	%FFF8(r14)
	jr	eq,L0b28
	ld	r7,%FFF8(r14)
	call	_ecomp
L0b28:
	ld	r7,%FFF4(r14)
	call	_branch
	ld	r7,_brklab
	call	_deflab
	bit	_flostat,#1
	jr	ne,L0a9e
	bit	_flostat,#4
	jr	ne,L0aa6
	jr	L0a9e
	test	_reached
	jr	eq,L0b54
	ld	r7,_brklab
	call	_branch
L0b54:
	ld	r7,%FFFC(r14)
	call	_deflab
	test	_reached
	jr	eq,L0b66
	calr	_swend
	jr	L0b72
L0b66:
	ld	_reached,#%0001
	calr	_swend
	clr	_reached
L0b72:
	ld	r7,_brklab
	call	_deflab
	bit	_flostat,#1
	jr	ne,L0b86
	bit	_flostat,#3
	jr	ne,L0b8c
L0b86:
	ld	_reached,#%0001
L0b8c:
	ldk	r7,#4
	jr	L0aac
	cp	_brklab,#%FFFF
	jr	ne,L0ba2
	ld	r7,#D18d3
	call	_uerror
	jr	L0bb0
L0ba2:
	test	_reached
	jr	eq,L0bb0
	ld	r7,_brklab
	call	_branch
L0bb0:
	set	_flostat,#1
L0bb4:
	clr	_reached
	jp	L030c
	cp	_contlab,#%FFFF
	jr	ne,L0bce
	ld	r7,#D18e1
	call	_uerror
	jr	L0bd6
L0bce:
	ld	r7,_contlab
	call	_branch
L0bd6:
	set	_flostat,#2
	jr	L0be8
	set	_retstat,#1
	ld	r7,_retlab
L0be4:
	call	_branch
L0be8:
	test	_reached
	jr	ne,L0bb4
	ld	r7,#D18f2
	call	_werror
	jr	L0bb4
	ld	r2,_curftn
	ld	_idname,r2
	ldk	r7,#2
	sub	r6,r6
	sub	r5,r5
	call	_buildtree
	ld	r8,r2
	cp	%0002(r8),#%0020
	jr	ne,L0c24
	ld	r6,_idname
	ld	r6,%0002(r6)
	ld	r7,#D1908
	call	_uerror
L0c24:
	ld	r2,%0002(r8)
	srl	r2,#2
	and	r2,#%FFF0
	ld	r3,%0002(r8)
	and	r3,#%000F
	or	r2,r3
	ld	%0002(r8),r2
	ld	r7,#%0024
	ld	r6,r8
	ld	r5,%FFFC(r14)
	call	_buildtree
	ld	r8,r2
	ld	r2,%0008(r8)
	ld	@r2,#%0064
	ld	@r8,#%0064
	ld	r7,#%0072
	ld	r6,%000A(r8)
	sub	r5,r5
	call	_buildtree
	ld	%0130(r15),r2
	ld	r7,r2
	call	_ecomp
	set	_retstat,#0
	ld	r7,_retlab
	call	_branch
	jr	L0bb4
	ld	r7,#%0064
	sub	r6,r6
	sub	r5,r5
	ld	r4,#%0034
	sub	r3,r3
	ldk	r2,#4
	call	_block
	ld	r8,r2
	ld	r2,%FFFC(r14)
	ld	_idname,r2
	ld	%000C(r8),r2
	ld	r7,r8
	ldk	r6,#7
	call	_defid
	ld	r2,_idname
	ld	r3,_lineno
	neg	r3
	ld	%0016(r2),r3
	ld	r7,_idname
	ldl	rr2,%000E(r7)
	ld	r7,r3
	jr	L0be4
	ld	r7,#%0064
	sub	r6,r6
	sub	r5,r5
	ld	r4,#%0034
	sub	r3,r3
	ldk	r2,#6
	call	_block
	ld	r8,r2
	ld	r2,%FFFC(r14)
	ld	%000C(r8),r2
	ld	r7,r8
	ldk	r6,#6
	call	_defid
	jp	L0a78
	ld	r7,%FFFC(r14)
	calr	_addcase
	jp	L0a78
	ld	_reached,#%0001
	calr	_adddef
	set	_flostat,#3
	jp	L030c
	calr	_savebc
	test	_reached
	jr	ne,L0d18
	ld	r7,#D192d
	call	_werror
L0d18:
	call	_getlab
	ld	_brklab,r2
	call	_getlab
	ld	_contlab,r2
	call	_getlab
	ld	_yyval,r2
	ld	%0130(r15),r2
	ld	r7,r2
	jp	L0a74
	call	_getlab
	ld	_yyval,r2
	ld	%0130(r15),r2
	ld	r7,r2
	call	_bcon
	ld	%0132(r15),r2
	ld	r7,#%0073
	ld	r6,%FFFC(r14)
	ld	r5,r2
	call	_buildtree
	ld	%0134(r15),r2
	ld	r7,r2
	call	_ecomp
	jp	L0a78
	test	_reached
	jr	eq,L0d86
	call	_getlab
	ld	_yyval,r2
	ld	%0130(r15),r2
	ld	r7,r2
	call	_branch
	jr	L0d8c
L0d86:
	ld	_yyval,#%FFFF
L0d8c:
	ld	r7,%FFF8(r14)
	jp	L0a74
	calr	_savebc
	test	_reached
	jr	ne,L0da4
	ld	r7,#D1945
	call	_werror
L0da4:
	ld	r2,%FFFC(r14)
	cp	@r2,#%0004
	jr	ne,L0dc0
	ld	r2,%FFFC(r14)
	ldl	rr2,%0008(r2)
	testl	rr2
	jr	eq,L0dc0
	ld	_flostat,#%0010
L0dc0:
	call	_getlab
	ld	_contlab,r2
	ld	%0130(r15),r2
	ld	r7,r2
	call	_deflab
	ld	_reached,#%0001
	call	_getlab
	ld	_brklab,r2
	cp	_flostat,#%0010
	jr	ne,L0df4
	ld	r7,%FFFC(r14)
	call	_tfree
	jp	L030c
L0df4:
	ld	r7,_brklab
	call	_bcon
	ld	%0130(r15),r2
	ld	r7,#%0073
	ld	r6,%FFFC(r14)
	ld	r5,r2
	call	_buildtree
	ld	%0132(r15),r2
	ld	r7,r2
	jp	L0a5e
	test	%FFF4(r14)
	jr	eq,L0e28
	ld	r7,%FFF4(r14)
	call	_ecomp
	jr	L0e36
L0e28:
	test	_reached
	jr	ne,L0e36
	ld	r7,#D195d
	call	_werror
L0e36:
	calr	_savebc
	call	_getlab
	ld	_contlab,r2
	call	_getlab
	ld	_brklab,r2
	call	_getlab
	ld	_yyval,r2
	ld	%0130(r15),r2
	ld	r7,r2
	call	_deflab
	ld	_reached,#%0001
	test	%FFFC(r14)
	jr	ne,L0df4
	set	_flostat,#4
	jp	L030c
	calr	_savebc
	call	_getlab
	ld	_brklab,r2
	ld	r2,%FFFC(r14)
	ld	r2,%0002(r2)
	and	r2,#%000F
	dec	r2,#2
	cp	r2,#%000D
	jr	ugt,L0f44
	rl	r2,#1
	ld	r2,D14b0(r2)
	jp	@r2
	ld	r7,#D1975
	call	_werror
	ld	r7,%FFFC(r14)
	ldk	r6,#4
	sub	r5,r5
	ld	r4,r6
	call	_makety
L0eaa:
	ld	%FFFC(r14),r2
	ld	r2,_swp
	sub	r2,#_swtab
	ld	r1,r2
	exts	rr0
	div	rr0,#%0006
	ld	_swx,r1
	ld	r7,%FFFC(r14)
	ld	r7,%0002(r7)
	sub	r6,r6
	call	_tempnode
	ld	r3,_swp
	ld	@r3,r2
	call	_talloc
	ld	%0130(r15),r2
	ld	%0132(r15),r2
	ld	r6,_swp
	ld	r6,@r6
	ld	r7,r2
	call	_ncopy
	ld	r2,%0130(r15)
	ldl	rr0,#%FFFFFFFF
	ldl	%0008(r2),rr0
	ld	r4,%FFFC(r14)
	ld	r4,%0002(r4)
	ld	r2,%FFFC(r14)
	ld	r2,%0006(r2)
	ld	r5,%FFFC(r14)
	ld	r7,#%003A
	sub	r3,r3
	ld	r6,%0130(r15)
	call	_block
	ld	%0132(r15),r2
	ld	r7,r2
	call	_ecomp
	inc	_swp,#6
	call	_getlab
	ld	_yyval,r2
	ld	%0132(r15),r2
	ld	r7,r2
	call	_branch
	calr	_swstart
	jp	L0bb4
L0f44:
	ld	r7,#D199c
	call	_uerror
	sub	r7,r7
	call	_bcon
	jr	L0eaa
	ld	r2,_instruct
	ld	_yyval,r2
	clr	_instruct
	jp	L0708
	ld	r7,@r14
	call	_icons
	ld	_yyval,r2
L0f6e:
	ld	r2,%FFFC(r14)
	ld	_instruct,r2
	jp	L030c
	ld	r7,@r14
	call	_lcons
	ldl	_yyval,rr2
	jr	L0f6e
	ld	r7,@r14
	call	_f3970
	jp	L1094
	ld	r7,@r14
	call	_f3970
	ld	%0130(r15),r2
	ld	r7,%FFFC(r14)
L0f9e:
	ld	r6,%FFF8(r14)
L0fa2:
	ld	r5,%0130(r15)
L0fa6:
	call	_buildtree
	jr	L1094
	ld	%FFFC(r14),#%003B
L0fb2:
	ld	r7,%FFFC(r14)
	ld	r6,%FFF8(r14)
L0fba:
	ld	r5,@r14
	jr	L0fa6
	cp	D14a2,#%001D
	jr	ne,L0fb2
	jr	L0fb2
	cp	D14a2,#%001D
	jr	ne,L0fb2
	jr	L0fb2
	cp	D14a2,#%0006
	jr	eq,L0fb2
	cp	D14a2,#%0008
	jr	ne,L0fb2
	jr	L0fb2
	cp	D14a2,#%001A
	jr	eq,L0fb2
	cp	D14a2,#%001B
	jr	ne,L0fb2
	jr	L0fb2
	cp	D14a2,#%001A
	jr	eq,L0fb2
	cp	D14a2,#%001B
	jr	ne,L0fb2
	jr	L0fb2
	cp	D14a2,#%001A
	jr	eq,L0fb2
	cp	D14a2,#%001B
	jr	ne,L0fb2
	jr	L0fb2
	ld	r7,%FFF8(r14)
	inc	r7,#1
	ld	r6,%FFF4(r14)
	jr	L0fba
	ld	r7,#%0016
	ld	r6,%FFF8(r14)
	ld	r5,@r14
	call	_buildtree
	ld	%0130(r15),r2
	ld	r7,#%0015
	ld	r6,%FFF0(r14)
	jr	L0fa2
	ld	r7,#D19bf
	call	_werror
	jr	L0fb2
	ldk	r7,#1
	call	_bcon
	ld	%0130(r15),r2
	ld	r7,@r14
	ld	r6,%FFFC(r14)
	jr	L0fa2
L105e:
	ld	r7,%FFFC(r14)
	inc	r7,#2
L1064:
	ld	r6,@r14
L1066:
	sub	r5,r5
	jr	L0fa6
	ld	r2,@r14
	ld	r2,%0002(r2)
	and	r2,#%0030
	cp	r2,#%0020
	jr	eq,L108a
	ld	r2,@r14
	ld	r2,%0002(r2)
	and	r2,#%0030
	cp	r2,#%0030
	jr	ne,L105e
L108a:
	ld	r7,#D19e1
	call	_werror
	ld	r2,@r14
L1094:
	ld	_yyval,r2
	jp	L030c
	ld	r7,%FFFC(r14)
	jr	L1064
	ldk	r7,#1
	call	_bcon
	ld	%0130(r15),r2
	cp	%FFFC(r14),#%004E
	jr	ne,L10b8
	ldk	r7,#7
	jr	L10ba
L10b8:
	ldk	r7,#9
L10ba:
	ld	r6,@r14
	jp	L0fa2
	ld	r7,@r14
L10c2:
	call	_doszof
	jr	L1094
	ld	r7,#%0075
	ld	r6,%FFF8(r14)
	ld	r5,@r14
	call	_buildtree
	ld	_yyval,r2
	ld	r2,%0008(r2)
	ld	@r2,#%0064
	ld	r2,_yyval
	ld	@r2,#%0064
	ld	r2,_yyval
	ld	r3,%000A(r2)
	ld	_yyval,r3
	jp	L030c
	ld	r7,%FFFC(r14)
	jr	L10c2
	ld	r7,#%0078
	ld	r6,%FFF4(r14)
	ld	r5,%FFFC(r14)
	call	_buildtree
	ld	%0130(r15),r2
	ldk	r7,#13
	ld	r6,r2
	jr	L1066
	test	_Fflag
	jr	eq,L1150
	ld	r2,%FFFC(r14)
	cp	@r2,#%0002
	jr	ne,L1144
	ld	r2,%FFFC(r14)
	ld	r2,%000C(r2)
	ld	r5,#_ntab
	add	r5,%0002(r2)
	ld	r6,#D1a05
	ld	r7,_Ffile
	jr	L114c
L1144:
	ld	r7,_Ffile
	ld	r6,#D1a09
L114c:
	call	_fprintf
L1150:
	ld	r7,#%0048
	ld	r6,%FFFC(r14)
	jr	L1066
	test	_Fflag
	jr	eq,L1190
	ld	r2,%FFF8(r14)
	cp	@r2,#%0002
	jr	ne,L1184
	ld	r2,%FFF8(r14)
	ld	r2,%000C(r2)
	ld	r5,#_ntab
	add	r5,%0002(r2)
	ld	r6,#D1a10
	ld	r7,_Ffile
	jr	L118c
L1184:
	ld	r7,_Ffile
	ld	r6,#D1a14
L118c:
	call	_fprintf
L1190:
	ld	r7,#%0046
	ld	r6,%FFF8(r14)
	ld	r5,%FFFC(r14)
	jp	L0fa6
	cp	%FFFC(r14),#%0044
	jr	ne,L11ce
	ld	r7,%FFF8(r14)
	call	_notlval
	test	r2
	jr	eq,L11bc
	ld	r7,#D1a1b
	call	_uerror
L11bc:
	ld	r7,#%0010
	ld	r6,%FFF8(r14)
	sub	r5,r5
	call	_buildtree
	ld	%FFF8(r14),r2
L11ce:
	ld	r2,@r14
	ld	_idname,r2
	ldk	r7,#2
	sub	r6,r6
	sub	r5,r5
	call	_buildtree
	ld	%0130(r15),r2
	ld	r7,#%0045
	jp	L0f9e
	ld	r2,@r14
	ld	_idname,r2
	test	_blevel
	jr	ne,L1230
	test	%0004(r2)
	jr	ne,L1230
	ld	r6,#_ntab
	add	r6,%0002(r2)
	ld	r7,#D1a43
	call	_werror
	ld	r7,#%0064
	sub	r6,r6
	sub	r5,r5
	ldk	r4,#4
	sub	r3,r3
	ld	r2,r4
	call	_block
	ld	r8,r2
	ld	r2,_idname
	ld	%000C(r8),r2
	ld	r7,r8
	ldk	r6,#2
	call	_defid
L1230:
	ldk	r7,#2
	sub	r6,r6
	sub	r5,r5
	call	_buildtree
	ld	_yyval,r2
	ld	r2,@r14
L1240:
	ld	r3,_lineno
	neg	r3
	ld	%0016(r2),r3
	jp	L030c
	sub	r7,r7
	call	_bcon
	ld	_yyval,r2
	ldl	rr0,_lastcon
	ldl	%0008(r2),rr0
	ld	r2,_yyval
	clr	%000C(r2)
	test	@r14
	jp	eq,L030c
	ldk	r7,#5
	call	_ctype
	ld	r3,_yyval
	ld	%0002(r3),r2
	ld	r3,_yyval
	jp	L08ca
	ldk	r7,#5
	sub	r6,r6
	sub	r5,r5
	call	_buildtree
	ld	_yyval,r2
	.word	%4F04,%8003,_dcon
	.word	%4F2C,%8003,%0008
	jp	L030c
	call	_getstr
	jp	L1094
	ld	r7,%FFFC(r14)
	ld	r6,@r14
	call	_tymerge
	ld	_yyval,r2
	ld	@r2,#%0002
	jp	L05f8
	ldk	r7,#2
	sub	r6,r6
	ld	r5,#%FFFF
	jp	L082c
	ldk	r7,#2
	sub	r6,r6
	ld	r5,#%FFFF
	call	_bdty
	ld	%0130(r15),r2
	ld	r7,#%0048
	ld	r6,r2
	jp	L082a
	ld	r7,#%0048
	ld	r6,%FFF4(r14)
	jp	L082a
	ld	r2,%FFFC(r14)
	test	%0004(r2)
	jr	ne,L1322
	ld	r7,#%0064
	sub	r6,r6
	sub	r5,r5
	ld	r4,#%0024
	sub	r3,r3
	ldk	r2,#4
	call	_block
	ld	r8,r2
	ld	r2,%FFFC(r14)
	ld	%000C(r8),r2
	ld	r7,r8
	ldk	r6,#2
	call	_defid
L1322:
	ld	r2,%FFFC(r14)
	ld	_idname,r2
	ldk	r7,#2
	sub	r6,r6
	sub	r5,r5
	call	_buildtree
	ld	_yyval,r2
	ld	r2,_idname
	jr	L1240
L133e:
	ldm	r8,%0136(r15),#7
	add	r15,#%0144
	ret
	.psec
	.data
	.byte	%40,%5b,%24,%5d,%63,%67,%72,%61
	.byte	%6d,%31,%2e,%79,%09,%32,%2e,%35
	.byte	%20,%20,%31,%30,%2f,%30,%36,%2f
	.byte	%38,%33,%20,%31,%32,%3a,%35,%38
	.byte	%3a,%33,%30,%20,%43,%6f,%70,%72
	.byte	%2e,%20,%31,%39,%38,%31,%20,%38
	.byte	%37,%77,%65,%67,%61,%33,%2e,%32
	.byte	%2e,%00
D003a:
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%02,%00,%0d,%00,%36
	.byte	%00,%48,%00,%98,%00,%94,%00,%88
	.byte	%00,%94,%ff,%ff,%00,%01,%00,%00
	.byte	%ff,%ff,%00,%02,%00,%15,%00,%0b
	.byte	%00,%15,%00,%32,%00,%15,%00,%39
	.byte	%00,%15,%ff,%fe,%00,%00,%ff,%ff
	.byte	%00,%13,%00,%38,%00,%4e,%00,%39
	.byte	%00,%4e,%ff,%fe,%00,%07,%ff,%ff
	.byte	%00,%18,%00,%38,%00,%4d,%00,%39
	.byte	%00,%4d,%ff,%fe,%00,%4b,%ff,%ff
	.byte	%00,%20,%00,%34,%00,%2b,%ff,%fe
	.byte	%00,%29,%ff,%ff,%00,%22,%00,%34
	.byte	%00,%23,%ff,%fe,%00,%21
D0120:
	.byte	%00,%e7,%00,%18,%01,%09,%00,%fb
	.byte	%00,%57,%00,%55,%00,%56,%00,%1a
	.byte	%00,%5b,%00,%4e,%00,%e5,%00,%1a
	.byte	%00,%4c,%00,%1a,%00,%40,%00,%4d
	.byte	%00,%15,%00,%0a,%00,%09,%00,%0e
	.byte	%00,%15,%00,%60,%00,%15,%00,%28
	.byte	%00,%2a,%00,%80,%00,%7f,%00,%5e
	.byte	%00,%06,%00,%12,%00,%62,%00,%50
	.byte	%00,%4f,%00,%10,%00,%8f,%00,%5a
	.byte	%00,%6a,%00,%f1,%00,%f2,%00,%f6
	.byte	%00,%9a,%00,%fa,%00,%ef,%00,%f0
	.byte	%00,%f7,%00,%f8,%00,%f9,%00,%fd
	.byte	%00,%fc,%00,%51,%00,%49,%00,%52
	.byte	%00,%39,%00,%6a,%00,%e6,%00,%16
	.byte	%00,%1a,%00,%5d,%00,%f3,%00,%16
	.byte	%00,%32,%00,%16,%00,%a1,%00,%48
	.byte	%00,%36,%00,%15,%00,%6c,%00,%e8
	.byte	%00,%e3,%00,%0a,%00,%09,%00,%0e
	.byte	%00,%c3,%00,%14,%01,%12,%00,%81
	.byte	%00,%5e,%01,%32,%01,%11,%01,%31
	.byte	%00,%1a,%01,%2c,%00,%4b,%00,%90
	.byte	%00,%94,%00,%10,%00,%5c,%00,%24
	.byte	%01,%03,%00,%15,%00,%68,%01,%21
	.byte	%00,%13,%00,%98,%00,%cd,%00,%24
	.byte	%00,%23,%01,%0d,%01,%0c,%00,%a2
	.byte	%00,%c7,%00,%59,%00,%44,%00,%bc
	.byte	%00,%16,%00,%c6,%00,%5d,%00,%45
	.byte	%00,%29,%00,%2b,%00,%99
D01fe:
	.byte	%00,%98,%00,%a3,%00,%a4,%00,%a5
	.byte	%00,%a7,%00,%a9,%00,%ab,%00,%ad
	.byte	%00,%af,%00,%b0,%00,%b2,%00,%b4
	.byte	%00,%b6,%00,%b7,%00,%b8,%00,%b9
	.byte	%00,%ba,%00,%16,%00,%65,%00,%61
	.byte	%00,%81,%01,%1b,%00,%63,%00,%be
	.byte	%00,%11,%01,%00,%00,%26,%00,%2f
	.byte	%00,%9e,%00,%a0,%00,%27,%00,%30
	.byte	%00,%90,%00,%26,%00,%43,%00,%64
	.byte	%00,%6e,%00,%27,%00,%bb,%00,%9d
	.byte	%00,%2f,%00,%42,%00,%6b,%00,%97
	.byte	%00,%30,%00,%ca,%00,%bd,%00,%47
	.byte	%00,%88,%00,%89,%00,%8a,%00,%8b
	.byte	%00,%8c,%00,%8d,%00,%3f,%00,%2f
	.byte	%00,%ce,%00,%08,%00,%cf,%00,%30
	.byte	%00,%d0,%00,%dd,%00,%d1,%00,%ff
	.byte	%00,%d2,%00,%1c,%00,%c0,%00,%d3
	.byte	%00,%d7,%00,%d4,%00,%84,%00,%d5
	.byte	%00,%86,%00,%fb,%00,%57,%00,%55
	.byte	%00,%56,%00,%cb,%00,%81,%00,%4e
	.byte	%00,%9c,%01,%02,%00,%4c,%00,%c9
	.byte	%00,%dd,%00,%4d,%00,%96,%00,%21
	.byte	%00,%e1,%00,%37,%00,%87,%00,%5d
	.byte	%00,%2e,%00,%1f,%00,%85,%01,%27
	.byte	%00,%1a,%01,%1f,%00,%e2,%00,%db
	.byte	%00,%d9,%00,%50,%00,%4f,%00,%d8
	.byte	%00,%fe,%00,%15,%00,%c1,%00,%f1
	.byte	%00,%f2,%00,%f6,%00,%c5,%00,%fa
	.byte	%00,%ef,%00,%f0,%00,%f7,%00,%f8
	.byte	%00,%f9,%00,%fd,%00,%fc,%00,%51
	.byte	%01,%18,%00,%52,%00,%e4,%00,%6a
	.byte	%00,%c5,%01,%01,%00,%46,%01,%0a
	.byte	%00,%f3,%00,%69,%00,%3e,%01,%0f
	.byte	%01,%28,%01,%05,%01,%06,%01,%07
	.byte	%01,%08,%00,%0a,%01,%0b,%00,%0e
	.byte	%00,%91,%00,%2d,%01,%19,%01,%1c
	.byte	%00,%16,%01,%13,%00,%f4,%00,%37
	.byte	%00,%95,%00,%c4,%00,%5f,%01,%17
	.byte	%01,%16,%00,%10,%00,%1b,%01,%15
	.byte	%00,%1b,%01,%14,%00,%df,%00,%e0
	.byte	%00,%95,%00,%33,%01,%1e,%00,%c4
	.byte	%00,%de,%00,%dc,%01,%22,%01,%23
	.byte	%01,%0a,%01,%25,%01,%24,%01,%1d
	.byte	%00,%09,%00,%34,%00,%54,%00,%57
	.byte	%00,%55,%00,%56,%00,%1e,%00,%64
	.byte	%00,%75,%00,%04,%01,%1a,%00,%72
	.byte	%01,%10,%00,%73,%01,%2e,%01,%29
	.byte	%00,%75,%00,%41,%01,%0a,%00,%dc
	.byte	%01,%2f,%00,%72,%00,%bf,%00,%73
	.byte	%00,%3a,%00,%71,%00,%75,%00,%5f
	.byte	%00,%91,%00,%77,%00,%50,%00,%4f
	.byte	%00,%78,%00,%71,%00,%79,%00,%6d
	.byte	%00,%7c,%00,%22,%00,%7a,%00,%7b
	.byte	%00,%7d,%00,%6f,%00,%76,%00,%71
	.byte	%00,%74,%00,%0a,%00,%09,%00,%0e
	.byte	%00,%51,%00,%07,%00,%52,%00,%1b
	.byte	%00,%20,%00,%9b,%00,%f5,%00,%67
	.byte	%00,%82,%00,%72,%00,%1d,%00,%73
	.byte	%00,%31,%00,%10,%00,%75,%00,%19
	.byte	%00,%3b,%00,%77,%01,%30,%00,%2c
	.byte	%00,%78,%00,%c8,%00,%79,%00,%70
	.byte	%00,%7c,%00,%7e,%00,%7a,%00,%7b
	.byte	%00,%7d,%00,%6f,%00,%76,%00,%71
	.byte	%00,%74,%00,%58,%00,%35,%00,%38
	.byte	%00,%9f,%00,%cc,%00,%66,%00,%3d
	.byte	%00,%3c,%00,%25,%00,%03,%00,%02
	.byte	%00,%93,%00,%72,%00,%53,%00,%73
	.byte	%00,%0b,%00,%0c,%00,%75,%00,%05
	.byte	%00,%17,%00,%77,%01,%2d,%00,%0d
	.byte	%00,%78,%00,%0f,%00,%79,%00,%70
	.byte	%00,%7c,%00,%7e,%00,%7a,%00,%7b
	.byte	%00,%7d,%00,%6f,%00,%76,%00,%71
	.byte	%00,%74,%00,%ee,%00,%ec,%00,%ed
	.byte	%00,%eb,%00,%e9,%00,%ea,%00,%01
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%72,%00,%00,%00,%73
	.byte	%00,%00,%00,%00,%00,%75,%00,%00
	.byte	%00,%00,%00,%77,%01,%2b,%00,%00
	.byte	%00,%78,%00,%00,%00,%79,%00,%70
	.byte	%00,%7c,%00,%7e,%00,%7a,%00,%7b
	.byte	%00,%7d,%00,%6f,%00,%76,%00,%71
	.byte	%00,%74,%00,%00,%00,%72,%00,%f4
	.byte	%00,%73,%00,%00,%00,%00,%00,%75
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%72
	.byte	%00,%00,%00,%73,%00,%00,%00,%00
	.byte	%00,%75,%00,%00,%01,%2a,%00,%77
	.byte	%00,%71,%00,%74,%00,%78,%00,%70
	.byte	%00,%79,%00,%7e,%00,%7c,%01,%26
	.byte	%00,%7a,%00,%7b,%00,%7d,%00,%6f
	.byte	%00,%76,%00,%71,%00,%74,%00,%72
	.byte	%00,%00,%00,%73,%00,%00,%00,%00
	.byte	%00,%75,%00,%00,%00,%00,%00,%77
	.byte	%00,%00,%00,%00,%00,%78,%00,%00
	.byte	%00,%79,%00,%00,%00,%7c,%00,%00
	.byte	%00,%7a,%00,%7b,%00,%7d,%00,%6f
	.byte	%00,%76,%00,%71,%00,%74,%00,%00
	.byte	%00,%72,%00,%70,%00,%73,%00,%7e
	.byte	%00,%00,%00,%75,%00,%00,%00,%00
	.byte	%00,%77,%00,%00,%00,%00,%00,%78
	.byte	%00,%00,%00,%79,%00,%00,%00,%7c
	.byte	%00,%00,%00,%7a,%00,%7b,%00,%7d
	.byte	%00,%6f,%00,%76,%00,%71,%00,%74
	.byte	%00,%00,%00,%70,%01,%20,%00,%7e
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%72,%00,%00
	.byte	%00,%73,%00,%00,%00,%00,%00,%75
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%70,%01,%04
	.byte	%00,%7e,%00,%72,%00,%6f,%00,%73
	.byte	%00,%71,%00,%74,%00,%75,%00,%00
	.byte	%00,%00,%00,%77,%00,%00,%00,%00
	.byte	%00,%78,%00,%00,%00,%79,%00,%00
	.byte	%00,%7c,%00,%00,%00,%7a,%00,%7b
	.byte	%00,%7d,%00,%6f,%00,%76,%00,%71
	.byte	%00,%74,%00,%72,%00,%00,%00,%73
	.byte	%00,%00,%00,%00,%00,%75,%00,%00
	.byte	%00,%00,%00,%77,%00,%00,%00,%00
	.byte	%00,%78,%00,%00,%00,%79,%00,%00
	.byte	%00,%7c,%00,%d6,%00,%7a,%00,%7b
	.byte	%00,%7d,%00,%6f,%00,%76,%00,%71
	.byte	%00,%74,%00,%00,%00,%da,%00,%70
	.byte	%00,%00,%00,%7e,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%72,%00,%00,%00,%73
	.byte	%00,%00,%00,%72,%00,%75,%00,%73
	.byte	%00,%00,%00,%77,%00,%75,%00,%00
	.byte	%00,%78,%00,%77,%00,%79,%00,%70
	.byte	%00,%7c,%00,%7e,%00,%7a,%00,%7b
	.byte	%00,%7d,%00,%6f,%00,%76,%00,%71
	.byte	%00,%74,%00,%6f,%00,%76,%00,%71
	.byte	%00,%74,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%72,%00,%00,%00,%73
	.byte	%00,%00,%00,%00,%00,%75,%00,%00
	.byte	%00,%00,%00,%77,%00,%c2,%00,%00
	.byte	%00,%78,%00,%00,%00,%79,%00,%70
	.byte	%00,%7c,%00,%7e,%00,%7a,%00,%7b
	.byte	%00,%7d,%00,%6f,%00,%76,%00,%71
	.byte	%00,%74,%00,%72,%00,%00,%00,%73
	.byte	%00,%00,%00,%00,%00,%75,%00,%00
	.byte	%00,%00,%00,%77,%00,%00,%00,%00
	.byte	%00,%78,%00,%00,%00,%79,%00,%00
	.byte	%00,%7c,%00,%00,%00,%7a,%00,%7b
	.byte	%00,%7d,%00,%6f,%00,%76,%00,%71
	.byte	%00,%74,%00,%00,%00,%00,%00,%70
	.byte	%00,%00,%00,%7e,%00,%54,%00,%57
	.byte	%00,%55,%00,%56,%00,%00,%00,%00
	.byte	%00,%4e,%00,%00,%00,%00,%00,%4c
	.byte	%00,%00,%00,%00,%00,%4d,%00,%54
	.byte	%00,%57,%00,%55,%00,%56,%00,%00
	.byte	%00,%00,%00,%4e,%00,%00,%00,%00
	.byte	%00,%4c,%00,%7e,%00,%00,%00,%4d
	.byte	%00,%00,%00,%00,%00,%50,%00,%4f
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%50
	.byte	%00,%4f,%00,%00,%00,%00,%00,%00
	.byte	%00,%51,%00,%00,%00,%52,%00,%00
	.byte	%00,%00,%00,%54,%00,%57,%00,%55
	.byte	%00,%56,%00,%00,%00,%b5,%00,%4e
	.byte	%00,%00,%00,%51,%00,%4c,%00,%52
	.byte	%00,%00,%00,%4d,%00,%54,%00,%57
	.byte	%00,%55,%00,%56,%00,%00,%00,%b3
	.byte	%00,%4e,%00,%00,%00,%00,%00,%4c
	.byte	%00,%00,%00,%00,%00,%4d,%00,%00
	.byte	%00,%00,%00,%50,%00,%4f,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%50,%00,%4f
	.byte	%00,%00,%00,%00,%00,%00,%00,%51
	.byte	%00,%00,%00,%52,%00,%00,%00,%00
	.byte	%00,%54,%00,%57,%00,%55,%00,%56
	.byte	%00,%00,%00,%b1,%00,%4e,%00,%00
	.byte	%00,%51,%00,%4c,%00,%52,%00,%00
	.byte	%00,%4d,%00,%54,%00,%57,%00,%55
	.byte	%00,%56,%00,%00,%00,%ae,%00,%4e
	.byte	%00,%00,%00,%00,%00,%4c,%00,%00
	.byte	%00,%00,%00,%4d,%00,%00,%00,%00
	.byte	%00,%50,%00,%4f,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%50,%00,%4f,%00,%00
	.byte	%00,%00,%00,%00,%00,%51,%00,%00
	.byte	%00,%52,%00,%00,%00,%00,%00,%54
	.byte	%00,%57,%00,%55,%00,%56,%00,%00
	.byte	%00,%ac,%00,%4e,%00,%00,%00,%51
	.byte	%00,%4c,%00,%52,%00,%00,%00,%4d
	.byte	%00,%54,%00,%57,%00,%55,%00,%56
	.byte	%00,%00,%00,%aa,%00,%4e,%00,%00
	.byte	%00,%00,%00,%4c,%00,%00,%00,%00
	.byte	%00,%4d,%00,%00,%00,%00,%00,%50
	.byte	%00,%4f,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%50,%00,%4f,%00,%00,%00,%00
	.byte	%00,%00,%00,%51,%00,%00,%00,%52
	.byte	%00,%00,%00,%00,%00,%54,%00,%57
	.byte	%00,%55,%00,%56,%00,%00,%00,%a8
	.byte	%00,%4e,%00,%00,%00,%51,%00,%4c
	.byte	%00,%52,%00,%00,%00,%4d,%00,%54
	.byte	%00,%57,%00,%55,%00,%56,%00,%00
	.byte	%00,%a6,%00,%4e,%00,%00,%00,%00
	.byte	%00,%4c,%00,%00,%00,%00,%00,%4d
	.byte	%00,%00,%00,%00,%00,%50,%00,%4f
	.byte	%00,%72,%00,%00,%00,%73,%00,%00
	.byte	%00,%00,%00,%75,%00,%00,%00,%00
	.byte	%00,%77,%00,%00,%00,%00,%00,%50
	.byte	%00,%4f,%00,%79,%00,%00,%00,%00
	.byte	%00,%51,%00,%00,%00,%52,%00,%00
	.byte	%00,%6f,%00,%76,%00,%71,%00,%74
	.byte	%00,%00,%01,%0e,%00,%00,%00,%00
	.byte	%00,%00,%00,%51,%00,%00,%00,%52
	.byte	%00,%00,%00,%83,%00,%54,%00,%57
	.byte	%00,%55,%00,%56,%00,%00,%00,%00
	.byte	%00,%4e,%00,%00,%00,%00,%00,%4c
	.byte	%00,%00,%00,%00,%00,%4d,%00,%54
	.byte	%00,%57,%00,%55,%00,%56,%00,%00
	.byte	%00,%00,%00,%4e,%00,%00,%00,%00
	.byte	%00,%4c,%00,%00,%00,%00,%00,%4d
	.byte	%00,%00,%00,%00,%00,%50,%00,%4f
	.byte	%00,%00,%00,%0a,%00,%72,%00,%0e
	.byte	%00,%73,%00,%00,%00,%00,%00,%75
	.byte	%00,%00,%00,%00,%00,%77,%00,%50
	.byte	%00,%4f,%00,%78,%00,%00,%00,%79
	.byte	%00,%51,%00,%10,%00,%52,%00,%7a
	.byte	%00,%00,%00,%00,%00,%6f,%00,%76
	.byte	%00,%71,%00,%74,%00,%00,%00,%00
	.byte	%00,%00,%00,%51,%00,%00,%00,%52
	.byte	%00,%00,%00,%4a,%00,%54,%00,%57
	.byte	%00,%55,%00,%56,%00,%00,%00,%00
	.byte	%00,%4e,%00,%00,%00,%00,%00,%4c
	.byte	%00,%00,%00,%00,%00,%4d,%00,%54
	.byte	%00,%57,%00,%55,%00,%56,%00,%00
	.byte	%00,%00,%00,%4e,%00,%00,%00,%00
	.byte	%00,%4c,%00,%00,%00,%00,%00,%4d
	.byte	%00,%00,%00,%00,%00,%50,%00,%4f
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%50
	.byte	%00,%4f,%00,%00,%00,%00,%00,%72
	.byte	%00,%51,%00,%73,%00,%52,%00,%92
	.byte	%00,%75,%00,%54,%00,%57,%00,%55
	.byte	%00,%56,%00,%00,%00,%00,%00,%4e
	.byte	%00,%00,%00,%51,%00,%4c,%00,%52
	.byte	%00,%00,%00,%4d,%00,%00,%00,%6f
	.byte	%00,%76,%00,%71,%00,%74,%00,%00
	.byte	%00,%72,%00,%00,%00,%73,%00,%00
	.byte	%00,%00,%00,%75,%00,%00,%00,%00
	.byte	%00,%77,%00,%50,%00,%4f,%00,%78
	.byte	%00,%00,%00,%79,%00,%00,%00,%7c
	.byte	%00,%00,%00,%7a,%00,%7b,%00,%00
	.byte	%00,%6f,%00,%76,%00,%71,%00,%74
	.byte	%00,%00,%00,%00,%00,%72,%00,%51
	.byte	%00,%73,%00,%8e,%00,%00,%00,%75
	.byte	%00,%00,%00,%00,%00,%77,%00,%00
	.byte	%00,%00,%00,%78,%00,%00,%00,%79
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%6f,%00,%76
	.byte	%00,%71,%00,%74
D0a22:
	.byte	%fc,%18,%00,%24,%fc,%18,%fc,%18
	.byte	%fc,%18,%00,%4e,%fc,%18,%00,%d7
	.byte	%00,%f9,%fc,%18,%01,%00,%fc,%18
	.byte	%fc,%18,%00,%98,%01,%4d,%00,%92
	.byte	%01,%3e,%fc,%18,%00,%27,%00,%57
	.byte	%fc,%18,%00,%cd,%00,%cd,%00,%c9
	.byte	%00,%74,%00,%02,%00,%de,%fc,%18
	.byte	%fc,%18,%fc,%18,%00,%fb,%00,%d7
	.byte	%fc,%18,%01,%31,%fc,%18,%fc,%18
	.byte	%fc,%18,%fc,%18,%00,%be,%00,%6e
	.byte	%00,%74,%00,%57,%00,%65,%00,%5e
	.byte	%00,%33,%fc,%18,%fc,%18,%00,%ba
	.byte	%00,%67,%03,%d4,%fc,%18,%fc,%18
	.byte	%fc,%18,%00,%2c,%fc,%18,%00,%36
	.byte	%00,%4a,%fc,%18,%ff,%e4,%00,%09
	.byte	%fc,%18,%ff,%f0,%fc,%18,%fc,%18
	.byte	%00,%62,%04,%14,%fc,%18,%fc,%18
	.byte	%fc,%18,%01,%3c,%fc,%18,%fc,%18
	.byte	%00,%5c,%02,%9a,%03,%94,%00,%97
	.byte	%04,%14,%04,%14,%04,%14,%04,%14
	.byte	%04,%14,%04,%3a,%03,%c7,%04,%07
	.byte	%00,%dd,%fc,%18,%fc,%18,%fc,%18
	.byte	%00,%90,%00,%d7,%00,%37,%fc,%18
	.byte	%00,%57,%00,%58,%fc,%18,%fc,%18
	.byte	%00,%8a,%01,%31,%fc,%18,%fc,%18
	.byte	%00,%57,%fc,%18,%fc,%18,%fc,%18
	.byte	%00,%05,%00,%2a,%fc,%18,%fc,%18
	.byte	%02,%82,%fc,%18,%fc,%18,%04,%14
	.byte	%04,%14,%03,%61,%03,%54,%03,%2e
	.byte	%03,%21,%02,%fb,%04,%14,%02,%ee
	.byte	%02,%c8,%02,%bb,%04,%14,%04,%14
	.byte	%04,%14,%04,%14,%04,%14,%00,%2f
	.byte	%fc,%18,%02,%9a,%03,%94,%fc,%18
	.byte	%fc,%18,%04,%14,%01,%2f,%fc,%18
	.byte	%00,%97,%00,%97,%00,%97,%00,%97
	.byte	%00,%97,%00,%97,%03,%c7,%00,%a6
	.byte	%02,%5e,%00,%d2,%fc,%18,%00,%31
	.byte	%02,%9a,%fc,%18,%fc,%18,%fc,%18
	.byte	%fc,%18,%fc,%18,%fc,%18,%04,%14
	.byte	%fc,%18,%fc,%18,%fc,%18,%01,%27
	.byte	%00,%25,%fc,%18,%fc,%18,%01,%af
	.byte	%02,%9a,%fc,%18,%04,%14,%01,%18
	.byte	%04,%14,%01,%18,%04,%14,%01,%20
	.byte	%04,%14,%fc,%18,%04,%14,%02,%0f
	.byte	%04,%30,%04,%14,%03,%a1,%04,%14
	.byte	%02,%62,%04,%14,%04,%63,%03,%e3
	.byte	%02,%3a,%02,%9a,%02,%9a,%00,%7e
	.byte	%03,%94,%00,%2f,%02,%22,%fc,%18
	.byte	%00,%9f,%04,%14,%fc,%18,%00,%76
	.byte	%00,%e0,%00,%d2,%04,%14,%fc,%18
	.byte	%00,%36,%fc,%18,%02,%9a,%00,%0b
	.byte	%00,%01,%fc,%18,%02,%9a,%02,%9a
	.byte	%02,%9a,%02,%9a,%02,%9a,%02,%9a
	.byte	%02,%9a,%02,%9a,%04,%14,%fc,%18
	.byte	%fc,%18,%00,%79,%fc,%18,%01,%1b
	.byte	%00,%97,%00,%51,%fc,%18,%00,%8d
	.byte	%00,%76,%02,%9a,%fc,%18,%fc,%18
	.byte	%00,%1f,%fc,%18,%fc,%18,%01,%ed
	.byte	%fc,%18,%00,%b6,%00,%b6,%00,%b6
	.byte	%00,%b6,%04,%14,%00,%b6,%00,%29
	.byte	%00,%28,%03,%87,%01,%25,%fc,%18
	.byte	%00,%15,%00,%b6,%00,%da,%00,%d8
	.byte	%fc,%18,%00,%d5,%00,%d4,%00,%d1
	.byte	%04,%14,%01,%0f,%04,%49,%fc,%18
	.byte	%fc,%18,%00,%4d,%00,%cc,%fc,%18
	.byte	%fc,%18,%00,%f3,%fc,%18,%fc,%18
	.byte	%00,%e6,%00,%9d,%02,%82,%fc,%18
	.byte	%fc,%18,%fc,%18,%fc,%18,%01,%d4
	.byte	%00,%22,%fc,%18,%fc,%18,%fc,%18
	.byte	%04,%14,%04,%14,%04,%14,%04,%14
	.byte	%fc,%18,%01,%bc,%fc,%18,%fc,%18
	.byte	%00,%9b,%fc,%18,%00,%c1,%00,%b6
	.byte	%fc,%18,%fc,%18,%01,%96,%01,%72
	.byte	%00,%18,%01,%4e,%fc,%18,%fc,%18
	.byte	%04,%14,%fc,%18,%fc,%18,%fc,%18
	.byte	%04,%14,%fc,%18,%01,%2a,%00,%16
	.byte	%00,%14,%fc,%18,%fc,%18
D0c88:
	.byte	%00,%00,%01,%96,%00,%28,%01,%95
	.byte	%01,%94,%01,%93,%01,%92,%01,%91
	.byte	%01,%90,%01,%84,%01,%82,%00,%0e
	.byte	%01,%7f,%00,%00,%00,%02,%00,%52
	.byte	%00,%1c,%01,%7e,%00,%a8,%01,%7c
	.byte	%01,%7b,%00,%22,%00,%48,%01,%79
	.byte	%00,%08,%00,%56,%00,%01,%01,%77
	.byte	%01,%76,%01,%75,%00,%1d,%01,%74
	.byte	%01,%73,%01,%72,%00,%43,%01,%71
	.byte	%01,%70,%00,%0a,%01,%6f,%00,%23
	.byte	%01,%4c,%01,%6e,%00,%15,%00,%34
	.byte	%01,%6d,%01,%6c,%00,%40,%01,%60
	.byte	%01,%5e,%00,%49,%01,%5b,%01,%5a
	.byte	%01,%57,%00,%1a,%00,%19,%01,%53
	.byte	%01,%52,%01,%51,%01,%50,%01,%2c
D0d00:
	.byte	%00,%00,%00,%01,%00,%01,%00,%1c
	.byte	%00,%1c,%00,%1d,%00,%1d,%00,%1f
	.byte	%00,%1d,%00,%20,%00,%21,%00,%21
	.byte	%00,%24,%00,%24,%00,%26,%00,%26
	.byte	%00,%26,%00,%23,%00,%23,%00,%23
	.byte	%00,%11,%00,%11,%00,%10,%00,%10
	.byte	%00,%10,%00,%10,%00,%28,%00,%12
	.byte	%00,%12,%00,%12,%00,%12,%00,%12
	.byte	%00,%13,%00,%13,%00,%09,%00,%09
	.byte	%00,%29,%00,%29,%00,%2b,%00,%2b
	.byte	%00,%14,%00,%14,%00,%0a,%00,%0a
	.byte	%00,%2c,%00,%2c,%00,%2e,%00,%2e
	.byte	%00,%27,%00,%2f,%00,%27,%00,%18
	.byte	%00,%18,%00,%18,%00,%18,%00,%18
	.byte	%00,%1a,%00,%1a,%00,%1a,%00,%1a
	.byte	%00,%1a,%00,%1a,%00,%19,%00,%19
	.byte	%00,%19,%00,%19,%00,%19,%00,%19
	.byte	%00,%19,%00,%0c,%00,%30,%00,%30
	.byte	%00,%1e,%00,%32,%00,%1e,%00,%33
	.byte	%00,%33,%00,%31,%00,%31,%00,%31
	.byte	%00,%31,%00,%35,%00,%35,%00,%36
	.byte	%00,%36,%00,%2a,%00,%2a,%00,%2d
	.byte	%00,%2d,%00,%34,%00,%34,%00,%37
	.byte	%00,%22,%00,%38,%00,%25,%00,%25
	.byte	%00,%25,%00,%25,%00,%25,%00,%25
	.byte	%00,%25,%00,%25,%00,%25,%00,%25
	.byte	%00,%25,%00,%25,%00,%25,%00,%25
	.byte	%00,%25,%00,%25,%00,%25,%00,%39
	.byte	%00,%39,%00,%39,%00,%07,%00,%04
	.byte	%00,%03,%00,%05,%00,%06,%00,%08
	.byte	%00,%3a,%00,%02,%00,%3b,%00,%0b
	.byte	%00,%0e,%00,%0e,%00,%1b,%00,%1b
	.byte	%00,%0d,%00,%0d,%00,%0d,%00,%0d
	.byte	%00,%0d,%00,%0d,%00,%0d,%00,%0d
	.byte	%00,%0d,%00,%0d,%00,%0d,%00,%0d
	.byte	%00,%0d,%00,%0d,%00,%0d,%00,%0d
	.byte	%00,%0d,%00,%0d,%00,%0d,%00,%0d
	.byte	%00,%0d,%00,%0d,%00,%0d,%00,%0d
	.byte	%00,%0d,%00,%0f,%00,%0f,%00,%0f
	.byte	%00,%0f,%00,%0f,%00,%0f,%00,%0f
	.byte	%00,%0f,%00,%0f,%00,%0f,%00,%0f
	.byte	%00,%0f,%00,%0f,%00,%0f,%00,%0f
	.byte	%00,%0f,%00,%0f,%00,%0f,%00,%15
	.byte	%00,%16,%00,%16,%00,%16,%00,%16
	.byte	%00,%16,%00,%16,%00,%16,%00,%17
	.byte	%00,%17
D0e6a:
	.byte	%00,%00,%00,%02,%00,%00,%00,%01
	.byte	%00,%01,%00,%02,%00,%03,%00,%00
	.byte	%00,%04,%00,%02,%00,%02,%00,%00
	.byte	%00,%02,%00,%00,%00,%03,%00,%04
	.byte	%00,%00,%00,%03,%00,%02,%00,%02
	.byte	%00,%01,%00,%00,%00,%02,%00,%02
	.byte	%00,%01,%00,%01,%00,%01,%00,%01
	.byte	%00,%02,%00,%03,%00,%01,%00,%01
	.byte	%00,%05,%00,%02,%00,%01,%00,%02
	.byte	%00,%01,%00,%03,%00,%01,%00,%03
	.byte	%00,%05,%00,%02,%00,%01,%00,%02
	.byte	%00,%01,%00,%03,%00,%02,%00,%01
	.byte	%00,%01,%00,%00,%00,%04,%00,%01
	.byte	%00,%01,%00,%03,%00,%02,%00,%01
	.byte	%00,%02,%00,%03,%00,%03,%00,%04
	.byte	%00,%01,%00,%03,%00,%02,%00,%03
	.byte	%00,%03,%00,%04,%00,%03,%00,%03
	.byte	%00,%02,%00,%02,%00,%01,%00,%03
	.byte	%00,%01,%00,%00,%00,%04,%00,%01
	.byte	%00,%01,%00,%01,%00,%01,%00,%03
	.byte	%00,%06,%00,%01,%00,%03,%00,%01
	.byte	%00,%04,%00,%00,%00,%01,%00,%00
	.byte	%00,%01,%00,%00,%00,%01,%00,%01
	.byte	%00,%04,%00,%01,%00,%02,%00,%01
	.byte	%00,%02,%00,%02,%00,%02,%00,%07
	.byte	%00,%04,%00,%02,%00,%02,%00,%02
	.byte	%00,%02,%00,%03,%00,%03,%00,%01
	.byte	%00,%02,%00,%02,%00,%02,%00,%02
	.byte	%00,%03,%00,%02,%00,%01,%00,%04
	.byte	%00,%03,%00,%04,%00,%06,%00,%04
	.byte	%00,%00,%00,%02,%00,%00,%00,%02
	.byte	%00,%01,%00,%00,%00,%01,%00,%03
	.byte	%00,%03,%00,%03,%00,%03,%00,%03
	.byte	%00,%03,%00,%03,%00,%03,%00,%03
	.byte	%00,%03,%00,%03,%00,%03,%00,%03
	.byte	%00,%03,%00,%04,%00,%04,%00,%04
	.byte	%00,%04,%00,%04,%00,%04,%00,%04
	.byte	%00,%04,%00,%05,%00,%03,%00,%03
	.byte	%00,%01,%00,%02,%00,%02,%00,%02
	.byte	%00,%02,%00,%02,%00,%02,%00,%02
	.byte	%00,%04,%00,%04,%00,%04,%00,%02
	.byte	%00,%03,%00,%03,%00,%01,%00,%01
	.byte	%00,%01,%00,%01,%00,%03,%00,%02
	.byte	%00,%00,%00,%02,%00,%05,%00,%02
	.byte	%00,%03,%00,%04,%00,%03,%00,%02
	.byte	%00,%02
D0fd4:
	.byte	%fc,%18,%ff,%ff,%ff,%e4,%ff,%e3
	.byte	%01,%00,%ff,%ef,%ff,%f0,%ff,%d8
	.byte	%ff,%ee,%00,%22,%00,%21,%ff,%ec
	.byte	%ff,%ed,%ff,%f6,%00,%23,%ff,%f7
	.byte	%00,%31,%00,%39,%ff,%e2,%ff,%e7
	.byte	%ff,%cf,%00,%0b,%00,%32,%ff,%f4
	.byte	%ff,%e6
D1006:
	.byte	%ff,%cd,%00,%02,%01,%00,%ff,%ee
	.byte	%ff,%d8,%00,%21,%00,%34,%00,%02
	.byte	%00,%34,%00,%02,%00,%39,%00,%38
	.byte	%ff,%e1,%00,%32,%00,%36,%ff,%e6
	.byte	%ff,%e7,%ff,%e6,%ff,%e7,%ff,%d0
	.byte	%00,%33,%00,%02,%00,%32,%00,%36
	.byte	%ff,%cc,%00,%3a,%00,%32,%00,%21
	.byte	%ff,%d4,%ff,%d2,%ff,%ee,%ff,%d7
	.byte	%ff,%d5,%00,%02,%ff,%ce,%ff,%e0
	.byte	%ff,%df,%00,%33,%00,%37,%ff,%f5
	.byte	%ff,%c5,%00,%33,%00,%33,%00,%33
	.byte	%00,%38,%00,%33,%00,%37,%ff,%f5
	.byte	%ff,%f3,%00,%34,%ff,%f1,%00,%0b
	.byte	%00,%0e,%00,%08,%00,%1f,%00,%1e
	.byte	%00,%30,%00,%32,%ff,%e9,%00,%02
	.byte	%00,%04,%00,%05,%00,%03,%ff,%d3
	.byte	%00,%39,%ff,%d9,%ff,%e8,%ff,%e7
	.byte	%ff,%e6,%00,%16,%01,%00,%ff,%d6
	.byte	%00,%38,%00,%3a,%ff,%cf,%ff,%e7
	.byte	%ff,%de,%ff,%dd,%ff,%c8,%ff,%f0
	.byte	%01,%00,%00,%34,%00,%37,%ff,%f3
	.byte	%00,%02,%00,%37,%00,%1a,%00,%38
	.byte	%00,%1c,%00,%06,%00,%08,%00,%1d
	.byte	%00,%0b,%00,%1b,%00,%0e,%00,%11
	.byte	%00,%13,%00,%17,%00,%18,%00,%15
	.byte	%00,%19,%00,%3a,%ff,%cb,%ff,%ca
	.byte	%ff,%f3,%ff,%c9,%00,%34,%00,%1e
	.byte	%00,%36,%00,%20,%00,%32,%ff,%f1
	.byte	%ff,%f1,%ff,%f1,%ff,%f1,%ff,%f1
	.byte	%ff,%f1,%00,%32,%ff,%eb,%ff,%f3
	.byte	%ff,%ee,%00,%33,%ff,%e5,%ff,%f3
	.byte	%00,%32,%00,%35,%ff,%d2,%00,%38
	.byte	%00,%16,%ff,%fe,%ff,%c6,%00,%35
	.byte	%ff,%d5,%ff,%fe,%ff,%da,%ff,%d9
	.byte	%00,%39,%00,%39,%ff,%f3,%ff,%f3
	.byte	%ff,%f3,%00,%3a,%ff,%f3,%00,%3a
	.byte	%ff,%f3,%00,%3a,%ff,%f3,%00,%3a
	.byte	%ff,%f3,%00,%3a,%ff,%f3,%ff,%f3
	.byte	%00,%3a,%ff,%f3,%00,%3a,%ff,%f3
	.byte	%00,%3a,%ff,%f3,%ff,%f3,%ff,%f3
	.byte	%ff,%f3,%ff,%f3,%ff,%d6,%00,%38
	.byte	%ff,%cb,%ff,%f3,%00,%02,%ff,%eb
	.byte	%00,%33,%00,%33,%ff,%ea,%00,%32
	.byte	%00,%0b,%00,%38,%00,%33,%ff,%d1
	.byte	%ff,%fe,%ff,%f3,%ff,%f0,%ff,%dc
	.byte	%00,%39,%ff,%f3,%ff,%f3,%ff,%f3
	.byte	%ff,%f3,%ff,%f3,%ff,%f3,%ff,%f3
	.byte	%ff,%f3,%00,%16,%00,%35,%ff,%ca
	.byte	%ff,%d6,%00,%37,%00,%33,%ff,%f1
	.byte	%00,%36,%00,%33,%ff,%ea,%ff,%ea
	.byte	%ff,%f3,%ff,%e8,%00,%39,%ff,%e2
	.byte	%ff,%db,%00,%35,%ff,%f3,%ff,%de
	.byte	%ff,%fc,%ff,%fd,%ff,%fb,%ff,%f9
	.byte	%ff,%fa,%ff,%f8,%00,%29,%00,%2a
	.byte	%00,%24,%00,%25,%00,%39,%01,%00
	.byte	%ff,%c7,%00,%26,%00,%2b,%00,%2c
	.byte	%00,%2d,%00,%28,%00,%02,%00,%2f
	.byte	%00,%2e,%ff,%f3,%00,%35,%00,%37
	.byte	%ff,%f5,%00,%33,%00,%39,%00,%39
	.byte	%ff,%db,%ff,%db,%ff,%db,%ff,%db
	.byte	%ff,%f2,%ff,%f3,%ff,%db,%00,%39
	.byte	%00,%39,%00,%39,%ff,%f3,%00,%02
	.byte	%00,%39,%00,%35,%ff,%db,%00,%32
	.byte	%00,%32,%00,%32,%00,%32,%00,%16
	.byte	%ff,%f3,%00,%16,%00,%37,%00,%32
	.byte	%00,%27,%00,%2b,%00,%33,%00,%39
	.byte	%00,%39,%ff,%f3,%ff,%f3,%ff,%f2
	.byte	%ff,%f3,%00,%16,%00,%33,%00,%32
	.byte	%ff,%db,%00,%33,%00,%33,%00,%39
	.byte	%00,%33,%ff,%f3,%ff,%f2,%00,%33
	.byte	%00,%39,%00,%39
D123a:
	.byte	%00,%02,%ff,%fe,%00,%01,%00,%03
	.byte	%00,%04,%00,%00,%00,%14,%00,%18
	.byte	%00,%19,%00,%1a,%00,%1b,%00,%1e
	.byte	%00,%1f,%00,%00,%00,%2a,%00,%00
	.byte	%00,%22,%00,%05,%00,%00,%ff,%fe
	.byte	%00,%48,%00,%00,%00,%00,%00,%00
	.byte	%ff,%fe,%00,%59,%00,%3c,%00,%4c
	.byte	%00,%16,%00,%17,%00,%1c,%00,%00
	.byte	%ff,%fe,%00,%00,%ff,%fe,%00,%06
	.byte	%00,%49,%00,%0b,%00,%00,%00,%7a
	.byte	%00,%38,%00,%3e,%00,%00,%00,%00
	.byte	%00,%00,%00,%44,%00,%46,%00,%00
	.byte	%00,%7a,%00,%00,%00,%5a,%00,%45
	.byte	%00,%1d,%00,%57,%00,%2c,%00,%2f
	.byte	%00,%55,%00,%24,%00,%26,%00,%00
	.byte	%00,%08,%00,%00,%00,%3f,%00,%40
	.byte	%00,%00,%00,%00,%00,%3d,%00,%42
	.byte	%00,%43,%00,%00,%00,%39,%00,%3a
	.byte	%00,%00,%00,%4f,%00,%00,%00,%98
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%a6,%00,%a7,%00,%a8,%00,%a9
	.byte	%00,%00,%00,%58,%00,%2e,%00,%30
	.byte	%00,%33,%00,%34,%00,%78,%00,%37
	.byte	%00,%00,%00,%56,%00,%78,%00,%4a
	.byte	%00,%4e,%00,%09,%00,%0a,%00,%10
	.byte	%00,%00,%00,%00,%00,%5d,%00,%41
	.byte	%00,%7b,%00,%47,%00,%3b,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%55
	.byte	%00,%51,%00,%53,%00,%00,%00,%5b
	.byte	%00,%99,%00,%00,%00,%00,%00,%b4
	.byte	%00,%9a,%00,%9b,%00,%9c,%00,%9d
	.byte	%00,%9e,%00,%9f,%00,%00,%00,%00
	.byte	%00,%00,%00,%ac,%00,%a3,%00,%00
	.byte	%00,%7e,%00,%b3,%00,%28,%00,%2d
	.byte	%00,%31,%00,%78,%00,%36,%00,%00
	.byte	%00,%20,%00,%25,%00,%27,%00,%0d
	.byte	%00,%00,%00,%12,%00,%13,%00,%80
	.byte	%00,%81,%00,%82
D1386:
	.byte	%00,%00,%00,%83,%00,%00,%00,%84
	.byte	%00,%00,%00,%85,%00,%00,%00,%86
	.byte	%00,%00,%00,%87,%00,%88,%00,%00
	.byte	%00,%89,%00,%00,%00,%8a,%00,%00
	.byte	%00,%8b,%00,%8c,%00,%00,%00,%96
	.byte	%00,%97,%00,%00,%00,%56,%00,%55
	.byte	%00,%00,%00,%a5,%00,%00,%00,%00
	.byte	%00,%aa,%00,%ab,%00,%ac,%00,%ac
	.byte	%00,%00,%00,%a4,%00,%00,%00,%35
	.byte	%00,%79,%00,%00,%00,%00,%00,%11
	.byte	%00,%8e,%00,%8f,%00,%90,%00,%91
	.byte	%00,%8d,%00,%92,%00,%93,%00,%94
	.byte	%00,%00,%00,%50,%00,%52,%00,%00
	.byte	%00,%a2,%00,%a1,%00,%a0,%00,%7a
	.byte	%00,%ad,%00,%00,%00,%af,%00,%7f
	.byte	%00,%32,%00,%0e,%00,%00,%00,%0c
	.byte	%00,%5c,%00,%00,%00,%5f,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%7d
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%6b,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%72,%00,%00
	.byte	%00,%00,%00,%a6,%00,%00,%00,%00
	.byte	%00,%95,%00,%54,%00,%b0,%00,%00
	.byte	%00,%b2,%00,%0f,%00,%5e,%00,%60
	.byte	%00,%61,%00,%62,%00,%00,%00,%00
	.byte	%00,%7c,%00,%65,%00,%66,%00,%67
	.byte	%00,%68,%00,%00,%00,%00,%00,%6c
	.byte	%00,%6d,%00,%6e,%00,%00,%00,%00
	.byte	%00,%7d,%00,%00,%00,%6f,%00,%00
	.byte	%00,%71,%00,%b1,%00,%00,%00,%74
	.byte	%00,%00,%00,%00,%00,%69,%00,%6a
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%70,%00,%ae,%00,%00,%00,%64
	.byte	%00,%73,%00,%75,%00,%7d,%00,%77
	.byte	%00,%00,%00,%00,%00,%00,%00,%76
	.byte	%00,%63
D14a0:
	.byte	%00,%00
D14a2:
	.byte	%ff,%ff
D14a4:
	.byte	%00,%00
D14a6:
	.byte	%00,%00
D14a8:
	.byte	%04,%4a,%04,%56,%04,%56,%04,%c0
D14b0:
	.byte	%0e,%c6,%0e,%c6,%0e,%d8,%0e,%be
	.byte	%0f,%6e,%0f,%6e,%0f,%6e,%0f,%6e
	.byte	%0e,%c6,%0e,%d8,%0e,%c6,%0e,%c6
	.byte	%0e,%d8,%0e,%be
D14cc:
	.byte	%05,%f4,%05,%74,%05,%74,%06,%22
	.byte	%05,%80,%05,%9c,%05,%d4,%03,%36
	.byte	%03,%36,%05,%fc,%03,%36,%06,%06
	.byte	%06,%22,%06,%18,%03,%36,%06,%14
	.byte	%06,%1e,%06,%3a,%03,%36,%06,%2e
	.byte	%10,%bc,%03,%36,%06,%42,%06,%3a
	.byte	%06,%4e,%03,%36,%06,%58,%06,%78
	.byte	%03,%36,%03,%36,%06,%a2,%06,%ae
	.byte	%06,%c6,%06,%d2,%03,%36,%03,%36
	.byte	%06,%d6,%06,%e0,%06,%a2,%06,%ee
	.byte	%07,%26,%07,%3a,%03,%36,%03,%36
	.byte	%07,%42,%07,%56,%07,%bc,%07,%d8
	.byte	%07,%e0,%03,%36,%03,%36,%07,%e6
	.byte	%08,%30,%08,%28,%08,%50,%08,%5c
	.byte	%08,%66,%08,%7e,%08,%fc,%09,%04
	.byte	%08,%50,%08,%5c,%08,%66,%08,%7e
	.byte	%09,%04,%09,%0c,%09,%36,%09,%46
	.byte	%09,%40,%09,%40,%03,%36,%07,%d8
	.byte	%03,%36,%09,%50,%03,%36,%09,%78
	.byte	%09,%90,%09,%b8,%09,%be,%03,%36
	.byte	%03,%36,%09,%c6,%09,%d0,%03,%36
	.byte	%03,%36,%03,%36,%03,%36,%09,%d8
	.byte	%03,%36,%09,%e4,%09,%ec,%0a,%36
	.byte	%0a,%84,%03,%36,%0a,%9a,%0a,%90
	.byte	%0a,%ac,%0a,%dc,%0b,%30,%0b,%70
	.byte	%0b,%ba,%0b,%e6,%0c,%06,%0c,%22
	.byte	%0c,%aa,%03,%36,%03,%36,%03,%36
	.byte	%03,%36,%0c,%ee,%0d,%18,%0d,%22
	.byte	%0d,%32,%0d,%64,%0d,%96,%0d,%be
	.byte	%0e,%42,%0e,%98,%0f,%7e,%0f,%8e
	.byte	%0f,%7e,%0f,%a4,%03,%36,%08,%28
	.byte	%0f,%b0,%0f,%ba,%0f,%dc,%0f,%d6
	.byte	%0f,%dc,%0f,%e8,%0f,%f2,%0f,%fc
	.byte	%0f,%dc,%0f,%dc,%10,%0e,%10,%20
	.byte	%10,%32,%0f,%dc,%0f,%dc,%10,%44
	.byte	%10,%44,%10,%44,%10,%44,%10,%44
	.byte	%10,%44,%10,%44,%10,%44,%10,%50
	.byte	%10,%6c,%0f,%dc,%03,%36,%10,%76
	.byte	%10,%88,%10,%94,%10,%88,%10,%c6
	.byte	%10,%cc,%10,%ea,%10,%f2,%11,%24
	.byte	%11,%2a,%11,%44,%11,%84,%11,%ca
	.byte	%12,%14,%12,%78,%12,%ae,%12,%cc
	.byte	%09,%04,%12,%d4,%12,%ea,%12,%f6
	.byte	%13,%10,%08,%50,%08,%66,%08,%7e
	.byte	%09,%04,%13,%1c
D1630:
	.byte	%62,%61,%64,%20,%62,%64,%74,%79
	.byte	%00
D1639:
	.byte	%64,%69,%6d,%65,%6e,%73,%69,%6f
	.byte	%6e,%20,%74,%61,%62,%6c,%65,%20
	.byte	%6f,%76,%65,%72,%66,%6c,%6f,%77
	.byte	%00
D1652:
	.byte	%77,%68,%69,%6c,%65,%73,%2c,%20
	.byte	%66,%6f,%72,%73,%2c,%20,%65,%74
	.byte	%63,%2e,%20,%74,%6f,%6f,%20,%64
	.byte	%65,%65,%70,%6c,%79,%20,%6e,%65
	.byte	%73,%74,%65,%64,%00
D1677:
	.byte	%6e,%6f,%6e,%2d,%63,%6f,%6e,%73
	.byte	%74,%61,%6e,%74,%20,%63,%61,%73
	.byte	%65,%20,%65,%78,%70,%72,%65,%73
	.byte	%73,%69,%6f,%6e,%00
D1694:
	.byte	%63,%61,%73,%65,%20,%6e,%6f,%74
	.byte	%20,%69,%6e,%20,%73,%77,%69,%74
	.byte	%63,%68,%00
D16a7:
	.byte	%73,%77,%69,%74,%63,%68,%20,%74
	.byte	%61,%62,%6c,%65,%20,%6f,%76,%65
	.byte	%72,%66,%6c,%6f,%77,%00
D16bd:
	.byte	%64,%75,%70,%6c,%69,%63,%61,%74
	.byte	%65,%20,%64,%65,%66,%61,%75,%6c
	.byte	%74,%20,%69,%6e,%20,%73,%77,%69
	.byte	%74,%63,%68,%00
D16d9:
	.byte	%64,%65,%66,%61,%75,%6c,%74,%20
	.byte	%6e,%6f,%74,%20,%69,%6e,%73,%69
	.byte	%64,%65,%20,%73,%77,%69,%74,%63
	.byte	%68,%00
D16f3:
	.byte	%73,%77,%69,%74,%63,%68,%20,%74
	.byte	%61,%62,%6c,%65,%20,%6f,%76,%65
	.byte	%72,%66,%6c,%6f,%77,%00
D1709:
	.byte	%64,%75,%70,%6c,%69,%63,%61,%74
	.byte	%65,%20,%63,%61,%73,%65,%20,%69
	.byte	%6e,%20,%73,%77,%69,%74,%63,%68
	.byte	%2c,%20,%25,%64,%00
D1726:
	.byte	%73,%74,%61,%74,%65,%20,%25,%64
	.byte	%2c,%20,%63,%68,%61,%72,%20,%30
	.byte	%25,%6f,%0a,%00
D173a:
	.byte	%79,%61,%63,%63,%20,%73,%74,%61
	.byte	%63,%6b,%20,%6f,%76,%65,%72,%66
	.byte	%6c,%6f,%77,%00
D174e:
	.byte	%73,%79,%6e,%74,%61,%78,%20,%65
	.byte	%72,%72,%6f,%72,%00
D175b:
	.byte	%65,%72,%72,%6f,%72,%20,%72,%65
	.byte	%63,%6f,%76,%65,%72,%79,%20,%70
	.byte	%6f,%70,%73,%20,%73,%74,%61,%74
	.byte	%65,%20,%25,%64,%2c,%20,%75,%6e
	.byte	%63,%6f,%76,%65,%72,%73,%20,%25
	.byte	%64,%0a,%00
D1786:
	.byte	%65,%72,%72,%6f,%72,%20,%72,%65
	.byte	%63,%6f,%76,%65,%72,%79,%20,%64
	.byte	%69,%73,%63,%61,%72,%64,%73,%20
	.byte	%63,%68,%61,%72,%20,%25,%64,%0a
	.byte	%00
D17a7:
	.byte	%72,%65,%64,%75,%63,%65,%20,%25
	.byte	%64,%0a,%00
D17b2:
	.byte	%49,%6e,%61,%70,%70,%72,%6f,%70
	.byte	%72,%69,%61,%74,%65,%20,%70,%61
	.byte	%72,%61,%6d,%65,%74,%65,%72,%28
	.byte	%73,%00
D17cc:
	.byte	%66,%63,%6e,%20,%6c,%65,%76,%65
	.byte	%6c,%20,%65,%72,%72,%6f,%72,%00
D17dc:
	.byte	%24,%25,%64,%46,%41,%4b,%45,%00
D17e4:
	.byte	%73,%74,%72,%75,%63,%74,%75,%72
	.byte	%65,%20,%74,%79,%70,%65,%64,%20
	.byte	%75,%6e,%69,%6f,%6e,%20,%6d,%65
	.byte	%6d,%62,%65,%72,%20,%6d,%75,%73
	.byte	%74,%20,%62,%65,%20,%6e,%61,%6d
	.byte	%65,%64,%00
D180f:
	.byte	%66,%69,%65,%6c,%64,%20,%6f,%75
	.byte	%74,%73,%69,%64,%65,%20,%73,%74
	.byte	%72,%75,%63,%74,%00
D1824:
	.byte	%69,%6c,%6c,%65,%67,%61,%6c,%20
	.byte	%66,%69,%65,%6c,%64,%20,%73,%69
	.byte	%7a,%65,%00
D1837:
	.byte	%66,%69,%65,%6c,%64,%20,%6f,%75
	.byte	%74,%73,%69,%64,%65,%20,%73,%74
	.byte	%72,%75,%63,%74,%00
D184c:
	.byte	%69,%6c,%6c,%65,%67,%61,%6c,%20
	.byte	%6e,%75,%6c,%6c,%20,%64,%69,%6d
	.byte	%65,%6e,%73,%69,%6f,%6e,%00
D1863:
	.byte	%7a,%65,%72,%6f,%20,%6f,%72,%20
	.byte	%6e,%65,%67,%61,%74,%69,%76,%65
	.byte	%20,%73,%75,%62,%73,%63,%72,%69
	.byte	%70,%74,%00
D187e:
	.byte	%64,%69,%6d,%65,%6e,%73,%69,%6f
	.byte	%6e,%20,%74,%6f,%6f,%20,%6c,%61
	.byte	%72,%67,%65,%00
D1892:
	.byte	%66,%63,%6e,%20,%69,%6e,%20,%62
	.byte	%61,%64,%20,%63,%6f,%6e,%74,%65
	.byte	%78,%74,%00
D18a5:
	.byte	%6f,%6c,%64,%2d,%66,%61,%73,%68
	.byte	%69,%6f,%6e,%65,%64,%20,%69,%6e
	.byte	%69,%74,%69,%61,%6c,%69,%7a,%61
	.byte	%74,%69,%6f,%6e,%00
D18c2:
	.byte	%6e,%65,%73,%74,%69,%6e,%67,%20
	.byte	%74,%6f,%6f,%20,%64,%65,%65,%70
	.byte	%00
D18d3:
	.byte	%69,%6c,%6c,%65,%67,%61,%6c,%20
	.byte	%62,%72,%65,%61,%6b,%00
D18e1:
	.byte	%69,%6c,%6c,%65,%67,%61,%6c,%20
	.byte	%63,%6f,%6e,%74,%69,%6e,%75,%65
	.byte	%00
D18f2:
	.byte	%73,%74,%61,%74,%65,%6d,%65,%6e
	.byte	%74,%20,%6e,%6f,%74,%20,%72,%65
	.byte	%61,%63,%68,%65,%64,%00
D1908:
	.byte	%76,%6f,%69,%64,%20,%66,%75,%6e
	.byte	%63,%74,%69,%6f,%6e,%20,%25,%73
	.byte	%20,%63,%61,%6e,%6e,%6f,%74,%20
	.byte	%72,%65,%74,%75,%72,%6e,%20,%76
	.byte	%61,%6c,%75,%65,%00
D192d:
	.byte	%6c,%6f,%6f,%70,%20,%6e,%6f,%74
	.byte	%20,%65,%6e,%74,%65,%72,%65,%64
	.byte	%20,%61,%74,%20,%74,%6f,%70,%00
D1945:
	.byte	%6c,%6f,%6f,%70,%20,%6e,%6f,%74
	.byte	%20,%65,%6e,%74,%65,%72,%65,%64
	.byte	%20,%61,%74,%20,%74,%6f,%70,%00
D195d:
	.byte	%6c,%6f,%6f,%70,%20
D1962:
	.byte	%6e,%6f,%74,%20,%65,%6e,%74,%65
	.byte	%72,%65,%64,%20,%61,%74,%20,%74
	.byte	%6f,%70,%00
D1975:
	.byte	%73,%77,%69,%74,%63,%68,%20,%65
	.byte	%78,%70,%72,%65,%73,%73,%69,%6f
	.byte	%6e,%20,%63,%6f,%6e,%76,%65,%72
	.byte	%74,%65,%64,%20,%74,%6f,%20,%69
	.byte	%6e,%74,%65,%67,%65,%72,%00
D199c:
	.byte	%69,%6e,%76,%61,%6c,%69,%64,%20
	.byte	%74,%79,%70,%65,%20,%66,%6f,%72
	.byte	%20,%73,%77,%69,%74,%63,%68,%20
	.byte	%65,%78,%70,%72,%65,%73,%73,%69
	.byte	%6f,%6e,%00
D19bf:
	.byte	%6f,%6c,%64,%2d,%66,%61,%73,%68
	.byte	%69,%6f,%6e,%65,%64,%20,%61,%73
	.byte	%73,%69,%67,%6e,%6d,%65,%6e,%74
	.byte	%20,%6f,%70,%65,%72,%61,%74,%6f
	.byte	%72,%00
D19e1:
	.byte	%26,%20,%62,%65,%66,%6f,%72,%65
	.byte	%20,%61,%72,%72,%61,%79,%20,%6f
	.byte	%72,%20,%66,%75,%6e,%63,%74,%69
	.byte	%6f,%6e,%3a,%20,%69,%67,%6e,%6f
	.byte	%72,%65,%64,%00
D1a05:
	.byte	%25,%73,%0a,%00
D1a09:
	.byte	%2a,%3f,%3f,%3f,%2a,%0a,%00
D1a10:
	.byte	%25,%73,%0a,%00
D1a14:
	.byte	%2a,%3f,%3f,%3f,%2a,%0a,%00
D1a1b:
	.byte	%73,%74,%72,%75,%63,%74,%75,%72
	.byte	%65,%20,%72,%65,%66,%65,%72,%65
	.byte	%6e,%63,%65,%20,%6d,%75,%73,%74
	.byte	%20,%62,%65,%20,%61,%64,%64,%72
	.byte	%65,%73,%73,%61,%62,%6c,%65,%00
D1a43:
	.byte	%75,%6e,%64,%65,%63,%6c,%61,%72
	.byte	%65,%64,%20,%69,%6e,%69,%74,%69
	.byte	%61,%6c,%69,%7a,%65,%72,%20,%6e
	.byte	%61,%6d,%65,%20,%25,%73,%00
