# include	<stdio.h>
# include	"../conf/gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/access.h"
# include	"../h/bs.h"

# ifdef P8000
ret_buf		AAjmp_buf;
# else
jmp_buf		AAjmp_buf;
# endif

int		Dflag;		/* print descriptor	*/
int		Lflag;		/* print line table	*/
short		Tflag;		/* print tuple		*/
extern int	AAdbstat;

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
# ifdef SETBUF
	char			stdbuf[BUFSIZ];

	setbuf(stdout, stdbuf);
# endif

	AAproc_name = "PAGER";

#	ifdef xSTR1
	AAtTrace(&argc, argv, 'X');
#	endif

	badf = 0;
	switch (i = initucode(argc, argv, TRUE, (char *) 0, -1))
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
		if (p[0] != '-')
			goto badflag;
		switch (p[1])
		{
		  case 'd':
		  case 'D':
			Dflag++;
			break;

		  case 'l':
		  case 'L':
			Lflag++;
			break;

		  case '1':
		  case '2':
		  case '3':
		  case '4':
		  case '5':
		  case '6':
		  case '7':
		  case '8':
		  case '9':
			AA_atoi(&p[1], &Tflag);
			Lflag++;
			break;

		  default:
badflag:
			printf("Unbekanntes Flag '%s'\n", p);
			badf++;
		}
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
	if (i < 2)
		AAsyserr(0, "HINWEIS: pager [-d] [-l] [-no] db_name {relation}");

	/*
	**  Check for superuser
	*/
	if (!(AAdbstat & U_SUPER))
		AAsyserr(0, "Sie duerfen PAGER nicht verwenden");

	p = Parmvect[0];	/* data base is first parameter */
	if (chdir(AAdbpath) < 0)
		AAsyserr(22201, AAproc_name, p);

	/* initialize access methods (and AAdmin struct) for user_ovrd test */
	AAam_start();

	av = &Parmvect[1];
	while (p = *av++)
		pager(p);
	fflush(stdout);
	exit(0);
}


# ifdef STACK
static ACCBUF		pg_buf;
# endif

pager(name)
register char	*name;
{
	register int		i;
	register ACCBUF		*b;
	register DESC		*d;
	register int		f;
	register int		k;
	long			page;
	DESC			desc;
# ifndef STACK
	ACCBUF			pg_buf;
# endif

	if ((i = AArelopen(d = &desc, 0, name)) == 1)
	{
		printf("PAGER: relation '%s' does not exist\n", name);
		return;
	}
	else if (i < 0)
		AAsyserr(22202, name, i);

	printf("\n\nPAGES for ");
	switch (abs(d->reldum.relspec))
	{
	  case M_HEAP:
		printf("HEAP");
		break;

	  case M_HASH:
		printf("HASH");
		break;

	  case M_ISAM:
		printf("ISAM");
		break;

	  default:
		printf("%4d", d->reldum.relspec);
	}
	printf("-relation %s: %5ld\n", name, d->reldum.relprim);
	printf("---------------------------------------\n\n");
	f = d->relfp;		/* file descriptor for relation file */
	page = 0;
	b = &pg_buf;

	if (Dflag)
	{
		AAprdesc(d);
		printf("\n");
	}
	if (d->reldum.relatts > Tflag)
		d->reldum.relatts = Tflag;

	while (read(f, b, AApgsize) == AApgsize)
	{
		printf("%5ld(%4d|%4d):%5ld->%5ld\n", page, b->nxtlino,
			b->linetab[LINETAB(b->nxtlino)], b->mainpg, b->ovflopg);
		if (Lflag)
		{
			if (Tflag)
			{
				for (i = 0; i < b->nxtlino; i++)
					if (k = b->linetab[LINETAB(i)])
					{
						printf("%5ld/%4d|%4d: ", page, i, k);
						AAprtup(d, ((char *) b) + k);
					}
			}
			else
			{
				printf("     ");
				for (i = 0; i < b->nxtlino; i++)
					if (b->linetab[LINETAB(i)])
						printf("%5d", i);
				printf("\n     ");
				for (i = 0; i < b->nxtlino; i++)
					if (k = b->linetab[LINETAB(i)])
						printf("%5d", k);
				printf("\n");
			}
		}
		page++;
	}
	AArelclose(d);
}


rubproc()
{
	fflush(stdout);
	exit(0);
}
