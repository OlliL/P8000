/*
 * Return ptr to first occurance of any character from `brkset'
 * in the character string `string'; NULL if none exists.
 *
 *	#define	NULL	(char *) 0
 *	
 *	char *
 *	strpbrk(string, brkset)
 *	register char *string, *brkset;
 *	{
 *		register char *p;
 *		char rl0, rl1;
 *	
 *		do {
 *			rl0 = *string;
 *			for(p=brkset; (rl1=*p++)!= '\0'; )
 *				if (rl1 == rl0)
 *					return(string);
 *		}
 *		while(string++, rl0 != '\0');
 *		return(NULL);
 *	}
 */
	.seg
	fp := r13
	FP := rr12
	sp := r15
	SP := rr14
	.psec
	.code
_strpbrk::
	{
	ldm	r2,|_stkseg+4|(sp),#4	// rr2 <-> string , rr4 <-> brkset
~do:
	ldb	rl0,@rr2		// rl0=*string
	ldl	rr6,rr4			// rr6 <-> p=brkset
~for:
	ldb	rl1,@rr6		// rl1=*p
	inc	r7,#1			// p++
	testb	rl1
	jr	z,~while
	cpb	rl0,rl1
	jr	ne,~for
	ret
~while:
	inc	r3,#1			// string++
	testb	rl0
	jr	nz,~do
	subl	rr2,rr2
	ret
	}
