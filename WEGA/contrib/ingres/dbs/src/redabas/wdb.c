/*
**	J. Wenzlaff, ZKI der AdW
**
**	REDABAS -- creation of REDABAS data base from DB relations
*/

# include	"db.h"
# ifdef MSDOS
# include	"../h/aux#.h"
# include	<fcntl.h>
# else
# include	"../h/aux.h"
# endif

# ifdef P8000
ret_buf		AAjmp_buf;
# else
jmp_buf		AAjmp_buf;
# endif

# define	VERBOSE		50

struct join
{
	struct join	*j_next;
	char		*j_val;
	char		j_dom;
	char		j_frmt;
	char		j_frml;
};

struct admin		AAdmin;
static char		Tuple[TABS * MAXTUP];	/* tuple buffers	*/
extern char		*N_db;		/* overwrinting for DB		*/
extern char		*N_dbd;		/* overwrinting for RDABAS.DBD	*/
static struct join	*Joins[TABS];		/* join pointer		*/
static int		Rels;
extern char		*Parmvect[];
extern char		*Flagvect[];
# ifdef MSDOS
char			*Database;
char			*Fileset;
extern char		*mkfileset();
extern char		*AAdbpath[];
# else
extern char		*AAdbpath;
# endif

# define	DBI_PARM	Parmvect[0]

main(argc, argv)
int	argc;
char	*argv[];
{
	register int		i;
	register char		**av;
	register char		*p;
	extern char		*AAproc_name;
# ifdef SETBUF
	char			buf[BUFSIZ];

	setbuf(stdout, buf);
# endif
#	ifdef xSTR1
	AAtTrace(&argc, argv, 'X');
#	endif

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
		{
# if (REDABAS == 2)
			goto badflag;
# endif
# if (REDABAS == 3)
			III_plus++;
			continue;
# endif
		}
		switch (p[1])
		{
		  case CMD_DBD:
			N_dbd = &p[2];
			break;

		  case 'i':
		  case 'I':
			N_db = &p[2];
			break;

		  default:
# if (REDABAS == 2)
badflag:
# endif
			printf("REDABAS\tUnbekanntes Flag '%s'\n", p);
			i++;
		}
	}
	if (i)
		goto usage;

	if (!DBI_PARM)
	{
usage:
		printf("usage:\t%s [flags] dbi\n", *argv);
		printf("flags:\t-%cdbd  change DBD-file-name\n", CMD_DBD);
		printf("\t-idb   change DB-name\n");
# if (REDABAS == 3)
		printf("\t+      REDABAS 4-flag\n");
# endif
		goto term;
	}

	RW_flag = CMD_WRITE;
	dbi(DBI_PARM);

term:
	fflush(stdout);
	return (0);
}

/*
** TRANSFER	read REDABAS records into DATABASE
**		return (0) = ok
**		return (1) = error
*/
transfer(rels)
register int	rels;
{
	register int		i;
	register int		verb;

	if (!rels)
		return (0);
	for (i = 1; i < rels; i++)
	{
		if (pre_join(i))
			return (1);
		if (!Joins[i])
		{
			printf("REDABAS\tmissing join DOMAINS on RELATION %.12s\n",
				Desc_s[i].reldum.relid);
			return (1);
		}
	}
	if (!(verb = (Desc_s[0].reldum.reltups + VERBOSE - 1) / VERBOSE))
		verb = 1;
	printf("|--------------------------------------------------|\n");
	printf("|               %5d records per \".\"              |\n", verb);
	printf("|--------------------------------------------------|\n");
	fflush(stdout);
	Rels = rels;
	clr_rec(Db_hdr.db_reclen);
	if (get_alltups(0, verb) < 0)
		return (1);
	clr_rec(512 + 1);
	*Record = DB_RECLAST;
	wrrec();
	printf("\n|__________________________________________________|\n");
	fflush(stdout);
	return (0);
}

/*
** PRE_JOIN	collect join information
**		return (0) = ok
**		return (1) = error
*/
pre_join(rel)
register int	rel;
{
	register short		*j;
	register short		*r;
	register DESC		*d;
	register struct join	*mem;
	register int		jd;
	register int		jr;
	extern struct join	*malloc();

	for (r = &Refs[rel * MAXDOM]; r < &Refs[(rel + 1) * MAXDOM]; r++)
	{
		if (*r < 0)
			continue;
		for (j = Refs; j < &Refs[rel * MAXDOM]; j++)
		{
			if (*j < 0 || *j != *r)
				continue;
			*r = -1;
			if (!(mem = malloc(sizeof (struct join))))
			{
				printf("REDABAS\t no space\n");
				return (1);
			}
			mem->j_next = Joins[rel]? Joins[rel]: (struct join *) 0;
			Joins[rel] = mem;
			jd = (j - Refs) % MAXDOM;
			jr = (j - Refs) / MAXDOM;
			d = &Desc_s[jr];
			mem->j_val = &Tuple[jr * MAXTUP] + d->reloff[jd];
			mem->j_frmt = d->relfrmt[jd];
			mem->j_frml = d->relfrml[jd];
			jd = (r - Refs) % MAXDOM;
			d = &Desc_s[rel];
			mem->j_dom = jd;
			if (mem->j_frmt != d->relfrmt[jd] || mem->j_frml != d->relfrml[jd])
			{
				printf("REDABAS\ttype error on join DOMAIN %d on RELATION %.12s\n",
					jd, Desc_s[rel].reldum.relid);
				printf("\texpected %c%d\n",
					typeunconv(mem->j_frmt),
					ctou(mem->j_frml));
				printf("\t   found %c%d\n",
					typeunconv(d->relfrmt[jd]),
					ctou(d->relfrml[jd]));
				return (1);
			}
		}
	}
	return (0);
}

/*
** GET_ALLTUP	convert all DATABASE tuples into REDABAS record
**		and write it into REDABAS.DBD
**		return ( 0) = no tuple found
**		return ( 1) = ok
**		return (-1) = error
*/
get_alltups(rel, verb)
int	rel;
int	verb;
{
	register DESC		*d;
	register char		*t;
	register struct join	*j;
	register short		*r;
	register int		i;
	long			lotid;
	long			hitid;
	char			key[MAXTUP];

	d = &Desc_s[rel];
	j = Joins[rel];
	r = &Refs[rel * MAXDOM];
	t = &Tuple[rel * MAXTUP];
	rel++;
	while (j)
	{
		AAam_setkey(d, key, j->j_val, j->j_dom);
		j = j->j_next;
	}
	if (AAam_find(d, EXACTKEY, &lotid, &hitid, key) < 0)
		return (-1);
	while ((i = get_onetup(d, r, key, t, &lotid, &hitid)) > 0)
	{
		if (rel < Rels)
		{
			if (get_alltups(rel, verb) < 0)
				return (-1);
			continue;
		}
		if (!(Recno % verb))
		{
			putchar('.');
			fflush(stdout);
		}
		if (wrrec())
			return (-1);
		clr_rec(Db_hdr.db_reclen);
	}
	return (i);
}

/*
** GET_ONETUP	convert one DATABASE tuple into REDABAS record
**		return ( 0) = no tuple found
**		return ( 1) = ok
**		return (-1) = error
*/
get_onetup(d, r, key, tup, ltid, htid)
register DESC	*d;
register short	*r;
char		*key;
char		*tup;
long		*ltid;
long		*htid;
{
	register struct db_fld	*f;
	register int		i;

	while (!(i = AAam_get(d, ltid, htid, tup, 1)))
		if (!AAkcompare(d, key, tup))
			break;
	if (i > 0)
		return (0);
	else if (i < 0)
		return (-1);
	for (i = 1; i <= d->reldum.relatts; i++)
	{
		if (*(++r) < 0)
			continue;	/* domain not filled */
		f = &Db_fld[*r];
		if (conv(f->db_f_adr,		   /* destination adr	    */
			f->db_f_type,		   /* destination format    */
			ctou(f->db_f_len),	   /* destination length    */
			d->relfrmt[i],		   /* source format	    */
			ctou(d->relfrml[i]),	   /* source length	    */
			tup + d->reloff[i],	   /* source adr	    */
			f->db_f_dec))		   /* destination precision */
		{
			printf("REDABAS\ton FIELD %d / DOMAIN %d\n", *r, i);
			return (-1);
		}
	}
	return (1);
}

/*
** CLR_TUP	create a fresh REDABAS record
*/
clr_rec(reclen)
register int	reclen;
{
	register char		*off;
	register char		*rec;

	off = &(rec = Record)[reclen];
	while (off > rec)
		*(--off) = ' ';
	*off = DB_RECVAL;
}

/*
** CONVERT	copies data from Tuple into Record
**		doing conversions whenever necessary
**		return (0) = ok
**		return (1) = error
*/
conv(out, df, dl, sf, sl, in, prec)
register char	*out;	/* destination adr	 */
register int	df;	/* destination format	 */
register int	dl;	/* destination length	 */
register int	sf;	/* source format	 */
register int	sl;	/* source length	 */
register char	*in;	/* source adr		 */
register int	prec;	/* destination precision */
{
	long		recs;
	double		f8_temp;
	char		c0_temp[MAXFIELD];
	extern char	*AA_iocv();
	extern char	*AA_locv();
	extern char	*AApmove();

	switch (df)
	{
# if (REDABAS == 3)
	  case TYPE_MEMO:
		goto conv_err;
# endif

	  case TYPE_CHAR:
		if (sf != CHAR)
			goto conv_err;
		if (sl > dl)
			sl = dl;
		break;

	  case TYPE_LOG:
		if (dl != 1)
		{
type_err:
			printf("\tunknown type %c%d\n", df, dl);
			return (1);
		}
		if (sf != CHAR || sl != 1)
		{
conv_err:
			printf("\tcan't convert type %c%d to type %c%d\n",
				typeunconv(sf), sl, df, dl);
			return (1);
		}
		break;

	  case TYPE_DATE:
		if (dl != 8)
			goto type_err;
		if (sf != CHAR || sl != 8)
			goto conv_err;
		break;

	  case TYPE_INT:
		if (sf != INT || sl != sizeof (long))
			goto conv_err;
		if ((recs = ((I4TYPE *) in)->i4type - Recno - 1) < 0)
		{
			printf("\ttry to write record %ld\n", ((I4TYPE *) in)->i4type);
			printf("\ton record %ld\n", Recno + 1);
			printf("\tRECORD sequence error\n");
			return (1);
		}
		*Record = DB_RECDEL;
		while (recs--)
			if (wrrec())
				return (1);
		*Record = DB_RECVAL;
		return (0);

	  case TYPE_NUM:
		AAbmove(in, &f8_temp, sl);
		in = (char *) &f8_temp;
		switch (sf)
		{
		  case FLOAT:
			switch (sl)
			{
# ifndef NO_F4
			  case sizeof (float):
				f8_temp = ((F4TYPE *) in)->f4type;
				/* fall through */
# endif

			  case sizeof (double):
				AA_ftoa(f8_temp, c0_temp, dl, prec, 'f');
				in = c0_temp;
				break;

			  default:
				goto conv_err;
			}
			break;

		  case INT:
			switch (sl)
			{
			  case sizeof (char):
				((I2TYPE *) in)->i2type = ctoi(*in);
				/* fall through */

			  case sizeof (short):
				in = AA_iocv(((I2TYPE *) in)->i2type);
				break;

			  case sizeof (long):
				in = AA_locv(((I4TYPE *) in)->i4type);
				break;

			  default:
				goto conv_err;
			}
			break;

		  default:
			goto conv_err;
		}
		sl = AAlength(in);
		if ((sf = sl - dl) > 0)
		{
			sl = dl;
			*in = 0;
			AApmove(in, in, sl, '*');
		}
		else if (sf < 0)
			out -= sf;
		break;

	  default:
		goto conv_err;
	}
	AAbmove(in, out, sl);
	return (0);
}
/*
** BGN_FLD	start of field check
**		return (0) = ok
**		return (1) = error
*/
bgn_fld(argc, argv)
int		argc;
register char	**argv;
{
	register int		i;

	if (!argc)
	{
		printf("\tmissing DBD-file-name\n");
		return (1);
	}
	if (wropen(argv[PTR_NAME]))
		return (1);
	/* clear field check area */
	for (i = 0; i < DB_FLDMAX + 2; i++)
		Flds[i] = 0;
	return (0);
}

/*
** CHK_FLD	field check
**		return (0) = ok
**		return (1) = error
*/
chk_fld(argc, argv)
int		argc;
register char	**argv;
{
	register int		i;
	register struct db_fld	*f;
	register int		found;
	register int		want;

	if (argc < 2)
	{
		printf("\tmissing FIELD-n%s\n", argc? "ame": "umber");
		return (1);
	}
	if ((i = atoi(argv[PTR_NAME])) >= DB_FLDMAX + 2 || i < 0)
	{
		printf("\tillegal FIELD-number %d\n", i);
		return (1);
	}
	if (Flds[i])
	{
		printf("\tduplicate FIELD-number %d\n", i);
		return (1);
	}
	Flds[i] = 1;
	if (i < 2)
		return (0);
	if (argc < 3)
	{
		printf("\tmissing FIELD-type & length\n");
		return (1);
	}
	(f = &Db_fld[i])->db_f_dec = 0;
	f->db_f_len = found = atoi(argv[PTR_TYPE] + 1);
	AAsmove(argv[PTR_NO], f->db_f_name);
	want = 254;
	if (found <= 0)
	{
len_err:
		printf("\tlength error on FIELD %d\n", i);
		printf("\texpected %d\n", want);
		printf("\t   found %d\n", found);
		return (1);
	}
	switch (f->db_f_type = *(argv[PTR_TYPE]))
	{
	  case TYPE_CHAR:
		if (found > want)
			goto len_err;
		break;

	  case TYPE_DATE:
		if (found != (want = 8))
			goto len_err;
		break;

	  case TYPE_LOG:
		if (found != (want = 1))
			goto len_err;
		break;

	  case TYPE_NUM:
		if (found > (want = 20))
			goto len_err;
		want = (argc > 3)? atoi(argv[PTR_DEC]): 0;
		if (want < 0 || want > found)
		{
			printf("\tpoint error on FIELD %d\n", i);
			printf("\texpected < %d\n", found);
			printf("\t   found   %d\n", want);
			return (1);
		}
		f->db_f_dec = want;
		break;

# if (REDABAS == 3)
	  case TYPE_MEMO:
		if (found != (want = 10))
			goto len_err;
		break;
# endif

	  default:
		printf("\tunknown type %c on FIELD %d\n", f->db_f_type, i);
		return (1);
	}
	return (0);
}

/*
** END_FLD	end of field check
**		return (0) = ok
**		return (1) = error
*/
end_fld()
{
	if (!Flds[2])
	{
		printf("\tmissing FIELDS\n");
		return (1);
	}
	return (wrfld());
}


/*
**	CREATE REDABAS.DBD file into Redabas
**	WRITE REDABAS header from Db_hdr
**	WRITE REDABAS fields from Db_fld
**	WRITE REDABAS records from Record
*/

struct db_hdr	Db_hdr;			/* REDABAS header		*/
struct db_fld	Db_fld[DB_FLDMAX + 2];	/* REDABAS fields		*/
char		Record[DB_RECMAX + 200];/* REDABAS record		*/
# if (REDABAS == 3)
char		III_plus;		/* REDABAS 3+ flag		*/
# endif
long		Recno = 0;		/* REDABAS record count		*/
static int	Redabas = -1;		/* file REDABAS.DBD		*/
char		List_rec;		/* flag to list tuples		*/

/*
** WROPEN	create REDABAS.DBD
**		return (0) = ok
**		return (1) = error
*/
static int	wropen(dbd)
register char	*dbd;
{
	if ((Redabas = creat(dbd, 0666)) < 0)
	{
		printf("\tcan't access DBD-file %s\n", dbd);
		return (1);
	}
# ifdef MSDOS
	setmode(Redabas, O_BINARY);
# endif
	return (0);
}

/*
** WRFLD	write field informations
**		return (0) = ok
**		return (1) = error
*/
static int	wrfld()
{
	register struct db_fld	*f;
	register char		*p;
	register int		i;
	register int		flds;
	register int		len;
	long			t;
	int			*lt;
	char			fld_buf[DB_FLDLEN];
	extern long		lseek();
	extern long		time();
	extern int		*localtime();

	time(&t);
# ifdef ESER_VMX
	lt = localtime(t);
# else
	lt = localtime(&t);
# endif
	/*----------------------------------------------*/
	/*	SECOND	==  0				*/
	/*	MINUTE	==  1				*/
	/*	HOUR	==  2				*/
	/*	M_DAY	==  3	month day		*/
	/*	MONTH	==  4				*/
	/*	YEAR	==  5				*/
	/*	W_DAY	==  6	week day 		*/
	/*	Y_DAY	==  7	year day 		*/
	/*	IS_DST	==  8				*/
	/*----------------------------------------------*/
	Db_hdr.db_day = lt[3];
	Db_hdr.db_month = lt[4] + 1;
	Db_hdr.db_year = lt[5];
	/* first dummy field: record number */
	(f = Db_fld)->db_f_adr = (char *) &Recno;
	f->db_f_name[0] = FLD_RECNO;
	f->db_f_type = TYPE_INT;
	f->db_f_len = sizeof (long);
	/* second dummy field: erase mark */
	(++f)->db_f_adr = p = Record;
	p++;
	f->db_f_name[0] = FLD_DELMARK;
	f->db_f_type = TYPE_CHAR;
	f->db_f_len = 1;
	for (f++; i = ctou(f->db_f_len); f++)
	{
		f->db_f_adr = p;
		p += i;
	}
	if ((i = p - Record) > DB_RECMAX)
	{
		printf("REDABAS\trecord too long (%d bytes)\n", i);
		return (1);
	}
	Db_hdr.db_reclen = i;
	Db_hdr.db_fields  = f - Db_fld;
# if (REDABAS == 2)
	Db_hdr.db_hdrlen = DB_HEAD;
# endif
# if (REDABAS == 3)
	Db_hdr.db_hdrlen = DB_HDR + (Db_hdr.db_fields - 2) * DB_FLDLEN + 1;
	if (!III_plus)
		Db_hdr.db_hdrlen++;
# endif
	t =  DB_HDR;
	if (lseek(Redabas, t, 0) != t)
		goto wr_err;
	len = DB_FLDLEN;
# if (REDABAS == 2)
	flds = DB_FLDMAX + 2;
# endif
# if (REDABAS == 3)
	flds = Db_hdr.db_fields;
# endif
	for (f = &Db_fld[2], i = 2; i <= flds; i++, f++)
	{
		/* clear data buffer */
		p = &fld_buf[DB_FLDLEN];
		while (p > fld_buf)
			*(--p) = 0;

		if (i >= Db_hdr.db_fields)
		{
# if (REDABAS == 2)
			if (i == Db_hdr.db_fields)
# endif
				p[DB_F_NAME] = DB_FLDLAST;
			if (i == flds)
			{
				len = 1;
# if (REDABAS == 3)
				if (!III_plus)
					len++;
# endif
			}
			goto wr_fld;
		}
		AAbmove(f->db_f_name, &p[DB_F_NAME], DBNAME + 1);
		p[DB_F_TYPE] = f->db_f_type;
		p[DB_F_LEN] = f->db_f_len;
		p[DB_F_DEC] = f->db_f_dec;
wr_fld:
		if (write(Redabas, p, len) != len)
		{
wr_err:
			printf("REDABAS\tfield write error on DBD-file\n");
			return (1);
		}
	}
	t =  Db_hdr.db_hdrlen;
	if (lseek(Redabas, t, 0) != t)
		goto wr_err;
	return (0);
}

/*
** WRREC	write next record to REDABAS.DBD
**		return (0) = ok
**		return (1) = error
*/
static int	wrrec()
{
	register int		len;
	register int		eof;
# if (REDABAS == 2)
	long			bytes;
# endif

	eof = 0;
	if (*Record == DB_RECLAST)
	{
# if (REDABAS == 2)
		bytes = Db_hdr.db_hdrlen + Recno * Db_hdr.db_reclen;
		if (!(len = 512 - (bytes % 512)))
			len = 512;
# endif
# if (REDABAS == 3)
		len = 1;
# endif
		eof++;
	}
	else
	{
		Recno++;
		len = Db_hdr.db_reclen;
	}
	if (write(Redabas, Record, len) != len)
	{
		printf("REDABAS\trecord write error on DBD-file\n");
		printf("\ton record %ld\n", Recno);
		goto last_rec;
	}
	if (eof)
	{
		wrhdr();
last_rec:
		close(Redabas);
		Redabas = -1;
		return (1);
	}
	return (0);
}

static int	wrshort(d, b)
register int	d;
register char	*b;
{
	*b++ = d;
	*b = d >> BYTE_LOG2;
}


# if (REDABAS == 2)
# define	wrword(d, b)	wrshort(d, b)
# endif

# if (REDABAS == 3)
static int	wrword(d, b)
register long	d;
register char	*b;
{
	register int		w;

	w = d & WORD_MASK;
	wrshort(w, b);
	w = (d >> WORD_LOG2) & WORD_MASK;
	wrshort(w, b + sizeof (short));
}
# endif

/*
** WRHDR	create REDABAS.DBD and write head informations
**		return (0) = ok
**		return (1) = error
*/
static int	wrhdr()
{
	register struct db_hdr	*h;
	register char		*p;
	word			recs;
	char			hdr_buf[DB_HDR];
	extern long		lseek();

	/* clear data buffer */
	p = &hdr_buf[DB_HDR];
	while (p > hdr_buf)
		*(--p) = 0;

	*p = REDABAS;
	recs = (h = &Db_hdr)->db_records = Recno;
	wrword(recs, &p[DB_RECORDS]);
	p[DB_DAY] = h->db_day;
	p[DB_MONTH] = h->db_month;
	p[DB_YEAR] = h->db_year;
	wrshort(h->db_reclen, &p[DB_RECLEN]);
# if (REDABAS == 3)
	wrshort(h->db_hdrlen, &p[DB_HDRLEN]);
# endif
	if (lseek(Redabas, 0L, 0) < 0 || write(Redabas, p, DB_HDR) != DB_HDR)
		printf("REDABAS\theader write error on DBD-file\n");
}


/*
**	SEVERAL replacement routines
*/
rubproc()	{ fflush(stdout); exit(1); }
