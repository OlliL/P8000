	.extern	__exit, _execl, _fork, _signal, _wait
	.psec
	.code
_system::
	dec	r15,#2
	ld	%0000(r15),r7
	ldk	r6,#1
	calr	__system
	inc	r15,#2
	ret
_csystem::
	dec	r15,#2
	ld	%0000(r15),r7
	sub	r6,r6
	calr	__system
	inc	r15,#2
	ret
__system::
	dec	r15,#14
	ldm	%000A(r15),r13,#2
	ldl	%0000(r15),rr6
	call	_fork
	ld	%0006(r15),r2
	test	r2
	jr	ne,L0066
	test	%0000(r15)
	jr	ne,L0048
	ld	r7,#D0000
	ld	r6,#D0009
	ld	r5,#D000d
	jr	L0054
L0048:
	ld	r7,#D0010
	ld	r6,#D0018
	ld	r5,#D001b
L0054:
	ld	r4,%0002(r15)
	sub	r3,r3
	call	_execl
	ld	r7,#%007F
	call	__exit
L0066:
	ldk	r7,#2
	ldk	r6,#1
	call	_signal
	ld	r14,r2
	ldk	r7,#3
	ldk	r6,#1
	call	_signal
	ld	r13,r2
L007a:
	lda	r7,%0004(r15)
	call	_wait
	ld	%0008(r15),r2
	cp	r2,%0006(r15)
	jr	eq,L0092
	cp	r2,#%FFFF
	jr	ne,L007a
L0092:
	cp	%0008(r15),#%FFFF
	jr	ne,L00a0
	ld	%0004(r15),#%FFFF
L00a0:
	ldk	r7,#2
	ld	r6,r14
	call	_signal
	ldk	r7,#3
	ld	r6,r13
	call	_signal
	ld	r2,%0004(r15)
	ldm	r13,%000A(r15),#2
	inc	r15,#14
	ret
	.psec
	.data
D0000:
	.byte	%2f,%62,%69,%6e,%2f,%63,%73,%68
	.byte	%00
D0009:
	.byte	%63,%73,%68,%00
D000d:
	.byte	%2d,%63,%00
D0010:
	.byte	%2f,%62,%69,%6e,%2f,%73,%68,%00
D0018:
	.byte	%73,%68,%00
D001b:
	.byte	%2d,%63,%00
