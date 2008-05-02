//*************************************************************************
//*************************************************************************
// 
//	W E G A - Quelle
//	KERN 3.2	Modul : clist.s
// 
//	Bearbeiter	: O. Lehmann
//	Datum		: 02.05.08
//	Version		: 1.0
//
//*************************************************************************
//*************************************************************************

.seg

.extern _cfreelist

.data

.byte	"@[$]clist.s	Rev : 4.1	09/16/83 03:43:09"
.byte	%25
.byte	%30
.byte	%30

.code

_putcf::
{
	ldctl	r1,fcw		//save processor status
	di	vi,nvi		//dvi
	ldl	rr4,_cfreelist
	ldl	@rr6,rr4	//cbptr->c_next = cfreelist.c_next
	ldl	_cfreelist,rr6	//cfreelist.c_next = cbptr
	ldctl	fcw,r1		//restore processor status
	ret
	}	/* _putcf */

_getcf::
{
	ldctl	r1,fcw		//save processor status
	di	vi,nvi		//dvi
	ldl	rr2,_cfreelist	//rr2 = cfreelist
	testl	rr2
	jr	z,~end		//if its 0, then just return.

	ldl	rr4,@rr2
	ldl	_cfreelist,rr4	//cfreelist.c_next = cfreelist.c_next->c_next

	subl	rr6,rr6		
	ldl	@rr2,rr6	//clear c_next in cblock
	ld	r0,_cfreelist+4
	ld	rr2(#4),r0
~end:
	ldctl	fcw,r1		//restore processor status
	ret
	}

_getcb::
{
	ldctl	r1,fcw		//save processor status
	di	vi,nvi		//dvi
	ldl	rr2,rr6(#2)	//rr2 = ptr->c_cf
	testl	rr2
	jr	z,~end		//if its 0, then just return.

	ld	r0,rr2(#4)	
	subb	rl0,rh0
	subb	rh0,rh0
	ld	r4,@rr6
	sub	r4,r0
	ld	@rr6,r4
	ldl	rr4,@rr2
	ldl	rr6(#2),rr4
	testl	rr4
	jpr	ne,~end
	subl	rr4,rr4
	ldl	rr6(#6),rr4
~end:
	ldctl	fcw,r1		//restore processor status
	ret
	}

_putcb::
{
	ldctl	r1,fcw		//save processor status
	push	@r14,r1
	di	vi,nvi		//dvi
	ldl	rr2,rr4(#6)
	testl	rr2
	jpr	ne,L1
	ldl	rr4(#2),rr6
	ldl	rr4(#6),rr6
	jpr	L2
L1:
	ldl	rr4(#6),rr6
	ldl	@rr2,rr6
L2:
	subl	rr0,rr0
	ldl	@rr6,rr0
	ld	r1,rr6(#4)
	subb	rl1,rh1
	subb	rh1,rh1
	ld	r0,@rr4
	add	r0,r1
	ld	@rr4,r0
	pop	r1,@rr14
	ldctl	fcw,r1		//restore processor status
	ret
	}

_getc::
{
	ldctl	r1,fcw		//save processor status
	push	@r14,r1
	di	vi,nvi		//dvi
	dec	@rr6,#1
	jpr	lt,L3
	ldl	rr2,rr6(#2)
	lda	rr4,rr2(#4)
	ldb	rl1,@rr4
	incb	@rr4,#1
	subb	rh1,rh1
	ldb	rl0,rr2(#5)
	cpb	rl0,@rr4
	jr	eq,L4
	inc	r5,#2
	add	r5,r1
	ldb	rl2,@rr4
	subb	rh2,rh2
~end:
	pop	r1,@rr14
	ldctl	fcw,r1		//restore processor status
	ret
L3:
	inc	@rr6,#1
	ld	r2,#-1
	jpr	~end
L4:
	inc	r5,#2
	add	r5,r1
	ldb	rl0,@rr4
	ldl	rr4,@rr2
	ldl	rr6(#2),rr4
	testl	rr4
	jpr	ne,L5
	ldl	rr6(#6),rr4
L5:
	ldl	rr4,_cfreelist	//get link to first free cblock
	ldl	@rr2,rr4	//put it into link of new free cblock
	ldl	_cfreelist,rr2	//put address of new free cblock into cfreelist
	ldb	rl2,rl0
	subb	rh2,rh2
	jpr	~end
	}

_putc::
{
	ldctl	r6,fcw		//save processor status
	di	vi,nvi		//dvi
	ldl	rr2,rr4(#6)
	testl	rr2
	jpr	eq,L7
L6:
	ldb	rl0,rr2(#5)
	subb	rh0,rh0
	cp	r0,_cfreelist+4
	jpr	ge,L10
	inc	@rr4,#1
	lda	rr4,rr2(#6)
	add	r5,r0
	ldb	@rr4,rl7
	incb	rl0,#1
	ldb	rr2(#5),rl0
	clr	r2
~end:
	ldctl	fcw,r6		//restore processor status
	ret
L7:
	ldl	rr2,_cfreelist
	testl	rr2
	jpr	eq,L9
	ldl	rr4(#2),rr2
L8:
	ldl	rr0,@rr2
	ldl	_cfreelist,rr0
	clr	@rr2
	subl	rr0,rr0
	ldl	rr2(#2),rr0
	ldl	rr4(#6),rr2
	jpr	L6
L9:
	ld	r2,#-1
	jpr	~end
L10:
	ldl	rr0,_cfreelist
	testl	rr0
	jpr	eq,L9
	ldl	@rr2,rr0
	ldl	rr2,rr0
	jpr	L8
	
	}
