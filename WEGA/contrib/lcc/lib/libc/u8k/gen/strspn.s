/*
 * Return the number of characters in the maximum leading segment
 * of string which consists solely of characters from charset.
 *
 *	int
 *	strspn(string, charset)
 *	char	*string;
 *	register char	*charset;
 *	{
 *		register char *p, *q;
 *		char rl0, rl1;
 *	
 *		for(q=string; (rl0=*q) != '\0'; ++q) {
 *			p = charset;
 *	goon:
 *			rl1 = *p++;
 *			if (rl1 == '\0')
 *				break;
 *			if (rl0 == rl1)
 *				continue;
 *			goto gogon;
 *		}
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
_strspn::
	ldm	r2,|_stkseg+4|(sp),#4	// rr2 <-> q=string , rr4 <-> charset
~for1:
	ldb	rl0,@rr2		// rl0=*q
	testb	rl0
	jr	nz,~L1
~finis:
	sub	r3,|_stkseg+6|(sp)
	ld	r2,r3
	ret
~L1:
	ldl	rr6,rr4			// rr6 <-> p=charset
~for2:
	ldb	rl1,@rr6		// rl1=*p
	testb	rl1
	jr	z,~finis
	inc	r7,#1			// p++
	cpb	rl0,rl1
	jr	ne,~for2
	inc	r3,#1			// q++
	jr	~for1
	}
