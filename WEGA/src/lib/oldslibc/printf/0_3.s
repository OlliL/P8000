	.seg
	.extern	_stkseg
	.extern	__iob
	.extern	__doprnt
	.extern	__flsbuf
	.code

_printf::
	sub	r15,#%001A
	ldm	|_stkseg+6|(r15),r2,#6
	ld	|_stkseg+18|(r15),r5
	ld	|_stkseg+20|(r15),r4
	ld	|_stkseg+22|(r15),r3
	ld	|_stkseg+24|(r15),r2
	ldk	r0,#1
	ld	|_stkseg+0|(r15),r0
	ldl	rr0,#__iob+12
	ldl	|_stkseg+2|(r15),rr0
	ldl	rr4,#%BF000012
	add	r5,r15
	ldl	rr2,#%BF00001E
	add	r3,r15
	ldl	rr6,|_stkseg+14|(r15)
	call	__doprnt
	ldb	rl2,__iob+22
	extsb	r2
	and	r2,#%0020
	test	r2
	jr	eq,La9a8
	ld	r2,#%FFFF
	jr	La9aa
La9a8:
	sub	r2,r2
La9aa:
	add	r15,#%001A
	ret
_fprintf::
	sub	r15,#%0016
	ldm	|_stkseg+6|(r15),r2,#6
	ld	|_stkseg+18|(r15),r3
	ld	|_stkseg+20|(r15),r2
	ldk	r0,#3
	ld	|_stkseg+0|(r15),r0
	ldl	|_stkseg+2|(r15),rr6
	ldl	rr4,#%BF000012
	add	r5,r15
	ldl	rr2,#%BF00001A
	add	r3,r15
	ldl	rr6,|_stkseg+10|(r15)
	call	__doprnt
	ldl	rr2,|_stkseg+14|(r15)
	ldb	rl2,rr2(#%000A)
	extsb	r2
	and	r2,#%0020
	test	r2
	jr	eq,La9fe
	ld	r2,#%FFFF
	jr	Laa00
La9fe:
	sub	r2,r2
Laa00:
	add	r15,#%0016
	ret
_sprintf::
	sub	r15,#%0022
	ldm	|_stkseg+6|(r15),r2,#6
	ldb	|_stkseg+28|(r15),#%42
	ldl	|_stkseg+18|(r15),rr6
	ld	|_stkseg+22|(r15),#%7FFF
	ld	|_stkseg+30|(r15),r3
	ld	|_stkseg+32|(r15),r2
	ldk	r0,#3
	ld	|_stkseg+0|(r15),r0
	ldl	rr0,#%BF000012
	add	r1,r15
	ldl	|_stkseg+2|(r15),rr0
	ldl	rr4,#%BF00001E
	add	r5,r15
	ldl	rr2,#%BF000026
	add	r3,r15
	ldl	rr6,|_stkseg+10|(r15)
	call	__doprnt
	dec	|_stkseg+22|(r15),#1
	cp	|_stkseg+22|(r15),#%0000
	jr	lt,Laa70
	ldl	rr2,|_stkseg+18|(r15)
	inc	|_stkseg+20|(r15),#1
	clrb	@rr2
	subb	rl2,rl2
	extsb	r2
	jr	Laa80
Laa70:
	ldl	rr4,#%BF000012
	add	r5,r15
	sub	r7,r7
	call	__flsbuf
Laa80:
	ldl	rr2,|_stkseg+14|(r15)
	add	r15,#%0022
	ret
