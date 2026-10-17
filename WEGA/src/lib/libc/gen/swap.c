/* @[$] swap.c	2.1  09/12/83 11:33:48 - 87wega3.2 */

#include <s.out.h>

swap(x)
int x;
{
	register char *p;
	int tmp;

	p = (char *)&tmp;
	*p = ((char *)&x)[1];
	p[1] = ((char *)&x)[0];
	return(*(int *)p);
}

longswap(l)
int *l;
{
	register int *p;

	p = l;
	if(*p)
		*p = swap(*p);
	p++;
	if(*p)
		*p = swap(*p);
}

swapsegt(s)
struct segt *s;
{
	register struct segt *sp;

	sp = s;
	if(sp->sg_code)
		sp->sg_code = swap(sp->sg_code);
	if(sp->sg_data)
		sp->sg_data = swap(sp->sg_data);
	if(sp->sg_bss)
		sp->sg_bss = swap(sp->sg_bss);
	if(sp->sg_atr)
		sp->sg_atr = swap(sp->sg_atr);
}
