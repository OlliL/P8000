/* calloc of System III with an unused label: the calloc of mkstr (11/82), ccv0 */
/*LINTLIBRARY*/
/*	calloc - allocate and clear memory block
*/
#define CHARPERINT (sizeof(int)/sizeof(char))
#define NULL 0

char *
calloc(num, size)
unsigned num, size;
{
	register char *mp;
	char *malloc();
	register int *q;
	register m;

start:	/* not jumped to: size is forgotten here and taken from its frame home, mult rr2,%0000(r15) (OLD_D3K) */
	num *= size;
	mp = malloc(num);
	if(mp == NULL)
		return(NULL);
	q = (int *) mp;
	m = (num+CHARPERINT-1)/CHARPERINT;
	while(--m >= 0)
		*q++ = 0;
	return(mp);
}

/*ARGSUSED*/
cfree(p, num, size)
char *p;
unsigned num, size;
{
	free(p);
}
