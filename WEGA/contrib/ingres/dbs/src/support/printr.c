# include	<stdio.h>
# include	"gen.h"
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

FILE		*Dbs_out = stdout;
extern int	AAdbstat;

main(argc, argv)
int	argc;
char 	*argv[];
{
	register char		**av;
	register char		*q;
	register char		*p;
	register int		i;
	register int		badf;
	register int		mode;
	register int		nc;
	char			style;
	char			stdbuf[BUFSIZ];
	extern struct out_arg	AAout_arg;
	extern char		*Parmvect[];
	extern char		*Flagvect[];
	extern char		*AAdbpath;
	extern char		*AAproc_name;

	AAproc_name = "PRINTR";

#	ifdef xSTR1
	AAtTrace(&argc, argv, 'X');
#	endif

	mode = -1;
	badf = 0;

	/*
	**  Scan the argument vector and otherwise initialize.
	*/
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
		  case 'H':
		  case 'h':		/* do headers on each page */
			mode = -2;
			if (!p[2])
				break;
			if (AA_atoi(&p[2], &AAout_arg.linesperpage))
				goto badflag;
			break;

		  case 'S':
		  case 's':		/* supress headers and footers */
			mode = -3;
			if (p[2])
				goto badflag;
			break;

		  case 'C':
		  case 'c':		/* set cNwidth */
			if (AA_atoi(&p[2], &AAout_arg.c0width))
			{
badflag:
				printf("Unbekanntes Flag '%s'\n", p);
				badf++;
				continue;
			}
			break;

		  case 'I':
		  case 'i':		/* set iNwidth */
			switch (p[2])
			{
			  case '1':
				if (AA_atoi(&p[3], &AAout_arg.i1width))
					goto badflag;
				break;

			  case '2':
				if (AA_atoi(&p[3], &AAout_arg.i2width))
					goto badflag;
				break;

			  case '4':
				if (AA_atoi(&p[3], &AAout_arg.i4width))
					goto badflag;
				break;

			  default:
				goto badflag;
			}
			break;

		  case 'F':
		  case 'f':		/* set fNwidth */
			style = p[3];
			switch (style)
			{
			  case 'E':
			  case 'e':
			  case 'F':
			  case 'f':
			  case 'G':
			  case 'g':
			  case 'N':
			  case 'n':
				break;

			  default:
				goto badflag;
			}
			for (q = &p[4]; *q != '.'; q++)
				if (!*q)
					goto badflag;
			*q++ = 0;
			switch (p[2])
			{
			  case '4':
# ifndef NO_F4
				if (AA_atoi(&p[4], &AAout_arg.f4width))
					goto badflag;
				if (AA_atoi(q, &AAout_arg.f4prec))
					goto badflag;
				AAout_arg.f4style = style;
				break;
# endif

			  case '8':
				if (AA_atoi(&p[4], &AAout_arg.f8width))
					goto badflag;
				if (AA_atoi(q, &AAout_arg.f8prec))
					goto badflag;
				AAout_arg.f8style = style;
				break;

			  default:
				goto badflag;
			}
			break;

		  case 'V':
		  case 'v':
			if (!p[2] || p[3])
				goto badflag;
			AAout_arg.coldelim = p[2];
			break;

		  case 'L':
		  case 'l':
			if (!p[2] || p[3])
				goto badflag;
			AAout_arg.linedelim = p[2];
			break;

		  default:
			goto badflag;
		}
	}

	/*
	**  Find end of Parmvect and store mode there
	*/
	for (nc = 1; Parmvect[nc]; nc++)
		continue;
	Parmvect[nc] = (char *) mode;

	/*
	**  Check for usage errors.
	*/
	if (nc < 2)
	{
		badf++;
		printf("HINWEIS: printr [flags] db_name {relation}\n");
	}
	if (badf)
	{
		AAunlall();
		fflush(stdout);
		exit(-1);
	}

	p = Parmvect[0];	/* data base is first parameter */
	if (chdir(AAdbpath) < 0)
		AAsyserr(22201, AAproc_name, p);

# ifdef SETBUF
	setbuf(stdout, stdbuf);
# endif

	/* initialize access methods (and AAdmin struct) for user_ovrd test */
	AAam_start();

	print(nc - 1, &Parmvect[1]);
	AAunldb();
	fflush(stdout);
	exit(0);
}


rubproc()
{
	AAunlall();
	fflush(stdout);
	exit(-1);
}
