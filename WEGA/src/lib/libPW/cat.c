/*
	Concatenate strings.
 
	cat(destination,source1,source2,...,sourcen,0);
 
	returns destination.
*/

#ifndef SEG
char *cat(dest, a1, a2, a3, a4, a5, args)
char *dest, *a1, *a2, *a3, *a4, *a5;
#else
char *cat(dest, a1, a2, args)
char *dest, *a1, *a2;
#endif
char *args;
{
#ifndef SEG
	char *regs[5];

	regs[0] = a1;
	regs[1] = a2;
	regs[2] = a3;
	regs[3] = a4;
	regs[4] = a5;
	_cat(dest, regs, &args);
#else
	struct { char *r0, *r1; } regs;

	regs.r0 = a1;
	regs.r1 = a2;
	_cat(dest, &regs, &args);
#endif
	return(dest);
}

_cat(dest, regs, stk)
char *dest;
char **regs, **stk;
{
	register char *d, *s, **sp;
	register int i;

	d = dest;
	for (sp = regs, i = 0; s = *sp; sp++, i++) {
#ifndef SEG
		if (i >= 5)
#else
		if (i >= 2)
#endif
			break;
		while (*d++ = *s++) ;
		d--;
	}
#ifndef SEG
	if (s)
#else
	if (i >= 2)
#endif
		for (sp = stk; s = *sp; sp++) {
			while (*d++ = *s++) ;
			d--;
		}
}
