/*
 * copyname.c - part of sld (WEGA 3.1), reconstructed from /bin/sld; the code range is in tools/module_ranges.txt.
 */

#include "ld.h"

static char sccsid[] = "@[$]copyname.c\t3.1  10/27/83 12:14:19 - 87wega3.2";

/* unpack_run 0x0056..0x00ee: findings:D2 */
int unpack_run(src, dst)
register char *src;
register char *dst;
{
	int n;
	char ch;
	register int k;

	n = 0;
	k = 0;
	ch = *src;
	if (ch & 0x80) {
		ch &= 0x7f;
		n = (ch + 7) / 16;
		ch = n * 16 + 8;
	} else if (g0476 > 0)
		ch = 14;
	else
		ch = 8;
	while (ch-- != 0) {
		*dst++ = *src;
		if (*src)
			k++;
	}
	return n;
}

/* pack_name..0178: findings:D2 */
pack_name(src, dst)
register char *src;
register char *dst;
{
	register char *s;
	register char *q;
	char cnt;

	cnt = 0;
	s = src;
	while (*s++ != 0)
		cnt++;
	if (cnt > 8) {
		*dst++ = cnt | 0x80;
		s--;
		q = src;
		while (q < s)
			*dst++ = *q++;
		cnt -= 8;
		while (cnt > 0)
			cnt -= 16;
		do {
			*dst++ = 0;
		} while (cnt-- != 0);
	} else
		unpack_run(src, dst);
}

/* f0178..01d2: findings:D2 */
f0178(src, dst)
char *src;
char *dst;
{
	register int k;
	register char *s;

	k = 0;
	s = src;
	if (*src & 0x80) {
		k++;
		if ((*src & 0x7f) > 8) {
			k--;
			put_name(s);
			report_error(1, "-- symbol name truncated\n");
			k++;
		}
	}
	return unpack_run(s, dst);
}
