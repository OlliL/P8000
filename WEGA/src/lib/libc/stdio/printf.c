/* @[$] printf.c	2.4  09/12/83 11:14:45 - 87wega3.2 */

#include	<stdio.h>

printf(fmt, args)
char *fmt;
{
	_doprnt(fmt, &args, stdout);
	return(ferror(stdout)? EOF: 0);
}
