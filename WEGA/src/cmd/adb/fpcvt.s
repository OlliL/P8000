// fpcvt: the 80-bit floating point register at p as a double (for $f); an EPU load, no C source
	.psec
	.code
_fpcvt::
	.word	%0F74,%8004
	ret
