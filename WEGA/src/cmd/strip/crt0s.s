	.extern	__callex, _exit, _main
	.comm	2, _environ
	.psec
	.code
start::
	jr	L1
	.byte	%43,%4F,%50,%52,%2E,%20,%31,%39,%38,%31,%20,%38,%37,%77,%65,%67,%61,%33,%2E,%32,%2E,%00
L1:
	ld	r7,@r15
	lda	r6,%0002(r15)
	ld	r2,@r6
	clr	%FFFE(r2)
	ld	r5,r6
	subl	rr0,rr0
	cpir	r0,@r5,r1,eq
	cp	r5,r2
	jr	c,L2
	dec	r5,#2
L2:
	ld	_environ,r5
	call	_main
	ld	r7,r2
	call	__callex
	sc	#%01
	.psec
	.data
	.word	%0000
	.byte	%40,%5B,%24,%5D,%63,%72,%74,%30,%2E,%73,%09,%09,%52,%65,%76,%20,%3A,%20,%33,%2E,%31,%09,%30,%31,%2F,%32,%34,%2F,%38,%33,%20,%30,%30,%3A,%30,%35,%3A,%30,%36,%00
