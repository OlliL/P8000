	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_sprofil::
_profil::
	{
	ldm	r0,|_stkseg+4|(sp),#6
	sc	#52			  /*sysphys*/
	ld	r2,r4
	jp	c,cerror
	ret
	}
