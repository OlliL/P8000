
fp		:= r15;
sp		:= r15;
_sccsid:
.byte	%40
.byte	%5b
.byte	%24
.byte	%5d
.byte	%73
.byte	%69
.byte	%7a
.byte	%65
.byte	%2e
.byte	%63
.byte	%9
.byte	%32
.byte	%2e
.byte	%31
.byte	%20
.byte	%20
.byte	%30
.byte	%31
.byte	%2f
.byte	%31
.byte	%34
.byte	%2f
.byte	%38
.byte	%33
.byte	%20
.byte	%32
.byte	%30
.byte	%3a
.byte	%32
.byte	%39
.byte	%3a
.byte	%30
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
.comm	4096,	_segt;
L24:
.byte	%61,%2e,%6f,%75,%74,%0
.even
L27:
.byte	%72,%0
.even
L30:
.byte	%73,%69,%7a,%65,%3a,%20,%63,%61
.byte	%6e,%6e,%6f,%74,%20,%6f,%70,%65
.byte	%6e,%20,%25,%73,%a,%0
.even
L34:
.byte	%73,%69,%7a,%65,%3a,%20,%25,%73
.byte	%20,%6e,%6f,%74,%20,%61,%6e,%20
.byte	%6f,%62,%6a,%65,%63,%74,%20,%66
.byte	%69,%6c,%65,%a,%0
.even
L41:
.byte	%25,%73,%3a,%a,%0
.even
L45:
.byte	%25,%55,%20,%2b,%20,%25,%55,%20
.byte	%2b,%20,%25,%55,%20,%3d,%20,%0
.even
L46:
.byte	%25,%55,%20,%3d,%20,%30,%78,%25
.byte	%6c,%78,%a,%0
.even
/*~~main:*/

.code

_main::
{
	sub	fp,#~L2
	ldl	~L1+0(fp),rr6
	cp	r7,#1
	jpr	ne,L23
	ld	r2,r6
	ld	@r2,#L24
	inc	~L1+2(fp),#1
	dec	~L1(fp),#2
L23:
	ld	r2,~L1+2(fp)
	ld	~L1+46(fp),r2
	jpr	L25
L20001:
	ld	r5,~L1(fp)
	ld	r4,@r5
	ld	r5,r4
	ld	r6,#L30
	ld	r7,#__iob+16
	callr	_fprintf
	jpr	L25
L20005:
	inc	~L1(fp),#2
	subl	rr2,rr2
	ldl	~L1+28(fp),rr2
	subl	rr2,rr2
	ldl	~L1+32(fp),rr2
	subl	rr2,rr2
	ldl	~L1+36(fp),rr2
	ld	r7,~L1(fp)
	ld	r6,@r7
	ld	r7,r6
	ld	r6,#L27
	callr	_fopen
	ld	~L1+50(fp),r2
	test	r2
	jpr	eq,L20001
	lda	r7,~L1+4(fp)
	ld	r6,#24
	ldk	r5,#1
	ld	r4,r2
	callr	_fread
	ld	r7,~L1+4(fp)
	callr	_goodmagic
	test	r2
	jpr	ne,L33
	ld	r5,~L1(fp)
	ld	r4,@r5
	ld	r5,r4
	ld	r6,#L34
	ld	r7,#__iob+16
	callr	_fprintf
	jpr	L20007
L33:
	test	_swap_flg
	jpr	eq,L36
	lda	r7,~L1+4(fp)
	lda	r6,~L1+4(fp)
	ld	r5,#24
	callr	_swab
L36:
	ld	r2,~L1+14(fp)
	srl	r2,#4
	ld	~L1+44(fp),r2
	ld	r6,r2
	sla	r6,#4
	ld	r7,#_segt
	ldk	r5,#1
	ld	r4,~L1+50(fp)
	callr	_fread
	test	_swap_flg
	jpr	eq,L38
	ld	r5,~L1+44(fp)
	sla	r5,#4
	ld	r6,#_segt
	ld	r7,r6
	callr	_swab
L38:
	cp	~L1+46(fp),#2
	jpr	le,L39
	ld	r6,~L1(fp)
	ld	r7,@r6
	ld	r6,r7
	ld	r7,#L41
	callr	_printf
L39:
	clr	~L1+48(fp)
	jpr	L44
L20003:
	ld	r2,~L1+48(fp)
	sla	r2,#4
	ld	r3,_segt+4(r2)
	ldk	r4,#0
	ld	r5,r3
	ld	r2,~L1+48(fp)
	sla	r2,#4
	ld	r3,_segt+6(r2)
	ld	r6,r4
	ld	r7,r3
	addl	rr4,rr6
	ld	r2,~L1+48(fp)
	sla	r2,#4
	ld	r3,_segt+8(r2)
	ld	r7,r3
	addl	rr4,rr6
	ldl	~L1+40(fp),rr4
	ld	r2,~L1+48(fp)
	sla	r2,#4
	ld	r3,_segt+4(r2)
	ld	r4,r6
	ld	r5,r3
	addl	rr4,~L1+28(fp)
	ldl	~L1+28(fp),rr4
	ld	r2,~L1+48(fp)
	sla	r2,#4
	ld	r3,_segt+6(r2)
	ldk	r4,#0
	ld	r5,r3
	addl	rr4,~L1+32(fp)
	ldl	~L1+32(fp),rr4
	ld	r2,~L1+48(fp)
	sla	r2,#4
	ld	r3,_segt+8(r2)
	ldk	r4,#0
	ld	r5,r3
	addl	rr4,~L1+36(fp)
	ldl	~L1+36(fp),rr4
	inc	~L1+48(fp),#1
L44:
	ld	r2,~L1+44(fp)
	dec	r2,#1
	cp	r2,~L1+48(fp)
	jpr	uge,L20003
	ldl	rr2,~L1+36(fp)
	ldl	@fp,rr2
	ld	r7,#L45
	ldl	rr4,~L1+28(fp)
	ldl	rr2,~L1+32(fp)
	callr	_printf
	ldl	rr2,~L1+28(fp)
	addl	rr2,~L1+32(fp)
	addl	rr2,~L1+36(fp)
	ldl	~L1+40(fp),rr2
	ld	r7,#L46
	ldl	rr4,rr2
	callr	_printf
L20007:
	ld	r7,~L1+50(fp)
	callr	_fclose
L25:
	dec	~L1+2(fp),#1
	jpr	ne,L20005
	add	fp,#~L2
	ret
	~L1 := 4
	~L2 := 56
	}	/* _main */

.data

