/*
 * printname.c - part of sld (WEGA 3.1), reconstructed from /bin/sld; the code range is in tools/module_ranges.txt.
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

static char sccsid[] __attribute__((unused)) = "@[$]printname.c\t2.2  10/27/83 12:24:42 - 87wega3.2";
int16_t pnflag = 0;

/* put_name: print a symbol name (0x6522..0x65c6 of /bin/sld) */
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
