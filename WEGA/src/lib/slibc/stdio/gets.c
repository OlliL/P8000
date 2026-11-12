/* @[$] gets.c	2.1  09/12/83 11:13:49 - 87wega3.2 */

#include	<stdio.h>

char *
gets(s)
char *s;
{
	register c;
	register char *cs;

	cs = s;
	while ((c = getchar()) != '\n' && c >= 0)
		*cs++ = c;
	if (c<0 && cs==s)
		return(NULL);
	*cs++ = '\0';
	return(s);
}
