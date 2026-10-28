	.seg
	.extern	__cleanup
	.code

_exit::
	ld	r8,r7
	call	__cleanup
	ld	r0,r8
	sc	#%01
