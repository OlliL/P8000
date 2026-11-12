/* @[$] stty.c	2.1  09/12/83 11:33:13 - 87wega3.2 */

/*
 * Writearound to old stty and gtty system calls
 */

#include <sgtty.h>

stty(fd, ap)
struct sgtty *ap;
{
	return(ioctl(fd, TIOCSETP, ap));
}

gtty(fd, ap)
struct sgtty *ap;
{
	return(ioctl(fd, TIOCGETP, ap));
}
