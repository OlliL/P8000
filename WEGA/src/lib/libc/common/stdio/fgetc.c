/* @[$] fgetc.c	2.1  09/12/83 11:07:36 - 87wega3.2 */

#include <stdio.h>

fgetc(fp)
FILE *fp;
{
	return(getc(fp));
}
