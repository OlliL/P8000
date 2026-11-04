	.extern	__exit, _execl, _fork, _signal, _wait
	.psec
	.code
_system::
	dec	r15,#4
	ld	@r15,r7
	ldk	r6,#1
	calr	L001a
	inc	r15,#4
	ret
	dec	r15,#4
	ld	@r15,r7
	ld	r6,r7
	sub	r6,r7
	calr	L001a
	inc	r15,#4
	ret
L001a:
	sub	r15,#%0014
	ldm	%0010(r15),r13,#2
	ldl	@r15,rr6
	call	_fork
	ld	%0006(r15),r2
	test	r2
	jr	ne,L0064
	test	%0000(r15)
	jr	ne,L0046
	ld	r7,#D0000
	ld	r6,#D0009
	ld	r5,#D000d
	jr	L0052
L0046:
	ld	r7,#D0010
	ld	r6,#D0018
	ld	r5,#D001b
L0052:
	ld	r4,%0002(r15)
	sub	r3,r3
	call	_execl
	ld	r7,#%007F
	call	__exit
L0064:
	ldk	r7,#2
	ldk	r6,#1
	call	_signal
	ld	r14,r2
	ldk	r7,#3
	ldk	r6,#1
	call	_signal
	ld	r13,r2
L0078:
	lda	r7,%0004(r15)
	call	_wait
	ld	%0008(r15),r2
	cp	r2,%0006(r15)
	jr	eq,L0090
	cp	r2,#%FFFF
	jr	ne,L0078
L0090:
	cp	%0008(r15),#%FFFF
	jr	ne,L009e
	ld	%0004(r15),#%FFFF
L009e:
	ldk	r7,#2
	ld	r6,r14
	call	_signal
	ldk	r7,#3
	ld	r6,r13
	call	_signal
	ld	r2,%0004(r15)
	ldm	r13,%0010(r15),#2
	add	r15,#%0014
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
