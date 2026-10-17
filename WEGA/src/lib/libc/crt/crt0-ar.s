	.extern	__callex, _exit, _main
	.comm	2, _environ
	.psec
	.code
start::
	jr	L1
	.byte	"......1981 87wega3.2.",%00
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
	.byte	"@[$]crt0.s",%09,%09,"Rev : 3.2",%09,"11/18/83 00:05:06",%00
