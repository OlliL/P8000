	.seg
	.extern	_stkseg
	.extern	_open
	.extern	_lseek
	.extern	_umask
	.extern	_creat
	.extern	_close
	.extern	_chmod
	.extern	_errno
	.psec
	.code

__endopen::
	sub	r15,#%001C
	ldm	_stkseg+20(r15),r10,#4
	ldm	|_stkseg+4|(r15),r4,#4
	ldl	rr12,rr2
	testl	rr2
	jr	ne,La9fc
La9f4:
	subl	rr2,rr2
	jp	Lab10
La9fc:
	ldl	rr2,|_stkseg+4|(r15)
	cpb	@rr2,#%72
	jr	ne,Laa10
	ldb	rl4,rr2(#%0001)
	cpb	rl4,#%77
	jr	eq,Laa24
Laa10:
	ldl	rr2,|_stkseg+4|(r15)
	cpb	@rr2,#%77
	jr	ne,Laa36
	ldb	rl4,rr2(#%0001)
	cpb	rl4,#%72
	jr	ne,Laa36
Laa24:
	ldl	rr2,|_stkseg+4|(r15)
	ldb	@rr2,#%72
	ldl	rr2,|_stkseg+4|(r15)
	inc	r3,#1
	ldb	@rr2,#%2B
Laa36:
	ldl	rr2,|_stkseg+4|(r15)
	ldb	rl4,rr2(#%0001)
	cpb	rl4,#%2B
	ldk	r4,#0
	tcc	eq,r4
	extsb	r4
	ld	r11,r4
	ldl	rr2,|_stkseg+4|(r15)
	ldb	rl5,@rr2
	extsb	r5
	ld	r4,r5
	ldl	rr2,#D900a
	ld	r5,#%0003
	cpir	r4,@rr2,r5,eq
	jr	ne,La9f4
	sll	r5,#2
	neg	r5
	ldl	rr2,D9018(r5)
	jp	@rr2
Laa72:
	ldl	rr6,|_stkseg+8|(r15)
	ld	r5,r11
	calr	Lab1e
	jr	Laace
Laa7c:
	test	r11
	jr	eq,Laa84
	ldk	r5,#2
	jr	Laa86
Laa84:
	ldk	r5,#1
Laa86:
	ldl	rr6,|_stkseg+8|(r15)
	call	_open
	ld	r10,r2
	cp	r2,#%0000
	jr	ge,Laaac
	cp	_errno,#%0002
	jr	ne,Laaac
	ldl	rr6,|_stkseg+8|(r15)
	ld	r5,r11
	calr	Lab1e
	ld	r10,r2
Laaac:
	ld	r7,r10
	subl	rr4,rr4
	ldk	r3,#2
	call	_lseek
	jr	Laad0
Laaba:
	test	r11
	jr	eq,Laac2
	ldk	r5,#2
	jr	Laac4
Laac2:
	sub	r5,r5
Laac4:
	ldl	rr6,|_stkseg+8|(r15)
	call	_open
Laace:
	ld	r10,r2
Laad0:
	cp	r10,#%0000
	jr	lt,La9f4
	ldl	rr2,rr12
	inc	r3,#4
	clr	@rr2
	ldl	rr2,rr12
	inc	r3,#11
	ld	r4,r10
	ldb	@rr2,rl4
	test	r11
	jr	eq,Laaf6
	ldl	rr2,rr12
	inc	r3,#10
	ldb	rl4,@rr2
	orb	rl4,#%80
	ldb	@rr2,rl4
	jr	Lab0e
Laaf6:
	ldl	rr2,|_stkseg+4|(r15)
	cpb	@rr2,#%72
	jr	ne,Lab08
	ldl	rr4,rr12
	inc	r5,#10
	setb	@rr4,#0
	jr	Lab0e
Lab08:
	ldl	rr2,rr12
	inc	r3,#10
	setb	@rr2,#1
Lab0e:
	ldl	rr2,rr12
Lab10:
	ldm	r10,_stkseg+20(r15),#4
	add	r15,#%001C
	ret
Lab1e:
	sub	r15,#%0014
	ldm	_stkseg+12(r15),r10,#4
	ld	|_stkseg+0|(r15),r5
	ldl	rr12,rr6
	test	r5
	jr	eq,Lab80
	sub	r7,r7
	call	_umask
	ld	r11,r2
	ldl	rr6,rr12
	ld	r5,#%01B6
	call	_creat
	ld	r10,r2
	cp	r2,#%0000
	jr	lt,Lab76
	ld	r7,r2
	call	_close
	ldl	rr6,rr12
	ldk	r5,#2
	call	_open
	ld	r10,r2
	ld	r5,r11
	com	r5
	and	r5,#%01B6
	ldl	rr6,rr12
	call	_chmod
Lab76:
	ld	r7,r11
	call	_umask
	jr	Lab8e
Lab80:
	ldl	rr6,rr12
	ld	r5,#%01B6
	call	_creat
	ld	r10,r2
Lab8e:
	ld	r2,r10
	ldm	r10,_stkseg+12(r15),#4
	add	r15,#%0014
	ret
	.psec data
	.data
D900a:
	.byte	%00
	.byte	%61
	.byte	%00
	.byte	%72
	.byte	%00
	.byte	%77
	.long	Laa7c
	.long	Laaba
D9018:
	.long	Laa72
