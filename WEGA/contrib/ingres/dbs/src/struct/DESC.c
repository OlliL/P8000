# include	<stdio.h>
# include	"../conf/conf.h"
# include	"../h/ing_db.h"

# define	off(x)		i = ((char *) x) - ((char *) d)
# define	out(arg)	printf("%4d %06o #%04x:\t%s\n", i, i, i, arg)

main()
{
	register DESC		*d;
	register int		i;
	DESC			desc;
	char			buf[BUFSIZ];

	setbuf(stdout, buf);
	printf("struct descriptor   ==   DESC [h/ing_db.h]\n");
	d = &desc;
	off(&d->reldum);
	out("reldum (struct relation)");
	off(d->reldum.relid);
	out("reldum.relid (char [MAXNAME])");
	off(d->reldum.relowner);
	out("reldum.relowner (char [2])");
	off(&d->reldum.relspec);
	out("reldum.relspec (char)");
	off(&d->reldum.relindxd);
	out("reldum.relindxd (char)");
# ifndef BIT_32
	off(&d->reldum.relstat);
	out("reldum.relstat (short)");
# endif
	off(&d->reldum.relsave);
	out("reldum.relsave (long)");
	off(&d->reldum.reltups);
	out("reldum.reltups (long)");
	off(&d->reldum.relatts);
	out("reldum.relatts (short)");
	off(&d->reldum.relwid);
	out("reldum.relwid (short)");
	off(&d->reldum.relprim);
	out("reldum.relprim (long)");
	off(&d->reldum.relspare);
	out("reldum.relspare (long)");
# ifdef BIT_32
	off(&d->reldum.relstat);
	out("reldum.relstat (short)");
# endif
	off(&d->relfp);
	out("relfp (short)");
	off(&d->relopn);
	out("relopn (short)");
	off(&d->reltid);
	out("reltid (long)");
	off(&d->reladds);
	out("reladds (long)");
	off(d->reloff);
	out("reloff (short [MAXDOM])");
	off(d->relfrmt);
	out("relfrmt (char [MAXDOM])");
	off(d->relfrml);
	out("relfrml (char [MAXDOM])");
	off(d->relxtra);
	out("relxtra (char [MAXDOM])");
	off(d->relgiven);
	out("relgiven (char [MAXDOM])");
	fflush(stdout);
}
