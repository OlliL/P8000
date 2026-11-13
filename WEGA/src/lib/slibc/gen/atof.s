! @[$] atof.s	2.1  09/12/83 11:19:34 - 87wega3.2 !

atof module

$segmented

  external
    __pow10	procedure

  internal
    str1:
	LONG := T8c
	LONG := T54
	LONG := T88
	LONG := T94
	LONG := T98
	LONG := T9a
	LONG := T9e
	LONG := Tac
	LONG := Tb6
	LONG := Tba
	LONG := Tbe
	LONG := Tc4
	LONG := Tce
	LONG := Td0
	LONG := Td4
	LONG := T114
	WORD := %004e
	WORD := %4965
	WORD := %452e
	WORD := %2b2d
	WORD := %3938
	WORD := %3736
	WORD := %3534
	WORD := %3332
	BYTE := %31
    str2:
	BYTE := %30
	WORD := %0004
	WORD := %0800
	WORD := %000c
	WORD := %1410
	WORD := %1c1c
	WORD := %1c1c
	WORD := %1c1c
	WORD := %1c1c
	WORD := %1c18
	WORD := %0004
	WORD := %0800
	WORD := %000c
	WORD := %0000
	WORD := %1c1c
	WORD := %1c1c
	WORD := %1c1c
	WORD := %1c1c
	WORD := %1c18
	WORD := %3c3c
	WORD := %3c24
	WORD := %2420
	WORD := %3c3c
	WORD := %1c1c
	WORD := %1c1c
	WORD := %1c1c
	WORD := %1c1c
	WORD := %1c1c
	WORD := %3c3c
	WORD := %3c3c
	WORD := %3c3c
	WORD := %3c3c
	WORD := %2c2c
	WORD := %2c2c
	WORD := %2c2c
	WORD := %2c2c
	WORD := %2c28
	WORD := %3c3c
	WORD := %3c24
	WORD := %243c
	WORD := %3c3c
	WORD := %2c2c
	WORD := %2c2c
	WORD := %2c2c
	WORD := %2c2c
	WORD := %2c2c
	WORD := %3c3c
	WORD := %3c3c
	WORD := %3c3c
	WORD := %3430
	WORD := %3838
	WORD := %3838
	WORD := %3838
	WORD := %3838
	WORD := %3838
	WORD := %3c3c
	WORD := %3c3c
	WORD := %3c3c
	WORD := %3c3c
	WORD := %3838
	WORD := %3838
	WORD := %3838
	WORD := %3838
	WORD := %3838
    str3:
	WORD := %414e
	WORD := %2800
  global
    _atof LABEL
    
    _atofs procedure
      entry
	calr	__atof
	flds	r6,f0
	ret
    end _atofs

    _atofd procedure
      entry
_atof:
	calr	__atof
	fldd	r4,f0
	ret
    end _atofd

    __atof procedure
      entry
	dec	r15,#12
	ldm	@rr14,r8,#6
	ldl	rr12,rr6
	subl	rr0,rr0
	ldl	rr2,rr0
	ldl	rr4,rr2
	ldl	rr6,rr4
	ldl	rr8,rr6
	ldb	rh2,#%13
	dec	r13,#1
L1:
	inc	r13,#1
	ldb	rl0,@rr12
	cpb	rl0,#%20
	jr	z,L1
	cpb	rl0,#%09
	jr	z,L1
	ld	r1,#%0013
L6:
	ldb	rl0,@rr12
	inc	r13,#1
	lda	rr10,str2
	ld	r8,#%0012
	cpdrb	rl0,@rr10,r8,z
	ldb	rl1,rr10(r1)
	ldl	rr10,str1(r1)
	jp	@rr10
T54:
	ldk	r8,#3
	lda	rr10,str3
	cpsirb	@rr12,@rr10,r8,nz
	jr	z,L3
L4:
	ldb	rl0,@rr12
	inc	r13,#1
	orb	rl0,rl0
	jr	z,L3
	cpb	rl0,#%29
	jr	z,L3
	rldb	rh6,rl6
	rldb	rl6,rh7
	rldb	rh7,rl7
	bitb	rl0,#6
	jr	z,L2
	incb	rl0,#9
L2:
	andb	rl0,#%0f
	orb	rl7,rl0
	jr	L4
L3:
	testl	rr6
	jr	nz,L5
	set	r6,#6
L5:
T88:
	or	r6,#%7f80
T8c:
	or	r6,r3
	flds	f0,r6
	jr	L10
T94:
	ldb	rl1,#%49
	jr	L6
T98:
	setb	rh3,#7
T9a:
	ldb	rl1,#%25
	jr	L6
T9e:
	ldb	rl1,#%25
L7:
	ldb	rl0,@rr12
	cpb	rl0,#%30
	jr	nz,L6
	inc	r13,#1
	jr	L7
Tac:
	ldb	rl1,#%37
	decb	rh2,#1
	jr	ge,L8
	incb	rl2,#1
	jr	L6
Tb6:
	ldb	rl1,#%5b
	jr	L6
Tba:
	ldb	rl1,#%6d
	jr	L6
Tbe:
	decb	rl2,#1
	ldb	rl1,#%49
	jr	L6
Tc4:
	ldb	rl1,#%5b
	decb	rh2,#1
	jr	lt,L6
	decb	rl2,#1
	jr	L8
Tce:
	setb	rh3,#6
Td0:
	ldb	rl1,#%7f
	jr	L6
Td4:
	mult	rr8,#%000a
	and	r0,#%000f
	add	r9,r0
	ldb	rl1,#%7f
	jr	L6
L8:
	calr	L9
	andb	rl0,#%0f
	orb	rl7,rl0
	jr	L6
L9:
	rldb	rh3,rl3
	rldb	rl3,rh4
	rldb	rh4,rl4
	rldb	rl4,rh5
	rldb	rh5,rl5
	rldb	rl5,rh6
	rldb	rh6,rl6
	rldb	rl6,rh7
	rldb	rh7,rl7
	ret
L14:
	rrdb	rh7,rl7
	rrdb	rl6,rh7
	rrdb	rh6,rl6
	rrdb	rl5,rh6
	rrdb	rh5,rl5
	rrdb	rl4,rh5
	rrdb	rh4,rl4
	rrdb	rl3,rh4
	rrdb	rh3,rl3
	ret
T114:
	bitb	rh3,#6
	resb	rh3,#6
	jr	z,L11
	neg	r9
L11:
	extsb	r2
	add	r9,r2
	jr	z,L12
	jr	mi,L13
L17:
	ldb	rl0,#%0f
	andb	rl0,rh3
	jr	nz,L12
	calr	L9
	dec	r9,#1
	jr	nz,L17
	jr	L12
L13:
	ldb	rl0,#%0f
	andb	rl0,rl7
	jr	nz,L12
	calr	L14
	inc	r9,#1
	jr	nz,L13
L12:
	dec	r15,#10
	ldm	@rr14,r3,#5
	ld	r7,r9
	bit	r7,#15
	jr	z,L15
	neg	r7
L15:
	call	__pow10
	fldbcd	f1,@rr14
	inc	r15,#10
	bit	r9,#15
	jr	z,L16
	fdiv	f1,f0
	fld	f0,f1
	jr	L10
L16:
	fmul	f0,f1
L10:
	ldm	r8,@rr14,#6
	inc	r15,#12
	ret
    end __atof
end atof
