# include	<stdio.h>
# include	"../conf/gen.h"
# include	"../h/dbs.h"
# include	"../h/access.h"
# include	"../h/aux.h"
# include	"../h/bs.h"

# ifdef P8000
ret_buf		AAjmp_buf;
# else
jmp_buf		AAjmp_buf;
# endif

extern int	AAdbstat;

main(argc, argv)
int	argc;
char	*argv[];
{
	struct admin		ad;
	register int		i;
	register int		fp;
	register char		*db;
	extern char		*AAdbpath;
	extern char		*AAproc_name;
	extern char		*Parmvect[];
	extern char		*Flagvect[];
# ifdef SETBUF
	char			stdbuf[BUFSIZ];

	setbuf(stdout, stdbuf);
# endif

	AAproc_name = "ADMIN";

#	ifdef xSTR1
	AAtTrace(&argc, argv, 'X');
#	endif

	i = initucode(argc, argv, TRUE, (char *) 0, -1);
	db = Parmvect[0];
	switch (i)
	{
	  case 0:
	  case 5:
		break;

	  case 1:
	  case 6:
		AAsyserr(0, "Die Datenbank '%s' existiert nicht", db);

	  case 2:
		AAsyserr(0, "Sie duerfen auf die Datenbank '%s' nicht zugreifen", db);

	  case 3:
		AAsyserr(0, "Sie sind kein eingetragener DBS-Nutzer");

	  case 4:
		printf("Sie haben keinen Datenbankname angegeben\n");
	usage:
		AAsyserr(0, "HINWEIS: admin db_name");

	  default:
		AAsyserr(22000, AAproc_name, i);
	}

	if (Flagvect[0])
	{
		printf("Fuer dieses Kommando sind keine Flags gestattet\n");
		goto usage;
	}

	if (Parmvect[1])
		goto usage;

	if (!(AAdbstat & U_SUPER))
		AAsyserr(0, "Sie duerfen ADMIN nicht verwenden");

	if (chdir(AAdbpath) < 0)
		AAsyserr(22001, AAproc_name, db);

	if ((fp = open(AA_ADMIN, 0)) < 0)
		AAsyserr(0, "Fehler beim Eroeffnen des DBA file '%s/admin'", AAdbpath);
	if (read(fp, &ad, sizeof ad) != sizeof ad)
		AAsyserr(22076);

	printf("Datenbank %s, DBA %.2s, Flags 0%o\n", db, ad.adhdr.adowner, ad.adhdr.adflags);

	AAprdesc(&ad.adreld);
	printf("\n\n");
	AAprdesc(&ad.adattd);
	fflush(stdout);
	exit(0);
}


rubproc()
{
	fflush(stdout);
	exit(1);
}
