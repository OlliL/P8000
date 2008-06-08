.comm	2,	_environ;

.code
start::
	{
	jr foo
.byte	"......1981 87wega3.2."
.byte	%00
foo:
		ld	r7,@r15
		lda	r6,%0000+2(r15)
		ld	r2,@r6
		clr	%0000-2(r2)
		ld	r5,r6
		subl	rr0,rr0
		cpir	r0,@r5,r1,eq
		cp	r5,r2
		jr	ult,_foo
		dec	r5,#2
_foo:
		ld	_environ,r5
		call	_main
		ld	r7,r2
		call	__callex
		sc	#1
	}
.data
. = .+2
