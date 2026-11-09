	.seg
	.extern	_stkseg
	.extern	_strlen
	.extern	_write
	.extern	_errno
	.extern	_deverr
	.extern	_dev_nerr
	.extern	_dev_errlist
	.extern	_sys_nerr
	.extern	_sys_errlist
	.psec
	.code

_perror::
	sub	r15,#%0012
	ldm	_stkseg+12(r15),r11,#3
	ldl	|_stkseg+0|(r15),rr6
	ldl	rr12,#D9110
	cp	_errno,#%0005
	jr	eq,Lac06
	cp	_errno,#%0006
	jr	ne,Lac28
Lac06:
	test	_deverr
	jr	eq,Lac28
	ld	r2,_deverr
	cp	r2,_dev_nerr
	jr	ge,Lac42
	sla	r2,#2
	ldl	rr4,_dev_errlist(r2)
	jr	Lac40
Lac28:
	ld	r2,_errno
	cp	r2,_sys_nerr
	jr	ge,Lac42
	sla	r2,#2
	ldl	rr4,_sys_errlist(r2)
Lac40:
	ldl	rr12,rr4
Lac42:
	ldl	rr6,|_stkseg+0|(r15)
	call	_strlen
	ld	r11,r2
	test	r2
	jr	eq,Lac70
	ldk	r7,#2
	ldl	rr4,|_stkseg+0|(r15)
	ld	r3,r2
	call	_write
	ldk	r7,#2
	ldl	rr4,#D911e
	ld	r3,r7
	call	_write
Lac70:
	ldl	rr6,rr12
	call	_strlen
	ld	|_stkseg+10|(r15),r2
	ldk	r7,#2
	ldl	rr4,rr12
	ld	r3,r2
	call	_write
	ldk	r7,#2
	ldl	rr4,#D9121
	ldk	r3,#1
	call	_write
	ldm	r11,_stkseg+12(r15),#3
	add	r15,#%0012
	ret
	.psec data
	.data
D9110:
	.byte	%55
	.byte	%6e
	.byte	%6b
	.byte	%6e
	.byte	%6f
	.byte	%77
	.byte	%6e
	.byte	%20
	.byte	%65
	.byte	%72
	.byte	%72
	.byte	%6f
	.byte	%72
	.byte	%00
D911e:
	.byte	%3a
	.byte	%20
	.byte	%00
D9121:
	.byte	%0a
	.byte	%00
	.byte	%00
