/*
 * Return the number of characters in the maximum leading segment
 * of string which consists solely of characters NOT from charset.
 *
 *	int
 *	strcspn(string, charset)
 *	char	*string;
 *	register char	*charset;
 *	{
 *		register char *p, *q;
 *		char rl0, rl1;
 *	
 *		for (q=string; (rl0=*q)!='\0'; q++)
 *			for (p=charset; (rl1=*p++)!='\0'; )
 *				if (rl1 == rl0)
 *					goto finis;
 *	finis:
 *		return(q-string);
 *	}
 */
	.seg
	fp := r13
	FP := rr12
	sp := r15
	SP := rr14
	.psec
	.code
_strcspn::
	{
	ldm	r2,|_stkseg+4|(sp),#4	// rr2 <-> q=string, rr4 <-> charset
~for1:
	ldb	rl0,@rr2		// fst/nxt *q
	testb	rl0
	jr	nz,~L1
~finis:
	sub	r3,|_stkseg+6|(sp)	// q-string
	ld	r2,r3
	ret
~L1:
	ldl	rr6,rr4			// rr6 <-> p=charset
~for2:
	ldb	rl1,@rr6		// fst/nxt *p
	testb	rl1
	jr	z,~L2
	inc	r7,#1			// p++
	cpb	rl0,rl1
	jr	eq,~finis
	jr	~for2
~L2:
	inc	r3,#1			// q++
	jr	~for1
	}
