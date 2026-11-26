static char sccsid[] = "@[$]setbrk.c\t2.1  08/06/83 19:36:49 - 87wega3.2";
#
/*
 *	UNIX shell
 *
 *	S. R. Bourne
 *	Bell Telephone Laboratories
 *
 */

#include	"defs.h"

setbrk(incr)
{
	REG BYTPTR	a=(STRING)(sbrk(incr));
	brkend=a+incr;
	return((INT)(a));
}
