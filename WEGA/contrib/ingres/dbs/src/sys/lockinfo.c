# include	<stdio.h>
# include	"../conf/gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/access.h"
# include	"../h/lock.h"
# include	"../h/bs.h"

# ifdef P8000
ret_buf		AAjmp_buf;
# else
jmp_buf		AAjmp_buf;
# endif

extern int	AAdbstat;
struct admin	AAdmin;

main(argc, argv)
int	argc;
char 	*argv[];
{
	register char		**av;
	register char		*p;
	register int		i;
	register int		badf;
	extern char		*AAdbpath;
	extern char		*AAproc_name;
	extern char		*Parmvect[];
	extern char		*Flagvect[];

	AAproc_name = "LOCKINFO";

#	ifdef xSTR1
	AAtTrace(&argc, argv, 'X');
#	endif

	badf = 0;
	switch (i = initucode(argc, argv, argc > 1, (char *) 0, -1))
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
		badf++;
		break;

	  default:
		AAsyserr(22200, AAproc_name, i);
	}

	for (av = Flagvect; p = *av; av++)
	{
		printf("Unbekanntes Flag '%s'\n", p);
		badf++;
	}
	if (badf)
	{
		flush();
		exit(-1);
	}

	/*
	**  Check for usage errors.
	*/
	for (i = 1; Parmvect[i]; i++)
		continue;
	if (i > 1)
		AAsyserr(0, "HINWEIS: lockinfo [db_name]");

	/*
	**  Check for superuser
	*/
	if (!(AAdbstat & U_SUPER))
		AAsyserr(0, "Sie duerfen LOCKINFO nicht verwenden");

	if (argc == 1)
		AALOCKREQ.l_db = 0L;

	AAlockdes = open(AA_DBS_LOCK, 1);
	AAsyslock(A_INFO, M_DB, 0L, 0L);
	sync();

	fflush(stdout);
	exit(0);
}

AApageflush()
{
}

rubproc()
{
	fflush(stdout);
	exit(0);
}
