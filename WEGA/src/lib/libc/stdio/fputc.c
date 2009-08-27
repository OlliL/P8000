/* @[$] fputc.c	2.1  09/12/83 11:10:06 - 87wega3.2 */

#include <stdio.h>

fputc(c, fp)
FILE *fp;
{
	return(putc(c, fp));
}
