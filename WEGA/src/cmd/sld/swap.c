/*
 * swap.c - part of sld (WEGA 3.1), reconstructed from /bin/sld; the code range is in tools/module_ranges.txt.
 */

#include "ld.h"

static char sccsid[] = "@[$]swap.c\t2.1  10/27/83 12:25:00 - 87wega3.2";

/* swap_bytes: swap the bytes of a word in place (0x65c6..0x65ec of /bin/sld) */
int swap_bytes(x)
struct { char c0, c1; } x;
{
	int tmp;
	register char *p;

	p = (char *)&tmp;
	*p = x.c1;
	p[1] = x.c0;
	return(*(int *)p);
}

/* swap_pair: swap the bytes of two words (0x65ec..0x6620 of /bin/sld) */
swap_pair(a)
long *a;
{
	register int *p;

	p = (int *)a;
	if (*p)
		*p = swap_bytes(*p);
	p++;
	if (*p)
		*p = swap_bytes(*p);
}

/* swap_quad: swap the bytes of four words (0x6620..0x6684 of /bin/sld) */
swap_quad(a)
int *a;
{
	register int *p;

	p = a;
	if (p[2])
		p[2] = swap_bytes(p[2]);
	if (p[3])
		p[3] = swap_bytes(p[3]);
	if (p[4])
		p[4] = swap_bytes(p[4]);
	if (p[5])
		p[5] = swap_bytes(p[5]);
}
