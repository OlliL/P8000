# include	<stdio.h>
# include	"../conf/gen.h"
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

extern int	AAdbstat;
char		*Fileset;

main(argc, argv)
int	argc;
char 	*argv[];
{
	register char		**av;
	register char		*p;
	register int		i;
	extern char		*AAdbpath;
	extern char		*AAproc_name;
	extern char		*Parmvect[];
	extern char		*Flagvect[];
# ifdef SETBUF
	char			stdbuf[BUFSIZ];

	setbuf(stdout, stdbuf);
# endif

	AAproc_name = "PRINT_BATCH_FILE";

#	ifdef xSTR1
	AAtTrace(&argc, argv, 'X');
#	endif

	switch (i = initucode(argc, argv, TRUE, (char *) 0, M_SHARE))
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
		AAsyserr(0, "Sie haben keinen Datenbanknamen angegeben");

	  default:
		AAsyserr(22200, AAproc_name, i);
	}

	if (Flagvect[0])
	{
		printf("Fuer dieses Kommando sind keine Flags gestattet\n");
		goto usage;
	}

	/*
	**  Check for usage errors.
	*/
	for (i = 1; Parmvect[i]; i++)
		continue;
	if (i < 2)
	{
usage:
		AAsyserr(0, "HINWEIS: prbatch db_name {batch_file}");
	}

	/*
	**  Check for superuser
	*/
	if (!(AAdbstat & U_SUPER))
		AAsyserr(0, "Sie duerfen PRBATCH nicht verwenden");

	p = Parmvect[0];	/* data base is first parameter */
	if (chdir(AAdbpath) < 0)
		AAsyserr(22201, AAproc_name, p);

	av = &Parmvect[1];
	while (p = *av++)
		prbatch(p);
	AAunldb();
	fflush(stdout);
	exit(0);
}


prbatch(name)
char	*name;
{
	register struct si_doms		*si;
	register int			i;
	register struct batchhd		*b;
	long				tupcnt;
	long				oldtid;
	char				oldtup[MAXTUP];
	long				newtid;
	char				newtup[MAXTUP];
	char				file_set_buf[IDSIZE + 1];
	extern long			lseek();

	if ((Batch_fp = open(name, 0)) < 0)
	{
		printf("BATCH file %s nicht lesbar\n", name);
		return;
	}

	read(Batch_fp, Fileset = file_set_buf, IDSIZE);
	lseek(Batch_fp, 0L, 0);
	file_set_buf[IDSIZE] = 0;

	b = &Batchhd;
	Batch_cnt = BATCHSIZE;
	getbatch(b, sizeof Batchhd);

	printf("BATCH\tfile\t%s\n", name);
	printf("\t  db\t%s\n", b->db_name);
	printf("\t rel\t%s\n", b->rel_name);
	printf("\tuser\t%s\n", b->userid);
	switch (b->mode_up)
	{
	  case mdAPP:
		printf("APPEND\n");
		break;

	  case mdDEL:
		printf("DELETE\n");
		break;

	  case mdREPL:
		printf("REPLACE\n");
		break;

	  default:
		printf("%d\n", b->mode_up);
	}
	printf("\t%4ld\tAnzahl der Aenderungen\n", b->num_updts);
	printf("\t%4d\tLaenge des alten   TID's\n", b->tido_size);
	printf("\t%4d\tLaenge des alten TUPLE's\n", b->tupo_size);
	printf("\t%4d\tLaenge des neuen   TID's\n", b->tidn_size);
	printf("\t%4d\tLaenge des neuen TUPLE's\n", b->tupn_size);
	printf("\t%4d\tAnzahl der Domaenen in Indizes\n", b->si_dcount);
	if (b->si_dcount)
	{
		printf("INDEX\tDomaenen\n");
		for (si = b->si, i = 0; i <= MAXDOM; i++, si++)
		{
			if (!si->dom_size)
				continue;

			printf("      %2d\tsize %3d", i, si->dom_size);
			printf(" tuple off %4d", si->rel_off);
			printf(" batch off %4d\n", si->tupo_off);
		}
	}

	printf("VALUES\n");
	tupcnt = b->num_updts;
	while (tupcnt--)
	{
		printf("%ld", tupcnt);
		getbatch(&oldtid, b->tido_size);/* read old tid */
		getbatch(oldtup, b->tupo_size);	/* and portions of old tuple */
		getbatch(newtup, b->tupn_size);	/* and the newtup */
		getbatch(&newtid, b->tidn_size);/* and the new tid */
		if (b->tido_size)
		{
			printf("\told");
			AAdumptid(&oldtid);
		}
		if (b->tupo_size)
		{
			printf("\toldtup:\t");
			for (i = 0; i < b->tupo_size; i++)
				AAxputchar(oldtup[i]);
			putchar('\n');
		}
		if (b->tidn_size)
		{
			printf("\tnew");
			AAdumptid(&newtid);
		}
		if (b->tupn_size)
		{
			printf("\tnewtup:\t");
			for (i = 0; i < b->tupn_size; i++)
				AAxputchar(newtup[i]);
			putchar('\n');
		}
	}
}


rubproc()
{
	AAunlall();
	fflush(stdout);
	exit(0);
}
