	.seg
	.code

_cerror::
	ld	_d1BAE+0,r2
	ld	_d1BAE+2,r5
	ldl	rr2,#%FFFFFFFF
	ret

	.data

_d1BAE::
	.word	%0000
	.word	%0000
