#include <stdio.h>
#include "ld.h"

int pnflag;

/* put_name: print a symbol name (sld put_name; 0x23e2..0x249a of /bin/ld) */
put_name(p)
char *p;
{
	int spare1, spare2;
	char spare3;
	char flag;
	register char *q;
	register int n;
	register int k;

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
		(--stderr->_cnt >= 0 ? (int)(char)(*stderr->_ptr++ = *q) : _flsbuf((unsigned)(*q), stderr));
		k++;
		q++;
	}
	if (flag) {
		fprintf(stderr, "...");
		k += 3;
	}
	return k;
}
