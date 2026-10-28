/*
 * ext.c - part of sld (WEGA 3.1), reconstructed from /bin/sld; the code range is in tools/module_ranges.txt.
 */

#include "ld.h"

static char sccsid[] = "@[$]ext.c\t3.3  10/27/83 12:15:38 - 87wega3.2";
int nblks = -1;
struct p1seg **blktab = 0;
int unused3be = 0;

/* get_block: allocate a block of the work area (malloc), fatal when it fails (0x06c6..0x0762 of /bin/sld) */
struct p1seg *get_block(n)
int n;
{
	register struct p1seg *p;

	if (n > maxblk)
		report_error(2, "Too many pass 1 segment table entries --try -Ap option");
	if (n > nblks) {
		goto test;
body:
		p = blktab[++nblks] = calloc(1, 0x36);
		if (p == 0)
			report_error(2, "malloc unable to allocate enough memory");
test:
		if (n > nblks)
			goto body;
		return p;
	}
	return blktab[n];
}

/* segtab_alloc: allocate the segment table (0x0762..0x078c of /bin/sld) */
segtab_alloc(a)
int a;
{
	blktab = calloc(a, 4);
	if (!blktab)
		report_error(2, "malloc unable to allocate enough memory");
}
