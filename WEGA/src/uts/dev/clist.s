/**************************************************************************
***************************************************************************
 
	W E G A - Quelle
	KERN 3.2	Modul : clist.s
 
	Bearbeiter	: O. Lehmann
	Datum		: 02.05.08
	Version		: 1.0
 
***************************************************************************
**************************************************************************/

.seg
.psec data
.data


fp              := r15;
sp              := rr14;
_clistwstr::
.byte   %40
.byte   %5b
.byte   %24
.byte   %5d
.byte   %63
.byte   %6c
.byte   %69
.byte   %73
.byte   %74
.byte   %2e
.byte   %73
.byte   %9
.byte   %52
.byte   %65
.byte   %76
.byte   %20
.byte   %3a
.byte   %20
.byte   %34
.byte   %2e
.byte   %31
.byte   %9
.byte   %30
.byte   %39
.byte   %2f
.byte   %31
.byte   %36
.byte   %2f
.byte   %38
.byte   %33
.byte   %20
.byte   %30
.byte   %33
.byte   %3a
.byte   %34
.byte   %33
.byte   %3a
.byte   %30
.byte   %39
.byte   %0

.psec
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
	subb	rh7,rh0				// FIXME
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
	subb	rh7,rh1				// FIXME
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

	pop	r1,@rr14
	ldctl	fcw,r1
	ret
	}
.psec data
.data
