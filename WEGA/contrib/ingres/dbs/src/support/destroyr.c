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

extern char		*Parmvect[];
extern char		*Flagvect[];
extern char		*AAdbpath;
extern int		Standalone;

main(argc, argv)
int	argc;
char 	*argv[];
{
	register char		**av;
	register char		*p;
	register int		i;
	extern char		*AAproc_name;

	AAproc_name = "DESTROYR";

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
		AAsyserr(0, "HINWEIS: destroyr db_name {rel_name}");

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

	if ((argc = i) < 2)
		goto usage;

	if (chdir(AAdbpath) < 0)
		AAsyserr(22201, AAproc_name, AAdbpath);

	/* hello eric */
	Standalone = 0;

	/* initialize access methods (and AAdmin struct) for user_ovrd test */
	AAam_start();
	opencatalog(AA_REL, 2);

	/* do function */
	Parmvect[argc] = (char *) -1;
	destroy(--argc, &Parmvect[1]);

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
