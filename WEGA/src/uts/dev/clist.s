.seg

.data

.byte	"@[$]clist.s	Rev : 4.1	09/16/83 03:43:09"
.byte	%25
.byte	%30
.byte	%30

.code

_putcf::
{
	ldctl	r1,fcw
	di	vi,nvi
	ldl	rr4,_cfreelist
	ldl	@rr6,rr4
	ldl	_cfreelist,rr6
	ldctl	fcw,r1
	ret
	}	/* _putcf */

_getcf::
{
	ldctl	r1,fcw
	di	vi,nvi
	ldl	rr2,_cfreelist
	testl	rr2
	jr	z,~egetcf

	ldl	rr4,@rr2
	ldl	_cfreelist,rr4

	subl	rr6,rr6
	ldl	@rr2,rr6
	ld	r0,_cfreelist+4
	ld	rr2(#4),r0
~egetcf:
	ldctl	fcw,r1
	ret
	}

_getcb::
{
	ldctl	r1,fcw
	di	vi,nvi
	ldl	rr2,rr6(#2)
	testl	rr2
	jr	z,~egetcb

	ld	r0,rr2(#4)
	subb	rl0,rh0
	subb	rh0,rh0
	ld	r4,@rr6
	sub	r4,r0
	ld	@rr6,r4
	ldl	rr4,@rr2
	ldl	rr6(#2),rr4
	testl	rr4
	jpr	ne,~egetcb
	subl	rr4,rr4
	ldl	rr6(#6),rr4
~egetcb:
	ldctl	fcw,r1
	ret
	}

_putcb::
{
	ldctl	r1,fcw
	push	@r14,r1
	di	vi,nvi
	ldl	rr2,rr4(#6)
	testl	rr2
	jpr	ne,~else_putcb
	ldl	rr4(#2),rr6
	ldl	rr4(#6),rr6
	jpr	~endif_putcb
~else_putcb:
	ldl	rr4(#6),rr6
	ldl	@rr2,rr6
~endif_putcb:
	subl	rr0,rr0
	ldl	@rr6,rr0
	ld	r1,rr6(#4)
	subb	rl1,rh1
	subb	rh1,rh1
	ld	r0,@rr4
	add	r0,r1
	ld	@rr4,r0
	pop	r1,@rr14
	ldctl	fcw,r1
	ret
	}

_getc::
{
	ldctl	r1,fcw
	push	@r14,r1
	di	vi,nvi
	dec	@rr6,#1
	jpr	lt,~else_getc
	ldl	rr2,rr6(#2)
	lda	rr4,rr2(#4)
	ldb	rl1,@rr4
	incb	@rr4,#1
	subb	rh1,rh1
	ldb	rl0,rr2(#5)
	cpb	rl0,@rr4
	jr	eq,~endif_getc
	inc	r5,#2
	add	r5,r1
	ldb	rl2,@rr4
	subb	rh2,rh2
~egetc:
	pop	r1,@rr14
	ldctl	fcw,r1
	ret
~else_getc:
	inc	@rr6,#1
	ld	r2,#-1
	jpr	~egetc
~endif_getc:
	inc	r5,#2
	add	r5,r1
	ldb	rl0,@rr4
	ldl	rr4,@rr2
	ldl	rr6(#2),rr4
	testl	rr4
	jpr	ne,~else2_getc
	ldl	rr6(#6),rr4
~else2_getc:
	ldl	rr4,_cfreelist	//get link to first free cblock
	ldl	@rr2,rr4	//put it into link of new free cblock
	ldl	_cfreelist,rr2	//put address of new free cblock into cfreelist
	ldb	rl2,rl0
	subb	rh2,rh2
	jpr	~egetc
	}

_putc::
{
	ldctl	r6,fcw
	di	vi,nvi
	ldl	rr2,rr4(#6)
	testl	rr2
	jpr	eq,~else_putc
~L0:
	ldb	rl0,rr2(#5)
	subb	rh0,rh0
	cp	r0,_cfreelist+4
	jpr	ge,~endif2_putc
	inc	@rr4,#1
	lda	rr4,rr2(#6)
	add	r5,r0
	ldb	@rr4,rl7
	incb	rl0,#1
	ldb	rr2(#5),rl0
	clr	r2
~eputc:
	ldctl	fcw,r6
	ret
~else_putc:
	ldl	rr2,_cfreelist
	testl	rr2
	jpr	eq,~endif_putc
	ldl	rr4(#2),rr2
~L1:
	ldl	rr0,@rr2
	ldl	_cfreelist,rr0
	clr	@rr2
	subl	rr0,rr0
	ldl	rr2(#2),rr0
	ldl	rr4(#6),rr2
	jpr	~L0
~endif_putc:
	ld	r2,#-1
	jpr	~eputc
~endif2_putc:
	ldl	rr0,_cfreelist
	testl	rr0
	jpr	eq,~endif_putc
	ldl	@rr2,rr0
	ldl	rr2,rr0
	jpr	~L1
	
	}
