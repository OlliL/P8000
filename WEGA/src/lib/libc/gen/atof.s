! @[$] atof.s	2.1  09/12/83 11:19:34 - 87wega3.2 !

atof module

  external
    __pow10	procedure

  internal
    str1:
	WORD := T8c
	WORD := T54
	WORD := T88
	WORD := T94
	WORD := T98
	WORD := T9a
	WORD := T9e
	WORD := Tac
	WORD := Tb6
	WORD := Tba
	WORD := Tbe
	WORD := Tc4
	WORD := Tce
	WORD := Td0
	WORD := Td4
	WORD := T114
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
	WORD := %0002
	WORD := %0400
	WORD := %0006
	WORD := %0a08
	WORD := %0e0e
	WORD := %0e0e
	WORD := %0e0e
	WORD := %0e0e
	WORD := %0e0c
	WORD := %0002
	WORD := %0400
	WORD := %0006
	WORD := %0000
	WORD := %0e0e
	WORD := %0e0e
	WORD := %0e0e
	WORD := %0e0e
	WORD := %0e0c
	WORD := %1e1e
	WORD := %1e12
	WORD := %1210
	WORD := %1e1e
	WORD := %0e0e
	WORD := %0e0e
	WORD := %0e0e
	WORD := %0e0e
	WORD := %0e0e
	WORD := %1e1e
	WORD := %1e1e
	WORD := %1e1e
	WORD := %1e1e
	WORD := %1616
	WORD := %1616
	WORD := %1616
	WORD := %1616
	WORD := %1614
	WORD := %1e1e
	WORD := %1e12
	WORD := %121e
	WORD := %1e1e
	WORD := %1616
	WORD := %1616
	WORD := %1616
	WORD := %1616
	WORD := %1616
	WORD := %1e1e
	WORD := %1e1e
	WORD := %1e1e
	WORD := %1a18
	WORD := %1c1c
	WORD := %1c1c
	WORD := %1c1c
	WORD := %1c1c
	WORD := %1c1c
	WORD := %1e1e
	WORD := %1e1e
	WORD := %1e1e
	WORD := %1e1e
	WORD := %1c1c
	WORD := %1c1c
	WORD := %1c1c
	WORD := %1c1c
	WORD := %1c1c
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
	ldm	@r15,r8,#6
	ldl	rr12,rr6
	subl	rr0,rr0
	ldl	rr2,rr0
	ldl	rr4,rr2
	ldl	rr6,rr4
	ldl	rr8,rr6
	ldb	rh2,#%13
	dec	r13
L1:
	inc	r13
	ldb	rl0,@r13
	cpb	rl0,#%20
	jr	z,L1
	cpb	rl0,#%09
	jr	z,L1
	ld	r1,#%0013
L6:
	ldb	rl0,@r13
	inc	r13
	lda	r11,str2			!%026d!
	ld	r8,#%0012
	cpdrb	rl0,@r11,r8,z
	ldb	rl1,r11(r1)
	ld	r11,str1(r1)			!%023c!
	jp	@r11
T54:
	ldk	r8,#3
	lda	r11,str3			!%02ec!
	cpsirb	@r13,@r11,r8,nz
	jr	z,L3
L4:
	ldb	rl0,@r13
	inc	r13
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
	ldb	rl0,@r13
	cpb	rl0,#%30
	jr	nz,L6
	inc	r13
	jr	L7
Tac:
	ldb	rl1,#%37
	decb	rh2
	jr	ge,L8
	incb	rl2
	jr	L6
Tb6:
	ldb	rl1,#%5b
	jr	L6
Tba:
	ldb	rl1,#%6d
	jr	L6
Tbe:
	decb	rl2
	ldb	rl1,#%49
	jr	L6
Tc4:
	ldb	rl1,#%5b
	decb	rh2
	jr	lt,L6
	decb	rl2
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
	dec	r9
	jr	nz,L17
	jr	L12
L13:
	ldb	rl0,#%0f
	andb	rl0,rl7
	jr	nz,L12
	calr	L14
	inc	r9
	jr	nz,L13
L12:
	dec	r15,#10
	ldm	@r15,r3,#5
	ld	r7,r9
	bit	r7,#15
	jr	z,L15
	neg	r7
L15:
	call	__pow10
	fldbcd	f1,@r15
	inc	r15,#10
	bit	r9,#15
	jr	z,L16
	fdiv	f1,f0
	fld	f0,f1
	jr	L10
L16:
	fmul	f0,f1
L10:
	ldm	r8,@r15,#6
	inc	r15,#12
	ret
    end __atof
end atof
