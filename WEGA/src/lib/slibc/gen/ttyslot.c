/* @[$] ttyslot.c	2.2  09/12/83 11:35:07 - 87wega3.2 */

/*
 * Return the number of the slot in the utmp file
 * corresponding to the current user: try for file 0, 1, 2.
 * Returns -1 if slot not found.
 */

#include <utmp.h>

char	*ttyname();
char	*strrchr();

static	char	utmp[]	= "/etc/utmp";

#define	NULL	0

ttyslot()
{
	register char *tp, *p;
	register int s, fd;

	struct utmp ubuf;

	if ((tp=ttyname(0))==NULL
 	 && (tp=ttyname(1))==NULL
	 && (tp=ttyname(2))==NULL)
		return -1;

	if ((p = strrchr(tp, '/')) == NULL)
		p = tp;
	else
		p++;

	if ((fd=open(utmp, 0)) < 0)
		return -1;
	s = 0;
	while (read (fd, (char *) &ubuf, sizeof ubuf) == sizeof ubuf) {
		if (strncmp (p, ubuf.ut_line, sizeof ubuf.ut_line)==0) {
			close(fd);
			return s;
		}
		s++;
	}
	close(fd);
	return -1;
}
