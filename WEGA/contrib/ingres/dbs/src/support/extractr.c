# include	"dmp.h"

# define	D_RELS		100		/* max # of relations	*/
static long		Rtid[D_RELS];		/* TID of relation tup	*/
static int		Rlock[D_RELS];		/* lock flag		*/
static char		Pagebuf[PGSIZE];	/* page buffer		*/

main(argc, argv)
int	argc;
char 	*argv[];
{
	register int		i;
	register char		**av;
	register char		*p;
	extern char		*AAproc_name;

	AAproc_name = "EXTRACTR";

#	ifdef xSTR1
	AAtTrace(&argc, argv, 'X');
#	endif

	i = initucode(argc, argv, TRUE, (char **) 0, M_SHARE);
	switch (i)
	{
	  case 0:
	  case 5:
		break;

	  case 1:
	  case 6:
		AAsyserr(0, "Die Datenbank '%s' existiert nicht", Parmvect[0]);

	  case 2:
		AAsyserr(0, "Sie duerfen auf die Datenbank '%s' nicht zugreifen", Parmvect[0]);

	  case 3:
		AAsyserr(0, "Sie sind kein eingetragener DBS-Nutzer");

	  case 4:
		printf("Sie haben keinen Datenbanknamen angegeben\n");
	usage:
		AAsyserr(0, "HINWEIS: extractr [flags] db rel file");

	  default:
		AAsyserr(22200, AAproc_name, i);
	}

	Hdr.d_status = D_32 | D_WORDS | D_BYTES;
	i = 0;
	for (av = Flagvect; p = *av; av++)
	{
		if (p[0] != '-')
			goto badflag;
		switch (p[1])
		{
		  case 'd':
		  case 'D':
			D_flag++;
			break;

		  case 'q':
		  case 'Q':
			Hdr.d_status |= D_QRYMOD;
			No_qm = D_NOQM;
			break;

		  default:
badflag:
			printf("Unbekanntes Flag '%s'\n", p);
			i++;
		}
	}
	if (i)
	{
		AAunlall();
		fflush(stdout);
		return (-1);
	}

	if (!Parmvect[1])
	{
		printf("Sie haben keinen Relationsnamen angegeben\n");
		goto usage;
	}
	if (!Parmvect[2])
	{
		printf("Sie haben keinen Filenamen angegeben\n");
		goto usage;
	}

	/* initialize access methods (and AAdmin struct) for user_ovrd test */
	AAam_start();
	if (!(AAdmin.adhdr.adflags & A_QRYMOD))
		No_qm = D_NOQM;

	i = extr();
	if (!i && D_flag)
		i = des(Parmvect[1]);

	/* logout database */
	unlk(i);
	AAunldb();
	AAam_exit();
	fflush(stdout);
	return (i);
}

static int	extr()
{
	register DESC		*d;
	register int		i;
	struct relation		*key;
	struct relation		*tup;
	long			reltid;
	struct relation		relkey;
	struct relation		reltup;

	if (i = op_cat())
		goto term;

	d = &AAdmin.adreld;
	/* make believe catalog AA_REL is read only for concurrency	*/
	/* that means, readed pages are not locked			*/
	d->relopn = RELROPEN(d->relfp);
	AAam_clearkeys(d);
	AAam_setkey(d, key = &relkey, Parmvect[1], RELID);
	AAam_setkey(d, key, AAusercode, RELOWNER);
	i = AAgetequal(d, key, tup = &reltup, &reltid);
	/* free all buffers connected with catalog AA_REL */
	AAam_flush_rel(d, TRUE);
	/* restore catalog AA_REL to read/write mode */
	d->relopn = RELWOPEN(d->relfp);
	if (i)
	{
		if (i < 0)
			AAsyserr(21182, Parmvect[1], i);
		else
			i = 5900;	/* non-existant relation */
	}
	else if (tup->relstat & S_VIEW)
		i = 5901;
	else if (tup->relstat & S_CATALOG)
		i = 5902;
	else if (ctoi(tup->relindxd) == SECINDEX)
		i = 5903;
	if (i)
		goto term;

	close(creat(Parmvect[2], 0644));
	if ((Fh = open(Parmvect[2], 2)) < 0 || (Fp = open(Parmvect[2], 2)) < 0)
		i = 5904;
	else
		i = init_extr(ctoi(tup->relindxd) == SECBASE);

	close(Fh);
	close(Fp);
term:
	if (i)
		error(i, Parmvect[1], Parmvect[2], (char *) 0);
	return (i);
}

static int	init_extr(hasindx)
int		hasindx;
{
	register struct d_hdr	*h;
	register DESC		*d;
	register int		i;
	register int		rel;
	struct index		indkey;
	struct index		indtup;
	long			lowtid;
	long			hightid;
	extern long		time();
	extern int		write();
	extern char		*AAbmove();

	AAbmove(&AAdmin.adhdr, &(h = &Hdr)->d_admin, sizeof (struct adminhdr));
	AAbmove(AAusercode, h->d_owner, 2);
	/* h->d_status already filled */
	h->d_date = time((char *) 0);
	AApmove(Parmvect[0], h->d_db, MAXNAME, ' ');
	h->d_db[MAXNAME] = 0;
	h->d_db[MAXNAME + 1] = 0;
	h->d_rels = 0;
	if (i = init_rel(Parmvect[1]))
		return (i);

	if (hasindx)
	{	/* relation has indexes  */
		d = Cdesc[D_INDEX];
		AAam_clearkeys(d);
		AAam_setkey(d, &indkey, Parmvect[1], IRELIDP);
		AAam_setkey(d, &indkey, AAusercode, IOWNERP);
		if (AAam_find(d, EXACTKEY, &lowtid, &hightid, &indkey))
			AAsyserr(21180);
		while (!(i = AAam_get(d, &lowtid, &hightid, &indtup, TRUE)))
		{
			if (AAkcompare(d, &indtup, &indkey))
				continue;
			init_rel(indtup.irelidi);
		}
		if (i < 0)
			AAsyserr(21181);
	}

	if (i = rw_hdr(write))
		return (i);

	for (rel = 0; rel < h->d_rels; rel++)
		if (i = do_extr(rel))
			return (i);
	return (0);
}

static int	do_extr(rel)
int		rel;
{
	register DESC		*d;
	register char		*tup;
	register char		*key;
	register int		cat;
	register int		i;
	union d_tup		keytup;
	long			lowtid;
	long			hightid;
	extern int		read();
	extern int		write();

	if (i = rw_rel(rel, read))
		return (i);
	if (i = eof(&Rel.d_tupoff))
		return (i);
	key = (char *) &keytup;
	for (tup = (char *) &Tup + sizeof (union d_tup); tup > (char *) &Tup; )
		*(--tup) = 0;
	for (cat = D_REL; cat < No_qm; cat++)
	{
		d = Cdesc[cat];
		AAam_clearkeys(d);
		AAam_setkey(d, key, Rel.d_relid, 1);
		AAam_setkey(d, key, AAusercode, 2);
		if (AAam_find(d, EXACTKEY, &lowtid, &hightid, key))
			AAsyserr(21180);
		while (!(i = AAam_get(d, &lowtid, &hightid, tup, TRUE)))
		{
			if (AAkcompare(d, tup, key))
				continue;
			if (cat == D_REL)
			{
				Rtid[rel] = lowtid;
				if (i = lk_rel(rel))
					return (i);
				if (Hdr.d_status & D_QRYMOD)
					((struct relation *) tup)->relstat &= ~(S_PROTUPS | S_INTEG);
			}
			w_tup(cat);
		}
		if (i < 0)
			AAsyserr(21181);
	}
	if (i = w_page())
		return (i);
	if (i = rw_rel(rel, write))
		return (i);
	return (0);
}

static int	init_rel(rel)
register char	*rel;
{
	register struct d_rel	*r;
	extern int		write();

	AApmove(rel, (r = &Rel)->d_relid, MAXNAME, ' ');
	r->d_relid[MAXNAME] = 0;
	r->d_relid[MAXNAME + 1] = 0;
	r->d_ntups = 0;
	r->d_tupoff = 0;
	r->d_npages = 0;
	r->d_pageoff = 0;
	if (Hdr.d_rels >= D_RELS - 1)
		return (5916);
	return (rw_rel(Hdr.d_rels++, write));
}

static int	w_tup(cat)
short		cat;
{
	short			len;

	if (write(Fp, &cat, sizeof (short)) != sizeof (short))
		return (5909);
	len = Clen[cat];
	if (write(Fp, &len, sizeof (short)) != sizeof (short))
		return (5909);
	if (write(Fp, &Tup, len) != len)
		return (5910);
	Rel.d_ntups++;
	return (0);
}

static int	w_page()
{
	register int		f;
	register int		i;
	char			filename[MAXNAME + 3];
	extern char		*AArelpath();

	AAdbname(Rel.d_relid, AAusercode, filename);
	if ((f = open(AArelpath(filename), 0)) < 0)
		return (5914);
	if (i = eof(&Rel.d_pageoff))
		return (i);
	while ((i = read(f, Pagebuf, AApgsize)) == AApgsize)
	{
		if (write(Fp, Pagebuf, AApgsize) != AApgsize)
			return (5912);
		Rel.d_npages++;
	}
	if (i)
		return (5911);
	return (0);
}

static int	eof(offset)
register long	*offset;
{
	register long		off;
	extern long		lseek();

	if ((off = lseek(Fp, 0L, 2)) < 0L)
		goto err;
	off += BUFSIZ - 1;
	off /= BUFSIZ;
	off *= BUFSIZ;
	if (lseek(Fp, off, 0) != off)
	{
err:
		return (5913);
	}
	*offset = off;
	return (0);
}

/*
** LK_REL	lock neede relations
*/
static int	lk_rel(rel)
register int	rel;
{
	register int		i;
	register int		pl;	/* problem lock */

loop:
	pl = -1;
	for (i = 0; i <= rel; i++)
	{
		if (Rlock[i])
			continue;
		if (AAsetrll(A_RTN, Rtid[i], M_SHARE) < 0)
		{
			pl = i;
			break;
		}
		Rlock[i]++;
	}
	if (pl < 0)
		return (0);
	for (i = 0; i <= rel; i++)
	{
		if (Rlock[i])
		{
			AAunlrl(Rtid[i]);
			Rlock[i] = 0;
		}
	}
	if (AAsetrll(A_SLP, Rtid[pl], M_SHARE) < 0)
		return (5915);
	Rlock[pl]++;
	goto loop;
}

static int	unlk(rm)
register int	rm;
{
	register int		i;

	for (i = 0; i < D_RELS; i++)
		if (Rlock[i])
			AAunlrl(Rtid[i]);	/* release relation lock */
	cl_cat(rm);
}

rubproc()
{
	unlk(1);
	AAunlall();
	fflush(stdout);
	exit(1);
}
