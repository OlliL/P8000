	.seg

	sp := r15
	SP := rr14

	.psec data
	.data
_par_uid::
	.word	%0

	.psec
	.code
_fork::
	{
	sc	#2			  /*fork*/
	jr	L1
	ld	r2,r4
	ret	nc
	jp	cerror
L1:
	ld	_par_uid,r4
	xor	r2,r2
	ret

	}
