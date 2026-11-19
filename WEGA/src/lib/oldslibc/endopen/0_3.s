	.seg
	.extern	_stkseg
	.extern	_open
	.extern	_errno
	.extern	_lseek
	.extern	_umask
	.extern	_creat
	.extern	_close
	.extern	_chmod
	.code

__endopen::
	dec	r15,#16
	ldm	_stkseg+8(r15),r10,#4
	ldm	|_stkseg+0|(r15),r4,#4
	ldl	rr12,rr2
	testl	rr2
	jr	ne,Lba72
Lba6a:
	subl	rr2,rr2
	jp	Lbb78
Lba72:
	ldl	rr2,|_stkseg+0|(r15)
	cpb	@rr2,#%72
	jr	ne,Lba86
	ldb	rl2,rr2(#%0001)
	cpb	rl2,#%77
	jr	eq,Lba9a
Lba86:
	ldl	rr2,|_stkseg+0|(r15)
	cpb	@rr2,#%77
	jr	ne,Lbaac
	ldb	rl2,rr2(#%0001)
	cpb	rl2,#%72
	jr	ne,Lbaac
Lba9a:
	ldl	rr2,|_stkseg+0|(r15)
	ldb	@rr2,#%72
	ldl	rr2,|_stkseg+0|(r15)
	inc	r3,#1
	ldb	@rr2,#%2B
Lbaac:
	ldl	rr2,|_stkseg+0|(r15)
	ldb	rl2,rr2(#%0001)
	cpb	rl2,#%2B
	ldk	r2,#0
	tcc	eq,r2
	extsb	r2
	ld	r11,r2
	ldl	rr2,|_stkseg+0|(r15)
	ldb	rl2,@rr2
	extsb	r2
	cp	r2,#%0061
	jr	eq,Lbae4
	cp	r2,#%0072
	jr	eq,Lbb22
	cp	r2,#%0077
	jr	ne,Lba6a
	ldl	rr6,|_stkseg+4|(r15)
	ld	r5,r11
	calr	Lbb84
	jr	Lbb36
Lbae4:
	test	r11
	jr	eq,Lbaec
	ldk	r5,#2
	jr	Lbaee
Lbaec:
	ldk	r5,#1
Lbaee:
	ldl	rr6,|_stkseg+4|(r15)
	call	_open
	ld	r10,r2
	cp	r2,#%0000
	jr	ge,Lbb14
	cp	_errno,#%0002
	jr	ne,Lbb14
	ldl	rr6,|_stkseg+4|(r15)
	ld	r5,r11
	calr	Lbb84
	ld	r10,r2
Lbb14:
	ld	r7,r10
	subl	rr4,rr4
	ldk	r3,#2
	call	_lseek
	jr	Lbb38
Lbb22:
	test	r11
	jr	eq,Lbb2a
	ldk	r5,#2
	jr	Lbb2c
Lbb2a:
	sub	r5,r5
Lbb2c:
	ldl	rr6,|_stkseg+4|(r15)
	call	_open
Lbb36:
	ld	r10,r2
Lbb38:
	cp	r10,#%0000
	jr	lt,Lba6a
	ldl	rr2,rr12
	inc	r3,#4
	clr	@rr2
	ldl	rr2,rr12
	inc	r3,#11
	ld	r0,r10
	ldb	@rr2,rl0
	test	r11
	jr	eq,Lbb5e
	ldl	rr2,rr12
	inc	r3,#10
	ldb	rl0,@rr2
	orb	rl0,#%80
	ldb	@rr2,rl0
	jr	Lbb76
Lbb5e:
	ldl	rr2,|_stkseg+0|(r15)
	cpb	@rr2,#%72
	jr	ne,Lbb70
	ldl	rr2,rr12
	inc	r3,#10
	setb	@rr2,#0
	jr	Lbb76
Lbb70:
	ldl	rr2,rr12
	inc	r3,#10
	setb	@rr2,#1
Lbb76:
	ldl	rr2,rr12
Lbb78:
	ldm	r10,_stkseg+8(r15),#4
	inc	r15,#16
	ret
Lbb84:
	dec	r15,#10
	ldm	_stkseg+2(r15),r10,#4
	ld	|_stkseg+0|(r15),r5
	ldl	rr12,rr6
	test	r5
	jr	eq,Lbbe4
	sub	r7,r7
	call	_umask
	ld	r11,r2
	ldl	rr6,rr12
	ld	r5,#%01B6
	call	_creat
	ld	r10,r2
	cp	r2,#%0000
	jr	lt,Lbbda
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
Lbbda:
	ld	r7,r11
	call	_umask
	jr	Lbbf2
Lbbe4:
	ldl	rr6,rr12
	ld	r5,#%01B6
	call	_creat
	ld	r10,r2
Lbbf2:
	ld	r2,r10
	ldm	r10,_stkseg+2(r15),#4
	inc	r15,#10
	ret
