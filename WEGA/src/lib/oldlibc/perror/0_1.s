	.extern	_dev_errlist, _dev_nerr, _deverr, _errno, _strlen, _sys_errlist, _sys_nerr, _write
	.psec
	.code
_perror::
	dec	r15,#8
	ldm	%0004(r15),r13,#2
	ld	%0000(r15),r7
	ld	r14,#D0000
	cp	_errno,#%0005
	jr	eq,L0020
	cp	_errno,#%0006
	jr	ne,L0038
L0020:
	test	_deverr
	jr	eq,L0038
	ld	r2,_deverr
	cp	r2,_dev_nerr
	jr	ge,L0048
	add	r2,r2
	ld	r14,_dev_errlist(r2)
	jr	L0048
L0038:
	ld	r2,_errno
	cp	r2,_sys_nerr
	jr	ge,L0048
	add	r2,r2
	ld	r14,_sys_errlist(r2)
L0048:
	ld	r7,%0000(r15)
	call	_strlen
	ld	r13,r2
	test	r2
	jr	eq,L006e
	ldk	r7,#2
	ld	r6,%0000(r15)
	ld	r5,r2
	call	_write
	ldk	r7,#2
	ld	r6,#D000e
	ld	r5,r7
	call	_write
L006e:
	ld	r7,r14
	call	_strlen
	ld	%0002(r15),r2
	ldk	r7,#2
	ld	r6,r14
	ld	r5,r2
	call	_write
	ldk	r7,#2
	ld	r6,#D0011
	ldk	r5,#1
	call	_write
	ldm	r13,%0004(r15),#2
	inc	r15,#8
	ret
	.psec
	.data
D0000:
	.byte	%55,%6e,%6b,%6e,%6f,%77,%6e,%20
	.byte	%65,%72,%72,%6f,%72,%00
D000e:
	.byte	%3a,%20,%00
D0011:
	.byte	%0a,%00,%00
