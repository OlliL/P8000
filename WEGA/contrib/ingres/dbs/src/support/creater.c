# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/access.h"
# include	"../h/symbol.h"
# include	"../h/lock.h"
# include	"../h/bs.h"

# ifdef P8000
ret_buf		AAjmp_buf;
# else
jmp_buf		AAjmp_buf;
# endif

char	SYSNAME[]	= "_SYS";
int	s_SYSNAME	= sizeof SYSNAME - 1;
extern char		*Parmvect[];
extern char		*Flagvect[];
extern char		*AAdbpath;

main(argc, argv)
int	argc;
char 	*argv[];
{
	register char		**av;
	register char		*p;
	register int		i;
	register int		err;
	extern char		*AAproc_name;

	AAproc_name = "CREATER";

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
		AAsyserr(0, "Die Datenbank `%s' existiert nicht", Parmvect[0]);

	  case 2:
		AAsyserr(0, "Sie duerfen auf die Datenbank `%s' nicht zugreifen", Parmvect[0]);

	  case 3:
		AAsyserr(0, "Sie sind kein eingetragener DBS-Nutzer");

	  case 4:
		printf("Sie haben keinen Datenbanknamen angegeben\n");
usage:
		AAsyserr(0, "HINWEIS: creater db_name rel_name {dom_name format}");

	  default:
		AAsyserr(22200, AAproc_name, i);
	}

	for (i = 0, av = Flagvect; p = *av; av++)
	{
		printf("Unbekanntes Flag `%s'\n", p);
		i++;
	}

	if (i)
		goto usage;

	for (i = 0; Parmvect[i]; i++)
		continue;

	if ((argc = i) < 4 || argc % 2)
		goto usage;

	err = 0;
	for (i = 2; i < argc; i += 2)
		err |= ck_name(Parmvect[i]);
	if (err)
	{
		AAunlall();
		fflush(stdout);
		exit(-1);
	}

	if (chdir(AAdbpath) < 0)
		AAsyserr(22201, AAproc_name, AAdbpath);

	/* initialize access methods (and AAdmin struct) for user_ovrd test */
	AAam_start();
	opencatalog(AA_REL, 2);

	/* do function */
	Parmvect[0] = "0";	/* regular create */
	ruboff("creater");
	create(argc, Parmvect);
	rubon();

	/* logout database */
	closecatalog(TRUE);
	AAunldb();
	AAam_exit();
	fflush(stdout);
	exit(0);
}


rubproc()
{
	AAunlall();
	fflush(stdout);
	exit(1);
}


ck_name(name)
register char	*name;
{
	register int	i;
	register char	*p;

	if (AAlength(p = name) > MAXNAME)
	{
		printf("Name `%s' ist zu lang\n", name);
		return (1);
	}
	if (!(((i = *p) == '_') || (i >= 'a'
# ifdef EBCDIC
	   && i <= 'i') || (i >= 'j'
	   && i <= 'r') || (i >= 's'
# endif
	   && i <= 'z')))
		goto illegal;
	while (i = *p++)
	{
		if ((i == '_') || (i >= '0' && i <= '9') || (i >= 'a'
# ifdef EBCDIC
		   && i <= 'i') || (i >= 'j'
		   && i <= 'r') || (i >= 's'
# endif
		   && i <= 'z'))
			continue;
illegal:
		printf("Unerlaubter Name `%s'\n", name);
		return (1);
	}
	return (0);
}
