//	 Assembler program to implement the following C program
//	int
//	abs(arg)
//	int	arg;
//	{
//		return((arg < 0)? -arg: arg);
//	}

	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_abs::
	{
	ld	r2,|_stkseg+4|(sp)
	test	r2
	ret	pl
	neg	r2
	ret
	}
