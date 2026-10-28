/*
 * swap.c - part of sld (WEGA 3.1), reconstructed from /bin/sld; the code range is in tools/module_ranges.txt.
 */

#include "ld.h"

static char sccsid[] = "@[$]swap.c\t2.1  10/27/83 12:25:00 - 87wega3.2";

/* swap_bytes..65ec: findings:D4 */
int swap_bytes(u)
struct { int i; } u;
{
	char b[2];
	register char *p;

	p = b;
	*p = u.i;
	p++;
	*p = u.i >> 8;
	return *(int *)b;
}

/* swap_pair 0x65ec..0x6620: findings:D3 */
swap_pair(a)
int *a;
{
	register int *p;

	p = a;
	if (*p)
		*p = swap_bytes(*p);
	p++;
	if (*p)
		*p = swap_bytes(*p);
}

/* swap_quad..6684: findings:D3 */
swap_quad(a)
char *a;
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
