/* @[$] fopen.c	2.2  09/12/83 11:09:29 - 87wega3.2. */

#include	<stdio.h>

FILE *
fopen(file, mode)
	char *file, *mode;
{
	FILE *_findiop(), *_endopen();

	return(_endopen(file, mode, _findiop()));
}
