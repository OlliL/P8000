// C library -- exit

// exit(code)
// code is return in r0 to system

	.seg
	sp := r15
	SP := rr14

	.psec
	.code
_exit::
	{
	call	__cleanup
	ld	r0,SP(#4)
	sc	#1		// exit
	}
