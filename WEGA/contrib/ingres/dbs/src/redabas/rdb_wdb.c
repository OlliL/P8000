/*
**	J. Wenzlaff, ZKI der AdW
**
**	READ and EXECUTE DBI-file
*/

# include	"db.h"
# include	"../h/catalog.h"
# ifdef MSDOS
# include	"../h/aux#.h"
# include	<process.h>
# else
# include	"../h/aux.h"
# endif

# define	DBI_LINE		512
# define	MARK		*Line

char		RW_flag;		/* read/write flag		*/
short		Refs[TABS * MAXDOM];	/* DB/REDABAS-references	*/
short		Flds[DB_FLDMAX + 2];	/* REDABAS/DB-references	*/
DESC		*Desc;			/* actual relation descriptor	*/
DESC		Desc_s[TABS];		/* descriptor cache		*/
char		*N_db;			/* overwrinting for DB		*/
char		*N_dbd;			/* overwrinting for RDABAS.DBD	*/
static ACCBUF	Xtrabufs[TABS];		/* buffer cache			*/
static FILE	*Dbi;			/* DBI-file			*/
static char	Line[DBI_LINE];		/* line buffer for DBI-file	*/
static int	Lines;			/* line count for DBI-file	*/
static char	*Ptrvec[20];		/* ptrs to fields of DBI-line	*/
static int	Ptrs;			/* field count for DBI-line	*/
extern char	*AAproc_name;
extern char	*AAdbpath;
extern char	*Parmvect[];

/*
** DBI		CONVERSION DRIVER
*/
dbi(dbi_file)
register char	*dbi_file;
{
	register int		i;
	register int		rels;
	register FILE		*dbi;
	register int		err;
# ifndef MSDOS
	static char		dbpbuf[MAXLINE];
# endif

	rels = 0;
	if (dbiopen(dbi_file))
		return;

	/*
	** command part before conversion
	*/
	while (!(i = dbi_line()))
	{
		if (MARK != CMD_READ && MARK != CMD_WRITE)
			break;
		if (MARK == RW_flag && shell(Ptrvec[PTR_NAME]))
		{
exec_err:
			printf("REDABAS\t%s FAILED\n", Ptrvec[PTR_NAME]);
			done(rels);
			return;
		}
	}
	if (i)
	{
eof:
		printf("REDABAS\tDBI-file: unexpected eof on line %d\n", Lines);
		done(rels);
		return;
	}

	/*
	** read REDABAS header informations
	*/
	if (MARK != CMD_DBD)
	{
		printf("\tmissing DBD-line [%c ...]\n", CMD_DBD);
syntax:
		printf("REDABAS\tDBI-file: syntax error on line %d\n", Lines);
		done(rels);
		return;
	}

	/*
	** read REDABAS fields
	*/
	if (N_dbd)
		Ptrvec[PTR_NAME] = N_dbd;
	if (bgn_fld(Ptrs, Ptrvec))
		goto syntax;
	while (!(i = dbi_line()) && MARK == CMD_FIELD)
		if (chk_fld(Ptrs, Ptrvec))
			goto syntax;
	if (i)
		goto eof;
	if (end_fld())
		goto syntax;

	/*
	** initialize data base
	*/
	if (MARK != CMD_DB)
	{
		printf("\tmissing DB-line [%c ...]\n", CMD_DB);
		goto syntax;
	}
	if (N_db)
		Ptrvec[PTR_NAME] = N_db;
	if (!Ptrs)
	{
		printf("\tmissing DB-name\n");
		goto syntax;
	}
# ifdef MSDOS
	i = initdbpath(Ptrvec[PTR_NAME]);
# else
	i = initdbpath(Ptrvec[PTR_NAME], dbpbuf, 1);
# endif
	switch (i)
	{
	  case 0:
	  case 1:
		break;

	  case 2:
	  case 3:
		printf("\tcan't access DB %s\n", Ptrvec[PTR_NAME]);
		goto syntax;

	  default:
		AAsyserr(22200, AAproc_name, i);
	}
# ifndef MSDOS
	AAdbpath = dbpbuf;
# endif
	AAam_start();
	AAam_addbuf(Xtrabufs, TABS);

	/*
	** read relations
	*/
	i = dbi_line();
	while (!i && MARK == CMD_REL)
	{
		if (rels >= TABS)
		{
			printf("\ttoo many RELATION-lines [%c ...]\n", CMD_REL);
			goto syntax;
		}
		Desc = &Desc_s[rels];
		if (bgn_dom(Ptrs, Ptrvec))
			goto syntax;
		while (!(i = dbi_line()) && MARK == CMD_DOMAIN)
			if (chk_dom(Ptrs, Ptrvec))
				goto syntax;
		if (end_dom())
			goto syntax;
		bgn_ref(++rels);
		while (!i && MARK == CMD_REF)
		{
			if (chk_ref(Ptrs, Ptrvec))
				goto syntax;
			i = dbi_line();
		}
		if (end_ref())
			goto syntax;
	}
	if (!rels)
	{
		printf("\tmissing RELATION-line [%c ...]\n", CMD_REL);
		goto syntax;
	}
	err = transfer(rels);
	dbi = Dbi;
	Dbi = (FILE *) 0;
	done(rels);
	Dbi = dbi;
	if (err)
		return;

	/*
	** command part after conversion
	*/
	while (!i && (MARK == CMD_READ || MARK == CMD_WRITE))
	{
		if (MARK == RW_flag && shell(Ptrvec[PTR_NAME]))
			goto exec_err;
		i = dbi_line();
	}
	if (!i)
	{
		printf("\textra lines\n");
		goto syntax;
	}
	done(rels);
}

done(rels)
register int	rels;
{
	register int		i;

	if (Dbi)
	{
		fclose(Dbi);
		Dbi = (FILE *) 0;
	}
	for (i = 0; i < rels; i++)
	{
		AAunlrl((Desc = &Desc_s[i])->reltid);
		AArelclose(Desc);
	}
	return (0);
}

/*
** DBIOPEN	open DBI-file
**		return (0) = ok
**		return (1) = error
*/
dbiopen(dbi_file)
register char	*dbi_file;
{
# ifdef SETBUF
	static char		dbi_buf[BUFSIZ];
# endif

	if ((Dbi = fopen(dbi_file, "r")) == (FILE *) 0)
	{
		printf("REDABAS\tcan't access DBI-file %s\n", dbi_file);
		return (1);
	}
# ifdef SETBUF
	setbuf(Dbi, dbi_buf);
# endif
	Lines = 0;
	return (0);
}

/*
**	DBI_LINE	get a line from DBI-file
**		return (0) = ok
**		return (1) = eof
*/
static int	dbi_line()
{
	register char		*l;
	register int		ch;
	register char		**p;
	register int		str;

	Ptrs = str = 0;
repeat:
	Lines++;
	ch = dbi_char(str);
	fflush(stdout);
	switch (ch)
	{
	  case 0:
		return (1);

	  case CMD_COMMENT:
		while (ch != '\n')
			if (!(ch = dbi_char(str)))
				return (1);
		/* fall through */

	  case '\n':
		goto repeat;

	  case CMD_READ:
	  case CMD_WRITE:
		str = ' ';
		break;
	}
	l = Line;
	*l++ = ch;
	p = Ptrvec - 1;
	while (ch = dbi_char(str))
	{
		if (ch == '\n')
		{
			*l++ = 0;
			break;
		}
		if (ch == ' ')
		{
			if (!(*l++ = str) || p < Ptrvec)
				*(++p) = l;
			continue;
		}
		*l++ = ch;
	}
	if (!**p)
		--p;
	Ptrs = p - Ptrvec + 1;
	return (!ch);
}

/*
**	DBI_CHAR	get a char from DBI-file
**		return (char)
**		return (0) = eof
*/
static int	dbi_char(str)
register int	str;
{
	register int		ch;
	register FILE		*f;
	static int		space;

	f = Dbi;
repeat:
	switch (ch = getc(f))
	{
	  case '\t':
		ch = ' ';
		/* fall through */

	  case ' ':
		if (str)
			break;
		if (space++)
			goto repeat;
		return (ch);

	  case EOF:
		ch = 0;		/* no errors */
		/* fall through */

	  case 0:
		fclose(f);
		Dbi = (FILE *) 0;
		break;
	}
	space = 0;
	return (ch);
}


char	*AArelpath(rel)
register char	*rel;
{
	extern char	*AAztack();

	return (AAztack(AAztack(AAdbpath, "/"), rel));
}


/*
**	CHECK domain specification of DBI-file
*/

static char		Dom[MAXDOM];	/* domain check area		*/
static short		*Db_ref;

/*
** BGN_DOM	start domain check
**		return (0) = ok
**		return (1) = error
*/
static int	bgn_dom(argc, argv)
int	argc;
char	**argv;
{
	register int		i;
	register DESC		*d;
	register char		*rel;
	register int		into;

	if (!argc)
	{
		printf("\tmissing RELATION-name\n");
		return (1);
	}
	/* clear domain check area */
	for (i = 0; i < MAXDOM; i++)
		Dom[i] = 0;

	/* relation must exist and not be a system relation */
	/* in addition a copy "from" can't be done if the user */
	/* doesn't own the relation */
	/* and furthermore it can't have an index */
	if (i = AArelopen(d = Desc, 2, rel = argv[PTR_NAME]))
	{
		if (i == AMOPNVIEW_ERR)
			printf("\tcan't access VIEW %s\n", rel);
		else
			goto not_owned;
	}
	else if (into = RW_flag == CMD_WRITE)
	{
		if ((d->reldum.relstat & S_PROTALL)
			&& (d->reldum.relstat & S_PROTRET)
			&& !AAbequal(AAusercode, d->reldum.relowner, 2))
				goto not_owned;
	}
	else
	{
		if (!AAbequal(AAusercode, d->reldum.relowner, 2) || (d->reldum.relstat & S_NOUPDT))
		{
not_owned:
			printf("\tcan't access RELATION %s\n", rel);
			i++;
		}
		else if (ctoi(d->reldum.relindxd) == SECBASE)
		{
			printf("\tRELATION %s has indexes\n", rel);
			i++;
		}
	}
	if (i)
		goto no_rel;

	/* set a lock on relation*/
	if (AAsetrll(A_SLP, d->reltid, into? M_SHARE: M_EXCL) < 0)
	{
		printf("\tcan't lock RELATION %s\n", rel);
no_rel:
		AArelclose(d);
		return (1);
	}
	return (0);
}

/*
** CHK_DOM	domain check
**		return (0) = ok
**		return (1) = error
*/
static int	chk_dom(argc, argv)
int		argc;
register char	**argv;
{
	register int		i;
	register DESC		*d;
	register int		l;

	if (argc < 3)
	{
		printf("\tmissing DOMAIN-%s\n",
			argc? ((argc > 1)? "type & length": "name"): "number");
		return (1);
	}
	if ((i = atoi(argv[PTR_NAME])) > (d = Desc)->reldum.relatts || i < 1)
	{
		printf("\tillegal DOMAIN-number %d\n", i);
		return (1);
	}
	if (Dom[l = i - 1])
	{
		printf("\tduplicate DOMAIN-number %d\n", i);
		return (1);
	}
	Dom[l] = 1;
	if ((l = typeunconv(d->relfrmt[i])) != *(argv[PTR_TYPE]))
	{
		printf("\ttype error on DOMAIN %d\n", i);
		printf("\texpected %c\n", l);
		printf("\t   found %c\n", *(argv[PTR_TYPE]));
		return (1);
	}
	if (ctou(d->relfrml[i]) != (l = atoi(argv[PTR_TYPE] + 1)))
	{
		printf("\tlength error on DOMAIN %d\n", i);
		printf("\texpected %d\n", ctou(d->relfrml[i]));
		printf("\t   found %d\n", l);
		return (1);
	}
	return (0);
}

/*
** END_DOM	end of domain check
**		return (0) = ok
**		return (1) = error
*/
static int	end_dom()
{
	register int		i;

	for (i = 0; i < MAXDOM; i++)
		if (!Dom[i])
		{
			--i;
			break;
		}
	if (++i != Desc->reldum.relatts)
	{
		printf("\tmissing DOMAIN with number %d\n", ++i);
		return (1);
	}
	return (0);
}

/*
** BGN_REF	start reference check
*/
static int	bgn_ref(argc)
register int	argc;
{
	register short		*r;
	register int		i;

	/* clear reference area */
	for (i = MAXDOM, r = &Refs[argc * MAXDOM]; i; --i)
		*(--r) = -1;	/* no reference */
	Db_ref = r;
}

/*
** CHK_REF	reference check
**		return (0) = ok
**		return (1) = error
*/
static int	chk_ref(argc, argv)
int		argc;
register char	**argv;
{
	register short		*r;
	register int		d;
	register int		f;

	if (argc < 2)
	{
		printf("\tmissing REFERENCE-%s-number\n", argc? "domain": "field");
		return (1);
	}
	if ((f = atoi(argv[PTR_NAME])) >= Db_hdr.db_fields || f < 0)
	{
		printf("\tillegal REFERENCE-field-number %d\n", f);
		return (1);
	}
	if ((d = atoi(argv[PTR_NO])) > Desc->reldum.relatts || d < 1)
	{
		printf("\tillegal REFERENCE-domain-number %d\n", d);
		return (1);
	}
	if ((r = Db_ref)[d] >= 0)
	{
		printf("\tduplicate REFERENCE to domain %d\n", d);
		return (1);
	}
	for (r = Db_ref + MAXDOM; r > Db_ref; )
		if (*(--r) == f)
		{
			printf("\tduplicate REFERENCE to field %d\n", f);
			return (1);
		}
	(r = Db_ref)[d] = f;
	return (0);
}

/*
** END_REF	end of reference check
**		return (0) = ok
**		return (1) = error
*/
static int	end_ref()
{
	register short		*r;

	for (r = Db_ref; r < Db_ref + MAXDOM; r++)
		if (*r >= 0)
			return (0);
	printf("\tmissing REFERENCES\n");
	return (1);
}


/*
**	SHELL	CALL BS SHELL
**		return (0) = ok
**		return (1) = error
*/
static int	shell(cmd)
register char	*cmd;
{
# ifdef MSDOS
	register char		*p;
	extern char		*getenv();
# else
	register int		i;
	register int		pid;
# endif
	int			status;

	if (!cmd || !*cmd)
		return (0);

	printf("REDABAS\tEXECUTING  ...  %s\n", cmd);
	fflush(stdout);
# ifdef MSDOS
	if (!(p = getenv("COMSPEC")))
	{
		printf("REDABAS\tno COMSPEC\n");
		fflush(stdout);
		exit(1);
	}
	if ((status = spawnlp(P_WAIT, p, p, "/C", cmd, (char *) 0)) < 0)
	{
		printf("REDABAS\texec error\n");
		fflush(stdout);
		exit(1);
	}
}
# else
	if ((pid = fork()) < 0)
	{
		printf("REDABAS\tfork error\n");
		fflush(stdout);
		return (1);
	}
	if (!pid)
	{
		setuid(getuid());
		setgid(getgid());
		for (i = 3; i < MAXFILES; i++)
			close(i);
		execlp("sh", "sh", "-c", cmd, (char *) 0);
		printf("REDABAS\texec error\n");
		fflush(stdout);
		exit(1);
	}

	/* wait for shell to complete */
	while ((i = wait(&status)) != -1)
		if (i == pid)
			break;
# endif
	return (status >> 8);
}
