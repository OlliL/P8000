/* calloc of System III: the calloc of df (04/83), ccv1; size declared int */
/*LINTLIBRARY*/
/*	calloc - allocate and clear memory block
*/
#define CHARPERINT (sizeof(int)/sizeof(char))
#define NULL 0

char *
calloc(num, size)
unsigned num;
int size;	/* not unsigned: num *= size then takes size from its register (r6), as the calloc of df */
{
	register char *mp;
	char *malloc();
	register int *q;
	register m;

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
