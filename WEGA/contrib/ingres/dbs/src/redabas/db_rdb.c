/*
**	J. Wenzlaff, ZKI der AdW
**
**	READ REDABAS header into Db_hdr
**	READ REDABAS fields into Db_fld
*/

# include	"db.h"
# ifdef MSDOS
# include	<fcntl.h>
# endif

struct db_hdr	Db_hdr;			/* REDABAS header		*/
struct db_fld	Db_fld[DB_FLDMAX + 2];	/* REDABAS fields		*/
char		Record[DB_RECMAX + 200];/* REDABAS record		*/
char		List_hdr;		/* flag to list header		*/
char		List_fld;		/* flag to list fields		*/
static int	R_fd;			/* file REDABAS.DBD		*/

static int	rdshort(buf)
register char	*buf;
{
	register int		w1;
	register int		w2;

	w1 = *buf++ & BYTE_MASK;
	w2 = *buf & BYTE_MASK;
	w1 += w2 << BYTE_LOG2;
	return (w1);
}

# if (REDABAS == 2)
# define	rdword(buf)	rdshort(buf)
# endif

# if (REDABAS == 3)
static long	rdword(buf)
register char	*buf;
{
	long			w1;
	long			w2;

	w1 = rdshort(buf);
	w1 &= WORD_MASK;
	w2 = rdshort(buf + sizeof (short));
	w2 &= WORD_MASK;
	w1 += w2 << WORD_LOG2;
	return (w1);
}
# endif

/*
** RDHDR	open REDABAS.DBD and read head informations
**		return (0) = ok
**		return (1) = error
*/
rdhdr(dbd)
register char	*dbd;
{
	register struct db_hdr	*h;
	register char		*p;
	register int		i;
	register struct db_fld	*f;
	long			recs;
	char			hdr_buf[DB_HDR];

# ifdef MSDOS
	if ((R_fd = open(dbd, 0 | O_BINARY)) < 0)
# else
	if ((R_fd = open(dbd, 0)) < 0)
# endif
	{
		printf("\tcan't access DBD-file %s\n", dbd);
		return (1);
	}
	if (read(R_fd, p = hdr_buf, DB_HDR) != DB_HDR)
		goto fmt_err;
	if ((*p & 0177) != REDABAS)
	{
		printf("REDABAS\tversion error on DBD-file %s\n", dbd);
		printf("\texpected REDABAS %d\n", REDABAS);
		printf("\t   found REDABAS %d\n", *p & 0177);
		return (1);
	}
	if (rdfld())
	{
fmt_err:
		printf("REDABAS\tformat error on DBD-file %s\n", dbd);
		return (1);
	}
	(h = &Db_hdr)->db_records = rdword(&p[DB_RECORDS]);
	h->db_day = p[DB_DAY];
	h->db_month = p[DB_MONTH];
	h->db_year = p[DB_YEAR];
	h->db_reclen = rdshort(&p[DB_RECLEN]);
# if (REDABAS == 2)
	h->db_hdrlen = DB_HEAD;
# endif
# if (REDABAS == 3)
	h->db_hdrlen = rdshort(&p[DB_HDRLEN]);
# endif

	if (List_hdr)
	{
		printf("\n---- REDABAS -- %d ----\n", REDABAS);
		printf("| database from file |\t%s\n", dbd);
		printf("| length of header   |\t%d\n", h->db_hdrlen);
		recs = h->db_records;
		printf("| number of records  |\t%ld\n", recs);
		printf("| length of record   |\t%d\n", h->db_reclen);
		if (h->db_day && h->db_month && h->db_year)
			printf("| last modification  |\t%d.%d.19%d\n",
				h->db_day, h->db_month, h->db_year);
		printf("| number of fields   |\t%d\n", h->db_fields);
		printf("----------------------\n");
		fflush(stdout);
	}
	if (List_fld)
	{
		printf("\n--- no --- off --- field --- type --- length --- dec ---");
		p = Record;
		for (i = 0, f = Db_fld; i < h->db_fields; i++, f++)
		{
			printf("\n|%5d%8d     %-10s", i,
				i? f->db_f_adr - p: 0, f->db_f_name);
			if (f->db_f_type)
			{
				printf("   %c%11d", f->db_f_type, ctou(f->db_f_len));
				if (f->db_f_dec)
					printf("%8d", f->db_f_dec);
			}
		}
		printf("\n--------------------------------------------------------\n");
		fflush(stdout);
	}
	close(R_fd);
	return (0);
}

/*
** RDFLD	read field informations
**		return (0) = ok
**		return (1) = error
*/
static int	rdfld()
{
	register struct db_fld	*f;
	register char		*p;
	register int		i;
	register char		*r;
	register int		l;
	char			fld_buf[DB_FLDLEN];

	/* first dummy field: record number */
	(f = Db_fld)->db_f_adr = (char *) &Recno;
	f->db_f_name[0] = FLD_RECNO;
	f->db_f_type = TYPE_INT;
	f->db_f_len = sizeof (long);

	/* second dummy field: erase mark */
	(++f)->db_f_adr = r = Record;
	r++;
	f->db_f_name[0] = FLD_DELMARK;
	f->db_f_type = TYPE_CHAR;
	f->db_f_len = 1;

	i = 0;
	p = fld_buf;
	while ((l = read(R_fd, p, DB_FLDLEN)) > 0)
	{
		if (*p == DB_FLDLAST)
			goto all_fields;
		if (l != DB_FLDLEN)
			break;
		(++f)->db_f_adr = r;
		r += ctou(p[DB_F_LEN]);
		AAbmove(&p[DB_F_NAME], f->db_f_name, DBNAME + 1);
		f->db_f_type = p[DB_F_TYPE];
		f->db_f_len = p[DB_F_LEN];
		f->db_f_dec = p[DB_F_DEC];
		if (++i < DB_FLDMAX)
			continue;
all_fields:
		Db_hdr.db_fields = i + 2;
		return (0);
	}
	return (1);
}


/*
**	OPEN REDABAS.DBD into R_file
**	READ REDABAS records into Record
*/

long		Recno = -1;		/* REDABAS record count		*/
char		List_rec;		/* flag to list tuples		*/
static FILE	*R_file;		/* file REDABAS.DBD		*/

/*
** RDOPEN	open REDABAS.DBD
**		return (0) = ok
**		return (1) = error
*/
rdopen(dbd)
register char	*dbd;
{
	register int		reclen;
	register int		i;
# ifdef SETBUF
	static char		redabas_buf[BUFSIZ];
# endif

# ifdef MSDOS
	if ((R_file = fopen(dbd, "rb")) == (FILE *) 0)
# else
	if ((R_file = fopen(dbd, "r")) == (FILE *) 0)
# endif
	{
		printf("REDABAS\tcan't access DBD-file %s\n", dbd);
		return (1);
	}
# ifdef SETBUF
	setbuf(R_file, redabas_buf);
# endif

	/* skip REDABAS head information */
	reclen = Db_hdr.db_reclen;
	Db_hdr.db_reclen = Db_hdr.db_hdrlen;
	i = rdrec() == 0;
	Db_hdr.db_reclen = reclen;
	return (i);
}

/*
** RDREC	read next record from REDABAS.DBD
**		return (0) = eof
**		return (1) = ok
*/
rdrec()
{
	register int		i;
	register char		*r;
	register FILE		*dbd;
	register int		len;
	long			recs;

	r = Record;
	dbd = R_file;
	for (len = 0; len < Db_hdr.db_reclen; len++)
	{
		if ((i = getc(dbd)) < 0)
		{
			printf("REDABAS\trecord length error\n");
			printf("\ton record %ld\n", Recno + 1);
			goto eof;
		}
		if (!len && i == DB_RECLAST)
		{
			if (Recno != (recs = Db_hdr.db_records))
			{
				printf("REDABAS\trecord count error\n");
				printf("\texpected %ld records\n", recs);
				printf("\t   found %ld records\n", Recno);
			}
eof:
			fclose(dbd);
			return (0);
		}
		*r++ = i;
	}
	Recno++;
	return (1);
}
