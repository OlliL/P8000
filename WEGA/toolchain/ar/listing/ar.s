
fp		:= r15;
sp		:= r15;
_sccsid:
.byte	%40
.byte	%5b
.byte	%24
.byte	%5d
.byte	%61
.byte	%72
.byte	%2e
.byte	%63
.byte	%9
.byte	%32
.byte	%2e
.byte	%32
.byte	%20
.byte	%20
.byte	%31
.byte	%31
.byte	%2f
.byte	%31
.byte	%38
.byte	%2f
.byte	%38
.byte	%33
.byte	%20
.byte	%31
.byte	%32
.byte	%3a
.byte	%34
.byte	%38
.byte	%3a
.byte	%30
.byte	%30
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
.comm	30,	_stbuf;
.comm	26,	_arbuf;
.even
_man::
.addr	L23
L23:
.byte	%6d,%72,%78,%74,%64,%70,%71,%0
.even
.even
_opt::
.addr	L25
L25:
.byte	%63,%75,%76,%6e,%62,%61,%69,%6c
.byte	%0
.even
.even
_signum::
.word	%1
.word	%2
.word	%3
.word	%0
.even
_tmp0::
.addr	L28
L28:
.byte	%2f,%74,%6d,%70,%2f,%76,%58,%58
.byte	%58,%58,%58,%0
.even
.even
_tmp1::
.addr	L30
L30:
.byte	%2f,%74,%6d,%70,%2f,%76,%31,%58
.byte	%58,%58,%58,%58,%0
.even
.even
_tmp2::
.addr	L32
L32:
.byte	%2f,%74,%6d,%70,%2f,%76,%32,%58
.byte	%58,%58,%58,%58,%0
.even
.comm	2,	_comfun;
.comm	26,	_flg;
.comm	2,	_namv;
.comm	2,	_namc;
.comm	2,	_arnam;
.comm	2,	_ponam;
.comm	2,	_tfnam;
.comm	2,	_tf1nam;
.comm	2,	_tf2nam;
.comm	2,	_file;
.comm	16,	_name;
.comm	2,	_af;
.comm	2,	_tf;
.comm	2,	_tf1;
.comm	2,	_tf2;
.comm	2,	_qf;
.comm	2,	_bastate;
.comm	512,	_buf;
.comm	2,	_av0;
L94:
.byte	%25,%73,%3a,%20,%62,%61,%64,%20
.byte	%6f,%70,%74,%69,%6f,%6e,%20,%60
.byte	%25,%63,%27,%a,%0
.even
.even
L98:
.byte	%76,%58,%58,%58,%58,%58,%0
.even
L99:
.byte	%76,%31,%58,%58,%58,%58,%58,%0
.even
L100:
.byte	%76,%32,%58,%58,%58,%58,%58,%0
.even
L106:
.byte	%25,%73,%3a,%20,%6f,%6e,%65,%20
.byte	%6f,%66,%20,%5b,%25,%73,%5d,%20
.byte	%6d,%75,%73,%74,%20,%62,%65,%20
.byte	%73,%70,%65,%63,%69,%66,%69,%65
.byte	%64,%a,%0
.even
/*~~main:*/

.code

_main::
{
	.data
	.even
L96:
	.word	L83
	.word	L83
	.word	L83
	.word	L86
	.word	L92
	.word	L92
	.word	L92
	.word	L92
	.word	L83
	.word	L92
	.word	L92
	.word	L83
	.word	L90
	.word	L83
	.word	L92
	.word	L89
	.word	L91
	.word	L84
	.word	L92
	.word	L88
	.word	L83
	.word	L83
	.word	L92
	.word	L87
	.code
	dec	fp,#~L2
	ldm	6(fp),r13,#2
	ldl	@fp,rr6
	ldk	r14,#0
	jpr	L67
L20001:
	ld	r2,r14
	add	r2,r14
	ld	r7,_signum(r2)
	ldk	r6,#1
	callr	_signal
	cp	r2,#1
	jpr	eq,L65
	ld	r2,r14
	add	r2,r14
	ld	r7,_signum(r2)
	ld	r6,#_sigdone
	callr	_signal
L65:
	inc	r14,#1
L67:
	ld	r2,r14
	add	r2,r14
	test	_signum(r2)
	jpr	ne,L20001
	ld	r7,~L1(fp)
	ld	r6,@r7
	ld	r7,r6
	callr	_trim
	ld	_av0,r2
	cp	~L1+2(fp),#3
	jpr	ge,L69
	callr	_usage
L69:
	ld	r2,~L1(fp)
	ld	r13,2(r2)
L73:
	testb	@r13
	jpr	eq,L72
	ldb	rl3,@r13
	extsb	r3
	ld	r2,r3
	sub	r2,#97
	cp	r2,#23
	jpr	ugt,L92
	rl	r2,#1
	ld	r2,L96(r2)
	jp	@r2
L83:
	ldb	rl3,@r13
	extsb	r3
	sub	r3,#97
	add	r3,#_flg
	ldb	rl3,@r13
	extsb	r3
	sub	r3,#97
	incb	_flg(r3),#1
L71:
	inc	r13,#1
	jpr	L73
L84:
	ld	r7,#_rcmd
L20002:
	callr	_setcom
	jpr	L71
L86:
	ld	r7,#_dcmd
	jpr	L20002
L87:
	ld	r7,#_xcmd
	jpr	L20002
L88:
	ld	r7,#_tcmd
	jpr	L20002
L89:
	ld	r7,#_pcmd
	jpr	L20002
L90:
	ld	r7,#_mcmd
	jpr	L20002
L91:
	ld	r7,#_qcmd
	jpr	L20002
L92:
	ld	r7,#__iob+16
	ld	r6,#L94
	ld	r5,_av0
	ldb	rl4,@r13
	extsb	r4
	callr	_fprintf
	ldk	r7,#1
	callr	_done
	jpr	L71
L72:
	testb	_flg+11
	jpr	eq,L97
	ld	_tmp0,#L98
	ld	_tmp1,#L99
	ld	_tmp2,#L100
L97:
	testb	_flg+8
	jpr	eq,L101
	incb	_flg+1,#1
L101:
	testb	_flg
	jpr	ne,L10003
	testb	_flg+1
	jpr	eq,L102
L10003:
	ld	_bastate,#1
	ld	r7,~L1(fp)
	ld	r6,4(r7)
	ld	r7,r6
	callr	_trim
	ld	_ponam,r2
	inc	~L1(fp),#2
	dec	~L1+2(fp),#1
	cp	~L1+2(fp),#3
	jpr	ge,L102
	callr	_usage
L102:
	ld	r2,~L1(fp)
	ld	r3,4(r2)
	ld	_arnam,r3
	inc	r2,#6
	ld	_namv,r2
	ld	r2,~L1+2(fp)
	dec	r2,#3
	ld	_namc,r2
	test	_comfun
	jpr	ne,L104
	testb	_flg+20
	jpr	ne,L105
	ld	r7,#__iob+16
	ld	r6,#L106
	ld	r5,_av0
	ld	r4,_man
	callr	_fprintf
	ldk	r7,#1
	callr	_done
L105:
	ld	r7,#_rcmd
	callr	_setcom
L104:
	ld	r2,_comfun
	call	@r2
	callr	_notfound
	ld	~L1+4(fp),r2
	ld	r7,r2
	callr	_done
	ldm	r13,6(fp),#2
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 10
	}	/* _main */

.data

L110:
.byte	%25,%73,%3a,%20,%6f,%6e,%6c,%79
.byte	%20,%6f,%6e,%65,%20,%6f,%66,%20
.byte	%5b,%25,%73,%5d,%20,%61,%6c,%6c
.byte	%6f,%77,%65,%64,%a,%0
.even
/*~~setcom:*/

.code

_setcom::
{
	dec	fp,#~L2
	ld	@fp,r7
	test	_comfun
	jpr	eq,L109
	ld	r7,#__iob+16
	ld	r6,#L110
	ld	r5,_av0
	ld	r4,_man
	callr	_fprintf
	ldk	r7,#1
	callr	_done
L109:
	ld	r2,~L1(fp)
	ld	_comfun,r2
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 2
	}	/* _setcom */

.data

L123:
.byte	%25,%73,%3a,%20,%63,%61,%6e,%6e
.byte	%6f,%74,%20,%6f,%70,%65,%6e,%20
.byte	%25,%73,%a,%0
.even
/*~~rcmd:*/

.code

_rcmd::
{
	dec	fp,#~L2
	ld	2(fp),r14
	callr	_init
	callr	_getaf
	jpr	L115
L20004:
	test	_namc
	jpr	eq,L124
	ld	r7,#__iob+16
	ld	r6,#L123
	ld	r5,_av0
	ld	r4,_file
	callr	_fprintf
	jpr	L124
L20006:
	ldl	rr2,_stbuf+22
	cpl	rr2,_arbuf+14
	jpr	gt,L125
	ld	r7,r14
	callr	_close
L124:
	ld	r7,#99
	callr	_mesg
	ld	r7,_af
	ld	r6,_tf
	ldk	r5,#14
	callr	_copyfil
	jpr	L115
L20008:
	callr	_bamatch
	test	_namc
	jpr	eq,L10010
	callr	_match
	test	r2
	jpr	eq,L124
L10010:
	callr	_stats
	ld	r14,r2
	cp	r2,#0
	jpr	lt,L20004
	testb	_flg+20
	jpr	ne,L20006
L125:
	ld	r7,#114
	callr	_mesg
	ld	r7,_af
	ld	r6,#-1
	ldk	r5,#3
	callr	_copyfil
	ld	r7,r14
	callr	_movefil
L115:
	callr	_getdir
	test	r2
	jpr	eq,L20008
	callr	_cleanup
	ld	r14,2(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 4
	}	/* _rcmd */

.data

/*~~dcmd:*/

.code

_dcmd::
{
	callr	_init
	callr	_getaf
	test	r2
	jpr	eq,L135
	callr	_noar
	jpr	L135
L20010:
	ld	r7,#100
	callr	_mesg
	ld	r7,_af
	ld	r6,#-1
	ldk	r5,#3
L20011:
	callr	_copyfil
	jpr	L135
L20013:
	callr	_match
	test	r2
	jpr	ne,L20010
	ld	r7,#99
	callr	_mesg
	ld	r7,_af
	ld	r6,_tf
	ldk	r5,#14
	jpr	L20011
L135:
	callr	_getdir
	test	r2
	jpr	eq,L20013
	callr	_install
	ret
	~L1 := 0
	~L2 := 0
	}	/* _dcmd */

.data

L146:
.byte	%25,%73,%3a,%20,%25,%73,%20,%63
.byte	%61,%6e,%6e,%6f,%74,%20,%63,%72
.byte	%65,%61,%74,%65,%a,%0
.even
/*~~xcmd:*/

.code

_xcmd::
{
	dec	fp,#~L2
	ld	@fp,r14
	callr	_getaf
	test	r2
	jpr	eq,L141
	callr	_noar
	jpr	L141
L20015:
	ld	r7,#__iob+16
	ld	r6,#L146
	ld	r5,_av0
	ld	r4,_file
	callr	_fprintf
L147:
	ld	r7,#99
	callr	_mesg
	ld	r7,_af
	ld	r6,#-1
	ldk	r5,#3
	callr	_copyfil
	cp	_namc,#0
	jpr	le,L141
	callr	_morefil
	test	r2
	jpr	ne,L141
	sub	r7,r7
	callr	_done
	jpr	L141
L20017:
	test	_namc
	jpr	eq,L10017
	callr	_match
	test	r2
	jpr	eq,L147
L10017:
	ld	r6,_arbuf+20
	and	r6,#511
	ld	r7,_file
	callr	_creat
	ld	r14,r2
	cp	r2,#0
	jpr	lt,L20015
	ld	r7,#120
	callr	_mesg
	ld	r7,_af
	ld	r6,r14
	ldk	r5,#2
	callr	_copyfil
	ld	r7,r14
	callr	_close
L141:
	callr	_getdir
	test	r2
	jpr	eq,L20017
	ld	r14,@fp
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 2
	}	/* _xcmd */

.data

L157:
.byte	%a,%3c,%25,%73,%3e,%a,%a,%0
.even
/*~~pcmd:*/

.code

_pcmd::
{
	callr	_getaf
	test	r2
	jpr	eq,L152
	callr	_noar
	jpr	L152
L10021:
	testb	_flg+21
	jpr	eq,L155
	ld	r7,#L157
	ld	r6,_file
	callr	_printf
	ld	r7,#__iob+8
	callr	_fflush
L155:
	ld	r7,_af
	ldk	r6,#1
	ldk	r5,#2
L20019:
	callr	_copyfil
	jpr	L152
L20021:
	test	_namc
	jpr	eq,L10021
	callr	_match
	test	r2
	jpr	ne,L10021
	ld	r7,_af
	ld	r6,#-1
	ldk	r5,#3
	jpr	L20019
L152:
	callr	_getdir
	test	r2
	jpr	eq,L20021
	ret
	~L1 := 0
	~L2 := 0
	}	/* _pcmd */

.data

L163:
.byte	%25,%73,%3a,%20,%63,%61,%6e,%6e
.byte	%6f,%74,%20,%63,%72,%65,%61,%74
.byte	%65,%20,%74,%68,%69,%72,%64,%20
.byte	%74,%65,%6d,%70,%a,%0
.even
/*~~mcmd:*/

.code

_mcmd::
{
	dec	fp,#~L2
	callr	_init
	callr	_getaf
	test	r2
	jpr	eq,L160
	callr	_noar
L160:
	ld	r7,_tmp2
	callr	_mktemp
	ld	_tf2nam,r2
	ld	r7,r2
	ld	r6,#384
	callr	_creat
	ld	~L1(fp),r2
	ld	r7,r2
	callr	_close
	ld	r7,_tf2nam
	ldk	r6,#2
	callr	_open
	ld	_tf2,r2
	cp	r2,#0
	jpr	ge,L164
	ld	r7,#__iob+16
	ld	r6,#L163
	ld	r5,_av0
	callr	_fprintf
	ldk	r7,#1
	callr	_done
	jpr	L164
L20023:
	ld	r7,#109
	callr	_mesg
	ld	r7,_af
	ld	r6,_tf2
L20025:
	ldk	r5,#14
	callr	_copyfil
	jpr	L164
L20027:
	callr	_bamatch
	callr	_match
	test	r2
	jpr	ne,L20023
	ld	r7,#99
	callr	_mesg
	ld	r7,_af
	ld	r6,_tf
	jpr	L20025
L164:
	callr	_getdir
	test	r2
	jpr	eq,L20027
	callr	_install
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 2
	}	/* _mcmd */

.data

L174:
.byte	%25,%73,%a,%0
.even
/*~~tcmd:*/

.code

_tcmd::
{
	dec	fp,#~L2
	callr	_getaf
	test	r2
	jpr	eq,L169
	callr	_noar
	jpr	L169
L20029:
	test	_namc
	jpr	eq,L10028
	callr	_match
	test	r2
	jpr	eq,L171
L10028:
	testb	_flg+21
	jpr	eq,L172
	callr	_longt
L172:
	ld	r7,_file
	callr	_trim
	ld	~L1(fp),r2
	ld	r7,#L174
	ld	r6,r2
	callr	_printf
L171:
	ld	r7,_af
	ld	r6,#-1
	ldk	r5,#3
	callr	_copyfil
L169:
	callr	_getdir
	test	r2
	jpr	eq,L20029
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 2
	}	/* _tcmd */

.data

L177:
.byte	%25,%73,%3a,%20,%61,%62,%69,%20
.byte	%6e,%6f,%74,%20,%61,%6c,%6c,%6f
.byte	%77,%65,%64,%20,%77,%69,%74,%68
.byte	%20,%71,%a,%0
.even
L188:
.byte	%25,%73,%3a,%20,%25,%73,%20,%63
.byte	%61,%6e,%6e,%6f,%74,%20,%6f,%70
.byte	%65,%6e,%a,%0
.even
/*~~qcmd:*/

.code

_qcmd::
{
	dec	fp,#~L2
	ldm	@fp,r13,#2
	testb	_flg
	jpr	ne,L10032
	testb	_flg+1
	jpr	eq,L176
L10032:
	ld	r7,#__iob+16
	ld	r6,#L177
	ld	r5,_av0
	callr	_fprintf
	ldk	r7,#1
	callr	_done
L176:
	callr	_getqf
	ldk	r14,#0
	jpr	L181
L20031:
	ld	r2,r14
	add	r2,r14
	ld	r7,_signum(r2)
	ldk	r6,#1
	callr	_signal
	inc	r14,#1
L181:
	ld	r2,r14
	add	r2,r14
	test	_signum(r2)
	jpr	ne,L20031
	ld	r7,_qf
	subl	rr4,rr4
	ldk	r3,#2
	callr	_lseek
	ldk	r14,#0
	jpr	L185
L20033:
	ld	r2,r14
	add	r2,r14
	ld	r3,_namv
	add	r3,r2
	ld	r2,@r3
	ld	_file,r2
	test	r2
	jpr	eq,L183
	ld	r2,r14
	add	r2,r14
	ld	r3,_namv
	add	r3,r2
	clr	@r3
	ld	r7,#113
	callr	_mesg
	callr	_stats
	ld	r13,r2
	cp	r2,#0
	jpr	ge,L187
	ld	r7,#__iob+16
	ld	r6,#L188
	ld	r5,_av0
	ld	r4,_file
	callr	_fprintf
	jpr	L183
L187:
	ld	r2,_qf
	ld	_tf,r2
	ld	r7,r13
	callr	_movefil
	ld	r2,_tf
	ld	_qf,r2
L183:
	inc	r14,#1
L185:
	cp	r14,_namc
	jpr	lt,L20033
	ldm	r13,@fp,#2
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 4
	}	/* _qcmd */

.data

.even
L190:
.word	%ff65
L192:
.byte	%25,%73,%3a,%20,%63,%61,%6e,%6e
.byte	%6f,%74,%20,%63,%72,%65,%61,%74
.byte	%65,%20,%74,%65,%6d,%70,%20,%66
.byte	%69,%6c,%65,%a,%0
.even
/*~~init:*/

.code

_init::
{
	dec	fp,#~L2
	ld	r7,_tmp0
	callr	_mktemp
	ld	_tfnam,r2
	ld	r7,r2
	ld	r6,#384
	callr	_creat
	ld	~L1(fp),r2
	ld	r7,r2
	callr	_close
	ld	r7,_tfnam
	ldk	r6,#2
	callr	_open
	ld	_tf,r2
	cp	r2,#0
	jpr	ge,L191
	ld	r7,#__iob+16
	ld	r6,#L192
	ld	r5,_av0
	callr	_fprintf
	ldk	r7,#1
	callr	_done
L191:
	ld	r7,_tf
	ld	r6,#L190
	ldk	r5,#2
	callr	_write
	cp	r2,#2
	jpr	eq,L10033
	callr	_wrerr
L10033:
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 2
	}	/* _init */

.data

L200:
.byte	%25,%73,%3a,%20,%25,%73,%20,%6e
.byte	%6f,%74,%20,%69,%6e,%20,%61,%72
.byte	%63,%68,%69,%76,%65,%20,%66,%6f
.byte	%72,%6d,%61,%74,%a,%0
.even
/*~~getaf:*/

.code

_getaf::
{
	dec	fp,#~L2
	ld	r7,_arnam
	sub	r6,r6
	callr	_open
	ld	_af,r2
	cp	r2,#0
	jpr	ge,L197
	ldk	r2,#1
	jpr	L10036
L197:
	lda	r6,~L1(fp)
	ld	r7,_af
	ldk	r5,#2
	callr	_read
	cp	r2,#2
	jpr	ne,L10039
	cp	~L1(fp),#65381
	jpr	eq,L199
L10039:
	ld	r7,#__iob+16
	ld	r6,#L200
	ld	r5,_av0
	ld	r4,_arnam
	callr	_fprintf
	ldk	r7,#1
	callr	_done
L199:
	sub	r2,r2
L10036:
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 4
	}	/* _getaf */

.data

L204:
.byte	%25,%73,%3a,%20,%63,%72,%65,%61
.byte	%74,%69,%6e,%67,%20,%25,%73,%a
.byte	%0
.even
L206:
.byte	%25,%73,%3a,%20,%63,%61,%6e,%6e
.byte	%6f,%74,%20,%63,%72,%65,%61,%74
.byte	%65,%20,%25,%73,%a,%0
.even
L210:
.byte	%25,%73,%3a,%20,%25,%73,%20,%6e
.byte	%6f,%74,%20,%69,%6e,%20,%61,%72
.byte	%63,%68,%69,%76,%65,%20,%66,%6f
.byte	%72,%6d,%61,%74,%a,%0
.even
/*~~getqf:*/

.code

_getqf::
{
	dec	fp,#~L2
	ld	r7,_arnam
	ldk	r6,#2
	callr	_open
	ld	_qf,r2
	cp	r2,#0
	jpr	ge,L202
	testb	_flg+2
	jpr	ne,L203
	ld	r7,#__iob+16
	ld	r6,#L204
	ld	r5,_av0
	ld	r4,_arnam
	callr	_fprintf
L203:
	ld	r7,_arnam
	ld	r6,#438
	callr	_creat
	ld	~L1+2(fp),r2
	ld	r7,r2
	callr	_close
	ld	r7,_arnam
	ldk	r6,#2
	callr	_open
	ld	_qf,r2
	cp	r2,#0
	jpr	ge,L205
	ld	r7,#__iob+16
	ld	r6,#L206
	ld	r5,_av0
	ld	r4,_arnam
	callr	_fprintf
	ldk	r7,#1
	callr	_done
L205:
	ld	~L1(fp),#65381
	lda	r6,~L1(fp)
	ld	r7,_qf
	ldk	r5,#2
	callr	_write
	cp	r2,#2
	jpr	eq,L10040
	callr	_wrerr
	jpr	L10040
L202:
	lda	r6,~L1(fp)
	ld	r7,_qf
	ldk	r5,#2
	callr	_read
	cp	r2,#2
	jpr	ne,L10043
	cp	~L1(fp),#65381
	jpr	eq,L10040
L10043:
	ld	r7,#__iob+16
	ld	r6,#L210
	ld	r5,_av0
	ld	r4,_arnam
	callr	_fprintf
	ldk	r7,#1
	callr	_done
L10040:
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 4
	}	/* _getqf */

.data

L212:
.byte	%75,%73,%61,%67,%65,%3a,%20,%25
.byte	%73,%20,%5b,%25,%73,%5d,%5b,%25
.byte	%73,%5d,%20,%61,%72,%63,%68,%69
.byte	%76,%65,%20,%66,%69,%6c,%65,%73
.byte	%20,%2e,%2e,%2e,%a,%0
.even
/*~~usage:*/

.code

_usage::
{
	ld	r7,#L212
	ld	r6,_av0
	ld	r5,_opt
	ld	r4,_man
	callr	_printf
	ldk	r7,#1
	callr	_done
	ret
	~L1 := 0
	~L2 := 0
	}	/* _usage */

.data

L214:
.byte	%25,%73,%3a,%20,%25,%73,%20,%64
.byte	%6f,%65,%73,%20,%6e,%6f,%74,%20
.byte	%65,%78,%69,%73,%74,%a,%0
.even
/*~~noar:*/

.code

_noar::
{
	ld	r7,#__iob+16
	ld	r6,#L214
	ld	r5,_av0
	ld	r4,_arnam
	callr	_fprintf
	ldk	r7,#1
	callr	_done
	ret
	~L1 := 0
	~L2 := 0
	}	/* _noar */

.data

/*~~sigdone:*/

.code

_sigdone::
{
	ld	r7,#100
	callr	_done
	ret
	~L1 := 0
	~L2 := 0
	}	/* _sigdone */

.data

/*~~done:*/

.code

_done::
{
	dec	fp,#~L2
	ld	@fp,r7
	test	_tfnam
	jpr	eq,L217
	ld	r7,_tfnam
	callr	_unlink
L217:
	test	_tf1nam
	jpr	eq,L219
	ld	r7,_tf1nam
	callr	_unlink
L219:
	test	_tf2nam
	jpr	eq,L220
	ld	r7,_tf2nam
	callr	_unlink
L220:
	ld	r7,~L1(fp)
	callr	_exit
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 2
	}	/* _done */

.data

L227:
.byte	%25,%73,%3a,%20,%25,%73,%20,%6e
.byte	%6f,%74,%20,%66,%6f,%75,%6e,%64
.byte	%a,%0
.even
/*~~notfound:*/

.code

_notfound::
{
	dec	fp,#~L2
	ldm	4(fp),r13,#2
	ldk	r13,#0
	ld	r14,r13
	jpr	L225
L20035:
	ld	r2,r14
	add	r2,r14
	ld	r3,_namv
	add	r3,r2
	test	@r3
	jpr	eq,L223
	ld	r2,r14
	add	r2,r14
	ld	r4,_namv
	add	r4,r2
	ld	r5,@r4
	ld	r4,r5
	ld	r6,#L227
	ld	r5,_av0
	ld	r7,#__iob+16
	callr	_fprintf
	inc	r13,#1
L223:
	inc	r14,#1
L225:
	cp	r14,_namc
	jpr	lt,L20035
	ld	r2,r13
	ldm	r13,4(fp),#2
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 8
	}	/* _notfound */

.data

/*~~morefil:*/

.code

_morefil::
{
	dec	fp,#~L2
	ldk	r4,#0
	ld	r1,r4
	jpr	L231
L20037:
	ld	r2,r1
	add	r2,r1
	ld	r3,_namv
	add	r3,r2
	test	@r3
	jpr	eq,L229
	inc	r4,#1
L229:
	inc	r1,#1
L231:
	cp	r1,_namc
	jpr	lt,L20037
	ld	r2,r4
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 6
	}	/* _morefil */

.data

L239:
.byte	%25,%73,%3a,%20,%25,%73,%20,%63
.byte	%61,%6e,%6e,%6f,%74,%20,%6f,%70
.byte	%65,%6e,%a,%0
.even
/*~~cleanup:*/

.code

_cleanup::
{
	dec	fp,#~L2
	ldm	@fp,r13,#2
	ldk	r14,#0
	jpr	L236
L20039:
	ld	r2,r14
	add	r2,r14
	ld	r3,_namv
	add	r3,r2
	ld	r2,@r3
	ld	_file,r2
	test	r2
	jpr	eq,L234
	ld	r2,r14
	add	r2,r14
	ld	r3,_namv
	add	r3,r2
	clr	@r3
	ld	r7,#97
	callr	_mesg
	callr	_stats
	ld	r13,r2
	cp	r2,#0
	jpr	ge,L238
	ld	r7,#__iob+16
	ld	r6,#L239
	ld	r5,_av0
	ld	r4,_file
	callr	_fprintf
	jpr	L234
L238:
	ld	r7,r13
	callr	_movefil
L234:
	inc	r14,#1
L236:
	cp	r14,_namc
	jpr	lt,L20039
	callr	_install
	ldm	r13,@fp,#2
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 4
	}	/* _cleanup */

.data

L246:
.byte	%25,%73,%3a,%20,%63,%72,%65,%61
.byte	%74,%69,%6e,%67,%20,%25,%73,%a
.byte	%0
.even
L248:
.byte	%25,%73,%3a,%20,%63,%61,%6e,%6e
.byte	%6f,%74,%20,%63,%72,%65,%61,%74
.byte	%65,%20,%25,%73,%a,%0
.even
/*~~install:*/

.code

_install::
{
	dec	fp,#~L2
	ld	@fp,r14
	ldk	r14,#0
	jpr	L243
L20041:
	ld	r2,r14
	add	r2,r14
	ld	r7,_signum(r2)
	ldk	r6,#1
	callr	_signal
	inc	r14,#1
L243:
	ld	r2,r14
	add	r2,r14
	test	_signum(r2)
	jpr	ne,L20041
	cp	_af,#0
	jpr	ge,L244
	testb	_flg+2
	jpr	ne,L244
	ld	r7,#__iob+16
	ld	r6,#L246
	ld	r5,_av0
	ld	r4,_arnam
	callr	_fprintf
L244:
	ld	r7,_af
	callr	_close
	ld	r7,_arnam
	ld	r6,#438
	callr	_creat
	ld	_af,r2
	cp	r2,#0
	jpr	ge,L247
	ld	r7,#__iob+16
	ld	r6,#L248
	ld	r5,_av0
	ld	r4,_arnam
	callr	_fprintf
	ldk	r7,#1
	callr	_done
L247:
	test	_tfnam
	jpr	eq,L249
	ld	r7,_tf
	subl	rr4,rr4
	sub	r3,r3
	callr	_lseek
	jpr	L250
L20043:
	ld	r7,_af
	ld	r6,#_buf
	ld	r5,r14
	callr	_write
	cp	r14,r2
	jpr	eq,L250
	callr	_wrerr
L250:
	ld	r7,_tf
	ld	r6,#_buf
	ld	r5,#512
	callr	_read
	ld	r14,r2
	cp	r2,#0
	jpr	gt,L20043
L249:
	test	_tf2nam
	jpr	eq,L253
	ld	r7,_tf2
	subl	rr4,rr4
	sub	r3,r3
	callr	_lseek
	jpr	L254
L20045:
	ld	r7,_af
	ld	r6,#_buf
	ld	r5,r14
	callr	_write
	cp	r14,r2
	jpr	eq,L254
	callr	_wrerr
L254:
	ld	r7,_tf2
	ld	r6,#_buf
	ld	r5,#512
	callr	_read
	ld	r14,r2
	cp	r2,#0
	jpr	gt,L20045
L253:
	test	_tf1nam
	jpr	eq,L10065
	ld	r7,_tf1
	subl	rr4,rr4
	sub	r3,r3
	callr	_lseek
	jpr	L258
L20047:
	ld	r7,_af
	ld	r6,#_buf
	ld	r5,r14
	callr	_write
	cp	r14,r2
	jpr	eq,L258
	callr	_wrerr
L258:
	ld	r7,_tf1
	ld	r6,#_buf
	ld	r5,#512
	callr	_read
	ld	r14,r2
	cp	r2,#0
	jpr	gt,L20047
L10065:
	ld	r14,@fp
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 2
	}	/* _install */

.data

/*~~movefil:*/

.code

_movefil::
{
	dec	fp,#~L2
	ldm	2(fp),r13,#2
	ld	@fp,r7
	ld	r7,_file
	callr	_trim
	ld	r14,r2
	ldk	r13,#0
L20049:
	ldb	rl2,@r14
	ldb	_arbuf(r13),rl2
	testb	rl2
	jpr	eq,L262
	inc	r14,#1
L262:
	inc	r13,#1
	cp	r13,#13
	jpr	lt,L20049
	cp	r13,#13
	jpr	ne,L266
	clrb	_arbuf(r13)
L266:
	ldl	rr2,_stbuf+14
	ldl	_arbuf+22,rr2
	ldl	rr4,_stbuf+22
	ldl	_arbuf+14,rr4
	ldb	rl2,_stbuf+9
	ldb	_arbuf+18,rl2
	ldb	rl3,_stbuf+11
	ldb	_arbuf+19,rl3
	ld	r2,_stbuf+4
	ld	_arbuf+20,r2
	ld	r7,~L1(fp)
	ld	r6,_tf
	ldk	r5,#12
	callr	_copyfil
	ld	r7,~L1(fp)
	callr	_close
	ldm	r13,2(fp),#2
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 6
	}	/* _movefil */

.data

/*~~stats:*/

.code

_stats::
{
	dec	fp,#~L2
	ld	2(fp),r14
	ld	r7,_file
	sub	r6,r6
	callr	_open
	ld	r14,r2
	cp	r2,#0
	jpr	lt,L270
	ld	r7,r2
	ld	r6,#_stbuf
	callr	_fstat
	cp	r2,#0
	jpr	ge,L270
	ld	r7,r14
	callr	_close
	ld	r2,#-1
	jpr	L10071
L270:
	ld	r2,r14
L10071:
	ld	r14,2(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 4
	}	/* _stats */

.data

/*~~copyfil:*/

.code

_copyfil::
{
	dec	fp,#~L2
	ldm	8(fp),r13,#2
	ldm	@fp,r5,#3
	bit	r5,#3
	jpr	eq,L272
	ld	r7,~L1+2(fp)
	ld	r6,#_arbuf
	ld	r5,#26
	callr	_write
	cp	r2,#26
	jpr	eq,L272
	callr	_wrerr
L272:
	clr	~L1+6(fp)
	jpr	L274
L20051:
	ld	r13,#512
	ld	r14,r13
	ld	r5,r13
	exts	rr4
	cpl	rr4,_arbuf+22
	jpr	le,L276
	ld	r13,_arbuf+24
	ld	r14,r13
	bit	r13,#0
	jpr	eq,L276
	bit	~L1(fp),#1
	jpr	eq,L278
	inc	r14,#1
L278:
	bit	~L1(fp),#2
	jpr	eq,L276
	inc	r13,#1
L276:
	ld	r7,~L1+4(fp)
	ld	r6,#_buf
	ld	r5,r14
	callr	_read
	cp	r14,r2
	jpr	eq,L280
	inc	~L1+6(fp),#1
L280:
	ld	r2,~L1(fp)
	and	r2,#1
	test	r2
	jpr	ne,L281
	ld	r7,~L1+2(fp)
	ld	r6,#_buf
	ld	r5,r13
	callr	_write
	cp	r13,r2
	jpr	eq,L281
	callr	_wrerr
L281:
	ldl	rr2,_arbuf+22
	subl	rr2,#512
	ldl	_arbuf+22,rr2
L274:
	ldl	rr2,_arbuf+22
	cpl	rr2,#0
	jpr	gt,L20051
	test	~L1+6(fp)
	jpr	eq,L10074
	callr	_phserr
L10074:
	ldm	r13,8(fp),#2
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 12
	}	/* _copyfil */

.data

/*~~getdir:*/

.code

_getdir::
{
	dec	fp,#~L2
	ld	2(fp),r14
	ld	r7,_af
	ld	r6,#_arbuf
	ld	r5,#26
	callr	_read
	ld	r14,r2
	cp	r2,#26
	jpr	eq,L286
	test	_tf1nam
	jpr	eq,L287
	ld	r14,_tf
	ld	r2,_tf1
	ld	_tf,r2
	ld	_tf1,r14
L287:
	ldk	r2,#1
	jpr	L10077
L286:
	ldk	r14,#0
L20053:
	ldb	rl2,_arbuf(r14)
	ldb	_name(r14),rl2
	inc	r14,#1
	cp	r14,#14
	jpr	lt,L20053
	ld	_file,#_name
	sub	r2,r2
L10077:
	ld	r14,2(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 4
	}	/* _getdir */

.data

/*~~match:*/

.code

_match::
{
	dec	fp,#~L2
	ld	2(fp),r14
	ldk	r14,#0
	jpr	L294
L20055:
	ld	r2,r14
	add	r2,r14
	ld	r3,_namv
	add	r3,r2
	test	@r3
	jpr	eq,L292
	ld	r2,r14
	add	r2,r14
	ld	r7,_namv
	add	r7,r2
	ld	r6,@r7
	ld	r7,r6
	callr	_trim
	ld	~L1(fp),r2
	ld	r7,r2
	ld	r6,_file
	callr	_strcmp
	test	r2
	jpr	ne,L292
	ld	r2,r14
	add	r2,r14
	ld	r3,_namv
	add	r3,r2
	ld	r2,@r3
	ld	_file,r2
	ld	r3,r14
	add	r3,r14
	ld	r4,_namv
	add	r4,r3
	clr	@r4
	ldk	r2,#1
	jpr	L10080
L292:
	inc	r14,#1
L294:
	cp	r14,_namc
	jpr	lt,L20055
	sub	r2,r2
L10080:
	ld	r14,2(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 4
	}	/* _match */

.data

L306:
.byte	%25,%73,%3a,%20,%63,%61,%6e,%6e
.byte	%6f,%74,%20,%63,%72,%65,%61,%74
.byte	%65,%20,%73,%65,%63,%6f,%6e,%64
.byte	%20,%74,%65,%6d,%70,%a,%0
.even
/*~~bamatch:*/

.code

_bamatch::
{
	dec	fp,#~L2
	ld	2(fp),r14
	ld	r2,_bastate
	jpr	L300
L301:
	ld	r7,_file
	ld	r6,_ponam
	callr	_strcmp
	test	r2
	jpr	ne,L10083
	ld	_bastate,#2
	testb	_flg
	jpr	ne,L10083
L304:
	clr	_bastate
	ld	r7,_tmp1
	callr	_mktemp
	ld	_tf1nam,r2
	ld	r7,r2
	ld	r6,#384
	callr	_creat
	ld	~L1(fp),r2
	ld	r7,r2
	callr	_close
	ld	r7,_tf1nam
	ldk	r6,#2
	callr	_open
	ld	r14,r2
	cp	r2,#0
	jpr	ge,L305
	ld	r7,#__iob+16
	ld	r6,#L306
	ld	r5,_av0
	callr	_fprintf
	jpr	L10083
L305:
	ld	r2,_tf
	ld	_tf1,r2
	ld	_tf,r14
	jpr	L10083
L300:
	cp	r2,#1
	jpr	eq,L301
	cp	r2,#2
	jpr	eq,L304
L10083:
	ld	r14,2(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 4
	}	/* _bamatch */

.data

L308:
.byte	%25,%73,%3a,%20,%70,%68,%61,%73
.byte	%65,%20,%65,%72,%72,%6f,%72,%20
.byte	%6f,%6e,%20,%25,%73,%a,%0
.even
/*~~phserr:*/

.code

_phserr::
{
	ld	r7,#__iob+16
	ld	r6,#L308
	ld	r5,_av0
	ld	r4,_file
	callr	_fprintf
	ret
	~L1 := 0
	~L2 := 0
	}	/* _phserr */

.data

L312:
.byte	%25,%63,%20,%2d,%20,%25,%73,%a
.byte	%0
.even
/*~~mesg:*/

.code

_mesg::
{
	dec	fp,#~L2
	ld	@fp,r7
	testb	_flg+21
	jpr	eq,L10089
	cp	r7,#99
	jpr	ne,L10092
	cpb	_flg+21,#1
	jpr	le,L10089
L10092:
	ld	r7,#L312
	ld	r6,~L1(fp)
	ld	r5,_file
	callr	_printf
L10089:
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 2
	}	/* _mesg */

.data

/*~~trim:*/

.code

_trim::
{
	dec	fp,#~L2
	ld	@fp,r7
	ld	r1,r7
	jpr	L316
L20057:
	inc	r1,#1
L316:
	testb	@r1
	jpr	ne,L20057
	jpr	L317
L20059:
	dec	r1,#1
	ldb	rl2,@r1
	cpb	rl2,#47
	jpr	ne,L318
	clrb	@r1
L317:
	cp	r1,~L1(fp)
	jpr	ugt,L20059
L318:
	ld	r3,~L1(fp)
	ld	r1,r3
	jpr	L322
L20061:
	cpb	@r1,#47
	jpr	ne,L320
	lda	r2,1(r1)
	ld	r3,r2
L320:
	inc	r1,#1
L322:
	testb	@r1
	jpr	ne,L20061
	ld	r2,r3
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 8
	}	/* _trim */

.data

L326:
.byte	%25,%33,%64,%2f,%25,%31,%64,%0
.even
L327:
.byte	%25,%37,%6c,%64,%0
.even
L328:
.byte	%20,%25,%2d,%31,%32,%2e,%31,%32
.byte	%73,%20,%25,%2d,%34,%2e,%34,%73
.byte	%20,%0
.even
/*~~longt:*/

.code

_longt::
{
	dec	fp,#~L2
	ld	2(fp),r14
	callr	_pmode
	ld	r7,#L326
	ldb	rl6,_arbuf+18
	extsb	r6
	ldb	rl5,_arbuf+19
	extsb	r5
	callr	_printf
	ld	r7,#L327
	ldl	rr4,_arbuf+22
	callr	_printf
	ld	r7,#_arbuf+14
	callr	_ctime
	ld	r14,r2
	lda	r6,4(r14)
	lda	r5,20(r14)
	ld	r7,#L328
	callr	_printf
	ld	r14,2(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 4
	}	/* _longt */

.data

.even
_m1::
.word	%1
.word	%100
.word	%72
.word	%2d
.even
_m2::
.word	%1
.word	%80
.word	%77
.word	%2d
.even
_m3::
.word	%2
.word	%800
.word	%73
.word	%40
.word	%78
.word	%2d
.even
_m4::
.word	%1
.word	%20
.word	%72
.word	%2d
.even
_m5::
.word	%1
.word	%10
.word	%77
.word	%2d
.even
_m6::
.word	%2
.word	%400
.word	%73
.word	%8
.word	%78
.word	%2d
.even
_m7::
.word	%1
.word	%4
.word	%72
.word	%2d
.even
_m8::
.word	%1
.word	%2
.word	%77
.word	%2d
.even
_m9::
.word	%2
.word	%200
.word	%74
.word	%1
.word	%78
.word	%2d
.even
_m::
.addr	_m1
.addr	_m2
.addr	_m3
.addr	_m4
.addr	_m5
.addr	_m6
.addr	_m7
.addr	_m8
.addr	_m9
/*~~pmode:*/

.code

_pmode::
{
	dec	fp,#~L2
	ld	@fp,r14
	ld	r14,#_m
	jpr	L342
L20063:
	ld	r7,r14
	inc	r14,#2
	ld	r6,@r7
	ld	r7,r6
	callr	_select
L342:
	cp	r14,#_m+18
	jpr	ult,L20063
	ld	r14,@fp
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 2
	}	/* _pmode */

.data

/*~~select:*/

.code

_select::
{
	dec	fp,#~L2
	ldm	2(fp),r13,#2
	ld	@fp,r7
	ld	r13,r7
	ld	r14,@r13
	jpr	L20064
L20068:
	ld	r7,@r13
	ld	r6,#__iob+8
	callr	__flsbuf
	jpr	L10102
L20066:
	ld	r2,r13
	inc	r13,#2
	ld	r3,@r2
	and	r3,_arbuf+20
	test	r3
	jpr	ne,L346
L20064:
	inc	r13,#2
	dec	r14,#1
	cp	r14,#0
	jpr	ge,L20066
L346:
	dec	__iob+10,#1
	cp	__iob+10,#0
	jpr	lt,L20068
	ld	r2,__iob+8
	inc	__iob+8,#1
	ldb	rl3,1(r13)
	ldb	@r2,rl3
	subb	rh3,rh3
	ld	r2,r3
L10102:
	ldm	r13,2(fp),#2
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 6
	}	/* _select */

.data

L350:
.byte	%61,%72,%20,%77,%72,%69,%74,%65
.byte	%20,%65,%72,%72,%6f,%72,%0
.even
/*~~wrerr:*/

.code

_wrerr::
{
	ld	r7,#L350
	callr	_perror
	ldk	r7,#1
	callr	_done
	ret
	~L1 := 0
	~L2 := 0
	}	/* _wrerr */

.data

