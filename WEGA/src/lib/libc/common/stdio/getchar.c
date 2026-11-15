/* @[$] getchar.c	2.1  09/12/83 11:11:25 - 87wega3.2 */

/*
 * A subroutine version of the macro getchar.
 */
#include <stdio.h>

#undef getchar

getchar()
{
	return(getc(stdin));
}
