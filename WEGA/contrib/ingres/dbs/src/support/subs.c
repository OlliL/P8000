# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/access.h"
# include	"../h/lock.h"

/*
** These are subroutines common to RESTORE and PURGE.
*/

char		All;
char		Qrymod;
char		Superuser;
char		Ask;
char		Purge;
char		No_purge;
char		No_sysmod;
char		Clean;
char		Lastflag;

struct directory
{
# ifndef NON_V7_FS
	short	inumber;
# endif
	char	fname[MAXNAME + 3];
};

# ifndef NON_V7_FS
int		Direc_fd;
# endif

extern int	AAdbstat;
extern char	*AAusercode;
char		**Dblist;

/*
**  INITIALIZE GLOBALS
**
**	Set up AAusercode and AAdbstat
*/
initialize(argc, argv)
int	argc;
char	**argv;
{
	register int	i;
	register char	*p;
	register char	**av;
	char		datadir[MAXLINE];
	extern char	*AAproc_name;
	extern char	*Flagvect[];
	extern char	*Parmvect[];
	extern long	lseek();

	AAproc_name = "INITIALIZE";

#	ifdef	xSTR2
	AAtTfp(40, 0, "entered initialize\n");
#	endif

	i = initucode(argc, argv, FALSE, (char *) 0, -1);

#	ifdef	xSTR2
	AAtTfp(40, 1, "initucode ret:%d\n", i);
#	endif

	switch (i)
	{
	  case 0:
		break;

	  case 3:
		AAsyserr(0, "Sie sind kein eingetragener DBS-Nutzer");

	  default:
		AAsyserr(22200, AAproc_name, i);
	}
	initdbpath((char *) 0, datadir, FALSE);

#	ifdef	xSTR2
	AAtTfp(40, 2, "scanning flags\n");
#	endif
	for (av = Flagvect; *av; av++)
	{
		p = *av;
		if (p[0] != '-')
		{
badflag:
			AAsyserr(0, "Unbekanntes Flag %s", p);
		}
		switch (p[1])
		{
		  case 'A':
		  case 'a':
			Ask++;
			break;

		  case 'f':
			Clean++;
			break;

		  case 'p':
			Purge++;
			break;

		  case 'P':
			No_purge++;
			break;

		  case 's':
			if (sucheck())
				Superuser++;
			else
				AAsyserr(0, "Sie duerfen das -s Flag nicht verwenden");
			break;

		  case 'S':
			No_sysmod++;
			break;

		  default:
			goto badflag;
		}
	}
	Dblist = Parmvect;
	if (!*Dblist)
	{
#		ifdef	xSTR2
		AAtTfp(40, 3, "doing all\n");
#		endif
		All++;
# ifdef NON_V7_FS
		if (!open_dir(datadir))
			AAsyserr(21131);
# else
		if ((Direc_fd = open(datadir, 0)) < 0)
			AAsyserr(21131);
		lseek(Direc_fd, 32L, 0);
# endif
	}
#	ifdef	xSTR2
	AAtTfp(40, 0, "leaving initialize\n");
#	endif

	return (0);
}


/*
**  CHECK FOR SUPERUSER
**
**	The user has requested the -s flag.  Can he do it?  Will Martha
**	recover from cancer?  Will Dick get the girl?  Stay tuned for
**	"sucheck".
**
**	Permission is based on the U_SUPER bit in the status field
**	in the AA_USERS.
*/
sucheck()
{
	return (AAdbstat & U_SUPER);
}


/*
**  GET NEXT DATABASE
**
**	The next database to be purged is selected.  It comes from
**	either the directory or the database list.
**
**	Getnxtdb() leaves the user in the database directory.
*/
char	*getnxtdb()
{
# ifdef NON_V7_FS
	extern char		*read_dir();
# endif
	static struct directory	buf;
	register char		*db;
	register int		admin_fd;
	register int		i;
	extern struct admin	AAdmin;
	static char		dbpbuf[MAXLINE];

#	ifdef	xSTR2
	AAtTfp(41, 0, "entered getnxtdb\n");
#	endif

	for ( ; ; )
	{
		if (All)
		{
# ifdef NON_V7_FS
			if (db = read_dir())
			{
				/* throw out legitimate files */
				if (AAsequal(db, ".") || AAsequal(db, ".."))
					continue;
				AAbmove(db, buf.fname, MAXNAME + 2);
				db = buf.fname;
			}
# else
			if ((i = read(Direc_fd, &buf, 16)) < 16)
				db = (char *) 0;
			else
			{
				if (!buf.inumber)
					continue;
				db = buf.fname;
			}
# endif
			buf.fname[MAXNAME + 2] = 0;
		}
		else
			db = *Dblist++;
		if (!db)
		{
# ifdef NON_V7_FS
			close_dir();
# else
			if (Direc_fd);
				close(Direc_fd);
# endif
			return ((char *) 0);
		}

#		ifdef	xSTR2
		AAtTfp(41, 1, "using %s as Database\n", db);
#		endif

		i = initdbpath(db, dbpbuf, TRUE);
#		ifdef	xSTR2
		AAtTfp(41, 3, "INITDBPATH: ret %d, %s\n", i, dbpbuf);
#		endif
		switch (i)
		{
		  case 0:
		  case 1:
			break;

		  case 2:
		  case 3:
			printf("Die Datenbank %s existiert nicht\n", db);
			continue;

		  default:
			AAsyserr(21132, i);
		}
		if (chdir(dbpbuf) < 0)
		{
			printf("Fehler bei cd %s.", dbpbuf);
			continue;
		}
#		ifdef	xSTR2
		AAtTfp(41, 4, "chdir ok, Superuser: %d\n", Superuser);
#		endif
		if ((admin_fd = open(AA_ADMIN, 0)) < 0)
		{
			printf("Fehler beim Eroeffnen des DBA file %s/admin\n", dbpbuf);
			continue;
		}
		read(admin_fd, &AAdmin.adhdr, sizeof AAdmin.adhdr);
		close(admin_fd);
#		ifdef	xSTR2
		AAtTfp(41, 5, "dba: %.2s\n", AAdmin.adhdr.adowner);
#		endif

		/* set qrymod flag from database status */
		Qrymod = ((AAdmin.adhdr.adflags & A_QRYMOD) == A_QRYMOD);

		/* check for dba of database if not superuser */
		if (Superuser || AAbequal(AAdmin.adhdr.adowner, AAusercode, 2))
			break;

		/*
		** not DBA isn't an error if running in all mode since user
		** couln't have specified the database
		*/
		if (All)
			continue;
		printf("Sie sind nicht der DBA der Datenbank %s\n", db);
	}
#	ifdef	xSTR2
	AAtTfp(41, 6, "leaving getnxtdb, %s ok\n", db);
#	endif
	return (db);
}


/*
** ASK
**	If Ask is set desplay prompt and look for 'y' and return TRUE
**	If Ask is not set return TRUE
*/
ask(prompt)
register char	*prompt;
{
	char		line[MAXLINE];
	extern char	Ask;
	extern char	*fgetline();

	if (!Ask)
		return (TRUE);

	printf("**\t\t%s ?  *\b", prompt);

	if (!fgetline(line, MAXLINE, stdin))
		AAsyserr(21133);

	return (line[0] == 'y');
}
