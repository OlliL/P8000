/* @[$] ctermid.c	2.1  09/12/83 11:17:07 - 87wega3.2 */

#include <stdio.h>

char *ctermid(s)
	char *s;
{
	static char res[L_ctermid];
	register char *r;

	r = s==NULL? res: s;
	strcpy (r, "/dev/tty");
	return r;
}
