/*
**	J. Wenzlaff, ZKI der AdW
**
**	REDABAS -- creation of DB relations from REDABAS data base
*/

# include	"db.h"
# ifdef MSDOS
# include	"../h/aux#.h"
# else
# include	"../h/aux.h"
# endif

# ifdef P8000
ret_buf		AAjmp_buf;
# else
jmp_buf		AAjmp_buf;
# endif

struct admin	AAdmin;
static char	Tuple[MAXTUP];		/* tuple buffer			*/
# ifdef MSDOS
char		*Database;
char		*Fileset;
extern char	*mkfileset();
# endif
extern char	*N_db;			/* overwrinting for DB		*/
extern char	*N_dbd;			/* overwrinting for RDABAS.DBD	*/
# define	VERBOSE		50
extern char	*Parmvect[];
extern char	*Flagvect[];
# ifdef MSDOS
extern char	*AAdbpath[];
# else
extern char	*AAdbpath;
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
			goto badflag;
		switch (p[1])
		{
		  case CMD_DBD:
			N_dbd = &p[2];
			break;

		  case 'i':
		  case 'I':
			N_db = &p[2];
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

	if (!DBI_PARM)
	{
usage:
		printf("usage:\t%s [flags] dbi\n", *argv);
		printf("flags:\t-%cdbd  change DBD-file-name\n", CMD_DBD);
		printf("\t-idb   change DB-name\n");
		printf("\t-rd    transfer deleted records\n");
		printf("\t-rv    transfer  valid  records\n");
		goto term;
	}

	RW_flag = CMD_READ;
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
	if (!(verb = (Db_hdr.db_records + VERBOSE - 1) / VERBOSE))
		verb = 1;
	printf("|--------------------------------------------------|\n");
	printf("|               %5d records per \".\"              |\n", verb);
	printf("|--------------------------------------------------|\n");
	fflush(stdout);
	while (rdrec())
	{
		if (!(Recno % verb))
		{
			putchar('.');
			fflush(stdout);
		}
		if (List_rec)
		{
			if (*Record == DB_RECVAL)
			{
				if (List_rec == 1)
					continue;
			}
			else if (List_rec == 2)
				continue;
		}
		for (i = 0; i < rels; i++)
			if (put_tup(&Desc_s[i], &Refs[i * MAXDOM]))
				return (1);
	}
	printf("\n|__________________________________________________|\n");
	fflush(stdout);
	return (0);
}

/*
** PUT_TUP	convert REDABAS record into DATABASE tuple
**		return (0) = ok
**		return (1) = error
*/
put_tup(d, r)
register DESC	*d;
register short	*r;
{
	register struct db_fld	*f;
	register int		i;
	long			tid;

	clr_rec(d);
	for (i = 1; i <= d->reldum.relatts; i++)
	{
		if (*(++r) < 0)
			continue;	/* domain not filled */
		f = &Db_fld[*r];
		if (conv(f->db_f_adr,		   /* ptr to input chars */
			f->db_f_type,		   /* source format	 */
			ctou(f->db_f_len),	   /* source length	 */
			d->relfrmt[i],		   /* destination format */
			ctou(d->relfrml[i]),	   /* destination length */
			d->reloff[i]))		   /* destination offset */
		{
			printf("REDABAS\ton FIELD %d / DOMAIN %d\n", *r, i);
			return (1);
		}
	}
	AAam_insert(d, &tid, Tuple, 1);
	return (0);
}

/*
** CLR_REC	create a fresh REDABAS record
*/
clr_rec(d)
register DESC		*d;
{
	register int		i;
	register int		len;
	register char		*off;
	register int		c;

	for (i = d->reldum.relatts; i; --i)
	{
		len = ctou(d->relfrml[i]);
		off = Tuple + d->reloff[i];
		c = d->relfrmt[i] == CHAR? ' ': 0;
		do *off++ = c; while (--len);
	}
}

/*
** CONV		copies data from Record into Tuple
**		doing conversions whenever necessary
**		return (0) = ok
**		return (1) = error
*/
conv(in, sf, sl, df, dl, doff)
register char	*in;	/* pointer to input chars	*/
register int	sf;	/* source format		*/
register int	sl;	/* source length		*/
register int	df;	/* destination format		*/
register int	dl;	/* destination length		*/
register int	doff;	/* destination offset		*/
{
	char		any[sizeof (double)];
	char		c0_temp[MAXFIELD];
	extern long	atol();

	switch (sf)
	{
# if (REDABAS == 3)
	  case TYPE_MEMO:
		goto conv_err;
# endif

	  case TYPE_CHAR:
		if (df != CHAR)
			goto conv_err;
		if (dl > sl)
			dl = sl;
		break;

	  case TYPE_LOG:
		if (sl != 1)
		{
type_err:
			printf("\n\tunknown type %c%d\n", sf, sl);
			return (1);
		}
		if (df != CHAR || dl != 1)
		{
conv_err:
			printf("\n\tcan't convert type %c%d to type %c%d\n",
				sf, sl, typeunconv(df), dl);
			return (1);
		}
		break;

	  case TYPE_DATE:
		if (sl != 8)
			goto type_err;
		if (df != CHAR || dl != 8)
			goto conv_err;
		break;

	  case TYPE_INT:
		if (df != INT || dl != sizeof (long))
			goto conv_err;
		break;

	  case TYPE_NUM:
		AAbmove(in, c0_temp, sl);
		c0_temp[sl] = 0;
		switch (df)
		{
		  case FLOAT:
			AA_atof(c0_temp, any);
			switch (dl)
			{
# ifndef NO_F4
			  case sizeof (float):
				((F4TYPE *) any)->f4type = ((F8TYPE *) any)->f8type;
				break;
# endif

			  case sizeof (double):
				break;

			  default:
				goto conv_err;
			}
			break;

		  case INT:
			((I4TYPE *) any)->i4type = atol(c0_temp);
			switch (dl)
			{
			  case sizeof (char):
				((I1TYPE *) any)->i1type = ((I4TYPE *) any)->i4type;
				break;

			  case sizeof (short):
				((I2TYPE *) any)->i2type = ((I4TYPE *) any)->i4type;
				break;

			  case sizeof (long):
				break;

			  default:
				goto conv_err;
			}
			break;

		  default:
			goto conv_err;
		}
		in = any;
		break;

	  default:
		goto conv_err;
	}
	AAbmove(in, &Tuple[doff], dl);
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
	if (rdhdr(argv[PTR_NAME]) || rdopen(argv[PTR_NAME]))
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
	register int		l;

	if (argc < 2)
	{
		printf("\tmissing FIELD-n%s\n", argc? "ame": "umber");
		return (1);
	}
	if ((i = atoi(argv[PTR_NAME])) >= Db_hdr.db_fields || i < 0)
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
	if (!i)
		return (0);
	if (argc < 3)
	{
		printf("\tmissing type & length on FIELD %d\n", i);
		return (1);
	}
	if ((f = &Db_fld[i])->db_f_type != *(argv[PTR_TYPE]))
	{
		printf("\ttype error on FIELD %d\n", i);
		printf("\texpected %c\n", f->db_f_type);
		printf("\t   found %c\n", *(argv[PTR_TYPE]));
		return (1);
	}
	if (ctou(f->db_f_len) != (l = atoi(argv[PTR_TYPE] + 1)))
	{
		printf("\tlength error on FIELD %d\n", i);
		printf("\texpected %d\n", ctou(f->db_f_len));
		printf("\t   found %d\n", l);
		return (1);
	}
	l = (argc > 3)? atoi(argv[PTR_DEC]): 0;
	if (l != f->db_f_dec)
	{
		printf("\tpoint error on FIELD %d\n", i);
		printf("\texpected %d\n", f->db_f_dec);
		printf("\t   found %d\n", l);
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
	register int		i;

	for (i = 0; i < DB_FLDMAX + 2; i++)
		if (!Flds[i])
			break;
	if (i != Db_hdr.db_fields)
	{
		printf("\tmissing FIELD with number %d\n", i);
		return (1);
	}
	return (0);
}

/*
**	SEVERAL replacement routines
*/
rubproc()	{ fflush(stdout); exit(1); }
