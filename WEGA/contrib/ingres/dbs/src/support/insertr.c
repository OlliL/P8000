# include	"dmp.h"

# define	D_RELS		100		/* max # of relations	*/
static int		L_flag;
static int		U_flag;
static char		*Parm;
static char		Pagebuf[PGSIZE];	/* page buffer		*/

main(argc, argv)
int	argc;
char 	*argv[];
{
	register int		i;
	register char		**av;
	register char		*p;
	register int		ret;
# ifdef SETBUF
	char			stdbuf[BUFSIZ];
# endif
	extern char		*AAproc_name;

	AAproc_name = "INSERTR";
# ifdef SETBUF
	setbuf(stdout, stdbuf);
# endif

#	ifdef xSTR1
	AAtTrace(&argc, argv, 'X');
#	endif

	i = initucode(argc, argv, TRUE, (char) 0, M_SHARE);
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
		AAsyserr(0, "HINWEIS: insertr [flags] db {file}");

	  default:
		AAsyserr(22200, AAproc_name, i);
	}

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

		  case 'l':
		  case 'L':
			L_flag = 1;
			break;

		  case 'q':
		  case 'Q':
			No_qm = D_NOQM;
			break;

		  case 'v':
		  case 'V':
			L_flag = -1;
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
		printf("Sie haben keinen Filenamen angegeben\n");
		goto usage;
	}

	/* initialize access methods (and AAdmin struct) for user_ovrd test */
	AAam_start();
	if (!(AAdmin.adhdr.adflags & A_QRYMOD))
		No_qm = D_NOQM;

	for (ret = 0, i = 1; Parmvect[1]; Parmvect[1] = Parmvect[++i])
		ret = ret & insr();

	/* logout database */
	if (L_flag <= 0)
		cl_cat(0);
	AAunldb();
	AAam_exit();
	fflush(stdout);
	return (ret);
}

static int	insr()
{
	register int		i;
	extern int		read();

	if ((Fh = open(Parmvect[1], 0)) < 0 || (Fp = open(Parmvect[1], 0)) < 0)
		i = 5920;
	else if (!(i = rw_hdr(read)))
		i = init_insr();

	close(Fh);
	close(Fp);
term:
	if (i)
	{
		if (L_flag)
			putchar('\n');
		error(i, Parmvect[1], Parm, (char *) 0);
	}
	return (i);
}

static int	init_insr()
{
	register struct d_hdr	*h;
	register int		i;
	register int		rel;
	extern char		*lookucode();
	extern char		*ctime();

	h = &Hdr;
# ifdef VPGSIZE
	/* get the page size of dump */
	i = BLKSIZ * ((h->d_admin.adflags >> 8) + 1);
# else
	i = PGSIZE;
# endif
	if (L_flag)
	{
		printf("RELATION DUMP\n");
		printf("\t** date\t\t%s",
# ifdef ESER_VMX
			ctime(h->d_date));
# else
			ctime(&h->d_date));
# endif
		printf("\t** qrymod\t%sdumped\n",
			(h->d_status & D_QRYMOD)? "not ": "");
		printf("\t** owner\t%s\n", lookucode(h->d_owner));
		printf("\t** user\t\t%s\n", lookucode(AAusercode));
		printf("FROM DB\n");
		printf("\t** database\t%s\n", h->d_db);
		printf("\t** dba\t\t%s\n", lookucode(h->d_admin.adowner));
		printf("\t** status\tconcurrency control o%s, ",
			(h->d_admin.adflags & A_DBCONCUR)? "n": "ff");
		printf("query modification o%s\n",
			(h->d_admin.adflags & A_QRYMOD)? "n": "ff");
		printf("\t** pagesize\t%d bytes\n", i);
		printf("FROM CPU\n");
		printf("\t**\t\t%d-bit, ",
			(h->d_status & X_32)? 32: 16);
		printf("%s word and ",
			(h->d_status & X_WORDS)? "reverse": "normal");
		printf("%s byte sequence\n",
			(h->d_status & X_BYTES)? "reverse": "normal");
		printf("TO DB\n");
		printf("\t** database\t%s\n", Parmvect[0]);
		printf("\t** dba\t\t%s\n", lookucode(AAdmin.adhdr.adowner));
		printf("\t** status\tconcurrency control o%s, ",
			(AAdmin.adhdr.adflags & A_DBCONCUR)? "n": "ff");
		printf("query modification o%s\n",
			(AAdmin.adhdr.adflags & A_QRYMOD)? "n": "ff");
		printf("\t** pagesize\t%d bytes\n", AApgsize);
		printf("TO CPU\n");
		printf("\t**\t\t%d-bit, ",
			D_32? 32: 16);
		printf("%s word and ",
			D_WORDS? "reverse": "normal");
		printf("%s byte sequence\n",
			D_BYTES? "reverse": "normal");
		printf("\t**\n");
		fflush(stdout);
	}
	if (h->d_status & D_QRYMOD)
		No_qm = D_NOQM;
	U_flag = !AAbequal(AAusercode, h->d_owner, 2);
	if (L_flag <= 0)
	{
		if ((h->d_status & D_MACHINE) != (D_32 | D_WORDS | D_BYTES))
			return (5931);
		if (AApgsize != i)
			return (5932);
		if (i = op_cat())
			return (i);
	}

	if (!init_rel(0))
	{
		if (D_flag)
		{
			if (des(Rel.d_relid))
				return (0);	/* destroy error */
		}
		else
			return (5922);
	}

	for (rel = 1; rel < h->d_rels; rel++)
		if (!init_rel(rel))
			return (5923);

	if (L_flag <= 0)
		for (rel = 0; rel < h->d_rels; rel++)
			if (i = do_insr(rel))
				return (i);

	return (0);
}

static int	do_insr(rel)
int		rel;
{
	register DESC		*d;
	register int		i;
	short			cat;
	long			tid;
	extern int		read();

	if ((i = rw_rel(rel, read)) || (i = pos(Rel.d_tupoff)))
		return (i);

	Parm = Rel.d_relid;
	rel = D_REL;
	while ((i = r_tup(&cat)) < 0)
	{
		if ((cat != rel) && (i = AApageflush((ACCBUF *) 0)))
			AAsyserr(21185, d->reldum.relid, i);
		rel = cat;
		if (i = AAam_insert(d = Cdesc[cat], &tid, &Tup, FALSE))
			AAsyserr(21184, d->reldum.relid, Tup.d_relation.relid, i);
	}
	if (i = AApageflush((ACCBUF *) 0))
		AAsyserr(21185, d->reldum.relid, i);
	return (i || (i = r_page()));
}

static int	init_rel(rel)
register int	rel;
{
	register DESC		*d;
	register int		i;
	struct relation		*key;
	long			reltid;
	struct relation		relkey;
	struct relation		reltup;
	extern int		read();

	if (i = rw_rel(rel, read))
		return (i);
	Parm = Rel.d_relid;
	i = 1;

	if (L_flag)
	{
		printf("%-11s%12s\t", rel? "index": "relation", Rel.d_relid);
		printf("%6d tups on %8ld, ", Rel.d_ntups, Rel.d_tupoff);
		printf("%6ld pages on %8ld\n", Rel.d_npages, Rel.d_pageoff);
		fflush(stdout);
	}

	if (L_flag <= 0)
	{
		d = &AAdmin.adreld;
		/* make believe catalog AA_REL is read only for concurrency	*/
		/* that means, readed pages are not locked			*/
		d->relopn = RELROPEN(d->relfp);
		AAam_clearkeys(d);
		AAam_setkey(d, key = &relkey, Rel.d_relid, RELID);
		AAam_setkey(d, key, AAusercode, RELOWNER);
		i = AAgetequal(d, key, &reltup, &reltid);
		/* free all buffers connected with catalog AA_REL */
		AAam_flush_rel(d, TRUE);
		/* restore catalog AA_REL to read/write mode */
		d->relopn = RELWOPEN(d->relfp);
		if (i < 0)
			AAsyserr(21183, Rel.d_relid, i);
	}
	return (i);
}

static int	r_tup(catalog)
register short	*catalog;
{
	short			cat;
	short			len;
	extern char		*AAbmove();

	for ( ; Rel.d_ntups; --Rel.d_ntups)
	{
		if (read(Fp, &cat, sizeof (short)) != sizeof (short))
			return (5927);
		if (cat < 0 || cat >= D_CATS)
			return (5928);
		if (read(Fp, &len, sizeof (short)) != sizeof (short))
			return (5927);
		if (len != Clen[cat])
			return (5929);
		if (read(Fp, &Tup, len) != len)
			return (5930);
		if (cat >= No_qm)
			continue;
		if (U_flag)
			AAbmove(AAusercode, Tup.d_relation.relowner, 2);
		if (cat == D_REL && No_qm == D_NOQM)
			Tup.d_relation.relstat &= ~(S_PROTUPS | S_INTEG);
		*catalog = cat;
		--Rel.d_ntups;
		return (-1);
	}
	return (0);
}

static int	r_page()
{
	register int		f;
	register int		i;
	register long		npages;
	char			filename[MAXNAME + 3];
	extern char		*AArelpath();

	AAdbname(Rel.d_relid, AAusercode, filename);
	if ((f = creat(AArelpath(filename), 0600)) < 0)
		return (5924);
	if (i = pos(Rel.d_pageoff))
		return (i);
	for (npages = Rel.d_npages; npages; --npages)
	{
		if (read(Fp, Pagebuf, AApgsize) != AApgsize)
			return (5926);
		if (write(f, Pagebuf, AApgsize) != AApgsize)
			return (5925);
	}
	return (0);
}

static int	pos(offset)
register long	offset;
{
	extern long		lseek();

	return ((lseek(Fp, offset, 0) != offset)? 5921: 0);
}

static char	*lookucode(ucode)
register char	*ucode;
{
	register char		*p;
	static char		buf[MAXLINE + 1];
	extern char		*AAbmove();

	if (getuser(ucode, buf))
		p = AAbmove(ucode, buf, 2);
	else
		for (p = buf; *p != ':'; p++)
			continue;
	*p = 0;
	return (buf);
}

rubproc()
{
	if (L_flag <= 0)
		cl_cat(0);
	AAunlall();
	fflush(stdout);
	exit(1);
}
