! @[$] pow10.s	2.2  09/12/83 11:29:05 - 87wega3.2 !

pow10 module
  INTERNAL
    MAP:
	WORD := %0d80
	WORD := %6cd7
	WORD := %bfce
	WORD := %0f5a
	WORD := %b8a6
	WORD := %761e
	WORD := %06c0
	WORD := %555d
	WORD := %9cb0
	WORD := %0bfd
	WORD := %6f02
	WORD := %5339
	WORD := %0360
	WORD := %4b25
	WORD := %8d9e
	WORD := %89d1
	WORD := %1346
	WORD := %bda5
	WORD := %01b0	
	WORD := %459a
	WORD := %86a3
	WORD := %364e
	WORD := %a62c
	WORD := %672d
	WORD := %00d8
	WORD := %42cc
	WORD := %b9a7
	WORD := %4a06
	WORD := %37ce
	WORD := %2ee1
	WORD := %006c
	WORD := %4165
	WORD := %da01
	WORD := %ee64
	WORD := %1a70
	WORD := %8dea
	WORD := %0036
	WORD := %40b2
	WORD := %a70c
	WORD := %3c40
	WORD := %a64e
	WORD := %6c52
	WORD := %001b
	WORD := %4058
	WORD := %cecb
	WORD := %8f27
	WORD := %f420
	WORD := %0f3a

  global
    __pow10 procedure
      entry
	dec	r15,#4
	fldctl	@r15,fflags
	fsetmode	rn
	ldl	rr2,#%3f800000
	cp	r7,#%1388
	jr	c,L1
	set	r2,#14
	flds	f0,r2
	inc	r15,#4
	ret
L1:
	flds	f0,r2
	cp	r7,#%001b
	jr	le,L4
	lda	r3,MAP
	ldk	r1,#8
L2:
	pop	r0,@r3
	cp	r7,r0
	jr	ult,L3
	fmul	f0,@r3
	sub	r7,r0
L3:
	inc	r3,#10
	djnz	r1,L2
L4:
	ldl	rr2,#%3f800000
	flds	f1,r2
	ld	r2,#%4120
	flds	f2,r2
	or	r7,r7
	jr	eq,L7
L5:
	bit	r7,#0
	jr	eq,L6
	fmul	f1,f2
L6:
	fmul	f2,f2
	res	r7,#0
	rr	r7
	jr	ne,L5
L7:
	fldctl	fflags,@r15
	inc	r15,#4
	fmul	f0,f1
	ret
    end __pow10
end pow10
