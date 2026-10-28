/*
 * ext.c - part of sld (WEGA 3.1), reconstructed from /bin/sld; the code range is in tools/module_ranges.txt.
 */

#include "ld.h"

static char sccsid[] = "@[$]ext.c\t3.3  10/27/83 12:15:38 - 87wega3.2";
int bE3B8 = -1;
struct p1seg **g03ba = 0;
int g03be = 0;

/* get_block 0x06c6..0x0762: done */
struct p1seg *get_block(n)
int n;
{
	register struct p1seg *p;

	if (n > bE4EC)
		report_error(2, "Too many pass 1 segment table entries --try -Ap option");
	if (n > bE3B8) {
		goto test;
body:
		p = g03ba[++bE3B8] = calloc(1, 0x36);
		if (p == 0)
			report_error(2, "malloc unable to allocate enough memory");
test:
		if (n > bE3B8)
			goto body;
		return p;
	}
	return g03ba[n];
}

/* segtab_alloc 0x0762..0x078c: done */
segtab_alloc(a)
int a;
{
	g03ba = calloc(a, 4);
	if (!g03ba)
		report_error(2, "malloc unable to allocate enough memory");
}
