/*
 * printname.c - part of sld (WEGA 3.1), reconstructed from /bin/sld; the code range is in tools/module_ranges.txt.
 */

#include "ld.h"

static char sccsid[] = "@[$]printname.c\t2.2  10/27/83 12:24:42 - 87wega3.2";
int pnflag = 0;

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
		if (nlflag == 0) {
			n = 8;
			if (pnflag == 0) {
				pnflag++;
				report_error(0, "Warning - only 8 characters of name are significant");
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
		printf("...");
		k += 3;
	}
	return k;
}
