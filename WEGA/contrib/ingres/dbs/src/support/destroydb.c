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

extern char	*AAusercode;
extern int	AAdbstat;
extern char	*AApath;
extern char	*Parmvect[];
extern char	*Flagvect[];
extern char	*AAdbpath;
extern char	*AAproc_name;
struct admin	AAdmin;
char		RMDIR[]		= "rmdir";

/*
**  DESTROY DATA BASE
**
**	This program destroys an existing database.  To be able
**	to wield this awesome power, you must be the dba for
**	the database.  Also, anyone has this power if the admin
**	the database, or the DBS superuser, and have the "-s"
**	flag requested.  If admin is trashed, the DBS superuser
**	must either destroy the database or recover it.
**
**	If -m is specified, then the directory is not removed.
**	This is useful if the directory is a mounted file system.
*/
main(argc, argv)
int	argc;
char	*argv[];
{
	register int		i;
	register char		*dbase;
	register int		iop;
	register char		*p;
	int			s_flag;
	int			mounted;
	int			indirect;
	char			dbdbuf[BUFSIZ];
	char			**av;
	char			*q;
	extern char		*AAztack();
# ifdef NON_V7_FS
	extern char		*read_dir();
# endif

	AAproc_name = "DESTROYDB";

#	ifdef xSTR1
	AAtTrace(&argc, argv, 'X');
#	endif

	i = initucode(argc, argv, TRUE, (char *) 0, M_EXCL);
	dbase = Parmvect[0];
	switch (i)
	{
	  case 0:
		indirect = FALSE;
		break;

	  case 5:
		indirect = TRUE;
		break;

	  case 1:
	  case 6:
		AAsyserr(0, "Die Datenbank '%s' existiert nicht", dbase);

	  case 2:
		AAsyserr(0, "Sie duerfen auf die Datenbank '%s' nicht zugreifen", dbase);

	  case 3:
		AAsyserr(0, "Sie sind kein eingetragener DBS-Nutzer");

	  case 4:
		printf("Sie haben keinen Datenbanknamen angegeben\n");
	usage:
		AAsyserr(0, "HINWEIS: destroydb [-s] [-m] db_name");

	  default:
		AAsyserr(22200, AAproc_name, i);
	}

	mounted = s_flag = 0;
	for (av = Flagvect; p = *av; av++)
	{
		if (p[0] != '-')
		{
		badflag:
			printf("Unbekanntes Flag '%s'\n", p);
			goto usage;
		}
		switch (p[1])
		{
		  case 's':
			s_flag++;
			break;

		  case 'M':
		  case 'm':
			mounted++;
			break;

		  default:
			goto badflag;
		}
	}

	if (Parmvect[1])
	{
		printf("Zu viele Argumente\n");
		goto usage;
	}
	if (AAlength(dbase) > MAXNAME)
		AAsyserr(0, "Unerlaubter Datenbankname '%s'", dbase);

	if (s_flag)
	{
		if (!(AAdbstat & U_SUPER))
			AAsyserr(0, "Sie duerfen das -s Flag nicht verwenden");
	}
	else if (!AAbequal(AAdmin.adhdr.adowner, AAusercode, 2))
		AAsyserr(0, "Sie sind nicht der DBA der Datenbank '%s'", dbase);

	if (chdir(AAdbpath) < 0)
		AAsyserr(22201, AAproc_name, AAdbpath);

# ifdef NON_V7_FS
	if (!open_dir("."))
		AAsyserr(21050, AAdbpath);
	while (dbase = read_dir())
	{
		/* throw out legitimate files */
		if (AAsequal(dbase, ".") || AAsequal(dbase, ".."))
			continue;
		unlink(dbase);
	}
	close_dir();
# else
	if ((iop = open(".", 0)) < 0)
		AAsyserr(21050, AAdbpath);
	clean(iop);
	close(iop);
# endif

	if (indirect)
	{
		initdbpath((char *) 0, dbdbuf, FALSE);
		unlink(AAztack(dbdbuf, dbase));
	}

	AAunldb();
	fflush(stdout);

	if (!mounted)
	{
		/* find end of AAdbpath and trim it off. */
		for (p = q = AAdbpath; *p; p++)
			if (*p == '/')
				q = p;
		*q++ = '\0';
		if (chdir(AAdbpath) < 0)
			AAsyserr(22201, AAproc_name, AAdbpath);
		execlp(RMDIR, RMDIR, q, (char *) 0);
	}
	exit(0);
}


# ifndef NON_V7_FS
clean(f)
register int	f;
{
	struct direc
	{
		short	inum;
		char	name[14];
		char	null;
	};
	struct direc	cur;
	extern long	lseek();

	cur.null = 0;

	/* skip "." and ".." entries */
	lseek(f, 32L, 0);

	/* scan directory */
	while (read(f, &cur, 16) == 16)
	{
		/* skip null entries */
		if (!cur.inum)
			continue;

		unlink(cur.name);
	}
}
# endif


/*
**  Rubout processing.
*/
rubproc()
{
	AAunlall();
	fflush(stdout);
	exit(-2);
}


AApageflush()
{
}
