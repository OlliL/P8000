/* @[$] puts.c	2.1  09/12/83 11:15:19 - 87wega3.2 */

#include	<stdio.h>

puts(s)
register char *s;
{
	register c;

	while (c = *s++)
		putchar(c);
	return(putchar('\n'));
}
