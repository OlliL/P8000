# include	<stdio.h>
# include	"gen.h"
# include	"../h/ing_db.h"
# include	"../h/aux.h"
# include	"../h/access.h"
# include	"../h/batch.h"
# include	"../h/lock.h"
# include	"../h/unix.h"

char			ISAM_DESC[]	= "_SYSdesc";
char			ISAM_SORTED[]	= "_SYSsort";
char			ISAM_SPOOL[]	= "_SYSspol";
char			MODBATCH[]	= "_SYSmod";
char			MODPREBATCH[]	= "_SYSpreb";

short			Reorg;
static short		Mod_pid;
static int		Mod_pipe;
static short		Fillfactor;
static long		Minpages;
static long		Maxpages;

struct modtab
{
	char	*type;
	short	newrelspec;
	short	yeskeys;
	short	sortit;
	short	yes_seq;
	short	f_fac;
	short	mn_pages;
	short	mx_pages;
};
static struct modtab	Modtab[] =
{
	/* type		spec	keys	sort	seq	ffac	min	max */

	"heap",		M_HEAP,	FALSE,	FALSE,	FALSE,	0,	0,	0,
	"cheap",	-M_HEAP,FALSE,	FALSE,	FALSE,	0,	0,	0,
	"hash",		M_HASH,	TRUE,	TRUE,	FALSE,	50,	10,	-1,
	"chash",	-M_HASH,TRUE,	TRUE,	FALSE,	75,	1,	-1,
	"isam",		M_ISAM,	TRUE,	TRUE,	FALSE,	80,	0,	0,
	"cisam",	-M_ISAM,TRUE,	TRUE,	FALSE,	100,	0,	0,
	"heapsort",	M_HEAP,	TRUE,	TRUE,	TRUE,	0,	0,	0,
	"cheapsort",	-M_HEAP,TRUE,	TRUE,	TRUE,	0,	0,	0,
	"truncated",	M_TRUNC,FALSE,	FALSE,	FALSE,	0,	0,	0,
	"same",		0,	FALSE,	FALSE,	FALSE,	0,	0,	0,
	(char *) 0
};

struct mod_info
{
	char	outfile[MAXNAME + 4];	/* result file filled by db_sort */
	char	formfile[MAXNAME + 4];	/* file with descriptor for db_sort */
	char	infile[MAXNAME + 4];	/* input file for db_sort (relation itself */
	char	reltemp[MAXNAME + 4];	/* file holding new relation */
	char	spfile[MAXNAME + 4];	/* isam spool file for overflow */
	short	spflag;			/* isam spool file flag */
};
static struct mod_info	Mod_info;


/*
**  MODIFY -- converts any relation to the specified
**		storage structure
**
**	arguments:
**	0 - relation name
**	1 - storage structure ("heap", "cheap", "hash", "chash",
**		"isam", "cisam")
**	2 - "name" for attribute names, or "num" for numbers
**	3 - key1
**	4 - key2
**	    .
**	    .
**	i - null
**	i+1 - option name (e.g., "fillfactor")
**	i+2 - option value
**	    .
**	    .
**
**	If all the options default, parameter i -> pc are omitted.
**	If no keys are provided, parameter 2 is omitted.
*/
modify(pc, pv)
int	pc;
char	**pv;
{
	register int		i;
	register char		*rname;
	register struct modtab	*mp;
	register int		sorted;
	long			temptid;
	DESC			dold;
	DESC			dnew;
	extern int		Noupdt;
	extern char		*AA_iocv();

#	ifdef xZTR1
	AAtTfp(20, -1, "enter MODIFY\n");
#	endif
#	ifdef xZTM
	if (AAtTf(76, 1))
		timtrace(15, 0);
#	endif

	/* check for nice parameters */
	if (pc < 2)
		AAsyserr(11060, pc);

	/* save relation name for error messages */
	rname = *pv++;	/* *pv now pointes to storage spec */

	/* check for good relation */
	if ((i = AArelopen(&dold, 0, rname)) == AMOPNVIEW_ERR)
		return (error(5519, rname, (char *) 0));

	if (i > 0)	/* reln does not exist */
		return (error(5500, rname, (char *) 0));

	else if (i < 0)
		AAsyserr(11061, rname, i);

	/* can only modify a relation you own and isn't a sys rel */
	if (!AAbequal(AAusercode, dold.reldum.relowner, 2))
		i = 5501;
	if ((dold.reldum.relstat & S_CATALOG) && Noupdt)
		i = 5504;
	if (i)
	{
abort:
		AArelclose(&dold);
		return (error(i, rname, *pv, (char *) 0));
	}

	/*
	** Form descriptor for new relation. Here we need to
	** separate the pages from the old and new relations.
	** Since pages are identified by the TID of the relation
	** relation tuple, both old and new have the same identifiers.
	** To avoid this problem, a special TID is hand crafted for
	** the new relation.
	*/
	dold.relxtra[0] = 0;
	dold.relgiven[0] = 0;
	AAbmove(&dold, &dnew, sizeof dnew);
	((struct tup_id *) &dnew.reltid)->line_id = MODTID;

	/*
	** In case of an interrupt from a previous modify,
	** there might be pages around. Get rid of them.
	*/
	cleanrel(&dnew);

	AAdbname(dold.reldum.relid, dold.reldum.relowner, Mod_info.infile);

	/* scan for entry in relspec table */
	for (mp = Modtab; mp->type; mp++)
		if (AAsequal(mp->type, *pv))
			goto newspec_ok;

	/* not found, error */
	i = 5510;
	goto abort;

newspec_ok:
	Fillfactor = mp->f_fac;
	Minpages = mp->mn_pages;
	Maxpages = mp->mx_pages;
	if (Reorg = !(i = mp->newrelspec))
	{
		i = ctoi(dold.reldum.relspec);
		for (mp = Modtab; mp->type; mp++)
			if (i == mp->newrelspec)
				goto oldspec_ok;

		/* not found, error */
		i = 5521;
		goto abort;

oldspec_ok:
		Fillfactor = mp->f_fac;
		if (abs(i) == M_HASH)
			Minpages = Maxpages = dold.reldum.relprim;
		else
		{
			Minpages = mp->mn_pages;
			Maxpages = mp->mx_pages;
		}
		AAbmove(dold.relxtra, dnew.relxtra, sizeof dold.relxtra);
		AAbmove(dold.relxtra, dnew.relgiven, sizeof dold.relxtra);
	}

	dnew.reldum.relspec = i == M_TRUNC? M_HEAP: i;

	pv++;	/* now points to first parameter */

	/* get the key domains information */
	if (i = getkeys(&pv, rname, &dnew, mp))
		goto user_error;

	/* get fillfactor and other options if any */
	if (i = getfill(pv, rname))
	{
user_error:
		AArelclose(&dold);
		return (i);
	}

	/* lock the relation relation */
	get_p_tid(&dold, &temptid);
	AAsetrll(A_SLP, temptid, M_EXCL);

	/* determine parameters & build descriptor for new relation */
	make_newrel(&dnew);

	if (sorted = (mp->sortit && dold.reldum.reltups && !(abs(mp->newrelspec) == M_HASH && Reorg)))
		sortrel(&dold, &dnew);

	/* actually create the new relation */
	if (formatpg(&dnew, dnew.reldum.relprim))
		AAsyserr(11062);

	/* clear relgiven field; if heap remove any keys */
	AAam_clearkeys(&dnew);
	if (abs(ctoi(dnew.reldum.relspec)) == M_HEAP)
		for (i = 1; i <= dnew.reldum.relatts; i++)
			dnew.relxtra[i] = 0;

	if (mp->newrelspec != M_TRUNC)
		fill_rel(&dold, &dnew, sorted);

	AArelclose(&dold);	/* error return is impossible */
	if (abs(ctoi(dnew.reldum.relspec)) == M_ISAM)
	{
		if (i = bldindex(&dnew))
			AAsyserr(11063, dnew.reldum.relid, i);

		unspool(&dnew);
	}

	/*
	** New relation is now complete. The system relations need to
	** be updated. First destroy all buffers with pages from the
	** new relation.
	*/
	if (i = cleanrel(&dnew))
		AAsyserr(11064, i, dnew.reldum.relid);

	fill_batch(&dold, &dnew);

	/*
	** Close the file for the new relation. This must be
	** done after the fill_batch in case we are modifing
	** the attribute relation.
	*/
	close(dnew.relfp);
	dnew.relopn = 0;
	ruboff("modify");
	modupdate();
	rubon();

	AAunlrl(temptid);

#	ifdef xZTM
	if (AAtTf(76, 1))
		timtrace(16, 0);
#	endif

	return (0);
}


getkeys(ppv, relname, d, mp)
char			***ppv;
char			*relname;
register DESC		*d;
struct modtab		*mp;
{
	register char			**pv;
	register char			*cp;
	register int			i;
	register int			keyno;
	register int			as_ds;
	register int			sort_only;
	int				keywid;
	int				namemode;
	struct attribute		attkey;
	struct attribute		atttup;
	struct tup_id			tid;
	extern DESC			Attdes;
	extern char			*AA_iocv();

	pv = *ppv;	/* copy list of params */

	/* determine whether there are any keys at all */
	keyno = 0;
	keywid = 0;
	sort_only = FALSE;
	cp = *pv;

	if (Reorg)
	{
		if (cp == (char *) -1)
			;	/* ok */
		else if (!*cp)
			pv++;
		else	/* bad argument to Reorganization */
			return (error(5522, relname, cp, (char *) 0));

		for (i = 1; i <= d->reldum.relatts; i++)
			if (d->relxtra[i])
				keyno++;
		goto def_keys;
	}

	/* zero key info */
	for (i = 0; i <= d->reldum.relatts; i++)
		d->relxtra[i] = d->relgiven[i] = 0;

	if (cp == (char *) -1 || !*cp)
	{
		/* no key information. default as needed */
		if (mp->yeskeys)
		{
			cp = "\1";	/* default to first key */
			namemode = FALSE;
		}
		else
			pv++;	/* point one to far */
	}
	else
	{
		/* check for name mode */
		if (namemode = AAsequal(cp, "name"))
		{
			/* check attribute names, and convert them to numbers */
			opencatalog(AA_ATT, 0);
			AAam_setkey(&Attdes, &attkey, Mod_info.infile, ATTRELID);
			AAam_setkey(&Attdes, &attkey, AAusercode, ATTOWNER);
		}
		pv++;	/* inc to next key */
		cp = *pv++;	/* pick up first key */
	}

	/* scan for attribute names */
	for ( ; cp != (char *) -1; cp = *pv++)
	{
		/* check for separator between keys & options */
		if (!*cp)
		{
			pv++;	/* point two past (char *) 0 */
			break;
		}

		if (namemode)
		{
			/* check for "sort only" attribute */
			if (*cp == '#')
			{
				cp++;	/* inc to start of name */
				sort_only = TRUE;
			}

			/* check for ascending/descending modifier */
			if ((as_ds = modseqkey(cp, relname, mp->yes_seq)) > 0)
				return (as_ds);	/* error */

			AAam_setkey(&Attdes, &attkey, cp, ATTNAME);
			if ((i = AAgetequal(&Attdes, &attkey, &atttup, &tid)) < 0)
				AAsyserr(11065, i);

			if (i > 0)
				return (error(5511, relname, cp, (char *) 0));	/* bad att name */

			i = atttup.attid;
		}
		else
		{
			i = *cp;
			as_ds = 0;
		}

		/* add new key to descriptor */
		keyno++;
		if (!sort_only)
		{
			d->relxtra[i] = keyno;
			keywid += ctou(d->relfrml[i]);
		}
		if (d->relgiven[i])	/* duplicate attribute */
			return (error(5507, relname, cp, (char *) 0));

		d->relgiven[i] = !as_ds? keyno: -keyno;
	}
	pv--;	/* back up one to point to "-1" terminator */

	if (abs(ctoi(d->reldum.relspec)) == M_ISAM && keywid > (AAmaxtup / sizeof (short) - sizeof (long)))
		return (error(5508, relname, AA_iocv(keywid), (char *) 0));

	/* if a heap, there can be no keys */
	if (!mp->yeskeys && keyno)
		return (error(5502, relname, mp->type, (char *) 0));	/* no keys allowed on heap */

	/* fill out default sort on remainder of keys */
def_keys:
	if (mp->yeskeys)
		for (i = 1; i <= d->reldum.relatts; i++)
			if (!d->relgiven[i])
				d->relgiven[i] = ++keyno;
	*ppv = pv;
	return (0);
}


modseqkey(domain, relname, seq_ok)
char	*domain;
char	*relname;
int	seq_ok;
{
	register char	*cp;
	register int	c;
	register int	ret;

	for (ret = 0, cp = domain; c = *cp++; )
		if (c == ':')
			break;

	if (c)
	{
		/* replace ":" with null */
		*(cp - 1) = '\0';

		/* verify sequence is valid */
		if (!seq_ok)
			ret = error(5520, relname, cp, domain, (char *) 0);
		else if (AAsequal("descending", cp) || AAsequal("d", cp))
			ret = -1;
		else if (!(AAsequal("ascending", cp) || AAsequal("a", cp)))
			ret = error(5518, relname, cp, domain, (char *) 0);
	}

	return (ret);
}


/*
**	GETFILL -- Get fill factor and minimum pages parameters
**		from argument list, convert them from ascii to integer
**		and store them in global variables.  If the global
**		variable for the corresponding parameter is zero,
**		it means that that parameter is not allowed and an
**		error is generated.
*/
getfill(pv, rel)
register char	**pv;
char		*rel;
{
	register char	*p1;
	register int	err;
	register char	*p2;
	register int	fill_flag;
	register int	min_flag;
	register int	max_flag;

	err = 0;
	fill_flag = min_flag = max_flag = FALSE;

	while ((p1 = *pv++) != (char *) -1)
	{
		p2 = *pv++;
		if (AAsequal(p1, "fillfactor"))
		{
			if (!Fillfactor || fill_flag)
			{
				err = 5512;
				break;
			}
			p1 = p2;
			AA_atoi(p1, &Fillfactor);
			if (Fillfactor > 100 || Fillfactor < 1)
			{
				err = 5513;
				break;
			}
			fill_flag = TRUE;
			continue;
		}
		if (AAsequal(p1, "minpages"))
		{
			if (!Minpages || min_flag)
			{
				err = 5512;
				break;
			}
			p1 = p2;
			AA_atol(p1, &Minpages);
			if (Minpages < 1)
			{
				err = 5514;
				break;
			}
			if (max_flag && (Minpages > Maxpages))
			{
				err = 5517;
				break;
			}
			min_flag = TRUE;
			continue;
		}
		if (AAsequal(p1, "maxpages"))
		{
			if (!Maxpages || max_flag)
			{
				err = 5512;
				break;
			}
			p1 = p2;
			AA_atol(p1, &Maxpages);
			if (Maxpages < 1)
			{
				err = 5516;
				break;
			}
			if (min_flag && (Minpages > Maxpages))
			{
				err = 5517;
				break;
			}
			max_flag = TRUE;
			continue;
		}
		err = 5515;
		break;
	}

	return (err? error(err, rel, p1, (char *) 0): 0);
}


/*
**	MAKE_NEWREL -- Create a file for the modified relation
**		and build one or more primary pages for the
**		relation based on its storage structure and the
**		number of tuples it must hold.
*/
make_newrel(desc)
register DESC		*desc;
{
	register int			tups_p_page;
	long				prim;

	AAconcat(MODTEMP, Fileset, Mod_info.reltemp);
	close(creat(Mod_info.reltemp, FILEMODE));
	if ((desc->relfp = open(Mod_info.reltemp, 2)) < 0)
		AAsyserr(11066, Mod_info.reltemp, desc->relfp);

	desc->relopn = RELWOPEN(desc->relfp);
	if (abs(ctoi(desc->reldum.relspec)) == M_HASH && Fillfactor > 0 && Minpages > 0)
	{
		/*
		** Determine the number of primary pages. The following
		** first determines the number of tuples/page which the
		** relation should have in order to get the requested
		** fillfactor. Then that number is divided into the
		** number of tuples to get the number of primary pages.
		** To avoid round off, it must guaranteed that the
		** number of tuples per page must be at least 1.
		**
		** primary_pages = #tuples / (#tuples/page * fillfactor)
		*/
		tups_p_page = (((AAmaxtup + sizeof (short)) / (desc->reldum.relwid + sizeof (short))) * Fillfactor) / 100;
		if (!tups_p_page)
			tups_p_page = 1;
		 /* we add one to simulate a ceiling function */
		prim = desc->reldum.reltups / tups_p_page + 1;
		if (prim < Minpages)
			prim = Minpages;
		if (Maxpages > 0 && prim > Maxpages)
			prim = Maxpages;

#		ifdef xZTR1
		AAtTfp(22, 0, "using %ld prim pages\n", prim);
#		endif

		Reorg = Reorg && prim == desc->reldum.relprim;
		desc->reldum.relprim = prim;
	}
	else
		desc->reldum.relprim = 1;
	desc->reldum.reltups = 0;
	return (0);
}


/*
**	SORTREL - Call KSORT to sort the given relation.  SORTREL
**		sets up the descriptor struct specifying the sort
**		keys and tells KSORT whether or not the hash key should
**		be included as a sort key.
*/
sortrel(odesc, desc)
DESC		*odesc;
register DESC	*desc;
{
	register int		fd;
	register int		i;
	register char		*p;
	char			savespec;
	int			sort_pipe[2];
	char			size_buf[6];
	extern char		**Xparams;
	extern char		*AA_iocv();

	AAconcat(ISAM_SORTED, Fileset, Mod_info.outfile);
	if (!Reorg && close(creat(Mod_info.outfile, FILEMODE)))
		AAsyserr(11067, Mod_info.outfile);

	AAconcat(ISAM_DESC, Fileset, Mod_info.formfile);
	if ((fd = creat(Mod_info.formfile, FILEMODE)) < 0)
		AAsyserr(11068, Mod_info.formfile, fd);

	if (abs(ctoi(desc->reldum.relspec)) == M_HASH)
	{
		/* sort on hash bucket first */
		desc->relgiven[0] = 1;
		for (i = 1; i <= desc->reldum.relatts; i++)
			desc->relgiven[i]++;
	}
	savespec = desc->reldum.relspec;
	desc->reldum.relspec = odesc->reldum.relspec;

	if (write(fd, desc, sizeof *desc) != sizeof *desc)
		AAsyserr(11069);

	close(fd);
	desc->reldum.relspec = savespec;

	sort_pipe[1] = -1;

	if (Reorg && pipe(sort_pipe))
		AAsyserr(11074);

	fd = sort_pipe[1];	/* write file descriptor */
	fflush(stdout);

	if ((i = fork()) == -1)
		AAsyserr(11070);

	if (!i)
	{
		/* child process */
		for (i = 3; i < MAXFILES; i++)
			if (i != fd)
				close(i);
		p = "___dbu";
		if (Reorg)
		{
			*p = 'r';
			p[1] = fd + 'A';
		}
		else
			*p = 's';

		AA_itoa(AApgsize, size_buf);

		execl(Xparams[0],
		/* argv[0] */	p,
		/* argv[1] */	Fileset,
#		ifdef xZTR1
		/* argv[2] */	AA_iocv(AAtTf(23, -1)),
#		else
		/* argv[2] */	"0",
#		endif
		/* argv[3] */	AApath,
		/* argv[4] */	size_buf,
		/* argv[5] */	Mod_info.formfile,
		/* argv[6] */	Mod_info.infile,
		/* argv[7] */	Mod_info.outfile, (char *) 0);
		AAsyserr(11071, Xparams[0]);
	}

	/* parent process */
	if (Reorg)
	{
		close(fd);
		Mod_pid = i;
		Mod_pipe = sort_pipe[0];
	}
	else if (fd = fullwait(i, "modify"))	/* wait for db_sort */
		AAsyserr(11072, fd);
}


# ifdef STACK
static char				act_tup[MAXTUP];
static char				last_tup[MAXTUP];
static struct tup_hdr			file;
static struct tup_hdr			unfile;
# endif
/*
**	FILL_REL -- Fill the new relation with tuples from either
**		the old relation or the output file of KSORT.
*/
fill_rel(sdesc, desc, sortit)
register DESC		*sdesc;
register DESC		*desc;
int			sortit;
{
	register int			i;
	char				*tuple;
# ifndef STACK
	char				act_tup[MAXTUP];
	char				last_tup[MAXTUP];
	struct tup_hdr			file;
# endif
	char				newreltype;
	char				anytups;
	int				need;
	int				j;
	long				lnum;
	struct tup_id			tid;
	struct tup_id			stid;
	struct tup_id			stidlim;
	extern char			*tup_read();

	newreltype = abs(ctoi(desc->reldum.relspec));
	Tup_size = desc->reldum.relwid;
	if (newreltype == M_HASH)
		Tup_size += sizeof (long);
	if (sortit)
	{
		if (Reorg)
			file.tup_fd = Mod_pipe;
		else if ((file.tup_fd = open(Mod_info.outfile, 0)) < 0)
			AAsyserr(11073, Mod_info.outfile);
		file.tup_ptr = file.tup_buf;
		file.tup_end = file.tup_buf;
		file.tup_len = PGSIZE;
	}
	else
	{
		cleanrel(sdesc);	/* make sure each page is read fresh */
		AAam_find(sdesc, NOKEY, &stid, &stidlim);
	}
	if (newreltype == M_ISAM)
	{
		lnum = 0L;
		AAstuff_page(&tid, &lnum);
		tid.line_id = 0;
		AAgetpage(desc, &tid);
		AAconcat(ISAM_SPOOL, Fileset, Mod_info.spfile);
		/* assume that spool file is not needed */
		Mod_info.spflag = FALSE;
		Tup_fd = -1;
		Tup_ptr = Tup_buf;
		if (!Fillfactor)
			Fillfactor = 100;
		/* setup relgiven field for AAkcompare later on */
		for (i = 1; i <= desc->reldum.relatts; i++)
			desc->relgiven[i] = desc->relxtra[i];
	}
	desc->reladds = 0;
	anytups = FALSE;
	for ( ; ; )
	{
		if (sortit)
		{
			if (!(tuple = tup_read(&file)))
				break;

		}
		else
		{
#			ifdef xZTR2
			if (AAtTfp(22, 1, "FILL_REL: stid "))
			{
				AAdumptid(&stid);
				printf("FILL_REL: stidlim ");
				AAdumptid(&stidlim);
			}
#			endif

			tuple = act_tup;
			i = AAam_get(sdesc, &stid, &stidlim, tuple, TRUE);

#			ifdef xZTR2
			if (AAtTfp(22, 1, "FILLREL: get %d ", i))
				AAprtup(sdesc, tuple);
#			endif

			if (i < 0)
				AAsyserr(11076, i);

			if (i == 1)
				break;
		}
		if (newreltype != M_ISAM)
		{
			if ((i = AAam_insert(desc, &tid, tuple, FALSE)) < 0)
				AAsyserr(11077, i);

#			ifdef xZTR2
			if (AAtTfp(22, 2, "FILL_REL: insert "))
			{
				AAprtup(desc, tuple);
				printf("FILL_REL: insert ret %d at", i);
				AAdumptid(&tid);
			}
#			endif

			continue;
		}
		if (anytups)
			i = AAkcompare(desc, tuple, last_tup);
		else
		{
			anytups = TRUE;
			i = 1;
		}
		AAbmove(tuple, last_tup, desc->reldum.relwid);
		need = AAcanonical(desc, tuple);
		if (!i && need > AAspace_left(AAcc_head))
		{
			/*
			** spool out this tuple.
			** will go on overflow page later
			*/
			if (Tup_fd < 0 && (Tup_fd = creat(Mod_info.spfile, FILEMODE)) < 0)
				AAsyserr(11073, Mod_info.spfile);

			tup_write(tuple);
			continue;
		}
		j = (100 - Fillfactor) * desc->reldum.relwid / 100;
		if (j < need)
			j = need;
		if (i && j > AAspace_left(AAcc_head) && (i = add_prim(&tid)))
			AAsyserr(11080, i);

		tid.line_id = AAnewlino(need);
		AAput_tuple(&tid, AAcctuple, need);
		desc->reladds++;
	}
	if (sortit)
	{
		close(file.tup_fd);
		unlink(Mod_info.formfile);
		if (!Reorg)
			unlink(Mod_info.outfile);
		else if (i = fullwait(Mod_pid, "modify"))
			AAsyserr(11072, i);
	}

	if (newreltype == M_ISAM)
	{
		if (i = AApageflush(AAcc_head))
			AAsyserr(11081, i);

		if (Tup_fd >= 0)
		{
			Mod_info.spflag = TRUE;
			tup_close();
		}
	}
	desc->reldum.reltups = desc->reladds;
	desc->reladds = 0;
	return (0);
}

# ifdef STACK
static ACCBUF				dirbuf;
static int				keyx[MAXDOM];
static char				tuple[MAXTUP];
static char				temptup[MAXTUP];
# endif

bldindex(d)
register DESC	*d;
{
	register struct tup_id		*tid;
	register int			tmp;
	register int			offset;
	register int			keywid;
	register int			len;
	register int			i;
	int				level;
	char				savespec;
	char				*key;
	long				pageid;
	long				start;
	long				stop;
	long				newstart;
	long				newstop;
	struct tup_id			tidx;
# ifndef STACK
	ACCBUF				dirbuf;
	int				keyx[MAXDOM];
	char				tuple[MAXTUP];
	char				temptup[MAXTUP];
# endif

	tid = &tidx;
	keywid = 0;
	for (tmp = 0; tmp < MAXDOM; tmp++)
		keyx[tmp] = 0;
	for (tmp = 1; tmp <= d->reldum.relatts; tmp++)
		if (d->relxtra[tmp] > 0)
		{
			i = d->relxtra[tmp];
			keyx[i - 1] = tmp;
			keywid += ctou(d->relfrml[tmp]);
		}

	/*
	** Determine the last page of the relation. This will
	** only work if all pages have been written out. Fill_rel
	** must guarantee that all pages have been written
	*/
	level = 0;
	AAlast_page(d, tid, 0);
	AApluck_page(tid, &stop);
	start = 0;
	dirbuf.filedesc = d->relfp;
	dirbuf.rel_tupid = d->reltid;
	savespec = d->reldum.relspec;
	for ( ; ; )
	{
#		ifdef xZTR2
		AAtTfp(21, 7, "ISAM: level %d\n", level);
#		endif
		dirbuf.ovflopg = start;
		dirbuf.mainpg = level;
		dirbuf.thispage = stop + 1;
		offset = dirbuf.firstup - (char *) &dirbuf;
		dirbuf.linetab[LINETAB(0)] = offset;
		dirbuf.bufstatus = BUF_DIRTY | BUF_DIRECT;

		dirbuf.nxtlino = 0;
		newstart = stop + 1;
		newstop = newstart;
		for (pageid = start; pageid <= stop; pageid++)
		{
#			ifdef xZTR2
			AAtTfp(21, 8, "ISAM: get key from %ld\n", pageid);
#			endif
			AAstuff_page(tid, &pageid);
			tid->line_id = 0;
			if (tmp = AAam_get(d, tid, tid, tuple, FALSE))
			{
				/*
				** If the relation is empty, then page 0 will
				** return AMINVL_ERR on a AAam_get(). Form a blank tuple
				** and use it to create a one tuple directory
				*/
				if (!pageid && tmp == AMINVL_ERR)
					clr_tuple(d, tuple);
				else
					return (-2);
			}

			/*
			** If this is the first level then form the tuple
			** from the mainpage of the relation. Otherwise
			** the tuple is the first tuple of a directory page
			** and it is already correctly formed.
			*/
			if (!level)
			{
				key = temptup;
				for (tmp = 0; keyx[tmp]; tmp++)
				{
					i = keyx[tmp];
					len = ctou(d->relfrml[i]);
					i = d->reloff[i];
					AAbmove(&tuple[i], key, len);
					key += len;
				}
				key = temptup;
			}
			else
				key = tuple;

			if (keywid > AAspace_left(&dirbuf))
			{
				if (AApageflush(&dirbuf))
					return (-3);
				dirbuf.thispage++;
				newstop = dirbuf.thispage;
				dirbuf.ovflopg = pageid;
				offset = dirbuf.firstup - (char *) &dirbuf;
				dirbuf.linetab[LINETAB(0)] = offset;
				dirbuf.bufstatus = BUF_DIRTY;
				dirbuf.nxtlino = 0;
			}
			/* copy key to directory page */
			AAbmove(key, (char *) &dirbuf + offset, keywid);

			/* update next line number */
			offset += keywid;
			dirbuf.nxtlino++;
			dirbuf.linetab[LINETAB(dirbuf.nxtlino)] = offset;
		}
		if (AApageflush(&dirbuf))
			return (-4);
		if (newstart == newstop)
			break;
		d->reldum.relspec = abs(ctoi(d->reldum.relspec));
		level++;
		start = newstart;
		stop = newstop;
	}
	d->reldum.relspec = savespec;
	d->reldum.relprim = newstart;
	return (0);
}


/*
**	UNSPOOL -- Take tuples saved in spool file and insert them
**		in new relation.  This is only for ISAM relations.
*/
unspool(desc)
register DESC		*desc;
{
	register int			i;
	register char			*tuple;
	struct tup_id			tid;
# ifndef STACK
	struct tup_hdr			unfile;
# endif
	extern char			*tup_read();

	if (Mod_info.spflag)
	{
		if ((unfile.tup_fd = open(Mod_info.spfile, 0)) < 0)
			AAsyserr(11082);

		unfile.tup_ptr = unfile.tup_buf;
		unfile.tup_end = unfile.tup_buf;
		unfile.tup_len = PGSIZE;
		Tup_size = desc->reldum.relwid;

		while (tuple = tup_read(&unfile))
			if ((i = AAam_insert(desc, &tid, tuple, FALSE)) < 0)
				AAsyserr(11083, desc->reldum.relid, i);

		close(unfile.tup_fd);
		unlink(Mod_info.spfile);
	}
	desc->reldum.reltups += desc->reladds;
	desc->reladds = 0;
}


/*
**	FILL_BATCH -- Create and fill a batch file containing the
**		updates for the system catalog so that MODIFY will
**		be recoverable if the system crashes.
*/
fill_batch(odesc, desc)
DESC		*odesc;
register DESC	*desc;
{
	register DESC			*dessys;
	register int			i;
	register int			j;
	struct relation			reltup;
	struct relation			rkey;
	struct tup_id			tid;
	struct tup_id			lotid;
	struct tup_id			hitid;
	struct attribute		atttup;
	struct attribute		akey;
	char				prebatch[MAXNAME + 4];
	char				modbatch[MAXNAME + 4];

	if (AAbequal(desc->reldum.relid, AA_REL, MAXNAME))
	{
		AAam_clearkeys(desc);
		AAam_setkey(desc, &rkey, desc->reldum.relid, RELID);
		AAam_setkey(desc, &rkey, desc->reldum.relowner, RELOWNER);
		if (i = AAgetequal(desc, &rkey, &reltup, &tid))
			AAsyserr(11085, i);

		AAbmove(&tid, &desc->reltid, sizeof desc->reltid);
	}
	else
		AAbmove(&odesc->reltid, &desc->reltid, sizeof desc->reltid);
	AAresetacc(AAcc_head);
	AAconcat(MODPREBATCH, Fileset, prebatch);
	close(creat(prebatch, FILEMODE));
	if ((Batch_fp = open(prebatch, 2)) < 0)
		AAsyserr(11086, prebatch, Batch_fp);

	AAsmove(Fileset, Batchbuf.file_id);
	Batch_cnt = 0;
	wrbatch(desc, sizeof *desc);
	if (AAbequal(desc->reldum.relid, AA_ATT, MAXNAME))
		dessys = desc;
	else
		dessys = &AAdmin.adattd;
	AAam_clearkeys(dessys);
	AAam_setkey(dessys, &akey, desc->reldum.relid, ATTRELID);
	AAam_setkey(dessys, &akey, desc->reldum.relowner, ATTOWNER);
	if (i = AAam_find(dessys, EXACTKEY, &lotid, &hitid, &akey))
		AAsyserr(11087, i);

	j = desc->reldum.relatts;
	while (!(i = AAam_get(dessys, &lotid, &hitid, &atttup, TRUE)) && j > 0)
		if (!AAkcompare(dessys, &akey, &atttup))
		{
			j--;
			i = atttup.attid;
			atttup.attxtra = desc->relxtra[i];
			wrbatch(&lotid, sizeof lotid);
			wrbatch(&atttup, sizeof atttup);
		}

	if (i < 0 || j > 0)
		AAsyserr(11088, i, j);

	/* get rid of attribute pages */
	cleanrel(dessys);
	flushbatch();
	close(Batch_fp);
	AAconcat(MODBATCH, Fileset, modbatch);
	if (link(prebatch, modbatch) == -1)
		AAsyserr(11089, prebatch, modbatch);

	unlink(prebatch);
	return (0);
}
