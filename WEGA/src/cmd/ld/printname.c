/*
 * printname.c - part of sld (WEGA 3.1), reconstructed from /bin/sld; the code range is in tools/module_ranges.txt.
 */

#include "ld.h"

/* put_name..65c6: done */
int put_name(p)
char *p;
{
	register char *q;
	register int n;
	register int k;
	char flag;

	q = p;
	flag = 0;
	n = *q & 0x7f;
	if (n != *q) {
		q++;
		n--;
		if (bE4EA == 0) {
			n = 8;
			if (bE1532 == 0) {
				bE1532++;
				report_error(0, "x");
			}
		} else if (n > 0x2c) {
			flag++;
			n = 0x28;
		}
	} else
		n = 8;
	k = 0;
	while (n-- != 0) {
		if (*q == 0)
			break;
		putchar(*q);
		k++;
		q++;
	}
	if (flag) {
		printf("y");
		k += 3;
	}
	return k;
}
