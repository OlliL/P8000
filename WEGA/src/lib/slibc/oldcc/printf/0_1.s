// printf.o: magic E607 segmented
	.seg
	.psec	data
	.data
	.comm	2, _rc
	.psec
	.code
	.extern	_printf
	.extern	_stkseg
	.extern	__iob
	.extern	__pfile
	.extern	__doprnt
	.extern	_fprintf
	.extern	_sprintf
	.extern	__flsbuf
	.psec	data
	.data

// segment table entry 0 (segment 0), text, 314 bytes
	.psec
	.code
_printf::
	sub	r15,#%001C
	ldm	|_stkseg+6|(r15),r2,#6
	ld	|_stkseg+18|(r15),r5
	ld	|_stkseg+20|(r15),r4
	ld	|_stkseg+22|(r15),r3
	ld	|_stkseg+24|(r15),r2
	ldl	rr0,#__iob+12
	ldl	__pfile,rr0
	ldk	r0,#1
	ld	|_stkseg|(r15),r0
	ldl	rr0,#__iob+12
	ldl	|_stkseg+2|(r15),rr0
	ldl	rr4,#_stkseg+18
	add	r5,r15
	ldl	rr2,#_stkseg+32
	add	r3,r15
	call	__doprnt
	ld	_rc,r2
	add	r15,#%001C
	ret
_fprintf::
	sub	r15,#%0018
	ldm	|_stkseg+6|(r15),r2,#6
	ld	|_stkseg+18|(r15),r3
	ld	|_stkseg+20|(r15),r2
	ldl	__pfile,rr6
	ldk	r0,#3
	ld	|_stkseg|(r15),r0
	ldl	|_stkseg+2|(r15),rr6
	ldl	rr4,#_stkseg+18
	add	r5,r15
	ldl	rr2,#_stkseg+28
	add	r3,r15
	ldl	rr6,|_stkseg+10|(r15)
	call	__doprnt
	ld	_rc,r2
	add	r15,#%0018
	ret
_sprintf::
	sub	r15,#%0024
	ldm	|_stkseg+6|(r15),r2,#6
	ldb	|_stkseg+28|(r15),#%42
	ldl	|_stkseg+18|(r15),rr6
	ld	|_stkseg+22|(r15),#%7FFF
	ld	|_stkseg+30|(r15),r3
	ld	|_stkseg+32|(r15),r2
	ldl	rr0,#_stkseg+18
	add	r1,r15
	ldl	__pfile,rr0
	ldk	r0,#3
	ld	|_stkseg|(r15),r0
	ldl	rr0,#_stkseg+18
	add	r1,r15
	ldl	|_stkseg+2|(r15),rr0
	ldl	rr4,#_stkseg+30
	add	r5,r15
	ldl	rr2,#_stkseg+40
	add	r3,r15
	ldl	rr6,|_stkseg+10|(r15)
	call	__doprnt
	ld	_rc,r2
	dec	|_stkseg+22|(r15),#1
	cp	|_stkseg+22|(r15),#%0000
	jr	lt,L0t_011e
	ldl	rr2,|_stkseg+18|(r15)
	inc	|_stkseg+20|(r15),#1
	clrb	@rr2
	subb	rl2,rl2
	extsb	r2
	jr	L0t_012e
L0t_011e:
	ldl	rr4,#_stkseg+18
	add	r5,r15
	sub	r7,r7
	call	__flsbuf
L0t_012e:
	ld	r2,_rc
	add	r15,#%0024
	ret
