# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/symbol.h"
# include	"../h/catalog.h"
# include	"../h/access.h"
# include	"../h/lock.h"
# include	"../h/bs.h"

# ifdef P8000
ret_buf		AAjmp_buf;
# else
jmp_buf		AAjmp_buf;
# endif

# define	put_in(c)		putc(c, Dbs_out)
# define	wr_in(a)		fprintf(Dbs_out, a)
# define	wr_fin(f, a)		fprintf(Dbs_out, f, a)
# define	put_out(c)		putc(c, Ofile)
# define	wr_out(a)		fprintf(Ofile, a)
# define	wr_fout(f, a)		fprintf(Ofile, f, a)
# define	put_both(c)		put_in(c); put_out(c)
# define	wr_both(a)		wr_in(a); wr_out(a)
# define	wr_fboth(f, a)		wr_fin(f, a); wr_fout(f, a)

extern char				*Parmvect[];
# define	DB			Parmvect[0]
# define	PATH			Parmvect[1]
# define	RELS			&Parmvect[2]

struct domain
{
	char	domname[MAXNAME];
	short	domfrmt;
	short	domfrml;
};

char		COPY_IN[]=		"/copy.in";
char		COPY_OUT[]=		"/copy.out";
extern char	*AAusercode;
extern char	*AAdbpath;
extern DESC	Reldes;
extern DESC	Attdes;
extern DESC	Inddes;
extern DESC	Treedes;
extern DESC	Prodes;
extern DESC	Intdes;
int		AAdbstat;
int		Standalone;
char		*Resrel;
FILE		*Dbs_out = (FILE *) 0;
FILE		*Ofile;
# ifdef SETBUF
char		Ibuf[BUFSIZ];
char		Obuf[BUFSIZ];
# endif

# ifdef STACK
static struct domain	pdomains[MAXDOM];
static struct domain	idomains[MAXDOM];
static short		pdomxtra[MAXDOM];
static short		idomxtra[MAXDOM];
# endif

main(argc, argv)
int	argc;
char	**argv;
{
	register DESC			*d;
	register struct domain		*domptr;
	register int			i;
	register char			**av;
	register int			badf;
	struct relation			relation;
	struct index			index;
	long				lowrel;
	long				highrel;
	long				lowind;
	long				highind;
	long				lowtid;
# ifndef STACK
	struct domain			pdomains[MAXDOM];
	struct domain			idomains[MAXDOM];
	short				pdomxtra[MAXDOM];
	short				idomxtra[MAXDOM];
# endif
	short				pkeydoms;
	short				ikeydoms;
	int				noqm;
	extern char			*AAproc_name;
	extern char			*Flagvect[];
	extern char			*AAztack();
	extern int			(*AAexitfn)();
	extern int			exitfn();

	AAproc_name = "COPYDB";
	if (argc < 3)
		AAsyserr(0, "HINWEIS: copydb db path [{rel}]");

#	ifdef xSTR1
	AAtTrace(&argc, argv, 'X');
#	endif

	/*
	**  Scan the argument vector and otherwise initialize.
	*/
	badf = 0;
	i = initucode(argc, argv, TRUE, (char *) 0, M_SHARE);
	switch (i)
	{
	  case 0:
	  case 5:
		break;

	  case 1:
	  case 6:
		printf("Die Datenbank '%s' existiert nicht\n", DB);
		goto badarg;

	  case 2:
		printf("Sie duerfen auf die Datenbank '%s' nicht zugreifen\n", DB);
		goto badarg;

	  case 3:
		printf("Sie sind kein eingetragener DBS-Nutzer\n");
		goto badarg;

	  case 4:
		printf("Sie haben keinen Datenbanknamen angegeben\n");
badarg:
		badf++;
		break;

	  default:
		AAsyserr(22200, AAproc_name, i);
	}

	for (av = Flagvect; *av; av++)
	{
		printf("Unbekanntes Flag '%s'\n", *av);
		badf++;
	}

	if (!(Dbs_out = fopen(AAztack(PATH, COPY_IN), "w" )))
	{
		printf("? '%s'\n", AAztack(PATH, COPY_IN));
		badf++;
	}
# ifdef SETBUF
	else
		setbuf(Dbs_out, Ibuf);
# endif

	if (!(Ofile = fopen(AAztack(PATH, COPY_OUT), "w")))
	{
		printf("? '%s'\n", AAztack(PATH, COPY_OUT));
		badf++;
	}
# ifdef SETBUF
	else
		setbuf(Ofile, Obuf);
# endif

	/*
	**  Check for usage errors.
	*/
	if (badf)
	{
		AAunlall();
		fflush(stdout);
		exit(-1);
	}

	AAexitfn = exitfn;

	/* initialize access methods (and AAdmin struct) */
	AAam_start();
		noqm = !(AAdmin.adhdr.adflags & A_QRYMOD);
	opencatalog(AA_REL, 0);
	opencatalog(AA_ATT, 0);
	opencatalog(AA_INDEX, 0);

	AAam_clearkeys(d = &Reldes);
	AAam_setkey(d, &relation, AAusercode, RELOWNER);
	AAam_find(d, EXACTKEY, &lowrel, &highrel, &relation);
	lowtid = lowrel;
	while (!AAam_get(d, &lowrel, &highrel, &relation, 1))
	{
		if (!AAbequal(relation.relowner, AAusercode, 2))
			continue;

		if (AAbequal(relation.relid, "_SYS", 4))
			continue;

		if (relation.relstat & (S_CATALOG | S_INDEX | S_VIEW))
			continue;

		if (notwanted(relation.relid))
			continue;

		relation.relowner[0] = 0;
		wr_in("create");
		wr_out("copy");
		wr_fboth(" %s\n(", relation.relid);
		pkeydoms = get_doms(relation.relid, pdomains, pdomxtra);
		for (i = 0; i < relation.relatts; i++)
		{
			domptr = &pdomains[i];
			if (i)
			{
				put_both(',');
			}
			wr_fboth("\n\t%12.12s = ", domptr->domname);
			switch (domptr->domfrmt)
			{
			  case INT:
				put_both('i');
				break;

			  case FLOAT:
				put_both('f');
				break;

			  case CHAR:
				put_both('c');
				break;

			  default:
				put_both('d');
			}
			wr_fboth("%d", domptr->domfrml);
		}

		wr_both("\n)");
		wr_out("into");
		wr_fin("\n\\p\\g\ncopy %s ()\nfrom", relation.relid);
		wr_fboth(" \"%s", PATH);
		wr_fboth("/%s", relation.relid);
		wr_fboth("%s\"\n\\p\\g\n", AAusercode);

		if (ctoi(relation.relspec) != M_HEAP)
			modify(relation.relid, pdomains, pdomxtra, ctoi(relation.relspec), pkeydoms);

		if (ctoi(relation.relindxd) == SECBASE)
		{
			AAam_clearkeys(d = &Inddes);
			AAam_setkey(d, &index, relation.relid, IRELIDP);
			AAam_setkey(d, &index, AAusercode, IOWNERP);
			AAam_find(d, EXACTKEY, &lowind, &highind, &index);
			while (!AAam_get(d, &lowind, &highind, &index, 1))
			{
				if (!AAbequal(index.irelidp, relation.relid, MAXNAME))
					continue;

				if (!AAbequal(index.iownerp, AAusercode, 2))
					continue;

				wr_fin("index on %s ", relation.relid);
				wr_fin("is %12.12s\n(", index.irelidi);

				for (i = 0; index.idom[i] && i < MAXKEYS; i++)
				{
					if (i)
						put_in(',');
					badf = index.idom[i];
					wr_fin("\n\t%12.12s", pdomains[badf - 1].domname);
				}
				wr_in("\n)\n\\p\\g\n");
				ikeydoms = get_doms(index.irelidi, idomains, idomxtra);
				if (ctoi(index.irelspeci) != M_ISAM && ikeydoms != i)
					modify(index.irelidi, idomains, idomxtra, ctoi(index.irelspeci), ikeydoms);
			}
			d = &Reldes;
		}

		if (noqm)
			continue;

		if ((i = relation.relstat) & S_INTEG)
		{
			pr_integrity(relation.relid, AAusercode);
			wr_in("\\p\\g\n");
		}

		if (i & S_PROTUPS || !(i & S_PROTALL) || !(i & S_PROTRET))
		{
			AAbmove(AAusercode, relation.relowner, 2);
			pr_prot(&relation);
			relation.relowner[0] = 0;
			wr_in("\\p\\g\n");
		}

	}

	if (!noqm)
	{
		while (!AAam_get(d, &lowtid, &highrel, &relation, 1))
		{
			if (!AAbequal(relation.relowner, AAusercode, 2))
				continue;

			if (AAbequal(relation.relid, "_SYS", 4))
				continue;

			if (!(relation.relstat & S_VIEW))
				continue;

			if (notwanted(relation.relid))
				continue;

			relation.relowner[0] = 0;
			Resrel = relation.relid;
			pr_def(relation.relid, AAusercode);
			wr_in("\\p\\g\n");
		}
	}
	term();
	exit(0);
}


modify(name, domains, domxtra, spec, keydoms)
char				*name;
register struct domain		*domains;
register short			*domxtra;
short				spec;
short				keydoms;
{
	register int			dom;

	wr_fin("modify %12.12s to ", name);
	if (spec < 0)
		put_in('c');
	switch (abs(spec))
	{
	  case M_HEAP:
		wr_in("heap");
		goto nokeys;

	  case M_ISAM:
		wr_in("isam on");
		break;

	  case M_HASH:
		wr_in("hash on");
		break;

	  default:
		wr_in("unknown");
		goto nokeys;
	}
	for (dom = 0; dom < keydoms; dom++)
	{
		if (dom)
			put_in(',');
		wr_fin("\n\t%12.12s", domains[domxtra[dom]].domname);
	}
nokeys:
	wr_in("\n\\p\\g\n");
}


get_doms(name, domains, domxtra)
char			*name;
struct domain		*domains;
short			domxtra[];
{
	register DESC			*d;
	register struct domain		*domptr;
	register int			dom;
	register int			keydoms;
	register int			i;
	struct attribute		attkey;
	struct attribute		atttup;
	long				lowtid;
	long				hightid;

	keydoms = 0;
	AAam_clearkeys(d = &Attdes);
	AAam_setkey(d, &attkey, name, ATTRELID);
	AAam_setkey(d, &attkey, AAusercode, ATTOWNER);
	AAam_find(d, EXACTKEY, &lowtid, &hightid, &attkey);
	while (!AAam_get(d, &lowtid, &hightid, &atttup, 1))
	{
		if (AAkcompare(d, &attkey, &atttup))
			continue;

		if ((dom = atttup.attid) >= MAXDOM)
			AAsyserr(21002);

		domptr = &domains[--dom];
		AAbmove(atttup.attname, domptr->domname, MAXNAME);
		domptr->domfrmt = atttup.attfrmt;
		domptr->domfrml = ctou(atttup.attfrml);
		if (atttup.attxtra)
		{
			i = atttup.attxtra;
			domxtra[--i] = dom;
			keydoms++;
		}
	}
	return (keydoms);
}


/*
** Check to see if relation is wanted.
*/

notwanted(relname)
register char	*relname;
{
	register char	**wantlist;

	if (!*(wantlist = RELS))
		return (0);

	while (*wantlist)
		if (!AAscompare(relname, MAXNAME, *wantlist++, 0))
			return (0);
	return (1);
}


term()
{
	fflush(Dbs_out);
	fflush(Ofile);
	closecatalog(TRUE);
	AAunlall();
	AAam_exit();
	fflush(stdout);
}


exitfn()
{
	extern char			*AAztack();

	term();
	unlink(AAztack(PATH, COPY_IN));
	unlink(AAztack(PATH, COPY_OUT));
	exit(1);
}


rubproc()
{
	AAunlall();
	fflush(stdout);
	exit(1);
}


char	*AArelpath(rel)
register char	*rel;
{
	extern char	*AAztack();

	return (AAztack(AAztack(AAdbpath, "/"), rel));
}
