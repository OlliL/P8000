# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/access.h"
# include	"../h/batch.h"
# include	"../h/symbol.h"
# include	"../h/lock.h"
# include	"../h/bs.h"

# ifdef P8000
ret_buf		AAjmp_buf;
# else
jmp_buf		AAjmp_buf;
# endif

char	KSORT[]	=	"/sys/bin/dbu___s";

char	SYSNAME[]	= "_SYS";
int	s_SYSNAME	= sizeof SYSNAME - 1;
char			*Fileset;
char			*Ksort;
char			**Xparams = &Ksort;
int			Noupdt;
int			Where;
char			*Fillflag;
char			*Fillfactor;
char			*Minflag;
char			*Minpages;
char			*Maxflag;
char			*Maxpages;
char			*Storage_structure = "isam";
int			Tidp;
struct batchbuf		Batchbuf;
extern char		*Parmvect[];
extern char		*Flagvect[];
extern char		*AAdbpath;
extern char		*AApath;
extern char		*AAproc_name;

main(argc, argv)
int	argc;
char 	*argv[];
{
	register char		**av;
	register char		*p;
	register int		i;
	char			null;
	char			ksort[100];
	char			fileset[6];

	AAproc_name = "INDEXR";

#	ifdef xSTR1
	AAtTrace(&argc, argv, 'X');
#	endif

	switch (i = initucode(argc, argv, TRUE, (char **) 0, M_SHARE))
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
		AAsyserr(0, "HINWEIS: indexr db_name prim_rel index_rel {dom_name}");

	  default:
		AAsyserr(22200, AAproc_name, i);
	}

	for (i = 0, av = Flagvect; p = *av; av++)
	{
		if (*p == '-')
		{
			if (where_flag(p + 1))
				continue;

			switch (p[1])
			{
			  case 'N':
			  case 'n':
				Storage_structure = &p[2];
				continue;

			  case 'T':
			  case 't':
				Tidp++;
				continue;
			}
		}

		printf("Unbekanntes Flag '%s'\n", p);
		i++;
	}

	if (i)
		goto usage;

	for (i = 0; Parmvect[i]; i++)
		continue;

	if ((argc = i) < 4 || argc > 9)
		goto usage;

	if (chdir(AAdbpath) < 0)
		AAsyserr(22201, AAproc_name, AAdbpath);

	/* initialize access methods (and AAdmin struct) for user_ovrd test */
	AAam_start();
	opencatalog(AA_REL, 2);

	/* do function index */
	if (sec_index(argc - 1, &Parmvect[1]))
		goto no_modify;

	/* do function modify */
	AA_itoa(getpid(), Fileset = fileset);
	AAconcat(AApath, KSORT, ksort);
	Ksort = ksort;
	Parmvect[0] = Parmvect[2];
	Parmvect[1] = Storage_structure;
	Parmvect[2] = "name";
	if (Tidp)
		Parmvect[argc++] = "tidp";
	if (Where)
	{
		i = argc;
		null = 0;
		Parmvect[i++] = &null;
		if (Fillflag)
		{
			Parmvect[i++] = Fillflag;
			Parmvect[i++] = Fillfactor;
		}
		if (Minflag)
		{
			Parmvect[i++] = Minflag;
			Parmvect[i++] = Minpages;
		}
		if (Maxflag)
		{
			Parmvect[i++] = Maxflag;
			Parmvect[i++] = Maxpages;
		}
		argc = i;
	}
	Parmvect[argc] = (char *) -1;
	Parmvect[argc + 1] = (char *) 0;
	modify(argc, Parmvect);

no_modify:
	/* logout database */
	closecatalog(TRUE);
	AAunldb();
	AAam_exit();
	fflush(stdout);
	exit(0);
}


where_flag(flag)
register char	*flag;
{
	register char	*f;
	long		i;

	for (f = flag; *f && *f != ':'; f++)
		continue;

	if (!*f)
		return (0);

	*f++ = 0;

	if ((AAsequal(flag, "fillfactor")
# ifdef ESER
		|| AAsequal(flag, "FILLFACTOR")
# endif
		) && !AA_atol(f, &i))
	{
		Fillflag = flag;
		Fillfactor = f;
		goto flag_ok;
	}

	if ((AAsequal(flag, "minpages")
# ifdef ESER
		|| AAsequal(flag, "MINPAGES")
# endif
		) && !AA_atol(f, &i))
	{
		Minflag = flag;
		Minpages = f;
		goto flag_ok;
	}

	if ((AAsequal(flag, "maxpages")
# ifdef ESER
		|| AAsequal(flag, "MAXPAGES")
# endif
		) && !AA_atol(f, &i))
	{
		Maxflag = flag;
		Maxpages = f;
flag_ok:
		Where++;
		return (1);
	}

	*--f = ':';
	return (0);
}


rubproc()
{
	AAunlall();
	fflush(stdout);
	exit(1);
}
