# include	<stdio.h>
# include	"../conf/conf.h"
# include	"../h/ing_db.h"
# include	"../h/access.h"

# define	off(x)		i = ((char *) x) - ((char *) d)
# define	out(arg)	printf("%4d %06o #%04x:\t%s\n", i, i, i, arg)

main()
{
	register struct admin		*d;
	register int			i;
	struct admin			desc;
	char				buf[BUFSIZ];

	setbuf(stdout, buf);
	printf("struct admin [h/access.h]\n");
	d = &desc;
	off(&d->adhdr);
	out("adhdr (struct adminhdr)");
	off(d->adhdr.adowner);
	out("adhdr.adowner (char [2])");
	off(&d->adhdr.adflags);
	out("adhdr.adflags (short)");
	off(&d->adreld);
	out("adreld (struct descriptor)");
	off(&d->adreld.reldum);
	out("adreld.reldum (struct relation)");
	off(d->adreld.reldum.relid);
	out("adreld.reldum.relid (char [MAXNAME])");
	off(d->adreld.reldum.relowner);
	out("adreld.reldum.relowner (char [2])");
	off(&d->adreld.reldum.relspec);
	out("adreld.reldum.relspec (char)");
	off(&d->adreld.reldum.relindxd);
	out("adreld.reldum.relindxd (char)");
# ifndef BIT_32
	off(&d->adreld.reldum.relstat);
	out("adreld.reldum.relstat (short)");
# endif
	off(&d->adreld.reldum.relsave);
	out("adreld.reldum.relsave (long)");
	off(&d->adreld.reldum.reltups);
	out("adreld.reldum.reltups (long)");
	off(&d->adreld.reldum.relatts);
	out("adreld.reldum.relatts (short)");
	off(&d->adreld.reldum.relwid);
	out("adreld.reldum.relwid (short)");
	off(&d->adreld.reldum.relprim);
	out("adreld.reldum.relprim (long)");
	off(&d->adreld.reldum.relspare);
	out("adreld.reldum.relspare (long)");
# ifdef BIT_32
	off(&d->adreld.reldum.relstat);
	out("adreld.reldum.relstat (short)");
# endif
	off(&d->adreld.relfp);
	out("adreld.relfp (short)");
	off(&d->adreld.relopn);
	out("adreld.relopn (short)");
	off(&d->adreld.reltid);
	out("adreld.reltid (long)");
	off(&d->adreld.reladds);
	out("adreld.reladds (long)");
	off(d->adreld.reloff);
	out("adreld.reloff (short [MAXDOM])");
	off(d->adreld.relfrmt);
	out("adreld.relfrmt (char [MAXDOM])");
	off(d->adreld.relfrml);
	out("adreld.relfrml (char [MAXDOM])");
	off(d->adreld.relxtra);
	out("adreld.relxtra (char [MAXDOM])");
	off(d->adreld.relgiven);
	out("adreld.relgiven (char [MAXDOM])");
	off(&d->adattd);
	out("adattd (struct descriptor)");
	off(&d->adattd.reldum);
	out("adattd.reldum (struct relation)");
	off(d->adattd.reldum.relid);
	out("adattd.reldum.relid (char [MAXNAME])");
	off(d->adattd.reldum.relowner);
	out("adattd.reldum.relowner (char [2])");
	off(&d->adattd.reldum.relspec);
	out("adattd.reldum.relspec (char)");
	off(&d->adattd.reldum.relindxd);
	out("adattd.reldum.relindxd (char)");
# ifndef BIT_32
	off(&d->adattd.reldum.relstat);
	out("adattd.reldum.relstat (short)");
# endif
	off(&d->adattd.reldum.relsave);
	out("adattd.reldum.relsave (long)");
	off(&d->adattd.reldum.reltups);
	out("adattd.reldum.reltups (long)");
	off(&d->adattd.reldum.relatts);
	out("adattd.reldum.relatts (short)");
	off(&d->adattd.reldum.relwid);
	out("adattd.reldum.relwid (short)");
	off(&d->adattd.reldum.relprim);
	out("adattd.reldum.relprim (long)");
	off(&d->adattd.reldum.relspare);
	out("adattd.reldum.relspare (long)");
# ifdef BIT_32
	off(&d->adattd.reldum.relstat);
	out("adattd.reldum.relstat (short)");
# endif
	off(&d->adattd.relfp);
	out("adattd.relfp (short)");
	off(&d->adattd.relopn);
	out("adattd.relopn (short)");
	off(&d->adattd.reltid);
	out("adattd.reltid (long)");
	off(&d->adattd.reladds);
	out("adattd.reladds (long)");
	off(d->adattd.reloff);
	out("adattd.reloff (short [MAXDOM])");
	off(d->adattd.relfrmt);
	out("adattd.relfrmt (char [MAXDOM])");
	off(d->adattd.relfrml);
	out("adattd.relfrml (char [MAXDOM])");
	off(d->adattd.relxtra);
	out("adattd.relxtra (char [MAXDOM])");
	off(d->adattd.relgiven);
	out("adattd.relgiven (char [MAXDOM])");
	fflush(stdout);
}
