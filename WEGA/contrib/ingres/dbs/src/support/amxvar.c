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
static char		db_name[MAXNAME + 1];
static char		rel_name[MAXNAME + 1];
static int		indent;

# define	AMX_VAR		0
# define	AMX_GET		1
# define	AMX_RET		2
# define	AMX_APP		3
# define	AMX_DEL		4
# define	AMX_REP		5
static int		amx;

main(argc, argv)
int	argc;
char 	*argv[];
{
	register int		i;
	register FILE		*file;
	register char		**av;
	register char		*p;
# ifdef SETBUF
	char			buf[BUFSIZ];
# endif
	extern char		*AAproc_name;

	p = argv[0];
	switch (p[i = AAlength(p) - 3])
	{
	  case 'A':
	  case 'a':
		amx = AMX_APP;
		AAproc_name = "AMXAPP";
		break;

	  case 'D':
	  case 'd':
		amx = AMX_DEL;
		AAproc_name = "AMXDEL";
		break;

	  case 'G':
	  case 'g':
		amx = AMX_GET;
		AAproc_name = "AMXGET";
		break;

	  case 'R':
	  case 'r':
		if ((i = p[i + 2]) == 't' || i == 'T')
		{
			amx = AMX_RET;
			AAproc_name = "AMXRET";
		}
		else
		{
			amx = AMX_REP;
			AAproc_name = "AMXREP";
		}
		break;

	  default:
		amx = AMX_VAR;
		AAproc_name = "AMXVAR";
	}

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
		AAsyserr(0, "HINWEIS: %s db_name rel_name [var_file]", AAproc_name);

	  default:
		AAsyserr(22200, AAproc_name, i);
	}

	i = 0;
	for (av = Flagvect; p = *av; av++)
	{
		if (p[0] != '-')
			goto badflag;
		switch (p[1])
		{
		  case '1':
		  case '2':
		  case '3':
		  case '4':
		  case '5':
		  case '6':
		  case '7':
		  case '8':
		  case '9':
			indent = p[1] - '0';
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

	if (!(file = fopen(p = "a", "w")))
	{
		error(5704, AAproc_name, p, (char *) 0);
		goto term;
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

	mk_var(file);

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


static char	*AAvmove(src, len)
register char	*src;
register int	len;
{
	register char		*dst;
	static char		dst_buf[MAXFIELD + 1];

	dst = dst_buf;
	src += len++;
	while (--len && *--src == ' ')
		continue;
	dst += len++;
	*dst = 0;
	while (--len)
		*--dst = *src--;
	return (dst_buf);
}


static int	mk_var(file)
register FILE	*file;
{
	register DESC			*d;
	register struct attribute	*a;
	register int			i;
	register int			len;
	register char			*name;
	struct attribute		att;
	DESC				des;
	extern char			*AAvmove();

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

	switch (amx)
	{
	  case AMX_APP:
		prefix(file);
		fprintf(file, "append %s\n", name);
		goto lbrkt;

	  case AMX_DEL:
		prefix(file);
		fprintf(file, "delete\n");
		return;

	  case AMX_GET:
		prefix(file);
# ifdef PSI
		fprintf(file, "get %s Otidp\n", name);
# else
		fprintf(file, "get %s tidp\n", name);
# endif
		goto lbrkt;

	  case AMX_REP:
		prefix(file);
		fprintf(file, "replace\n");
		goto lbrkt;

	  case AMX_RET:
		prefix(file);
		fprintf(file, "retrieve %s\n", name);
lbrkt:
		prefix(file);
		fprintf(file, "(\n");
		if (amx == AMX_RET)
		{
			prefix(file);
# ifdef PSI
			fprintf(file, "\tOtidp,\n");
# else
			fprintf(file, "\ttidp,\n");
# endif
		}
		break;

	  default:
		fprintf(file, "\n\/\* vars of relation %s \*\/\n", name);
		prefix(file);
# ifdef PSI
		fprintf(file, "tid\t\tOtidp;\n");
# else
		fprintf(file, "tid\t\ttidp;\n");
# endif
	}
	seq_init(&Attdes, d);
	for (i = 1; seq_attributes(&Attdes, d, a); i++)
	{
		if (amx != AMX_VAR)
		{
			if (i > 1)
				fprintf(file, ",\n");
			prefix(file);
			putc('\t', file);
			name = AAvmove(a->attname, MAXNAME);
		}
		switch (amx)
		{
		  case AMX_APP:
		  case AMX_REP:
# ifdef PSI
			fprintf(file, "$%s = O%s", name, name);
# else
			fprintf(file, "$%s = %s", name, name);
# endif
			break;

		  case AMX_GET:
		  case AMX_RET:
# ifdef PSI
			fprintf(file, "O%s = $%s", name, name);
# else
			fprintf(file, "%s = $%s", name, name);
# endif
			break;

		  default:
			len = ctou(a->attfrml);
			switch (a->attfrmt)
			{
			  case INT:
				prefix(file);
# ifdef PSI
				fprintf(file, "i%d\t\tO%s;\n",
# else
				fprintf(file, "i%d\t\t%s;\n",
# endif
					len, AAvmove(a->attname, MAXNAME));
				break;

			  case FLOAT:
				prefix(file);
# ifdef PSI
				fprintf(file, "f%d\t\tO%s;\n",
# else
				fprintf(file, "f%d\t\t%s;\n",
# endif
					len, AAvmove(a->attname, MAXNAME));
				break;

			  case CHAR:
				prefix(file);
# ifdef PSI
				fprintf(file, "char\t\tO%s[%d];\n",
# else
				fprintf(file, "char\t\t%s[%d];\n",
# endif
					AAvmove(a->attname, MAXNAME), len + 1);
				break;
			}
		}
	}
	if (amx != AMX_VAR)
	{
		putc('\n', file);
		prefix(file);
		fprintf(file, ")\n");
		if (amx == AMX_GET || amx == AMX_RET)
		{
			prefix(file);
			fprintf(file, "{\n");
			prefix(file);
			fprintf(file, "}\n");
		}
	}
	fclose(file);
	AArelclose(d);
}


static int	prefix(file)
register FILE	*file;
{
	register int		i;

	fprintf(file, "##");
	if (!(i = indent))
		putc(' ', file);
	else
		while (i--)
			putc('\t', file);
}
