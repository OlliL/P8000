# include	"AA_liba.h"
# include	<signal.h>

int		AA;
long		AAdaemon;
long		AA_TID;
DESCXX		*AA_TA;
DESCXX		*AA_TE;
int		AA_NOLOCKS;
int		AA_LOCKS;
int		(*AAMAIN)();
int		(*AAEXIT)();
DESCXX		*AA_DESC;

# ifdef MSDOS
/* ALL INDES catalog names */
char		AA_REL[]	= "relation";
char		AA_ATT[]	= "domain";
char		AA_INDEX[]	= "indexes";
# endif

AAmain(xbufs, XBUFS, xlocks)
ACCBUF 	*xbufs;
int	XBUFS;
int	xlocks;
{
	register struct tup_id	*tid;
	register int		dbconcur;
	register int		dbexcl;
	long			page_id;
	static DESCXX		rel_desc;
	extern int		exit();

# ifdef AMX_TRACE
# ifdef MSDOS
	AAtTfp(22, 0, "database\t`%s'\n", AAdatabase);
# else
	extern char		*AAcustomer();

	AAtTfp(22, 0, "database\t`%s' on %s\n", AAdbpath, AAcustomer(exit));
# endif
# endif

	tid = (struct tup_id *) &AA_LOW;
	page_id = 0L;
	AAstuff_page(tid, &page_id);
	tid->line_id = SCANTID;

	tid = (struct tup_id *) &AA_HIGH;
	page_id = -1L;
	AAstuff_page(tid, &page_id);
	tid->line_id = SCANTID;

	/* catch interrupts */
	signal(SIGINT, exit);
# ifndef MSDOS
	signal(SIGQUIT, exit);
	signal(SIGTERM, exit);
# endif

	if (AAMAIN)
		(*AAMAIN)();

	/* open data base */
	AAam_start();

# ifdef AMX_TRACE
	AAtTfp(22, 1, "\t\tlock want 0%o got 0%o\n", xlocks, bitset(A_DBCONCUR, AAdmin.adhdr.adflags));
# endif

	dbconcur = bitset(A_DBCONCUR, xlocks);
	if (dbconcur != bitset(A_DBCONCUR, AAdmin.adhdr.adflags))
# ifdef MSDOS
		AAerror(107, AAdatabase);
# else
		AAerror(107, AAdbpath);
# endif

	dbexcl = !bitset(DB_SHARE, xlocks);
	AA_NOLOCKS = dbexcl || !dbconcur;
	if (dbconcur && AAsetdbl(A_RTN, dbexcl? M_EXCL: M_SHARE) < 0)
# ifdef MSDOS
		AAerror(100, AAdatabase);
# else
		AAerror(100, AAdbpath);
# endif

# ifdef AMX_TRACE
	AAtTfp(22, 2, "\t\tworking %s on %s database\n",
		AA_NOLOCKS? "exclusive": "shared",
		dbconcur? "concurrent": "private");
# endif

	AA_DESC = &rel_desc;
	AA_DESC->xx_next = (DESCXX *) 0;
	AA_rel = &rel_desc.xx_desc;
	AArelopen(AA_rel, 2, AA_REL);
	AAopenr(&AA_att, AA_ATT);
	AAopenr(&AA_ind, AA_INDEX);

	/* OK, this is cheet */
	close(AAdmin.adreld.relfp);
	AAdmin.adreld.relfp = AA_rel->relfp;
	AAdmin.adreld.relopn = AA_rel->relopn;
	close(AAdmin.adattd.relfp);
	AAdmin.adattd.relfp = AA_att->relfp;
	AAdmin.adattd.relopn = AA_att->relopn;

	if (XBUFS)
	{
# ifdef AMX_TRACE
		AAtTfp(22, 3, "\t\tadd %d bufs\n", XBUFS);
# endif
		AAam_addbuf(xbufs, XBUFS);
	}

# ifdef AMX_TRACE
	AAtTfp(22, 0, "----------------\n");
# endif

	return;
}


/*
**   AApmove    Packed Move
**
**	Moves string `s' to storage area `b' of length `l' bytes.
**	If `s' is too long, it is truncated, otherwise it is
**	padded to length `l' with character `pad'.
**	`b' after the transfer is returned.
*/

char	*AApmove(s, b, l, pad)
register char	*s;
register char	*b;
register int	l;
int		pad;
{
	register int	c;

	/* move up to `l' bytes */
	while (l > 0 && (c = *s++ & LAST_CHAR))
	{
		--l;
		if ((c < ' ' && c > '\r') || c == LAST_CHAR)
			c = ' ';	/* convert to blank */
		*b++ = c;
	}

	/* if we still have some `l', pad */
	while (l-- > 0)
		*b++ = pad;

	return (b);
}


# ifndef MSDOS
char	*AArelpath(rel)
register char	*rel;
{
	extern char	*AAztack();

	return (AAztack(AAztack(AAdbpath, "/"), rel));
}



static	AAdummy()
{
	AAerrfn();
}
# endif
