/*
 * ext.c - part of sld (WEGA 3.1), reconstructed from /bin/sld; the code range is in tools/module_ranges.txt.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/stat.h>
#include "ld.h"

static char sccsid[] __attribute__((unused)) = "@[$]ext.c\t3.3  10/27/83 12:15:38 - 87wega3.2";
int16_t nblks = -1;
struct p1seg **blktab = 0;
int16_t unused3be = 0;

/* get_block: allocate a block of the work area (malloc), fatal when it fails (0x06c6..0x0762 of /bin/sld) */
/* the block of the index -1 (copy_blocks takes the segment of the successor of the last block): in the original it is read behind
   the table blktab; a block that is all zero here */
static struct p1seg block_m1;

struct p1seg *get_block(int16_t n)
{
	register struct p1seg *p;

	if (n < 0)
		return &block_m1;

	if (n > maxblk)
		error(2, "Too many pass 1 segment table entries --try -Ap option");
	if (n > nblks) {
		goto test;
body:
		p = blktab[++nblks] = (struct p1seg *) calloc(1, sizeof (struct p1seg));
		if (p == 0)
			error(2, "malloc unable to allocate enough memory");
test:
		if (n > nblks)
			goto body;
		return p;
	}
	return blktab[n];
}

/* segtab_alloc: allocate the segment table (0x0762..0x078c of /bin/sld) */
void segtab_alloc(int16_t a)
{
	blktab = (struct p1seg **) calloc(a, sizeof (struct p1seg *));
	if (!blktab)
		error(2, "malloc unable to allocate enough memory");
}
