/*
**	J. Wenzlaff, ZKI der AdW
**
**	REDABAS -- creation of REDABAS / DB relations
*/

# include	"db.h"
# include	"../h/version.h"

# ifdef P8000
ret_buf		AAjmp_buf;
# else
jmp_buf		AAjmp_buf;
# endif

static char	D = 1;			/* don't convert record mark&no	*/
static char	List;			/* flag to list only		*/
struct admin	AAdmin;
extern char	*Parmvect[];
extern char	*Flagvect[];
# ifdef MSDOS
char		*Database;
char		*Fileset;
extern char	*mkfileset();
extern char	*AAdbpath[];
# else
extern char	*AAdbpath;
# endif

# define	DBI_PARM	Parmvect[0]
# define	DBD_PARM	Parmvect[1]
# define	DB_PARM		Parmvect[2]
# define	REL_PARM	Parmvect[3]

main(argc, argv)
int	argc;
char 	*argv[];
{
	register int		i;
	register char		**av;
	register char		*p;
	extern char		*AAproc_name;
# ifdef SETBUF
	char			buf[BUFSIZ];

	setbuf(stdout, buf);
# endif
	AAproc_name = *argv;

# ifdef MSDOS
	Fileset = mkfileset();
# endif

	i = initucode(argc, argv, 0, (char **) 0, M_SHARE);
	switch (i)
	{
	  case 0:
		break;

	  case 3:
		AAsyserr(0, "REDABAS\tSie sind kein eingetragener %s-Nutzer", USERDBS);

	  default:
		AAsyserr(22200, AAproc_name, i);
	}
# ifdef MSDOS
	Database = Parmvect[0];
# endif

	i = 0;
	for (av = Flagvect; p = *av; av++)
	{
		if (p[0] != '-')
			goto badflag;
		switch (p[1])
		{
		  case 'f':
		  case 'F':
			List_fld++;
			break;

		  case 'h':
		  case 'H':
			List_hdr++;
			break;

		  case 'i':
		  case 'I':
			D = -1;
			break;

		  case 'l':
		  case 'L':
			List++;
			break;

		  case 'r':
		  case 'R':
			switch (p[2])
			{
			  case 'd':
			  case 'D':
				List_rec = 1;
				break;

			  case 'v':
			  case 'V':
				List_rec = 2;
				break;

			  case 0:
				List_rec = 3;
				break;

			  default:
				goto badflag;
			}
			break;

		  default:
badflag:
			printf("REDABAS\tUnbekanntes Flag '%s'\n", p);
			i++;
		}
	}
	if (i)
		goto usage;

	if (!REL_PARM)
	{
usage:
		printf("usage:\t%s [flags] dbi dbd db {rel}\n", *argv);
		printf("flags:\t-f     list field descriptions\n");
		printf("\t-h     list header\n");
		printf("\t-i     convert into %s format\n", USERDBS);
		printf("\t-l     don't create DBI-file\n");
		printf("\t-r     list   all   records\n");
		printf("\t-rd    list deleted records\n");
		printf("\t-rv    list  valid  records\n");
		goto term;
	}

	/* open REDABAS head informations */
	if (rdhdr(DBD_PARM))
	{
		printf("REDABAS\tsyntax error\n");
		goto term;
	}

	/* create DBI-file (if wanted) */
	if (!List && dbi())
		goto term;

	/* list REDABAS records (if wanted) */
	if (!List_rec || rdopen(DBD_PARM))
		goto term;
	printf("\n----no-*------------------------------------------------\n");
	for (i = 1; rdrec(); i++)
		prrec();
	printf("--------------------------------------------------------\n");

term:
	fflush(stdout);
	return (0);
}

/*
** DBI		create DBI-file
**		return (0) = ok
**		return (1) = error
*/
static int	dbi()
{
	register int		i;
	register FILE		*dbi;
	register struct db_fld	*f;
	register char		**rel;
	extern char		*lower();
# ifdef SETBUF
	static char		dbi_buf[BUFSIZ];
# endif

	if ((dbi = fopen(DBI_PARM, "w")) == (FILE *) 0)
	{
		printf("REDABAS\tcan't access DBI-file %s\n", DBI_PARM);
		return (1);
	}
# ifdef SETBUF
	setbuf(dbi, dbi_buf);
# endif

	fprintf(dbi, "%c command  part before conversion\n", CMD_COMMENT);
	fprintf(dbi, "\n%c REDABAS %c part\n", CMD_COMMENT, CMD_DBD);
	fprintf(dbi, "%c\t%s\n\n", CMD_DBD, DBD_PARM);
	for (f = Db_fld, i = Db_hdr.db_fields; i; --i, f++)
	{
		fprintf(dbi, "%c\t%3d\t%-12s", CMD_FIELD,
				f - Db_fld, f->db_f_name);
		if (f->db_f_len)
		{
			fprintf(dbi, "\t%c%-3d",
				f->db_f_type, ctou(f->db_f_len));
			if (f->db_f_dec)
				fprintf(dbi, " %3d", f->db_f_dec);
		}
		putc('\n', dbi);
	}
	fprintf(dbi, "\n%c %s %s part\n", CMD_COMMENT, USERDBS, VERSION);
	fprintf(dbi, "%c\t%s\n", CMD_DB, DB_PARM);
	for (rel = &REL_PARM; *rel; rel++)
	{
		fprintf(dbi, "\n%c relation %s part\n", CMD_COMMENT, *rel);
		fprintf(dbi, "%c\t%s\n\n", CMD_REL, *rel);
		for (f = Db_fld, i = Db_hdr.db_fields; i; --i, f++)
		{
			if (f->db_f_name[0] == FLD_RECNO)
			{
				if (D < 0)
					continue;
				fprintf(dbi, "%c\t  1\trec_no\t\t%c%d\n",
					CMD_DOMAIN, typeunconv(INT), sizeof (long));
				continue;
			}
			if (f->db_f_name[0] == FLD_DELMARK)
			{
				if (D < 0)
					continue;
				fprintf(dbi, "%c\t  2\trec_mark\t%c%d\n",
					CMD_DOMAIN, typeunconv(CHAR), sizeof (char));
				continue;
			}
			fprintf(dbi, "%c\t%3d\t%-12s\t", CMD_DOMAIN,
				f - Db_fld + D, lower(f->db_f_name));
			switch (f->db_f_type)
			{
			  case TYPE_CHAR:
				fprintf(dbi, "c%-3d", ctou(f->db_f_len));
				break;

			  case TYPE_DATE:
				fprintf(dbi, "%c8", typeunconv(CHAR));
				break;

			  case TYPE_LOG:
				fprintf(dbi, "%c%d", typeunconv(CHAR), sizeof (char));
				break;

			  case TYPE_NUM:
				if (f->db_f_dec || ctou(f->db_f_len) > 10)
				{
					fprintf(dbi, "%c%d",
						typeunconv(FLOAT), sizeof (double));
					break;
				}
				if (ctou(f->db_f_len) < 4)
					fprintf(dbi, "%c%d",
						typeunconv(INT), sizeof (char));
				else if (ctou(f->db_f_len) < 6)
					fprintf(dbi, "%c%d",
						typeunconv(INT), sizeof (short));
				else
					fprintf(dbi, "%c%d",
						typeunconv(INT), sizeof (long));
				break;

# if (REDABAS == 3)
			  case TYPE_MEMO:
				fprintf(dbi, "c10");
				break;
# endif

			  default:
				printf("REDABAS\tunknown type %c on field %d/%s\n",
					f->db_f_type, f -  Db_fld, f->db_f_name);
				fclose(dbi);
				return (1);
			}
			putc('\n', dbi);
		}
		putc('\n', dbi);
		for (i = 1 - D; i < Db_hdr.db_fields; i++)
			fprintf(dbi, "%c\t%3d\t%3d\n", CMD_REF, i, i + D);
	}
	fprintf(dbi, "\n%c command  part after  conversion\n", CMD_COMMENT);
	fclose(dbi);
	return (0);
}

static char	*lower(db)
register char	*db;
{
	register char		*n;
	register int		chr;
	static char		name[DBNAME + 1];

	for (n = name; chr = *db++; )
	{
		if ((chr >= 'A'
# ifdef EBCDIC
		   && chr <= 'I') || (chr >= 'J'
		   && chr <= 'R') || (chr >= 'S'
# endif
		   && chr <= 'Z'))
			chr += 'a' - 'A';
		if (chr == ':')
			chr = '_';
		*n++ = chr;
	}
	*n = 0;
	return (name);
}

static int	prrec()
{
	register int		len;
	register char		*r;
	register struct db_fld	*f;
	register int		i;

	r = Record;
	if (*r == DB_RECVAL)
	{
		if (!(List_rec & 2))
			return;
	}
	else if (!(List_rec & 1))
		return;
	printf("|%5ld", Recno);
	for (f = &Db_fld[1], i = Db_hdr.db_fields - 1; i; --i, f++)
	{
		putchar('|');
		for (len = ctou(f->db_f_len); len; --len, r++)
			putchar(*r? *r: '*');
	}
	putchar('\n');
	fflush(stdout);
}

rubproc()
{
	fflush(stdout);
	exit(1);
}

AApageflush() {}
