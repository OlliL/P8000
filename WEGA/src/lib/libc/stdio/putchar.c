/* @[$] putchar.c	2.1  09/12/83 11:15:05 - 87wega3.2 */

/*
 * A subroutine version of the macro putchar
 */
#include <stdio.h>

#undef putchar

putchar(c)
register c;
{
	putc(c, stdout);
}
