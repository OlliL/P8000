# include	"amx.h"

tmp(db, uflag)
register char	*db;
char		 *uflag;
{
	register char		*p;
	register int		argc;
	register char		*id;
	char			*argv[4];
	char			arg[MAXNAME + 3];
	char			buf[MAXLINE + 1];
	static char		db_buf[MAXNAME + 1];

	/* setup argvector for initucode */
	AAbmove(db, db_buf, MAXNAME + 1);
	*argv = db = db_buf;
	argc = 1;
	if (uflag)
	{
		argv[argc++] = p = arg;
		*p++ = '-';
		AAbmove(uflag, p, MAXNAME + 2);
	}
	argv[argc++] = db;
	argv[argc] = (char *) 0;
	switch (initucode(argc, argv, TRUE, (char *) 0, M_SHARE))
	{
	  case 0:	/* everything is ok */
	  case 5:	/* ok, but there was an indirect token */
		break;

	  case 1:	/* the database does not exist */
	  case 6:	/* dto., but there was an indirect token */
		yyexit(9, db);

	  case 2:	/* you are not authorisized to access this database */
		yyexit(7, db);

	  case 3:	/* you are not a valid DBS user */
		yyexit(5, USERDBS);

	  case 4:	/* no database name specified */
		/* cannot happen */
		break;
	}

	/* open data base */
# ifdef MSDOS
	AAdatabase = Parmvect[0];
# endif
	AAam_start();

	if (AAdmin.adhdr.adflags & A_DBCONCUR)
		setbit(A_DBCONCUR, Xlocks);
	else
		No_locks++;

	if (AArelopen(&Indes, 0, AA_INDEX))
		yyexit(8, AA_INDEX);

	/* make believe catalog AA_REL is read only for concurrency	*/
	/* that means, readed pages are not locked			*/
	AAdmin.adreld.relopn = RELROPEN(AAdmin.adreld.relfp);

	/* setup file name for scan temps */
	*Filespec = 's';

	/* write include files */
	fprintf(FILE_amx, msg(49), AApath);

# ifdef AMX_SC
	if (ExpImp < 0)
		goto lookup;
# endif
	/* write variables */
	for (argc = 126; argc < 130; argc++)
		fprintf(FILE_amx, msg(argc));
	fprintf(FILE_amx, msg(30), AAusercode);
	fprintf(FILE_amx, msg(31), AApath);
# ifdef MSDOS
	fprintf(FILE_amx, msg(32));
	for (argc = DBP_STD; argc < DBP_MAX; argc++)
		fprintf(FILE_amx, msg(206), AAdbpath[argc]);
	fprintf(FILE_amx, msg(201));
	fprintf(FILE_amx, msg(204), AAdatabase);

	if (Xbufs)
		fprintf(FILE_amx, msg(35), (Xbufs * sizeof (ACCBUF)) / sizeof (int));
# else
	fprintf(FILE_amx, msg(32), AAdbpath);

	/* get user and group id of (alter ego) user */
	getuser(AAusercode, p = buf);
	getuser((char *) 0);
	for (argc = -2; argc < 2; argc++)
	{
		id = p;
		while (*p != ':')
			p++;
		*p++ = 0;
		if (argc >= 0)
			fprintf(FILE_amx, msg(28 + argc), id);
	}
# endif


# ifdef AMX_SC
lookup:
# endif
	/* lookup catalogs */
	rel_lookup(AA_REL, AMX_INTERNAL);		/* = AA_0 */
	rel_lookup(AA_ATT, AMX_INTERNAL);		/* = AA_1 */
	rel_lookup(AA_INDEX, AMX_INTERNAL);		/* = AA_2 */
}


# ifndef MSDOS
char	*AArelpath(rel)
register char	*rel;
{
	extern char		*AAztack();

	return (AAztack(AAztack(AAdbpath, "/"), rel));
}
# endif


# ifdef AMX_SC
import(file)
register char	*file;
{
	register FILE		*inp;

	ExpImp = -1;
	if (!(inp = fopen(file, "r")))
		yyexit(188, file);
# ifdef SETBUF
	setbuf(inp, (char *) 0);
# endif
	FILE_exp = FILE_inp;
	FILE_inp = inp;
	Line_exp = Line;
	Line = 1;
	backup('\n');
}
# endif
