# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/bs.h"
# include	"../h/access.h"
# include	"../h/lock.h"

# define	MAXFVEC	15
# define	MAXPVEC	2 * MAXDOM + 20

char		*AAusercode;	/* the usercode of the effective user	*/
int		AAdbstat;	/* the user status of the real user	*/
int		Rubignored;	/* set if rubouts ignored		*/
				/* also in initproc for sys processes	*/
int		Wait_action;	/* the action on the db_lock		*/
char		*AAdbpath;	/* the pathname of the database		*/
char		*AApath;	/* the pathname of the system		*/
char		*Flagvect[MAXFVEC + 1];	/* the flags from argv		*/
char		*Parmvect[MAXPVEC + 1];	/* the parameters from argv	*/
short		Dbs_uid;	/* the user id of the DBS user		*/
int		Standalone;	/* this is stand alone code		*/
# ifdef ESER_PSU
char		AA_PASSWD[]	="/SYS/PASSWD";
# else
char		AA_PASSWD[]	="/etc/passwd";
# endif
LOCKREQ		AALOCKREQ;	/* the database lock structure		*/
static int	pvi;
extern char	*fgetline();

/*
**  INITUCODE -- initialize standalone process
**
**	This function initializes a standalone process, initializing
**	a lot of global variables, scanning the argument vector for
**	some special flags (-u and +-w), seperating flags and
**	parameters, and so forth.
**
**	Every standalone program should begin with the lines:
**		#	ifdef xXTR1
**			AAtTrace(&argc, argv, 'X');
**		#	endif
**			switch (i = initucode(argc, argv, ...))
**				...
**
**	On a return of 2, 3, or 4, essentially none of the processing
**	is done (particularly true with return 4).  Virtually nothing
**	can be done in the calling program except print a "usage"
**	message and exit.  The exception to this is that 'AApath'
**	is set, so that it can be used in the error printing. 
**
**	If it is preferable to not lock the database at this time,
**	the 'waitmode' parameter should be passed as -1.  This still
**	causes the 'Wait_action' variable to be initialized, but the
**	database is not actually locked.  It can be locked by calling:
**		db_lock(AAdbpath, M_EXCL);
**	at the proper time.
**
**	For the main effects of this routine, see the "Side Effects"
**	section below.
**
**	argv[argc] is set to (char *) 0 so that it is well known on version
**	six or version seven systems.
**
**	Parameters:
**		argc -- argc from main.
**		argv -- argv from main.
**		dbflag -- TRUE -- take the first parameter as the
**				database name.
**			FALSE -- don't take the first parameter as
**				the database name.
**		paramlist -- a pointer to an array[4] of pointers
**			to character; set to the extra fields of
**			the AA_USERS entry for the real user
**			executing the code (not the user on the
**			-u flag).  If (char *) 0, this is ignored.
**		waitmode -- M_EXCL -- set an exclusive lock on the
**				database.
**			M_SHARE -- set a shared lock on the database.
**			-1 -- don't set a lock on the database.
**				However, other stuff (Wait_action) is
**				still set up so that the lock can be
**				placed later by calling 'db_lock'.
**
**	Returns:
**		0 -- everything is ok.
**		1 -- the database does not exist.
**		2 -- you are not authorized to access this database.
**		3 -- you are not a valid DBS user.
**		4 -- no database name was specified (only if dbflag
**			== TRUE).
**		5 -- everything is ok, but there was an indirect
**			taken.
**		6 -- there was an indirect taken, but there was no
**			database there.
**
**		If dbflag == FALSE, you can only get returns 0 and
**			3.
**
**	Side Effects:
**		A lot of variables are set, as follows:
**
**		AAdbpath -- set to the pathname of the database (only
**			if dbflag == TRUE).  It is set even if the
**			database does not exist.
**		Parmvect -- set to the parameters from argv, that is,
**			anything not beginning with '+' or '-'.
**		Flagvect -- set to the flags from argv, that is,
**			everything beginning with '+' or '-'.  The
**			flags '+w', '-w', and '-u' are stripped out,
**			however.
**		Wait_action -- set to the appropriate action (A_SLP
**			or A_RTN) based on the +-w flags and whether
**			we are running in background or not.
**			This is automatically used by 'db_lock()'.
**		AAusercode -- set to the persons effective user code
**			(that is, after the -u processing).  Only
**			the DBS user or the DBA can use the -u
**			flag.
**		AApath -- set to the pathname of the DBS subtree.
**		AAdbstat -- an integer set to the user status field
**			of the AA_USERS for the real user.
**		Dbs_uid -- set to the user id of the DBS user.
**
**		The rubout signal (signal 2) is caught, and refered
**		to the standard rubout processor (see rub.c); thus,
**		a routine called 'rubproc' must be defined in the
**		standalone code (which will just call exit, in the
**		normal case).
**
**		The 'adminhdr' part of the 'AAdmin' struct is filled
**		in.  This is not done with AAreadadmin() and is not
**		equivalent to an 'admininit()', but it does make
**		the DBA and database status available.
**
**		This routine can also exit immediately with an
**		error message.
**
**	Defined Constants:
**		MAXPVEC -- the maximum number of parameter type
**			arguments to any standalone program.
**		MAXFVEC -- the maximum number of flag type arg-
**			uments to any standalong program (not inclu-
**			ding flags in the AA_USERS, and the +-w
**			and -u flags).
**
**	Defines:
**		initucode
**		db_lock -- to lock the database.
**		initdbpath -- to initialize the database pathname.
**		Parmvect[]
**		Flagvect[]
**		AAusercode
**		Wait_action
**		AAdbpath
**		AAdbstat -- the user status.
**		AALOCKREQ
**		Dbs_uid
**		Standalone -- '1' to indicate running standalone.
**
**	Files:
**		AA_PASSWD -- to get the pathname for USERDBS.
**		AA_USERS -- to get all the per-user information,
**			and to process the -u flag.
*/

initucode(argc, argv, dbflag, paramlist, waitmode)
int	argc;
char	**argv;
int	dbflag;
char	*paramlist[4];
int	waitmode;
{
	static int	reenter;
	static char	sbuf[MAXLINE * 2];
	register char	*p;
	register char	*sbufp;
	register int	i;
	register char	*q;
	register int	c;
	register FILE	*iop;
	register int	npermit;
	register int	rtval;
# ifdef SETBUF
	char		iobufx[BUFSIZ];
# endif
	char		buf[MAXLINE + 1];
	char		*field[UF_NFIELDS];
	char		alter_ego[2 + 1];
	register int	actualuid;
	short		uid;
	int		waitflag;
	char		*userflag;
	int		fvi;
	char		**avp;
	extern		rubcatch();
	extern int	(*signal())();

	Standalone = TRUE;
	sbufp = sbuf;
	argv[argc] = (char *) 0;

	/*
	**  Set up interrupts.
	*/
	reenter = 0;
	setexit();
	if (reenter++)
		init_term();

	if (signal(2, (int (*)()) 1) == (int (*)()) 0)
	{
		signal(2, rubcatch);
		signal(3, rubcatch);
	}

	/*
	**  Get pathname of DBS subtree from AA_PASSWD
	**  entry for USERDBS and save it
	**  in 'AApath'.
	**
	**  This algorithm suggested by Jim Popa.
	*/
	if (!(iop = fopen(AA_PASSWD, "r")))
		AAsyserr(15004);
# ifdef SETBUF
	setbuf(iop, iobufx);
# endif

	do
	{
		if (!fgetline(buf, MAXLINE, iop))
			AAsyserr(15005);

		/* decode passwd entry */
		for (i = 0, p = buf; *p; p++)
		{
			if (*p == ':')
			{
				*p = 0;
				field[++i] = p + 1;
			}
		}

	} while (!AAsequal(buf, USERDBS));

	fclose(iop);

	/* check for enough fields for valid entry      */
	if (i < PF_HOME)
		AAsyserr(15006, buf);

	/* we now have the DBS AA_PASSWD entry in 'buf' */
	/* copy pathname entry into 'AApath' variable   */
	AApath = sbufp;
	sbufp += AAsmove(field[PF_HOME], sbufp) + 1;

	/* create the DBS user id */
	if (AA_atoi(p = field[PF_UID], &Dbs_uid))
		AAsyserr(15007, p);

	/*
	**  Scan the argument vector.  The following flags are pulled
	**  out of the vector (and argc and argv are adjusted so it
	**  looks like they never existed):
	**	+w, -w -- (don't) wait for the database to be free.
	**	-uxxx -- run as user xxx.  If first character is a
	**	colon, the format must be '-u:xx' where 'xx' is the
	**	internal user code.
	*/
	avp = argv;
	fvi = 0;
	pvi = 0;
	waitflag = 0;
	userflag = (char *) 0;
	*alter_ego = 0;

	for (; --argc > 0; )
	{
		p = *++avp;
		if (p[0] == '+')
		{
			if (p[1] == 'w' || p[1] == 'W')
				waitflag = 1;
			else
				goto boring;
		}
		else if (p[0] == '-')
		{
			switch (p[1])
			{
			  case 'W':
			  case 'w':
				waitflag = -1;
				break;

			  case 'u':
				userflag = &p[2];
				break;

			  case '-':
				get_parms(&p[2]);
				break;

			  default:
				/* not an interesting flag */
			boring:
				if (fvi >= MAXFVEC)
				{
					printf("Zu viele Flags\n");
					init_term();
				}
				Flagvect[fvi++] = p;
				break;
			}
		}
		else	/* not a flag: save in Parmvect */
			save_parm(p);
	}

	if (pvi <= 0 && dbflag)
		return (4);	/* no database name specified */

	/*
	**  Scan the AA_USERS.
	*/
	if (!(iop = fopen(AAztack(AApath, AA_USERS), "r")))
		AAsyserr(15009);
# ifdef SETBUF
	setbuf(iop, iobufx);
# endif

	/* get uid (out of loop) for test */
	actualuid = getuid();

	/* scan AA_USERS, one line at a time */
	rtval = 3;
	while ((!AAusercode || userflag) && fgetline(buf, MAXLINE, iop))
	{
		/* decode AA_USERS entry */
		i = 0;
		field[UF_NAME] = buf;
		for (p = buf; *p; p++)
			if (*p == ':')
			{
				*p = 0;
				field[++i] = p + 1;
			}

		/* check for correct number of fields */
		if (i != UF_NFIELDS - 1)
			AAsyserr(15010, buf);

		/*
		**  Check to see if this entry is the override user.
		**  If so, save his user code in alter_ego.
		*/
		if (userflag && AAsequal(userflag, field[UF_NAME]))
		{
			AAsmove(field[UF_UCODE], alter_ego);
			userflag = (char *) 0;
		}

		/* don't bother with this shit if not needed */
		if (AAusercode)
			continue;

		/*
		**  Build the user id of this entry into 'uid'
		**  and see if it is this user.
		*/
		if (AA_atoi(p = field[UF_UID], &uid))
			AAsyserr(15011, p);

		if (uid != actualuid)
			continue;

		/*
		**  We now have the real user entry.
		**	Fetch the usercode, the status bits, and other
		**	fields from the AA_USERS, and save them in
		**	a safe place (sbuf).
		*/
		AAusercode = sbufp;
		sbufp += AAsmove(field[UF_UCODE], sbufp) + 1;
		AAdbstat = AA_oatoi(field[UF_STAT]);
		if (paramlist)
			for (i = 0; i < 4; i++)
			{
				paramlist[i] = sbufp;
				sbufp += AAsmove(field[UF_FLAGS + i], sbufp) + 1;
			}

		/* validate access permission */
		rtval = 0;
		if (!dbflag || (AAdbstat & U_SUPER))
			continue;

		p = field[UF_DBLIST];
		if (!*p)
			continue;

		/* select permission/no-permission */
		npermit = 0;
		if (*p == '-')
		{
			p++;
			npermit++;
		}

		/* scan for database listed */
		if (!npermit)
			rtval = 2;
		for (c = *p; c; p = q + 1)
		{
			for (q = p; *q != ',' && *q; q++)
				continue;
			c = *q;
			*q = 0;
			if (AAsequal(Parmvect[0], p))
			{
				rtval = npermit? 2: 0;
				break;
			}
		}
	}
	fclose(iop);

	if (rtval)
		return (rtval);

	/*
	**  Check for existance of the database.  This is done by
	**	first building the pathname of the database into
	**	'AAdbpath', and then reading the AA_ADMIN file (just
	**	the adhdr part).
	*/
	if (dbflag)
	{
		AAdbpath = sbufp;
		switch (i = initdbpath(Parmvect[0], AAdbpath, TRUE))
		{
		  case 0:
			rtval = 0;
			break;

		  case 1:
			rtval = 5;
			break;

		  case 2:
			rtval = 1;
			break;

		  case 3:
			rtval = 6;
			break;

		  default:
			AAsyserr(15013, i);
		}
		sbufp += AAlength(AAdbpath) + 1;

		if (!rtval || rtval == 5)
		{
			i = open(AAztack(AAztack(AAdbpath, "/"), AA_ADMIN), 0);
			if (i < 0)
				rtval += 1;
			else
			{
				read(i, &AAdmin.adhdr, sizeof AAdmin.adhdr);
				close(i);
				if (AAdmin.adhdr.adflags & A_NEWFMT)
					AAsyserr(15014);
			}
		}
	}

	/*
	**  Check to see if the name on the -u flag is valid, and
	**	that this user is allowed to use it.
	*/
	if (userflag)
	{
		printf("Unbekannter Nutzername %s\n", userflag);
		init_term();
	}
	if (*alter_ego)
	{
		if (!(AAdbstat & U_SUPER))
		{
			if (!dbflag || !AAbequal(AAdmin.adhdr.adowner, AAusercode, 2))
			{
				printf("Sie duerfen das -u Flag nicht verwenden\n");
				init_term();
			}
		}
		AAbmove(alter_ego, AAusercode, 2);
	}

	/*
	**  Process the +-w flag.
	**	First, determine the locking mode.  If +w, always
	**	wait; if -w, never wait; if unspecified, wait if in
	**	background, but print error and exit if running
	**	interactive.
	*/
	if (waitflag > 0 || (!waitflag && !isatty(0)))
		Wait_action = A_SLP;
	else
		Wait_action = A_RTN;

	if (dbflag && waitmode >= 0)
		db_lock(waitmode);

	/*
	**  Return authorization value.
	*/
	return (rtval);
}


static int	get_parms(file)
char	*file;
{
	register int	i;
	register FILE	*f;
	register char	*p;
	char		buf[MAXFIELD + 1];
	extern char	*malloc();

	if (!(f = fopen(file, "r")))
	{
		printf("? File %s\n", file);
		init_term();
	}
	for (p = buf; (i = getc(f)) > 0; )
	{
		if (i == ' ' || i == '\t' || i == '\r' || i == '\n')
		{
			if (p == buf)
				continue;
			*p++ = 0;
			file = malloc(i = p - buf);
			AAbmove(p = buf, file, i);
			save_parm(file);
		}
		else
			*p++ = i;
	}
	fclose(f);
}


static int	save_parm(p)
register char	*p;
{
	register int	i;

	if (pvi >= MAXPVEC)
	{
		printf("Zu viele Parmameter\n");
		init_term();
	}
	Parmvect[pvi++] = p;
	while (i = ctou(*p))
	{
		if ((i >= 'A'
# ifdef EBCDIC
		   && i <= 'I') || (i >= 'J'
		   && i <= 'R') || (i >= 'S'
# endif
		   && i <= 'Z'))
			*p = i + 'a' - 'A';
		p++;
	}
}


init_term()
{
	fflush(stdout);
	exit(-1);
}


/*
**  DB_LOCK -- lock database
**
**	AALOCKREQs the database.  Everyone should do this before using any
**	database.
**
**	Parameters:
**		database -- the pathname of the database.
**		mode -- M_EXCL -- get an exclusive lock.
**			M_SHARE -- get a shared lock.
**
**	Side Effects:
**		AAlockdes is opened.
*/
db_lock(mode)
register int	mode;
{
	if (!(AAdmin.adhdr.adflags & A_DBCONCUR))
		return;
	if (AAlockdes < 0)
		AAlockdes = open(AA_DBS_LOCK, 1);
	if (AAsetdbl(Wait_action, mode) < 0)
		AAsyserr(0, "Datenbank zeitweilig nicht verfuegbar\n");
}


/*
**  INITDBPATH -- initialize the pathname of the database
**
**	The pathname of a specified database is created.  Indirection
**	via a file is supported, so that if the pathname is a file,
**	the first line of the file is read and used as the pathname
**	of the real database.
**
**	Parameters:
**		database -- the name of the database.  If (char *) 0,
**			the pathname of data/base is returned.
**		dbbuf -- a buffer into which the pathname should
**			be dumped.
**		follow -- if set, follow the indirect chain of
**			database pathnames.
**
**	Returns:
**		0 -- database exists in data/base
**		1 -- database exists, but I followed a pointer.
**		2 -- database doesn't exist in data/base.
**		3 -- databae doesn't exist, but I followed a pointer.
*/
initdbpath(database, dbpath, follow)
char	*database;
char	*dbpath;
int	follow;
{
	struct stat	ibuf;
	register char	*d;
	register FILE	*f;
	register int	phase;
# ifdef SETBUF
	char		fbuf[BUFSIZ];
# endif
	register int	retval;
	register int	uid;

	d = dbpath;

	if (!database)
	{
		AAconcat(AApath, "/data/base/", d);
		return (0);
	}

	/* get the basic pathname */
	AAconcat(AAztack(AApath, "/data/base/"), database, d);

	/*
	** Iterate looking for database.
	**	"Phase" is what we are trying:
	**	    0 -- looking in data/base
	**	    1 -- following indirect.
	*/

	retval = 2;
	for (phase = 0; ; )
	{
		/* find out what sort of filesystem node this is */
		if (stat(d, &ibuf) < 0)
			return (retval);	/* not found */

		/* set up the lock structure for future use */
		AAbmove(&ibuf, &AALOCKREQ.l_db, sizeof (long));
		AALOCKREQ.l_pid = getpid();

		retval -= 2;
		if ((ibuf.st_mode & 060000) == 040000)
			return (retval);	/* found database dir */

		/* if second time through, the database must be a directory */
		if (phase > 0)
			AAsyserr(15015);

		/* if we shouldn't follow the chain, say it exists */
		if (!follow)
			return (3);

		/* it's a file -- see if we can use it */
		uid = ibuf.st_uid;
		if (uid != Dbs_uid || (ibuf.st_mode & 0777) != 0600)
			return (3);

		if (!(f = fopen(d, "r")))
			AAsyserr(15016);
# ifdef SETBUF
		setbuf(f, fbuf);
# endif

		/* read the pathname of the database */
		if (!fgetline(d, MAXLINE, f) || d[0] != '/')
			AAsyserr(15017);
		fclose(f);

		/* prepare for next iteration */
		retval = 3;
		phase = 1;
	}
}
