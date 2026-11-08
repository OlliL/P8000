#include <stdio.h>

static char sccsid[] __attribute__((unused)) = "@[$]printname.c\t1.5  03/11/83 10:29:11 - 87wega3.2";

#include "ld.h"

int16_t pnflag = 0;

/* put_name: print a symbol name to stderr; the number of characters (sld put_name; 0x23e2..0x249a of /bin/ld) */
int16_t put_name(char *p)
{
	register char *q;
	register int16_t n;
	register int16_t k;
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
				error(0, "Warning - only 8 characters of name are significant");
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
		putc(*q, stderr);
		k++;
		q++;
	}
	if (flag) {
		fprintf(stderr, "...");
		k += 3;
	}
	return k;
}
