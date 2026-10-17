/* @[$] execvp.c	2.1  09/12/83 11:23:54 - 87wega3.2 */

/*
 *	execvp(name, argv)	(like execv, but does path search)
 *	(execlp is a separate member, see execlp.s)
 */
#include <errno.h>
#define	NULL	0

static	char shell[] =	"/bin/sh";
static	char cshell[] =	"/bin/csh";
char	*execat(), *getenv(), *index();
char	*whichshell();
extern	errno;

execvp(name, argv)
char *name, **argv;
{
	char *pathstr;
	char *sh;
	register char *cp;
	char fname[128];
	char *newargs[256];
	int i;
	register unsigned etxtbsy = 1;
	register eacces = 0;

	if ((pathstr = getenv("PATH")) == NULL)
		pathstr = ":/bin:/usr/bin";
	cp = index(name, '/')? "": pathstr;

	do {
		cp = execat(cp, name, fname);
	retry:
		execv(fname, argv);
		switch(errno) {
		case ENOEXEC:
			sh = whichshell(fname);
			if (sh == shell)
				newargs[0] = "sh";
			else
				newargs[0] = "csh";
			newargs[1] = fname;
			for (i=1; newargs[i+1]=argv[i]; i++) {
				if (i>=254) {
					errno = E2BIG;
					return(-1);
				}
			}
			execv(sh, newargs);
			return(-1);
		case ETXTBSY:
			if (++etxtbsy > 5)
				return(-1);
			sleep(etxtbsy);
			goto retry;
		case EACCES:
			eacces++;
			break;
		case ENOMEM:
		case E2BIG:
			return(-1);
		}
	} while (cp);
	if (eacces)
		errno = EACCES;
	return(-1);
}

static char *
execat(s1, s2, si)
register char *s1, *s2;
char *si;
{
	register char *s;

	s = si;
	while (*s1 && *s1 != ':' && *s1 != '-')
		*s++ = *s1++;
	if (si != s)
		*s++ = '/';
	while (*s2)
		*s++ = *s2++;
	*s = '\0';
	return(*s1? ++s1: 0);
}

static char *
whichshell(fname)
register char *fname;
{
	register fd;
	char c;

	fd = open(fname, 0);
	if (fd < 0) {
		close(fd);
		return(shell);
	}
	read(fd, &c, 1);
	close(fd);
	if (c == '#')
		return(cshell);
	return(shell);
}
