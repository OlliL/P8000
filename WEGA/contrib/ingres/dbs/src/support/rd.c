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
extern DESC		Attdes;

main(argc, argv)
int	argc;
char 	*argv[];
{
	register int		i;
	register FILE		*file;
	register char		**av;
	register char		*p;
	register int		oflag;
	register int		rflag;
# ifdef SETBUF
	char			buf[BUFSIZ];
# endif
	extern char		*AAproc_name;

	AAproc_name = "RD";

#	ifdef xSTR1
	AAtTrace(&argc, argv, 'X');
#	endif

	i = initucode(argc, argv, TRUE, (char **) 0, M_SHARE);

#	ifdef xSTR2
	AAtTfp(0, 1, "initucode=%d, AAdbpath='%s'\n", i, AAdbpath);
#	endif

	switch (i)
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
		AAsyserr(0, "HINWEIS: rd db_name rel_name [desc_file]");

	  default:
		AAsyserr(22200, AAproc_name, i);
	}

	i = 0;
	rflag = oflag = 0;
	for (av = Flagvect; p = *av; av++)
	{
		if (p[0] != '-')
			goto badflag;
		switch (p[1])
		{
		  case 'o':
		  case 'O':
			if (!p[1])
				goto badflag;
			oflag++;
			break;

		  case 'r':
		  case 'R':
			if (!p[1])
				goto badflag;
			rflag++;
			break;

		  default:
badflag:
			printf("Unbekanntes Flag '%s'\n", p);
			i++;
		}
	}
	if (i)
	{
term:
		AAunlall();
		fflush(stdout);
		exit(-1);
	}

	if (!Parmvect[1])
	{
		printf("Sie haben keinen Relationsnamen angegeben\n");
		goto usage;
	}

	if (oflag)
		file = stdout;
	else
	{
		p = Parmvect[2];
		if (!p)
			p = rflag? "report": "desc";
		if (!(file = fopen(p, "w")))
		{
			error(5704, AAproc_name, p, (char *) 0);
			goto term;
		}
	}
# ifdef SETBUF
	setbuf(file, buf);
# endif

	if (chdir(AAdbpath) < 0)
		AAsyserr(22201, AAproc_name, AAdbpath);

#	ifdef xSTR2
	AAtTfp(1, 0, "entered database %s\n", AAdbpath);
#	endif

	/* initialize access methods (and AAdmin struct) for user_ovrd test */
	AAam_start();
	opencatalog(AA_REL, 0);

	mk_desc(file, rflag);

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


mk_desc(file, rflag)
register FILE	*file;
register int	rflag;
{
	register DESC			*d;
	register struct attribute	*a;
	register int			i;
	register char			*name;
	struct attribute		att;
	DESC				des;

	d = &des;
	a = &att;
	opencatalog(AA_ATT, 0);

	name = Parmvect[1];
	i = AArelopen(d, 0, name);
	if (i == AMOPNVIEW_ERR)
		return (error(5702, AAproc_name, name, (char *) 0));
	if (i > 0)
		return (error(5701, AAproc_name, name, (char *) 0));
	if (i < 0)
		AAsyserr(21179, AAproc_name, name, i);
	if ((d->reldum.relstat & S_PROTALL) && (d->reldum.relstat & S_PROTRET) &&
		!AAbequal(AAusercode, d->reldum.relowner, 2))
	{
		AArelclose(d);
		error(5703, AAproc_name, name, (char *) 0);
		return;
	}

	if (rflag)
	{
		fprintf(file, "report r_code # desc\n");
		fprintf(file, "with\n");
		fprintf(file, "\t\/\* page layout \*\/\n\n");
		fprintf(file, "\t\/\* procedures \*\/\n\n");
		fprintf(file, "\t\/\* variables \*\/\n\n");
		fprintf(file, "\t\/\* switches \*\/\n\n");
		fprintf(file, "\t\/\* domains \*\/\n");
	}

	seq_init(&Attdes, d);
	for (i = 1; seq_attributes(&Attdes, d, a); i++)
	{
		if (!rflag)
		{
			putc(a->attfrmt, file);
			putc(a->attfrml, file);
			name = (char *) &a->attoff;
			putc(*name, file);
			name++;
			putc(*name, file);
			continue;
		}

		fprintf(file, "\tdefine #%.12s = ", a->attname);
		switch (a->attfrmt)
		{
		  case INT:
			fprintf(file, "integer");
			break;

		  case FLOAT:
			fprintf(file, "real   ");
			break;

		  case CHAR:
			fprintf(file, "char   ");
			break;
		}
		fprintf(file, " %3d\n", ctou(a->attfrml));
	}
	if (rflag)
	{
		fprintf(file, "\nas\n\n");
		fprintf(file, "on pagebegin do\n");
		fprintf(file, "end\n\n");
		fprintf(file, "on pageend do\n");
		fprintf(file, "end\n\n");
		fprintf(file, "on linebegin do\n");
		fprintf(file, "end\n\n");
		fprintf(file, "on lineend do\n");
		fprintf(file, "end\n\n");
		fprintf(file, "on batchline do\n");
		fprintf(file, "end\n\n");
		fprintf(file, "for all do\n");
		fprintf(file, "\tfor one do\n");
		fprintf(file, "\tend one\n");
		fprintf(file, "end all\n");
		fprintf(file, ";\n");
	}
	fclose(file);
	AArelclose(d);
}
