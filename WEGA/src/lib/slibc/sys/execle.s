! @[$] execle.s	2.1  09/12/83 11:40:35 - 87wega3.2 !

execle module

$segmented

  external
    cerror	procedure

  global
    _execle procedure
      entry
	ldl	rr0,rr6
	ldl	rr6,@rr14
	ldl	@rr14,rr2
	pushl	@rr14,rr4
	ldl	rr4,rr14
	pushl	@rr14,rr4
	pushl	@rr14,rr6
	xor	r2,r2
L1:
	ldl	rr6,@rr4
	cpl	rr6,#%00000000
	jr	z,L2
	inc	r2,#1
	cp	r2,#%4000
	jr	z,L2
	inc	r5,#4
	jr	L1
L2:
	popl	rr6,@rr14
	popl	rr2,@rr14
	inc	r5,#4
	ldl	rr4,@rr4
	sc	#59
	ld	r2,r4
	inc	r15,#4
	ldl	@rr14,rr6
	jp	cerror
    end _execle
end execle
