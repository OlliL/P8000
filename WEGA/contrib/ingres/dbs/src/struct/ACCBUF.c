# include	<stdio.h>
# include	"../conf/conf.h"
# include	"../h/ing_db.h"
# include	"../h/access.h"

# define	off(x)		i = ((char *) x) - ((char *) d)
# define	out(arg)	printf("%4d %06o #%04x:\t%s\n", i, i, i, arg)

main()
{
	register ACCBUF		*d;
	register int		i;
	ACCBUF			desc;
	char			buf[BUFSIZ];

	setbuf(stdout, buf);
	printf("struct accbuf   ==   ACCBUF [h/access.h]\n");
	d = &desc;
	off(&d->mainpg);
	out("mainpg (long)");
	off(&d->ovflopg);
	out("ovflopg (long)");
	off(&d->nxtlino);
	out("nxtlino (short)");
	off(d->firstup);
	out("firstup (char [PGSIZE - PGHDR])");
	off(d->linetab);
	out("linetab (short [1])");
	off(&d->rel_tupid);
	out("rel_tupid (long)");
	off(&d->thispage);
	out("thispage (long)");
	off(&d->filedesc);
	out("filedesc (int)");
	off(&d->modf);
	out("modf (struct accbuf *)");
	off(&d->modb);
	out("modb (struct accbuf *)");
	off(&d->bufstatus);
	out("bufstatus (int)");
	fflush(stdout);
}
