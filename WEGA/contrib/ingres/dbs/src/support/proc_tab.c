# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/access.h"
# include	"../h/pipes.h"
# include	"../h/lock.h"
# include	"../h/bs.h"

# ifdef P8000
ret_buf		AAjmp_buf;
# else
jmp_buf		AAjmp_buf;
# endif

/*
**  DBS -- DBS startup
**
**	This program starts up the entire system.
**
**	Parameters:
**		1 -- database name
**		2 -- optional process table name
**		x -- flags of the form +x or -x may be freely inter-
**			sperced in the argument list.
**
**	Return:
**		none if successful
**		1 -- user error (no database, etc)
**		-1 -- system error
**
**	Flags:
**		-&xxxx -- EQL flag: xxxx are file descriptors for the
**			status return pipe, the command write pipe, the
**			data return pipe, and the data transfer pipe
**			respectively.
**		-@xxxx -- xxxx is same as EQL flag, but no flags
**			are set.
**		-*?? -- Masterid flag. Gives the siteid of the master
**			site in a distributed DBS. (Used in dist.
**			DBS' initproc() function.)
**		-|xxxx -- Network flag.  This flag is just passed to
**			the other processes, to be processed by the
**			DBU's.
**		-uusername -- set effective user to be username.  You
**			must be DBS or the DBA for the database to
**			use this option.
**		-cN -- set minimum character field output width to be
**			N, default 6.  This is the fewest number of
**			characters which may be output in any "c" type
**			field.
**		-inN -- integer output width.  this is the width of
**			an integer field.  The small "n" is the size
**			of the internal field ("1", "2", or "4") and
**			N is the width of the field for that flag.
**			The defaults are -i16, -i26, and -i413.
**		-fnxN.M -- floating point output width and precision.
**			Small "n" is the internal width in bytes ("4"
**			or "8"), x is the format (f, F, g, G, e, E,
**			n, or N), N is the field width, and M is the
**			precision (number of digits after the decimal
**			point).  The formats are:
**			"f" or "F": FORTRAN-style F format: digits,
**				decimal point, digits, no exponent.
**			"e" or "E": FORTRAN-style E format: digits,
**				decimal point, digits, letter "e" (or
**				"E", depending on "x" in the param-
**				eter), and an exponent.  The scaling
**				factor is always one, that is, there
**				is always one digit before the decimal
**				point.
**			"g" or "G": F format if it will fit in the
**				field, otherwise E format.  Space is
**				always left at the right of the field
**				for the exponent, so that decimal
**				points will align.
**			"n" or "N": like G, except that space is not
**				left for the decimal point in F style
**				format (useful if you expect everything
**				to fit, but you're not sure).
**			The default is -fn10.3.
**		-lx -- set horizontal seperator for print operations
**		-vx -- set vertical seperator for print operations
**			and retrieves to the terminal to be "x".  The
**			default is vertical bar ("|").
**		+w -- database wait state.  If set ("+w"), you will
**			wait until the database is not busy.  If clear,
**			you will be informed if the database is busy.
**			If not specified, the same operations take
**			place depending on whether or not you are
**			running in background (determined by whether
**			or not your input is a teletype).  If in fore-
**			ground, you are informed; if in background,
**			you wait.
**		-M -- monitor trace flag
**		-P -- parser trace flag
**		-O -- ovqp trace flag
**		-Q -- qrymod trace flag
**		-D -- decomp trace flag
**		-Z -- dbu trace flag.  These flags require the 020 bit
**			in the status field of the AA_USERS to be
**			set.  The syntax is loose and is described
**			elsewhere.  Briefly, "-Z" sets all flags except
**			the last 20, "-Z4" sets flag 4, and "-Z5/7"
**			sets all flags from 5 through 7.
**		+L -- enable/disable upper to lower case mapping in the
**			parser.  Used for debugging.
**		-rmode -- retrieve into mode
**		-nmode -- index mode.  These flags give the default
**			modify mode for retrieve into and index.  They
**			default to cheapsort and isam.  "Mode" can be
**			any mode to modify except "truncated".
**		+a -- enable/disable autoclear function in monitor.
**			Default on.
**		+b -- enable/disable batch update.  Default on.
**			The 02 bit is needed to clear this flag.
**		+d -- enable/disable printing of the dayfile.  Default
**			on.
**		+s -- enable/disable printing of almost everything from
**			the monitor.
**		+U -- enable/disable direct update of system catalogs.
**			Default off.  The 04 bit is needed to set this
**			option.
**
**	Files:
**		USE_PROC -- to print a "usage: ..." message.
**		.../data/base/<database>/admin -- to determine
**			existance and some info about <database>.
**		.../sys/etc/dayfile<VERSION> -- dayfile (printed by
**			monitor).
**		AA_USERS -- file with BS uid -> DBS code
**			mapping, plus a pile of other information about
**			the user.
**		.../sys/etc/proctab<VERSION> -- default process table
*/

# define	PTSIZE		2048		/* maximum size of the process table */
# define	PTPARAM		'$'		/* parameter expansion indicator */
# define	PTDELIM		"$"		/* separator string in proctab */
# define	MAXPARAMS	10		/* max number of params in proctab */
# define	MAXOPTNS	10		/* maximum number of options you can specify */
# define	MAXPROCS	10		/* maximum number of processes in the system */
# define	PVECTSIZE	6		/* number of pipes to each process */
# define	EQLFLAG	'&'
# define	NETFLAG		'|'		/* network slave flag */
# define	CLOSED		077

char		Fileset[10];
char		*AAdatabase;
extern char	*AAdbpath;		/* defined in initucode */
struct admin	AAdmin;			/* set in initucode */
char		Fdesc[] =	"??????????";
struct lockreq	AALOCKREQ;
char		Ptbuf[PTSIZE + 1];
char		*Ptptr =	Ptbuf;	/* ptr to freespace in Ptbuf */
char		*Opt[MAXOPTNS + 1];
int		Nopts;
int		No_exec;		/* if set, don't execute */
char		*User_ovrd;		/* override usercode from -u flag */
int		R_up;			/* pipe descriptor: read from above */
int		W_up;			/* pipe descriptor: write to above */
int		R_down;			/* pipe descriptor: read from below */
int		W_down;			/* pipe descriptor: write to below */
char		USE_PROC[]	= "/sys/etc/";

/* system error messages, etc. */
extern int	sys_nerr;
extern char	*sys_errlist[];
extern int	errno;

main(argc, argv)
int	argc;
char	**argv;
{
	register int		i;
	register int		j;
	register char		*p;
	register int		fd;
	register char		*proctab;
	register char		*ptr;
	char			*uservect[4];
	extern int		AAdbstat;
	extern char		*AAproc_name;
	extern char		*Flagvect[];	/* defined in initucode.c */
	extern char		*Parmvect[];	/* ditto */
	extern char		AAversion[];	/* version number */
	extern char		*AAztack();

	AAproc_name = argv[0];

#	ifdef xSTR1
	AAtTrace(&argc, argv, 'X');
#	endif

	AA_itoa(getpid(), Fileset);
	proctab = (char *) 0;
	AAdatabase = (char *) 0;

	/*
	**  Initialize everything, like Flagvect, Parmvect, AAusercode,
	**	etc.
	*/
	switch (i = initucode(argc, argv, TRUE, uservect, -1))
	{
	  case 0:
	  case 5:
		break;

	  case 1:
	  case 6:
		AAsyserr(0, "Die Datenbank '%s' existiert nicht", Parmvect[0]);

	  case 2:
		AAsyserr(0, "Sie duerfen auf die Datenbank '%s' nicht zugreifen", AAdatabase);

	  case 3:
		AAsyserr(0, "Sie sind kein eingetragener DBS-Nutzer");

	  case 4:
		printf("Sie haben keinen Datenbanknamen angegeben\n");
		goto usage;

	  default:
		AAsyserr(22200, AAproc_name, i);
	}

	/*
	**  Extract database name and process table name from
	**	parameter vector.
	*/
	proctab = Parmvect[1];
	if (checkdbname(AAdatabase = Parmvect[0]))
	{
		printf("Unerlauber Datenbankname '%s'\n", AAdatabase);
		goto usage;
	}

	/* scan flags in AA_USERS */
	for (p = uservect[0]; *p; p++)
	{
		/* skip initial blanks and tabs */
		if (*p == ' ' || *p == '\t')
			continue;
		ptr = p;

		/* find end of flag and null-terminate it */
		while (*p && *p != ' ' && *p != '\t')
			p++;
		i = *p;
		*p = '\0';

		/* process the flag */
		doflag(ptr, 1);
		if (!i)
			break;
	}

	/* scan flags on command line */
	for (i = 0; p = Flagvect[i]; i++)
		doflag(p, 0);

	/* check for query modification specified for this database */
	if (!(AAdmin.adhdr.adflags & A_QRYMOD))
		doflag("-q", -1);

	/* do the -u flag processing */
	if (User_ovrd)
		AAbmove(User_ovrd, AAusercode, 2);

	/* close any extraneous files, being careful not to close anything we need */
	for (i = 'C'; i < 0100 + MAXFILES; i++)
	{
		for (j = 0; j < (sizeof Fdesc / sizeof *Fdesc); j++)
		{
			if (Fdesc[j] == i)
				break;
		}
		if (j >= (sizeof Fdesc / sizeof *Fdesc))
			close(i & 077);
	}

	/* determine process table */
	if (!proctab)
	{
		/* use default proctab */
		proctab = uservect[1];
		if (!*proctab)
		{
			/* no proctab in AA_USERS */
			AAconcat(AAztack(AAztack("=", argv[0]), "_tab"), AAversion, Ptbuf);

			/* strip off the mod number */
			for (p = Ptbuf; *p; p++)
				if (*p == '/')
					break;
			*p = 0;
			proctab = Ptbuf;
		}
	}
	else
	{
		/* proctab specified; check permissions */
		if (!(AAdbstat & ((*proctab == '=')? U_EPROCTAB: U_APROCTAB)))
		{
			printf("Sie duerfen die Prozesstabelle '%s' nicht angeben\n", proctab);
			goto usage;
		}
	}

	/* expand process table name */
	if (*proctab == '=')
	{
		AAsmove(AAztack(AAztack(AApath, "/sys/etc/"), &proctab[1]), Ptbuf);
		proctab = Ptbuf;
	}

	/* open and read the process table */
	if ((fd = open(proctab, 0)) < 0)
	{
		printf("Prozesstabelle %s: %s\n", proctab, sys_errlist[errno]);
		goto usage;
	}

	if ((i = read(fd, Ptbuf, PTSIZE + 1)) < 0)
		AAsyserr(21058, proctab);

	if (i > PTSIZE)
		AAsyserr(21059, proctab, PTSIZE);

	close(fd);
	Ptbuf[i] = 0;

	/* build internal form of the process table */
	buildint();

	/* don't bother executing if we have found errors */
	if (No_exec)
	{
usage:		/* cat USE_PROC */
		AAcat(AAztack(AAztack(AAztack(AApath, USE_PROC), argv[0]), "_use"));
		fflush(stdout);
		exit(1);
	}

	/* A lock is set on the database */
	db_lock(flagval('E') > 0? M_EXCL: M_SHARE);

	/* satisfy process table (never returns) */
	satisfypt();
}


/*
**  Process rubouts (just exit)
*/
rubproc()
{
	AAunlall();
	fflush(stdout);
	exit(1);
}


/*
**  DOFLAG -- process flag
**
**	Parameters:
**		flag -- the flag (as a string)
**		where -- where it is called from
**			-1 -- internally inserted
**			0 -- on user command line
**			1 -- from AA_USERS
**
**	Side effects:
**		All flags are inserted on the end of the
**		"Flaglist" vector for passing to the processes.
**		The "No_exec" flag is set if the flag is bad or you
**		are not authorized to use it.
*/
struct flag
{
	char	flagname;	/* name of the flag */
	char	flagstat;	/* status of flag (see below) */
	int	flagsyntx;	/* syntax code for this flag */
	int	flagperm;	/* status bits needed to use this flag */
};

/* status bits for flag */
# define	F_PLOK		01	/* allow +x form */
# define	F_PLD		02	/* defaults to +x */
# define	F_DBA		04	/* must be the DBA to use */
# define	F_DROP		010	/* don't save in Flaglist */

/* syntax codes */
# define	F_ACCPT		1	/* always accept */
# define	F_C_SPEC	3	/* -cN spec */
# define	F_I_SPEC	4	/* -inN spec */
# define	F_F_SPEC	5	/* -fnxN.M spec */
# define	F_CHAR		6	/* single character */
# define	F_MODE		7	/* a modify mode */
# define	F_INTERNAL	8	/* internal flag, e.g., -q */
# define	F_EQL		9	/* EQL flag */
# define	F_STRING	10	/* text */

struct flag	Flagok[] =
{
	'a',		F_PLD|F_PLOK,	F_ACCPT,	0,
	'b',		F_PLD|F_PLOK,	F_ACCPT,	U_DRCTUPDT,
	'c',		0,		F_C_SPEC,	0,
	'd',		F_PLD|F_PLOK,	F_ACCPT,	0,
	'f',		0,		F_F_SPEC,	0,
	'i',		0,		F_I_SPEC,	0,
	'l',		0,		F_CHAR,		0,
	'n',		0,		F_MODE,		0,
	'q',		F_PLD|F_PLOK,	F_INTERNAL,	0,
	'r',		0,		F_MODE,		0,
	's',		F_PLD|F_PLOK,	F_ACCPT,	0,
	'v',		0,		F_CHAR,		0,
	'w',		F_PLOK|F_DROP,	F_ACCPT,	0,
	'D',		0,		F_ACCPT,	U_TRACE,
	'E',		F_PLOK|F_DBA,	F_ACCPT,	0,
	'I',		0,		F_STRING,	0,
	'L',		F_PLOK,		F_ACCPT,	0,
	'M',		0,		F_ACCPT,	U_TRACE,
	'O',		0,		F_ACCPT,	U_TRACE,
	'P',		0,		F_ACCPT,	U_TRACE,
	'Q',		0,		F_ACCPT,	U_TRACE,
	'U',		F_PLOK,		F_ACCPT,	U_UPSYSCAT,
	'Z',		0,		F_ACCPT,	U_TRACE,
	EQLFLAG,	0,		F_EQL,		0,
	NETFLAG,	0,		F_EQL,		0,
	'@',		0,		F_EQL,		0,
	'*',		0,		F_ACCPT,	0,
	0,		0,		0,		0
};

/* list of valid retrieve into or index modes */
char	*Relmode[] =
{
	"isam",
	"cisam",
	"hash",
	"chash",
	"heap",
	"cheap",
	"heapsort",
	"cheapsort",
	(char *) 0
};

doflag(p, where)
register char	*p;
int		where;
{
	register struct flag	*f;
	register char		*ptr;
	register int		i;
	short			intxx;
	extern int		AAdbstat;

	/* check for valid flag format (begin with + or -) */
	if (*p != '+' && *p != '-')
		goto badflag;

	/* check for flag in table */
	for (f = Flagok; f->flagname != p[1]; f++)
		if (!f->flagname)
			goto badflag;

	/* check for +x form allowed */
	if (*p == '+' && !(f->flagstat & F_PLOK))
		goto badflag;

	/* check for permission to use the flag */
	if ((f->flagperm && !(AAdbstat & f->flagperm) &&
	    (!(f->flagstat & F_PLD)? (*p == '+'): (*p == '-'))) ||
	    ((f->flagstat & F_DBA) && !(AAdbstat & U_SUPER) &&
	    !AAbequal(AAusercode, AAdmin.adhdr.adowner, 2)))
	{
		printf("Sie duerfen das %s Flag nicht verwenden\n", p);
		No_exec++;
	}

	/* check syntax */
	switch (f->flagsyntx)
	{
	  case F_ACCPT:
		break;

	  case F_C_SPEC:
		if (AA_atoi(&p[2], &intxx) || intxx > MAXFIELD)
			goto badflag;
		break;

	  case F_I_SPEC:
		i = p[2] - '0';
		if (i != sizeof (char) && i != sizeof (short) && i != sizeof (long))
			goto badflag;
		if (AA_atoi(&p[3], &intxx) || intxx > MAXFIELD)
			goto badflag;
		break;

	  case F_F_SPEC:
		i = p[2] - '0';
		if
		(
# ifndef NO_F4
			i != sizeof (float) &&
# endif
			i != sizeof (double)
		)
			goto badflag;
		switch (p[3])
		{
		  case 'e':
		  case 'E':
		  case 'f':
		  case 'F':
		  case 'g':
		  case 'G':
		  case 'n':
		  case 'N':
			break;

		  default:
			goto badflag;

		}
		ptr = &p[4];
		while (*ptr != '.')
			if (!*ptr)
				goto badflag;
			else
				ptr++;
		*ptr = 0;
		if (AA_atoi(&p[4], &intxx) || intxx > MAXFIELD)
			goto badflag;
		*ptr++ = '.';
		if (AA_atoi(ptr, &intxx) || intxx > MAXFIELD)
			goto badflag;
		break;

	  case F_CHAR:
		if (!p[2] || p[3])
			goto badflag;
		break;

	  case F_STRING:
		if (!p[2])
			goto badflag;
		break;

	  case F_MODE:
		for (i = 0; ptr = Relmode[i]; i++)
		{
			if (AAsequal(&p[2], ptr))
				break;
		}
		if (!ptr)
			goto badflag;
		break;

	  case F_INTERNAL:
		if (where >= 0)
			goto badflag;
		break;

	  case F_EQL:
		ptr = &p[2];
		for (i = 0; i < (sizeof Fdesc / sizeof *Fdesc); i++, ptr++)
		{
			if (*ptr == CLOSED)
				continue;
			if (!*ptr || *ptr < 0100 || *ptr >= 0100 + MAXFILES)
				break;
			Fdesc[i] = (*ptr & 077) | 0100;
		}
		break;

	  default:
		AAsyserr(21060, f->flagsyntx);

	}

	/* save flag */
	if (Nopts >= MAXOPTNS)
		AAsyserr(0, "Zu viele Argumente");

	if (!(f->flagstat & F_DROP))
		Opt[Nopts++] = p;
	return;

badflag:
	printf("Unbekanntes Flag '%s'\n", p);
	No_exec++;
	return;
}


/*
**  BUILDINT -- build internal form of process table
**
**	The text of the process table is scanned and converted to
**	internal form.  Non-applicable entries are deleted in this
**	pass, and the EQL pipes are turned into real file descrip-
**	tors.
*/
struct proc
{
	char	*prpath;
	char	*prcond;
	int	prstat;
	char	*prstdout;		/* file for stdout this proc */
	char	prpipes[PVECTSIZE + 2];
	char	*prparam;
};

/* bits for prstat */
# define	PR_REALUID	01	/* run the process as the real user */
# define	PR_NOCHDIR	02	/* run in the user's directory */
# define	PR_CLSSIN	04	/* close standard input */
# define	PR_CLSDOUT	010	/* close diagnostic output */

struct proc	Proctab[MAXPROCS];

struct param
{
	char	*pname;
	char	*pvalue;
};

struct param	Params[MAXPARAMS];

buildint()
{
	register struct proc	*pr;
	register char		*p;
	register char		*pipes;
	register struct param	*pp;
	register int		i;
	register int		j;
	char			*ptp;
	extern char		*getptline();

	/* scan the template */
	pr = Proctab;
	while (ptp = getptline())
	{
		/* check for overflow of internal Proctab */
		if (pr >= &Proctab[MAXPROCS])
			AAsyserr(21061);

		/* collect a line into the fixed-format proctab */
		pr->prpath = ptp;
		scanpt(&ptp, ':');
		pr->prcond = ptp;
		scanpt(&ptp, ':');
		pipes = ptp;
		scanpt(&ptp, ':');
		pr->prstat = AA_oatoi(pipes);
		pr->prstdout = ptp;
		scanpt(&ptp, ':');
		pipes = ptp;
		scanpt(&ptp, ':');
		if (AAlength(pipes) != PVECTSIZE)
			AAsyserr(21062, pr->prpath, pipes);
		AAsmove(pipes, pr->prpipes);
		for (p = pr->prpipes; p < &pr->prpipes[PVECTSIZE]; p++)
			if (!*p)
				*p = CLOSED;
		pr->prparam = ptp;

		/* check to see if this entry is applicable */
		for (p = pr->prcond; *p; )
		{
			/* determine type of flag */
			switch (*p++)
			{
			  case '=':	/* flag must not be set */
				j = 1;
				break;

			  case '+':	/* flag must be set on */
			  case '>':
				j = 2;
				if (*p == '=')
				{
					j++;
					p++;
				}
				break;

			  case '-':	/* flag must be set off */
			  case '<':
				j = 4;
				if (*p == '=')
				{
					j++;
					p++;
				}
				break;

			  default:
				/* skip any initial garbage */
				continue;
			}

			/* get value of this flag */
			i = flagval(*p++);

			/* check to see if we should keep looking */
			switch (j)
			{
			  case 1:
				if (!i)
					continue;
				break;

			  case 2:
				if (i > 0)
					continue;
				break;

			  case 3:
				if (i >= 0)
					continue;
				break;

			  case 4:
				if (i < 0)
					continue;
				break;

			  case 5:
				if (i <= 0)
					continue;
				break;
			}

			/* mark this entry as a loser and quit */
			p = (char *) 0;
			break;
		}

		if (!p)	/* this entry does not apply; throw it out */
			continue;

		/* replace EQL codes with actual file descriptors */
		for (p = pr->prpipes; *p; p++)
		{
			if (*p < '0' || *p > '9')
				continue;

			*p = Fdesc[*p - '0'];
		}

		/* this entry is valid, move up to next entry */
		pr++;
	}
	pr->prpath = 0;

	/* scan for parameters */
	pp = Params;
	pp->pname = "pathname";
	pp++->pvalue = AApath;
	while (p = getptline())
	{
		/* check for parameter list overflow */
		if (pp >= &Params[MAXPARAMS])
			AAsyserr(21063);

		pp->pname = p;

		/* get value for parameter */
		if (!(pp->pvalue = p = getptline()))	/* if no lines, make it null string */
			pp->pvalue = "";
		else	/* get 2nd thru nth and concatenate them */
			do
			{
				ptp = &p[AAlength(p)];
				p = getptline();
				*ptp = '\n';
			} while (p);

		/* bump to next entry */
		pp++;
	}

	/* adjust free space pointer for eventual call to expand() */
	Ptptr++;
}


/*
**  FLAGVAL -- return value of flag
**
**	Parameter:
**		flag -- the name of the flag
**
**	Return:
**		-1 -- flag is de-asserted (-x)
**		0 -- flag is not specified
**		1 -- flag is asserted (+x)
*/
flagval(f)
register int	f;
{
	register char	**p;
	register char	*o;

	/* start scanning option list */
	for (p = Opt; o = *p; p++)
		if (o[1] == f)
			return ((*o == '+')? 1: -1);
	return (0);
}


/*
**  SCANPT -- scan through process table for a set of delimiters
**
**	Parameters:
**		pp -- a pointer to a pointer to the process table
**		delim -- a primary delimiter.
**
**	Returns:
**		The actual delimiter found
**
**	Side effects:
**		Updates pp to point the the delimiter.  The delimiter
**		is replaced with null.
*/
scanpt(pp, d)
register char	**pp;
register int	d;
{
	register char	*p;
	register int	c;

	for (p = *pp; ; p++)
	{
		/* check to see if it is a delimiter */
		if ((c = *p) == d)
		{
			/* we have a delim match */
			*p++ = 0;
			*pp = p;
			return (c);
		}
		if (!c)
			AAsyserr("SCANPT: d=%c, *pp=%s", d, *pp);
	}
}


/*
**  GETPTLINE -- get line from process table
**
**	Return:
**		zero -- end of process table or section
**		else -- pointer to a line, which is a null-terminated
**			string (without the newline).
**
**	Side effects:
**		sticks a null byte into Ptbuf at the end of the line.
**
**	Note:
**		sequential lines returned will be sequential in core,
**			that is, they can be AAconcatenated by just
**			changing the null-terminator back to a newline.
*/
char	*getptline()
{
	register int	c;
	register char	*line;

	/* mark the beginning of the line */
	line = Ptptr;

	/* scan for a newline or a null */
	while ((c = *Ptptr++) != '\n')
	{
		if (!c)
		{
			/* arrange to return zero next time too */
			Ptptr--;
			return ((char *) 0);
		}
	}

	/* replace the newline with null */
	Ptptr[-1] = 0;

	/* see if it is an "end-of-section" mark */
	if (AAsequal(line, PTDELIM))
	{
		line[0] = 0;
		return ((char *) 0);
	}
	return (line);
}


/*
**  EXPAND -- macro expand a string
**
**	A string is expanded: $name constructs are looked up and
**	expanded if found.  If not, the old construct is passed
**	thru.  The return is the address of a new string that will
**	do what you want.  Calls may be recursive.  The string is
**	not copied unless necessary.
**
**	Parameters:
**		s1 -- the string to expand
**		intflag -- a flag set if recursing
**
**	Return:
**		the address of the expanded string, not copied unless
**		necessary.
**
**	Side effects:
**		Ptbuf is allocated (from Ptptr) for copy space.  There
**			must be enough space to copy, even if the copy
**			is not saved.
**		Ptptr is updated to point to the new free space.
*/
char	*expand(s1, intflag)
char	*s1;
int	intflag;
{
	register char		*s;
	register struct param	*pp;
	register int		c;
	register int		flag;
	register int		count;
	register char		*mark;
	register char		*xmark;

	s = s1;
	xmark = Ptptr;
	flag = 0;		/* set if made a macro expansion */
	count = 0;		/* set if any characters copied directly */

	/* scan input string to end */
	while (c = *s++)
	{
		/* check for possible parameter expansion */
		if (c != PTPARAM)
		{
			/* nope, copy through if space */
			if (Ptptr >= &Ptbuf[PTSIZE])
				goto ptsizeerr;
			*Ptptr++ = c;
			count++;
			continue;
		}

		/* possible expansion; mark this point */
		mark = Ptptr;

		/* collect the name of the parameter */
		do
		{
			if (Ptptr >= &Ptbuf[PTSIZE])
			{
ptsizeerr:
				AAsyserr(21065);
			}
			*Ptptr++ = c;
			c = *s++;
		} while ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
		s--;

		/* look up in parameter table */
		*Ptptr = 0;
		for (pp = Params; pp->pname; pp++)
			if (AAsequal(pp->pname, mark + 1))
				break;

		if (!pp->pname)
		{
			/* search failed, take string as is */
			count++;
			continue;
		}

		/* reprocess name delimiter unless a newline */
		if (c == '\n')
			s++;

		/* found entry, copy it in instead */
		/* back up over name */
		Ptptr = mark;

		/* check to see if we can return expansion as value */
		if (!count && !*s)
		{
			/* then nothing before or after this call */
			if (!intflag)
				Ptptr = xmark;
			return (pp->pvalue = expand(pp->pvalue, 1));
		}

		/* expand to new buffer */
		pp->pvalue = expand(pp->pvalue, 1);

		/* we are appending */
		Ptptr--;
		flag++;
	}

	/* null terminate the new entry (which must be copied) */
	*Ptptr++ = 0;

	/* see if we expanded anything */
	if (flag)	/* yes, return the new stuff in buffer */
		return (xmark);

	/* no, then we can return the original and reclaim space */
	if (!intflag)
		Ptptr = xmark;
	return (s1);
}


/*
**  SATISFYPT -- satisfy the process table
**
**	Well folks, now that you've read this far, this is it!!!  I
**	mean, this one really does it!!!  It takes the internal form
**	built by buildint() and creates pipes as necessary, forks, and
**	execs the DBS processes.  Isn't that neat?
**
**		* * * * *   W A R N I N G   * * * * *
**
**	In some versions, the pipe() system call support function
**	in libc.a clobbers r2.  In versions 6 and 7 PDP-11 C compilers,
**	this is the third register variable declared (in this case,
**	"i").  This isn't a problem here, but take care if you change
**	this routine.
**
**	Returns:
**		never
*/
satisfypt()
{
	register struct proc	*pr;
	register char		*p;
	register int		i;
	struct pipfrmt		pbuf;
	int			pipex[2];

	/* scan the process table */
	for (pr = Proctab; pr->prpath; pr++)
	{
		/* scan pipe vector, creating new pipes as needed */
		for (p = pr->prpipes; *p; p++)
		{
			if ((*p >= 0100 && *p < 0100 + MAXFILES) || *p == CLOSED)
				continue;	/* then already open or never open */

			/* not yet open: create a pipe */
			if (pipe(pipex))
				AAsyserr(21066, *p);

			/* propagate pipe through rest of proctab */
			fillpipe(*p, pr, pipex);
		}

		/* fork if necessary */
		fflush(stdout);
		if (!pr[1].prpath || !(i = fork()))
			proc_exec(pr);	/* child!! */

		/* parent */
		if (i < 0)
			AAsyserr(21067);

		/* resync to child */
		rdpipe(P_PRIME, &pbuf);
		rdpipe(P_INT, &pbuf, pr[1].prpipes[2] & 077);

		/* scan pipes.  close all not used in the future */
		for (p = pr->prpipes; *p; p++)
		{
			if (*p == CLOSED)
				continue;
			if (checkpipes(&pr[1], *p))
				if (close(*p & 077))
					AAsyserr(21068, pr->prpath, *p & 077);
		}
	}
	AAsyserr(21069);
}


/*
**  FILLPIPE -- extend pipe through rest of process table
**
**	The only tricky thing in here is that "rw" alternates between
**	zero and one as a pipe vector is scanned, so that it will know
**	whether to get the read or the write end of a pipe.
**
**	Parameters:
**		name -- external name of the pipe
**		proc -- the point in the process table to scan from
**		pipex -- the pipe
*/
fillpipe(name, pr, pipex)
int			name;
register struct proc	*pr;
int			pipex[2];
{
	register char		*p;
	register int		rw;

	/* scan rest of processes */
	for ( ; pr->prpath; pr++)
	{
		/* scan the pipe vector */
		rw = 1;
		for (p = pr->prpipes; *p; p++)
		{
			rw = 1 - rw;
			if (*p == name)
				*p = pipex[rw] | 0100;
		}
	}
}


/*
**  CHECKPIPES -- check for pipe referenced in the future
**
**	Parameters:
**		proc -- point in the process table to start looking
**			from.
**		fd -- the file descriptor to look for.
**
**	Return:
**		zero -- it will be referenced in the future.
**		one -- it is never again referenced.
*/
checkpipes(pr, fd)
register struct proc	*pr;
int			fd;
{
	register char		*p;
	register int		fdx;

	fdx = fd | 0100;

	for ( ; pr->prpath; pr++)
	{
		for (p = pr->prpipes; *p; p++)
			if (*p == fdx)
				return (0);
	}
	return (1);
}


/*
**  TABEXEC -- execute DBS process
**
**	This routine handles all the setup of the argument vector
**	and then executes a process.
**
**	Parameters:
**		process -- a pointer to the process table entry which
**			describes this process.
**
**	Returns:
**		never
**
**	Side Effects:
**		never returns, but starts up a new overlay.  Notice
**			that it does NOT fork.
*/
proc_exec(pr)
register struct proc	*pr;
{
	register char		*p;
	register char		**v;
	register char		**opt;
	register int		i;
	register int		outfd;
	char			*vect[30];
	extern char		*AAproc_name;

	v = vect;
	*v++ = expand(pr->prpath);
	for (p = pr->prpipes; *p; p++)
		continue;
	*p++ = (AAlockdes < 0)? CLOSED: AAlockdes | 0100;
	*p = 0;
	*v++ = pr->prpipes;
	*v++ = Fileset;
	*v++ = AAusercode;
	*v++ = AAdatabase;
	*v++ = AApath;

	/* expand variable number of parameters */
	for (p = pr->prparam; *p; )
	{
		for (*v = p; *p; p++)
			if (*p == ':')
			{
				*p++ = 0;
				break;
			}

		/* expand macros in parameters */
		*v = expand(*v);
		v++;
	}

	/* insert flag parameters */
	for (opt = Opt; *opt; opt++)
		*v++ = *opt;
	*v = 0;

	/* close extra pipes (those not in pipe vector) */
	for (i = 0100; i < 0100 + MAXFILES; i++)
	{
		for (p = pr->prpipes; *p; p++)
			if (i == *p)
				break;
		if (!*p && i != 0101 &&
		    (i != 0100 || (pr->prstat & PR_CLSSIN)) &&
		    (i != 0102 || (pr->prstat & PR_CLSDOUT)))
			close(i & 077);
	}

	/* change to the correct directory */
	if (!(pr->prstat & PR_NOCHDIR) && chdir(AAdbpath))
		AAsyserr(22201, AAproc_name, AAdbpath);

	/* change to normal userid/groupid if a non-dangerous process */
	if (pr->prstat & PR_REALUID)
	{
		setuid(getuid());
		setgid(getgid());
	}

	/* change standard output if specified in proctab */
	p = pr->prstdout;
	if (*p)
	{
		/* chew up fd 0 (just in case) */
		outfd = dup(1);
		close(1);
		if (creat(p, 0666) != 1)
		{
			/* restore standard output and print error */
			close(1);
			dup(outfd);	/* better go into slot 1 */
			AAsyserr(21071, p);
		}
		close(outfd);
	}

	/* give it the old college (or in this case, University) try */
	fflush(stdout);
	execv(vect[0], vect);
	AAsyserr("PROC_EXEC: \"%s\" not executable", vect[0]);
}


/*
**  CHECKDBNAME -- check for valid database name
**
**	Parameter:
**		name -- the database name
**
**	Return:
**		zero -- ok
**		else -- bad database name
*/
checkdbname(name)
register char	*name;
{
	register int	c;

	if (AAlength(name) > MAXNAME)
		return (1);

	while (c = *name++)
		if (c == '/')
			return (1);

	return (0);
}


AApageflush()
{
}
