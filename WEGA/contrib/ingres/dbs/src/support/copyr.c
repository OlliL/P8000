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

int			Noupdt;
extern char		*Parmvect[];
extern char		*Flagvect[];
extern char		*AAdbpath;
extern char		*AAproc_name;

main(argc, argv)
int	argc;
char 	*argv[];
{
	register char		**av;
	register char		*p;
	register int		i;
	char			null;

	AAproc_name = "COPYR";

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
		AAsyserr(0, "HINWEIS: copyr db_name rel_name {dom_name format} into|from file_name");

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

	/* initialize access methods (and AAdmin struct) for user_ovrd test */
	AAam_start();
	opencatalog(AA_REL, 2);

	/* do function */
	for (i = 0; i < argc - 3; i++)
		Parmvect[i] = Parmvect[i + 1];
	null = 0;
	Parmvect[argc - 3] = &null;
	copy(argc, Parmvect);

	/* logout database */
	closecatalog(TRUE);
	AAunldb();
	AAam_exit();
	fflush(stdout);
	exit(0);
}


char	*AArelpath(rel)
register char	*rel;
{
	extern char	*AAztack();

	return (AAztack(AAztack(AAdbpath, "/"), rel));
}


rubproc()
{
	AAunlall();
	fflush(stdout);
	exit(1);
}


/* substitution */
fullwait()	{}
fork()		{}
pipe()		{}
