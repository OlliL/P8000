/* getlogin of V7 (ttyslot() == 0, no close when the read fails, no test of an empty name): su (02/83) */
#include <utmp.h>

#define NULL 0

static	char	UTMP[]	= "/etc/utmp";
static	struct	utmp ubuf;

char *
getlogin()
{
	register me, uf;
	register char *cp;

	if((me = ttyslot()) == 0)
		return NULL;
	if((uf = open(UTMP, 0)) < 0)
		return NULL;
	lseek(uf, (long)(me * sizeof ubuf), 0);
	if (read (uf, (char *) &ubuf, sizeof ubuf) != sizeof ubuf)
		return NULL;
	close (uf);
	ubuf.ut_name[8] = ' ';
	for (cp=ubuf.ut_name; *cp++!=' ';)
		;
	*--cp = '\0';
	return ubuf.ut_name;
}
