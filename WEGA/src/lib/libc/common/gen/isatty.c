/* @[$] isatty.c	2.1  09/12/83 11:25:21 - 87wega3.2 */

/*
 * Returns 1 iff file is a tty
 */

#include <sgtty.h>

isatty(f)
{
	struct sgttyb ttyb;

	if (gtty(f, &ttyb) < 0)
		return(0);
	return(1);
}
