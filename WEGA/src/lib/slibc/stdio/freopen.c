/* @[$] freopen.c	2.2 09/12/83 11:10:40 -      , Inc */

#include <stdio.h>

FILE *
freopen(file, mode, iop)
	char *file, *mode;
	register FILE *iop;
{
	FILE *_endopen();

	fclose(iop);
	return(_endopen(file, mode, iop));
}
