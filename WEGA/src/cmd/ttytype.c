/*
 * ttytype.c: the terminal type of a tty from /etc/ttytype, for login (WEGA).
 * A file of its own: login calls gettype with call, not calr (the name of the file is a guess).
 * getcl is not called; it brings in termcap (tgetent, tgetstr).
 */

#include <stdio.h>

/* the type of the terminal TTY from /etc/ttytype */
char *
typeof(tty)
char *tty;
{
	FILE *f;
	static char type[8];
	static char name[20];

	if ((f = fopen("/etc/ttytype", "r")) != NULL)
		while (fscanf(f, "%s%s", type, name) != EOF)
			if (strcmp(tty, name) == 0) {
				fclose(f);
				return(type);
			}
	fclose(f);
	return("un");
}

char *
gettype(ttyn)
char *ttyn;
{
	char *p;

	p = strrchr(ttyn, '/') + 1;
	return(typeof(p));
}

/* the clear string of the terminal TYPE (not called) */
char *
getcl(type)
char *type;
{
	char buf[1024];
	char area[100];
	char *ap, *cp;
	char *tgetstr();

	if (tgetent(buf, type) > 0) {
		ap = area;
		if ((cp = tgetstr("cl", &ap)) == 0)
			return("\n");
		return(cp);
	}
	return("\n");
}
