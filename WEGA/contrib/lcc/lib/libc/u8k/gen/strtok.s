/*
 * uses strpbrk and strspn to break string into tokens on
 * sequentially subsequent calls.  returns NULL when no
 * non-separator characters remain.
 * `subsequent' calls are calls with first argument NULL.
 *
 *	#define	NULL	(char*)0
 *	
 *	extern int strspn();
 *	extern char *strpbrk();
 *	
 *	char *
 *	strtok(string, sepset)
 *	char	*string;
 *	register char *sepset;
 *	{
 *		register char	*p, *r;
 *		static char	*savept;
 *	
 *		//first or subsequent call
 *		p = string;
 *		if (p == NULL)
 *			p = savept;
 *		if(p == NULL)		// return if no tokens remaining
 *			return(NULL);
 *	
 *		p += strspn(p, sepset);	// skip leading separators
 *	
 *		if(*p == '\0')		// return if no tokens remaining
 *			return(NULL);
 *	
 *		if((r = strpbrk(p, sepset)) != NULL)	// move past token
 *		{
 *			*r = '\0';
 *			r++;
 *		}
 *		savept = r;
 *		return(p);
 *	}
 */
	.seg
	fp := r13
	FP := rr12
	sp := r15
	SP := rr14
	.psec	data   
	.data
	.even
savept:
	.=.+4
	.psec
	.code
_strtok::
	{
	dec	sp,#12
	ldm	@SP,r8,#6
	ld	fp,sp
	inc	fp,#8
	ldm	r8,|_stkseg+8|(fp),#4	// rr8 <-> p=string , rr10 <-> sepset
	testl	rr8
	jr	nz,~L1
	ldl	rr8,savept
~L1:
	testl	rr8
	jr	z,~zerofinis
	pushl	@SP,rr10
	pushl	@SP,rr8
	call	_strspn
	add	r9,r2			// p += strspn(p,sepset)
	testb	@rr8
	jr	nz,~L2
~zerofinis:
	subl	rr2,rr2
	jr	~finis
~L2:
	ldl	@SP,rr8
	call	_strpbrk		// rr2 <-> r=strpbrk(p,sepset)
	testl	rr2
	jr	z,~L3
	clrb	@rr2
	inc	r3,#1			// r++
~L3:
	ldl	savept,rr2
	ldl	rr2,rr8
~finis:
	ld	sp,fp
	dec	fp,#8
	ldm	r8,@FP,#6
	inc	sp,#4
	ret
	}
