# include	<stdio.h>
# include	"../amx_lib/AA_liba.h"

# define	off(x)		i = ((char *) x) - ((char *) d)
# define	out(arg)	printf("%4d %06o #%04x:\t%s\n", i, i, i, arg)

main()
{
	register DESCXX		*d;
	register int		i;
	DESCXX			desc;
	char			buf[BUFSIZ];

	setbuf(stdout, buf);
	printf("struct descxx   ==   DESCXX [amx_lib/AA_liba.h]\n");
	d = &desc;
	off(&d->xx_desc);
	out("xx_desc (DESC)");
	off(&d->xx_desc.reldum);
	out("xx_desc.reldum (struct relation)");
	off(d->xx_desc.reldum.relid);
	out("xx_desc.reldum.relid (char [MAXNAME])");
	off(d->xx_desc.reldum.relowner);
	out("xx_desc.reldum.relowner (char [2])");
	off(&d->xx_desc.reldum.relspec);
	out("xx_desc.reldum.relspec (char)");
	off(&d->xx_desc.reldum.relindxd);
	out("xx_desc.reldum.relindxd (char)");
# ifndef BIT_32
	off(&d->xx_desc.reldum.relstat);
	out("xx_desc.reldum.relstat (short)");
# endif
	off(&d->xx_desc.reldum.relsave);
	out("xx_desc.reldum.relsave (long)");
	off(&d->xx_desc.reldum.reltups);
	out("xx_desc.reldum.reltups (long)");
	off(&d->xx_desc.reldum.relatts);
	out("xx_desc.reldum.relatts (short)");
	off(&d->xx_desc.reldum.relwid);
	out("xx_desc.reldum.relwid (short)");
	off(&d->xx_desc.reldum.relprim);
	out("xx_desc.reldum.relprim (long)");
	off(&d->xx_desc.reldum.relspare);
	out("xx_desc.reldum.relspare (long)");
# ifdef BIT_32
	off(&d->xx_desc.reldum.relstat);
	out("xx_desc.reldum.relstat (short)");
# endif
	off(&d->xx_desc.relfp);
	out("xx_desc.relfp (short)");
	off(&d->xx_desc.relopn);
	out("xx_desc.relopn (short)");
	off(&d->xx_desc.reltid);
	out("xx_desc.reltid (long)");
	off(&d->xx_desc.reladds);
	out("xx_desc.reladds (long)");
	off(d->xx_desc.reloff);
	out("xx_desc.reloff (short [MAXDOM])");
	off(d->xx_desc.relfrmt);
	out("xx_desc.relfrmt (char [MAXDOM])");
	off(d->xx_desc.relfrml);
	out("xx_desc.relfrml (char [MAXDOM])");
	off(d->xx_desc.relxtra);
	out("xx_desc.relxtra (char [MAXDOM])");
	off(d->xx_desc.relgiven);
	out("xx_desc.relgiven (char [MAXDOM])");
	off(&d->xx_next);
	out("xx_next (DESCXX *)");
	off(&d->xx_index);
	out("xx_index (DESCXX *)");
	off(&d->xx_indom);
	out("xx_indom (char *)");
	off(&d->xx_ta);
	out("xx_ta (DESCXX *)");
	off(&d->xx_refresh);
	out("xx_refresh (char)");
	off(&d->xx_excl);
	out("xx_excl (char)");
	off(&d->xx_locks);
	out("xx_locks (short)");
	fflush(stdout);
}
