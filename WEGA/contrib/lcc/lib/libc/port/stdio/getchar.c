/*
 * A subroutine version of the macro getchar.
 */
#include <stdio.h>
#undef getchar

int
getchar()
{
	return(getc(stdin));
}
