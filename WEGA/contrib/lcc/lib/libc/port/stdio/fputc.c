#include <stdio.h>

int
fputc(c, fp)
int	c;
register FILE *fp;
{
	return(putc(c, fp));
}
