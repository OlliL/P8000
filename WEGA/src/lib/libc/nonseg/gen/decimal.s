! @[$] decimal.s	2.1  09/12/83 11:22:59 - 87wega3.2 !

decimal module

  global
    __deciml procedure
      entry
	ldl	rr0,rr4
	subl	rr2,rr2
	ldl	rr4,rr2
	ldk	r6,#1
	testl	rr0
	jr	z,L3
L1:
	srll	rr0,#1
	jr	nc,L2
	addb	rh4,rl6
	dab	rh4
	adcb	rl3,rh6
	dab	rl3
	adcb	rh3,rl5
	dab	rh3
	adcb	rl2,rh5
	dab	rl2
	adcb	rh2,rl4
	dab	rh2
	testl	rr0
	jr	z,L3
L2:
	addb	rl6,rl6
	dab	rl6
	adcb	rh6,rh6
	dab	rh6
	adcb	rl5,rl5
	dab	rl5
	adcb	rh5,rh5
	dab	rh5
	adcb	rl4,rl4
	dab	rl4
	jr	L1
L3:
	ld	r6,r7
	ldb	rl0,#%30
	rrdb	rl0,rh4
	ldb	@r6,rl0
	rrdb	rl0,rh4
	dec	r6,#1
	ldb	@r6,rl0
	rrdb	rl0,rl3
	dec	r6,#1
	ldb	@r6,rl0
	rrdb	rl0,rl3
	dec	r6,#1
	ldb	@r6,rl0
	rrdb	rl0,rh3
	dec	r6,#1
	ldb	@r6,rl0
	rrdb	rl0,rh3
	dec	r6,#1
	ldb	@r6,rl0
	rrdb	rl0,rl2
	dec	r6,#1
	ldb	@r6,rl0
	rrdb	rl0,rl2
	dec	r6,#1
	ldb	@r6,rl0
	rrdb	rl0,rh2
	dec	r6,#1
	ldb	@r6,rl0
	rrdb	rl0,rh2
	dec	r6,#1
	ldb	@r6,rl0
	ldb	rl0,#%30
	ldk	r1,#10
	cpirb	rl0,@r6,r1,nz
	dec	r6,#1
	ld	r2,r6
	ret	
  end __deciml
end decimal
