# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/access.h"
# include	"../h/pipes.h"
# include	"../h/symbol.h"
# include	"../h/lock.h"
# include	"../h/bs.h"

# ifdef P8000
ret_buf		AAjmp_buf;
# else
jmp_buf		AAjmp_buf;
# endif

# ifdef ESER
# define	getw		gethw

# ifdef ESER_PSU
union hword
{
	char	hw_char[2];
	short	hw_short;
};

gethw(fp)
FILE	*fp;
{
	union hword	hw;

	hw.hw_char[0] = getc(fp);
	hw.hw_char[1] = getc(fp);
	return (hw.hw_short);
}
# endif
# endif

/*
**  CREATDB -- create database (or modify database status)
**
**	This program creates a new database.  It takes the name of
**	the new database (syntax defined below) and a series of
**	flags (also defined below).
**
**	In order to perform this command, you must be enabled by
**	having the U_CREATDB bit set in the user status word
**	of the AA_USERS.
**
**	The -m flag specifies that the directory for the database
**	already exists.  It stands for "mounted-file-system",
**	because this is presumably when you might use this feature.
**	The directory must be empty.
**
**	The -e flag specifies that the database already exists.
**	It must be in all ways a valid database.  This mode allows
**	you to turn flags on and off, as controlled by the other
**	flags.
**
**	Usage:
**		creatdb [flags] databasename
**
**	Positional Parameters:
**		databasename -- the name of the database to create.
**			It must conform to all the usual rules
**			about names.  Notice that this is more
**			restrictive than BS usually is:  names
**			must begin with an alpha, and must be
**			composed of alphanumerics.  It may be
**			at most MAXNAME characters long.  Underscore
**			counts as an alphabetic.
**
**	Flags:
**		-m
**			This is a mounted filesystem.  Actually,
**			this just means that the directory in which
**			the database is to reside already exists.
**			It must be empty.
**		-e
**			This database exists.  When the -e flag is
**			specified, the database is brought up to date,
**			rather than created.  Things which may be
**			changed with the -e flag is anything that
**			affects the database status or the relation
**			status bits.
**		-uXX
**			Run as user XX (usercode or login name).  This
**			flag may only be used by the DBS superuser.
**			Normally, the database administrator for the
**			new database is the user who performs the
**			command, but the -u flag allows DBS to
**			give the database to someone else.  Thus, it
**			is possible for someone to be a DBA without
**			having the U_CREATDB bit set.
**		-Fpathname
**			Use the named file as the database template.
**			This is, of course, for debugging use only.
**		+-c
**			Turn concurrency control on/off.  The default
**			for a new database depends on the DBTMPLTFILE,
**			but as of this writing it defaults on.
**		+-q
**			Turn query modification on/off.
**		+-l
**			Turn protection violation logging on/off.
**
**	Files:
**		USE_CREATDB -- to print a "usage: ..." message.
**		DBTMPLTFILE<VERSION>
**			This file drives the entire program.  The
**			syntax of this file is defined below in
**			readdbtemp().  Briefly, it tells the database
**			status, the relations in an 'empty' database,
**			and the status and format of those relations.
**		.../data/base
**			This directory is the directory in which all
**			databases eventually reside.  Each database is
**			a subdirectory of this directory.
**
**	Return Codes:
**		zero -- success
**		else -- failure.
**
**	Defined Constants:
**		MAXRELNS
**			This defines the maximum number of relations
**			which may be declared in the DBTMPLTFILE.
**		MAXDBTEMP
**			The maximum size of the DBTMPLTFILE.  This
**			determines the maximum number of characters
**			which may be in the file.
**
**	Requires:
**		Much.  Notably, be careful about changes to any
**		access methods, especially automatic references to
**			to the AA_ADMIN file or AA_REL relation.
*/

# define	MAXRELNS	20
# define	MAXDBTEMP	2000

/* AA_REL & AA_ATT reln descriptors used in access methods */
extern DESC	Reldes;
extern DESC	Attdes;

extern int	AAdbstat;		/* user status, set by initucode */
extern char	*AAdbpath;
extern char	*AAztack();

int		Dbstat;		/* database status */
# ifdef VPGSIZE
int		AApgsize = PGSIZE;
int		AAmaxtup = MAXTUP;
int		AAltsize = 0;
# endif
int		Dbson;
int		Dbsoff;	/* same: bits turned on, bits turned off */
char		Delim;
char		DBTMPLTFILE[]	= "/sys/etc/dbtmplt";
char		MKDIR[]		= "mkdir";
char		SYSNAME[]	= "_SYS";
int		s_SYSNAME	= sizeof SYSNAME - 1;
char		USE_CREATDB[]	= "/sys/etc/creatdb_use";

struct reldes
{
	int	bitson;
	int	bitsoff;
	char	*parmv[MAXPARMS];
};
struct reldes	Rellist[MAXRELNS];


main(argc, argv)
int	argc;
char	*argv[];
{
	register int		i;
	register char		*p;
	register short		*flagv;
	register char		*dbtmpfile;
	register char		*database;
	register int		faterr;
	register int		exists;
	char			**av;
	int			admin;
	int			code;
	struct stat		stbuf;
	char			adminbuf[BUFSIZ];
	extern int		errno;
	extern struct admin	AAdmin;
	extern char		AAversion[];
	extern char		*AAproc_name;
	extern char		*Parmvect[];
	extern char		*Flagvect[];
	extern short		*flaglkup();
# ifdef NON_V7_FS
	extern char		*read_dir();
	register char		*direc;
# endif

	AAproc_name = "CREATDB";

#	ifdef xSTR1
	AAtTrace(&argc, argv, 'X');
#	endif

	/*
	** The concurrency device will be opened for writing.
	** If the lock device isn't installed, then AAlockdes = -1
	** and no locking will (or can) occure.
	*/
	AAlockdes = open(AA_DBS_LOCK, 1);
	errno = 0;	/* clear in case AA_DBS_LOCK isn't available */
	AALOCKREQ.l_db = 0L;
	AALOCKREQ.l_pid = getpid();
	if (AAsetcsl(NO_REL) < 0)
		AAsyserr(0, "Zeitweilig koennen keine Datenbanken erzeugt werden");
	exists = 0;
	i = initucode(argc, argv, TRUE, (char *) 0, -1);
	database = Parmvect[0];
	switch (i)
	{
	  case 0:
	  case 5:
		exists = 1;
		break;

	  case 6:
		exists = -1;

	  case 1:
		break;

	  case 2:
		AAsyserr(0, "Sie duerfen auf die Datenbank '%s' nicht zugreifen", database);

	  case 3:
		AAsyserr(0, "Sie sind kein eingetragener DBS-Nutzer");

	  case 4:
		printf("Sie haben keinen Datenbanknamen angegeben\n");
usage:
		/* cat USE_CREATDB */
		AAcat(AAztack(AApath, USE_CREATDB));
term:
		AAunlall();
		fflush(stdout);
		exit(-1);

	  default:
		AAsyserr(22200, AAproc_name, i);
	}

	faterr = 0;
	dbtmpfile = (char *) 0;
	for (av = Flagvect; p = *av; av++)
	{
		if (p[0] != '-' && p[0] != '+')
			AAsyserr(21010, p);
		switch (p[1])
		{
		  case 'F':		/* DBTMPLTFILE */
			if (!p[2])
				goto badflag;
			dbtmpfile = &p[2];
			break;

# ifdef VPGSIZE
		  case '1':		/* BLOCKSIZE */
		  case '2':
		  case '4':
		  case '8':
			i = p[1] - '0';
			if (p[2] || i > PGBLKS)
				goto badflag;
			AApgsize = BLKSIZ * i;
			AAltsize = (PGSIZE - AApgsize) / sizeof (short);
			break;
# endif

		  case 'C':
			p[1] = 'c';
			goto def;

		  case 'E':
			p[1] = 'e';
			goto def;

		  case 'M':
			p[1] = 'm';
			goto def;

		  case 'Q':
			p[1] = 'q';
			/* fall through */

		  default:
def:
			if (flagv = flaglkup(p[1], p[0]))
			{
				*flagv = ((p[0] == '+')? 1: -1);
				continue;
			}
badflag:
			printf("Unbekanntes Flag '%s'\n", p);
			faterr++;
			continue;

		}
		if (p[0] == '+')
			goto badflag;
	}

	/* check for legality of database name */
	if (Parmvect[1])
	{
		printf("Zu viele Argumente\n");
		goto usage;
	}
	if (!check(database))
		AAsyserr(0, "Unerlaubter Datenbankname '%s'", database);

	if (!(AAdbstat & U_CREATDB))
		AAsyserr(0, "Sie duerfen keine Datenbanken erzeugen");

	/* end of input checking */
	if (faterr)
		goto term;

	/* now see if it should have been there */
	if (flagval('m') || flagval('e'))
	{
#		ifdef xSTR3
		AAtTfp(1, 14, "AAdbpath = '%s'\n", AAdbpath);
#		endif
		if (flagval('e') && exists <= 0)
			AAsyserr(0, "Die Datenbank '%s' existiert nicht", database);

		if (chdir(AAdbpath) < 0)
			AAsyserr(0, "Es existiert kein directory fuer die Datenbank '%s'", database);

		if (!flagval('e'))
		{
			/* make certain that it is empty */
# ifdef NON_V7_FS
			open_dir(".");
			while (direc = read_dir())
			{
				/* throw out legitimate files */
				if (AAsequal(direc, ".") || AAsequal(direc, ".."))
					continue;
				AAsyserr(0, "directory '%s' ist nicht leer", database);
			}
			close_dir();
# else
			freopen(".", "r", stdin);
			for (i = 0; i < 16; i++)
				getw(stdin);
			while ((i = getw(stdin)) != EOF)
			{
				if (i)
					AAsyserr(0, "directory '%s' ist nicht leer", database);
				for (i = 0; i < 7; i++)
					getw(stdin);
			}
# endif
		}
		else
		{
			/* check for correct owner */
			AAam_start();
			if (!AAbequal(AAusercode, AAdmin.adhdr.adowner, 2))
				AAsyserr(0, "Sie sind nicht der DBA der Datenbank '%s'", database);
		}
	}
	else
	{
		if (exists)
			AAsyserr(0, "Die Datenbank '%s' existiert bereits", database);

		/* create it */
		fflush(stdout);
		if ((i = fork()) < 0)
			AAsyserr(21013);
		if (!i)
		{
			/* enter database directory */
			initdbpath((char *) 0, adminbuf, FALSE);
			if (chdir(adminbuf) < 0)
				AAsyserr(22201, AAproc_name, adminbuf);

			/* arrange for proper permissions */
			if (setuid(getuid()))
				AAsyserr(21015);
			if (setgid(getgid()))
				AAsyserr(21016);
			fflush(stdout);
			execlp(MKDIR, MKDIR, database, 0);
			AAsyserr(21017, MKDIR);
		}
		while (wait(&code) != -1)
			continue;

		if (chdir(AAdbpath) < 0)
			AAsyserr(22201, AAproc_name, AAdbpath);

		fflush(stdout);
		if ((i = fork()) < 0)
			AAsyserr(21019);
		if (!i)
		{
			setuid(getuid());
			if (chmod(".", 0777))
				AAsyserr(21020);
			exit(0);
		}

		while (wait(&code) != -1)
			continue;
		if (ctou(code))
		{
			fflush(stdout);
			AAunlall();
			exit(code);
		}
	}

	/* reset 'errno', set from possibly bad chdir */
	errno = 0;

	/* Set up locking information. */
	AALOCKREQ.l_db = 0L;
	AAunlcs(NO_REL);
	stat(".", &stbuf);
	AAbmove(&stbuf, &AALOCKREQ.l_db, sizeof (long));
	if (AAsetdbl(A_RTN, M_EXCL) < 0)
		AAsyserr(0, "Datenbank zeitweilig nicht verfuegbar");

	/* determine name of DBTMPLTFILE and open */
	if (!dbtmpfile)
	{
		AAsmove(AAversion, adminbuf);
		for (i = 0; adminbuf[i]; i++)
			if (adminbuf[i] == '/')
				break;
		adminbuf[i] = 0;
		dbtmpfile = AAztack(AAztack(AApath, DBTMPLTFILE), adminbuf);
	}
	if (!freopen(dbtmpfile, "r", stdin))
		AAsyserr(21021);

	readdbtemp();

	/* check for type -- update database status versus create */
	if (flagval('e'))
		changedb();
	else
		makedb();

	/* close the cache descriptors */
#	ifdef xSTR3
	if (AAtTfp(50, 0, "Attdes.reltid = "))
	{
		AAdumptid(&Attdes.reltid);
		printf("Reldes.reltid = ");
		AAdumptid(&Reldes.reltid);
	}
#	endif

	if (i = AArelclose(&Attdes))
		AAsyserr(21022, i);

	if (i = AArelclose(&Reldes))
		AAsyserr(21023, i);

	/* bring tupcount in AA_ADMIN file to date */
	AAbmove(&Reldes.reldum.reltups, &AAdmin.adreld.reldum.reltups,
		sizeof Reldes.reldum.reltups);
	AAbmove(&Attdes.reldum.reltups, &AAdmin.adattd.reldum.reltups, sizeof Attdes.reldum.reltups);

	/* clean up some of the fields to catch problems later */
	AAdmin.adreld.relfp = AAdmin.adattd.relfp = -1;
	AAdmin.adreld.relopn = AAdmin.adattd.relopn = 0;

	if ((admin = creat(AA_ADMIN, FILEMODE)) < 0)
		AAsyserr(21024);
	if (write(admin, &AAdmin, sizeof AAdmin) != sizeof AAdmin)
		AAsyserr(21025);
	close(admin);

	/* exit successfully */
	AAunldb();
	fflush(stdout);
	exit(0);
}


/*
**  Rubout processing.
*/
rubproc()
{
	AAunlall();
	fflush(stdout);
	exit(-2);
}


/*
**  READDBTEMP -- read the DBTMPLTFILE and build internal form
**
**	This routine reads the DBTMPLTFILE (presumably openned as
**	the standard input) and initializes the Dbstat (global database
**	status word) and Rellist variables.
**
**	Rellist is an array of argument vectors, exactly as passed to
**	'create'.
**
**	The syntax of the DBTMPLTFILE is as follows:
**
**	The first line is a single status word (syntax defined below)
**	which is the database status.
**
**	The rest of the file is sets of lines separated by blank lines.
**	Each set of lines define one relation.  Two blank lines in a
**	row or an end-of-file define the end of the file.  Each set
**	of lines is broken down:
**
**	The first line is in the following format:
**		relname:status
**	which defines the relation name and the relation status for
**	this relation (syntax defined in 'getstat' below).  Status
**	may be omitted, in which case a default status is assumed.
**
**	Second through n'th lines of each set define the attributes.
**	They are of the form:
**		attname		format
**	separated by a single tab.  'Format' is the same as on a
**	'create' statement in QL.
**
**	Notice that we force the S_CATALOG bit to be on in any
**	case.  This is because the changedb() routine will fail
**	if the -e flag is ever used on this database if any
**	relation appears to be a user relation.
**
**	Side Effects:
**		Dbstat gets the database status.
**		Rellist is created with a list of the relations,
**			(as parameter vectors -- just as passed to
**			create).  The entry after the last entry
**			has its pv[0] == (char *) 0.
*/
readdbtemp()
{
	static char		buf[MAXDBTEMP];
	register struct reldes	*r;
	register char		**q;
	register int		i;
	register char		*p;
	register int		j;
	register int		defrstat;
	int			bitson;
	int			bitsoff;

	/* read database status */
	defrstat = S_CATALOG | S_NOUPDT /* | S_CONCUR */ | S_PROTALL;
	bitson = bitsoff = 0;
	Dbstat = getstat(A_DBCONCUR, &Dbson, &Dbsoff);
	if (Delim == ':')
		defrstat = getstat(defrstat, &bitson, &bitsoff);
	if (Delim != '\n')
		AAsyserr(21026, Delim);

	/* compute default relation status */

	/* start reading relation info */
	p = buf;
	for (r = Rellist; ; r++)
	{
		r->bitson = bitson;
		r->bitsoff = bitsoff;

		q = r->parmv;

		/* get relation name */
		q[1] = p;
		p += getname(p) + 1;

		/* test for end of DBTMPLTFILE */
		if (!q[1][0])
			break;

		/* get relation status */
		i = getstat(defrstat, &r->bitson, &r->bitsoff);
		i |= S_CATALOG;		/* guarantee system catalog */
		*q++ = p;
		*p++ = ((i >> 15) & 1) + '0';
		for (j = 12; j >= 0; j -= 3)
			*p++ = ((i >> j) & 07) + '0';
		*p++ = 0;
		q++;
		if (Delim != '\n')
			AAsyserr(21027, Delim);

		/* read attribute names and formats */
		for ( ; ; )
		{
			/* get attribute name */
			*q++ = p;
			p += getname(p) + 1;
			if (!q[-1][0])
				break;
			if (Delim != '\t')
				AAsyserr(21028, Delim);

			/* get attribute type */
			*q++ = p;
			p += getname(p) + 1;
			if (Delim != '\n')
				AAsyserr(21029, Delim);
		}

		/* insert end of argv signal */
		*--q = (char *) 0;

		/* ad-hoc overflow test */
		if (p >= &buf[MAXDBTEMP])
			AAsyserr(21030);
	}
	/* mark the end of list */
	q[1] = (char *) 0;
}


/*
**  GETSTAT -- Get status word
**
**	A status word is read from the standard input (presumably
**	DBTMPLTFILE).  The string read is interpreted as an octal
**	number.  The syntax is:
**		N{+c+N[~N]}
**	where N is a number, + is a plus or a minus sign, and c is
**	a flag.  '+c+N1[~N2]' groups are interpreted as follows:
**	If flag 'c' is set (assuming the preceeding character is a +,
**	clear if it is a -), then set (clear) bits N1.  If tilde N2
**	is present, then if flag 'c' is unset (called as '-c' ('+c'))
**	clear (set) bits N2; if ~N2 is not present, clear (set)
**	bits N1.
**
**	For example, an entry might be (but probably isn't):
**		1-c-1+q+6~2
**	having the following meaning:
**
**	1. Default to the 1 bit set.
**
**	2. If the -c flag is specified, clear the '1' bit.  If the
**	+c flag is specified, set the '1' bit.  If it is unspecified,
**	leave the '1' bit alone.
**
**	3. If the +q flag is specified, set the '2' bit and the '4'
**	bit.  If the -q flag is specified, clear the '2' bit (but leave
**	the '4' bit alone).  If the +-q flag is unspecified, leave
**	those bits alone.
**
**	Thus, a database with this entry is initially created with
**	the 1 bit on.  The '4' bit is a history, which says if the
**	'q' flag has ever been set, while the '2' bit tells if it is
**	currently set.
**
**	Parameters:
**		def -- the default to return if there is no number
**			there at all.
**		bitson -- a pointer to a word to contain all the
**			bits to be turned on -- used for the -e flag.
**		bitsoff -- same, for bits turned off.
**
**	Returns:
**		The value of the status word.
**		There are no error returns.
*/
getstat(def, bitson, bitsoff)
int	def;
int	*bitson;
int	*bitsoff;
{
	register int	c;
	register int	st;
	register int	i;
	register int	setbits;
	register int	clrbits;
	register int	ctlch;

	/* reset bits being turned on and off */
	*bitson = *bitsoff = 0;

	/* check to see if a base status word is defined */
	if (Delim == '\n' || (Delim = c = getchar()) < '0' || c > '7')
		st = def;	/* no, use default */
	else
	{
		/* read base status field */
		ungetc(c, stdin);
		st = roctal();
	}

	/* scan '+c+N' entries */
	for ( ; ; )
	{
		/* check for a flag present */
		c = Delim;
		if (c == '\n' || c == ':')
			return (st);
		if (c != '+' && c != '-')
		{
badfmt:
			AAsyserr(21031, c);
		}

		/* we have some flag -- get it's value */
		i = flagval(getchar());

		/* save sign char on flag */
		ctlch = c;

		/* get sign on associated number and the number */
		if ((c = getchar()) != '+' && c != '-')
			goto badfmt;
		setbits = roctal();

		/* test whether action on -X same as on +X */
		if (Delim == '~')
			clrbits = roctal();	/* they have different bits (see above) */
		else
			clrbits = setbits;	/* on 'creatdb -e -X', use opposite bits of +X */

		/* test for any effect at all */
		if (!i)
			continue;

		/* test whether we should process the '+N' */
		if ((ctlch == '-')? (i < 0): (i > 0))
			i = setbits;
		else
		{
			i = clrbits;

			/* switch sense of bit action */
			c = (c == '+')? '-': '+';
		}

		if (c == '+')
		{
			st |= i;
			*bitson |= i;
		}
		else
		{
			st &= ~i;
			*bitsoff |= i;
		}
	}
}


/*
**  ROCTAL -- Read an octal number from standard input.
**
**	This routine just reads a single octal number from the standard
**	input and returns its value.  It will only read up to a non-
**	octal digit.  It will also skip initial and trailing blanks.
**	'Delim' is set to the next character in the input stream.
**
**	Returns:
**		value of octal number in the input stream.
**
**	Side Effects:
**		'Delim' is set to the delimiter which terminated the
**			number.
*/
roctal()
{
	register int	c;
	register int	val;

	val = 0;

	/* skip initial blanks */
	while ((c = getchar()) == ' ')
		continue;

	/* get numeric value */
	while (c >= '0' && c <= '7')
	{
		val = (val << 3) | (c - '0');
		c = getchar();
	}

	/* skip trailing blanks */
	while (c == ' ')
		c = getchar();

	/* set Delim and return numeric value */
	Delim = c;
	return (val);
}


/*
**  GETNAME -- get name from standard input
**
**	This function reads a name from the standard input.  A
**	name is defined as a string of letters and digits.
**
**	The character which caused the scan to terminate is stored
**	into 'Delim'.
**
**	Parameters:
**		ptr -- a pointer to the buffer in which to dump the
**			name.
**
**	Returns:
**		The length of the string.
*/
getname(p)
register char	*p;
{
	register int	len;
	register int	c;

	for (len = 0; (c = getchar()) != EOF; len++)
	{
		/* check for end of name */
		if ((c < '0' || c > '9') && (c < 'a' || c > 'z'))
			break;

		/* store character into buffer */
		*p++ = c;
	}

	/* null-terminate the string */
	*p = '\0';

	/* store the delimiting character and return length of string */
	Delim = c;
	return (len);
}


/*
**  MAKEDB -- make a database from scratch
**
**	This is the code to make a database if the -e flag is off.
**
**	The first step is to make a copy of the AA_ADMIN file
**	in the internal 'AAdmin' struct.  This is the code which
**	subsequently gets used by AArelopen and opencatalog.  Notice
**	that the AA_ADMIN file is not written out; this happens after
**	makedb returns.
**
**	Next, the physical files are created with one initial (empty)
**	page.  This has to happen before the 'create' call so
**	that it will be possible to flush AA_REL and AA_ATT
**	relation pages during the creates of the AA_REL and
**	AA_ATT relations.  Other relations don't need this,
**	but it is more convenient to be symmetric.
**
**	The next step is to create the relations.  Of course, all
**	this really is is inserting stuff into the system catalogs.
**
**	When we are all done we open the AA_REL relation for the
**	admin cache (which of course should exist now).  Thus,
**	the AArelclose's in main (which must be around to update the
**	tuple count) will work right.
**
**	Side Effects:
**		A database is created!!
**		Several files will be created in the current directory,
**			one for each relation mentioned in the
**			DBTMPLTFILE.
**		The 'AAdmin' struct will be filled in.
*/
makedb()
{
	register struct reldes	*r;
# ifdef VPGSIZE
	register int		i;
# endif

#	ifdef xSTR3
	AAtTfp(51, 0, "MAKEDB: AAusercode = %s (0%o)\n", AAusercode, AAusercode);
#	endif

	/* create the physical files */
	for (r = Rellist; r->parmv[1]; r++)
		makefile(r);

	/* initialize the AA_ADMIN file internal cache */
	AAbmove(AAusercode, AAdmin.adhdr.adowner, 2);
# ifdef VPGSIZE
	i = AApgsize / BLKSIZ - 1;
	Dbstat |= i << 8;
# endif
	AAdmin.adhdr.adflags = Dbstat;
	makeadmin(&AAdmin.adreld, Rellist[0].parmv);
	makeadmin(&AAdmin.adattd, Rellist[1].parmv);

	/* done with admin initialization */

	/* initialize relations */
	for (r = Rellist; r->parmv[1]; r++)
		makereln(r);
}


/*
**  MAKEADMIN -- manually initialize descriptor for AA_ADMIN file
**
**	The relation descriptor pointed to by 'pv' is turned into
**	a descriptor, returned in 'd'.  Presumably, this descriptor
**	is later written out to the AA_ADMIN file.
**
**	Notice that the 'reltid' field is filled in sequentially.
**	This means that the relations put into the AA_ADMIN file
**	must be created in the same order that they are 'made'
**	(by this routine), that the format of tid's must not
**	change, and that there can not be over one page worth of
**	relations in the AA_ADMIN file.  Our current system currently
**	handles this easily.
**
**	Parameters:
**		d -- the descriptor to get the result.
**		pv -- a parm vector in 'create' format, which drives
**			this routine.
*/
makeadmin(des, p)
register DESC		*des;
register char		**p;
{
	static int			tid;
	register int			i;
	register int			j;
	short				len;
	char				fname[MAXNAME + 3];

#	ifdef xSTR2
	AAtTfp(10, -1, "creating %s in admin\n", p[1]);
#	endif

	i = AA_oatoi(*p++);
	AAdbname(*p++, AAusercode, fname);
	AAbmove(fname, des->reldum.relid, MAXNAME + 2);
	des->reldum.relstat = i;
	des->reldum.relatts = 0;
	des->reldum.relwid = 0;
	des->reldum.relspec = M_HEAP;
	des->reltid = tid++;
	if ((des->relfp = open(fname, 2)) < 0)
		AAsyserr(21032, fname);
	des->relopn = RELWOPEN(des->relfp);

	/* initialize domain info */
	for ( ; *p++; p++)
	{
		if ((i = typeconv(p[0][0])) < 0)
			AAsyserr(21033, p[0][0]);
		j = ++(des->reldum.relatts);
		des->relfrmt[j] = i;
		if (AA_atoi(&p[0][1], &len))
			AAsyserr(21034, p[0]);
		des->relfrml[j] = len;
		des->reloff[j] = des->reldum.relwid;
		des->reldum.relwid += len;
	}
}


/*
**  MAKEFILE -- make an 'empty' file for a relation
**
**	This routine creates a file with a single (empty) page
**	on it -- it is part of the 'create' code, essentially.
**
**	Parameters:
**		rr -- a pointer to the 'reldes' structure for this
**			relation (file).
**
**	Side Effects:
**		A file with one page is created.
*/
makefile(r)
register struct reldes	*r;
{
	DESC			d;
	long			npages;

	AAdbname(r->parmv[1], AAusercode, d.reldum.relid);

#	ifdef xSTR1
	AAtTfp(12, 0, "creat %s\n", d.reldum.relid);
#	endif

	if ((d.relfp = creat(d.reldum.relid, FILEMODE)) < 0)
		AAsyserr(21035, d.reldum.relid);

	npages = 1;
	if (formatpg(&d, npages))
		AAsyserr(21036);

	close(d.relfp);
}


/*
**  MAKERELN -- make a relation
**
**	This is the second half of the create, started by 'makefile'.
**
**	This routine just sets up argument vectors and calls create,
**	which does the real work.
**
**	Parameters:
**		rr -- a pointer to the Rellist entry for the relation
**			to be created.
**
**	Side Effects:
**		Information will be inserted into the AA_REL and
**			AA_ATT relations.
*/
makereln(r)
register struct reldes	*r;
{
	register int		pc;
	register char		**pv;
	register int		i;

	for (pc = 0, pv = r->parmv; *pv; pv++)
		pc++;
	pv = r->parmv;

#	ifdef xSTR1
	if (AAtTf(1, 0))
		AAprargs(pc, pv);
#	endif

	if (i = create(pc, pv))
		AAsyserr(21037, i);
}


/*
**  CHECK -- check database name syntax
**
**	The name of a database is checked for validity.  A valid
**	database name is not more than 14 characters long, begins
**	with an alphabetic character, and contains only alpha-
**	numerics.  Underscore is considered numeric.
**
**	Parameters:
**		st -- the string to check.
**
**	Returns:
**		TRUE -- ok.
**		FALSE -- failure.
*/
check(p)
register char	*p;
{
	register int	c;

	/* check string length */
	if (AAlength(p) > MAXNAME)
		return (FALSE);

	/* check the first character of the string for alphabetic */
	if (!(((c = *p++) >= 'a'
# ifdef EBCDIC
	   && c <= 'i') || (c >= 'j'
	   && c <= 'r') || (c >= 's'
# endif
	   && c <= 'z')))
		return (FALSE);

	/* check the rest for alphanumeric */
	while (c = *p++)
	{
		if (c == '_' || (c >= '0' && c <= '9') || (c >= 'a'
# ifdef EBCDIC
		   && c <= 'i') || (c >= 'j'
		   && c <= 'r') || (c >= 's'
# endif
		   && c <= 'z'))
			continue;
		return (FALSE);
	}
	return (TRUE);
}


/*
**  FLAGLKUP -- look up user flag
**
**	This routine helps support a variety of user flags.  The
**	routine takes a given user flag and looks it up (via a
**	very crude linear search) in the 'Flags' vector, and
**	returns a pointer to the value.
**
**	The 'flag' struct defines the flags.  The 'flagname' field
**	is the character which is the flag id, for example, 'c'
**	in the flag '-c'.  The 'flagtype' field defines how the
**	flag may appear; if negative, only '-c' may appear, if
**	positive, only '+c' may appear; if zero, either form may
**	appear.  Finally, the 'flagval' field is the value of the
**	flag -- it may default any way the user wishes.
**
**	Parameters:
**		flagname -- the name (as defined above) of the
**			flag to be looked up.
**		plusminus -- a character, '+' means the '+x' form
**			was issued, '-' means the '-x' form was
**			issued, something else means *don't care*.
**			If an illegal form was issued (that is,
**			that does not match the 'flagtype' field
**			in the structure), the "not found" return
**			is taken.
**
**	Returns:
**		(int *) 0 -- flag not found, or was incorrect type,
**			as when the '+x' form is specified in the
**			parameters, but the 'Flags' struct says
**			that only a '-x' form may appear.
**		else -- pointer to the 'flagval' field of the correct
**			field in the 'Flags' vector.
*/
struct flag
{
	char	flagname;	/* the name of the flag */
	char	flagtype;	/* -1: -x form; +1: +x form; 0: both */
	short	flagval;	/* user-defined value of the flag */
};

struct flag	Flags[] =
{
	'q',	0,	0,
	'l',	0,	0,
	'c',	0,	0,
	'e',	-1,	0,
	'm',	-1,	0,
	0
};

short	*flaglkup(f, pm)
register int	f;
register int	pm;
{
	register struct flag	*p;

	/* look up flag in vector */
	for (p = Flags; p->flagname != f; p++)
		if (!p->flagname)
			return ((short *) 0);

	/* found in list -- check type */
	if ((pm == '+' && ctoi(p->flagtype) < 0) || (pm == '-' && ctoi(p->flagtype) > 0))
		return ((short *) 0);

	/* type is OK -- return pointer to value */
	return (&p->flagval);
}


/*
**  FLAGVAL -- return value of a flag
**
**	Similar to 'flaglkup', except that the value is returned
**	instead of the address, and no error return can occur.
**
**	Parameters:
**		f -- the flag to look up (see flaglkup).
**
**	Returns:
**		The value of flag 'f'.
*/
flagval(f)
register int	f;
{
	register short	*p;

	/* get value of flag */
	p = flaglkup(f, 0);

	/* test for error return, AAsyserr if so */
	if (!p)
		AAsyserr(21038, f);

	/* return value */
	return (*p);
}


/*
**  CHANGEDB -- change status bits for database/relations
**
**	In this function we change the status bits for use with the
**	-e flag.
**
**	This module always uses the differential status
**	change information, so that existing bits are not touched.
**
**	We check to see that invalid updates, such as turning off
**	query modification when it is already on, can not occur.
**	This is because of potential AAsyserr's when the system is
**	later run, e.g., because of views without instantiations.
**
**	In the second step, the database status is updated.  This is
**	done strictly in-core, and will be updated in the database
**	after we return.
**
**	The list of valid relations are then scanned.  For each
**	relation listed, a series of steps occurs:
**
**	(1) The relation is checked for existance.  If it does not
**	exist, it is created, and we return to the beginning of the
**	loop.  Notice that we don't have to change modes in this
**	case, since it already has been done.
**
**	(2) If the relation does exist, we check to see that it
**	is a system catalog.  If it is not, we have an error, since
**	this is a user relation which just happenned to have the
**	same name.  We inform the user and give up.
**
**	(3) If the relation exists, is a system catalog, and all
**	that, then we check the changes we need to make in the
**	bits.  If no change need be made, we continue the loop;
**	otherwise, we change the bits and replace the tuple in
**	the AA_REL relation.
**
**	(4) If the relation being updated was the AA_REL or
**	AA_ATT relation, we change the AAdmin struct accordingly.
**
**	Notice that the result of all this is that all relations
**	which might ever be used exist and have the correct status.
**
**	Notice that it is fatal for either the AA_ATT or AA_REL
**	relations to not exist, since the file is created at the
**	same time that relation descriptors are filled in.  This
**	should not be a problem, since this is only called on an
**	existing database.
**
**	As a final note, we open the AA_ATT relation cache not
**	because we use it, but because we want to do a AArelclose
**	in main() to insure that the tupcount is updated in all
**	cases.
**
**	Side Effects:
**		The database is brought up to date, as described
**			above.
**		Tuples may be added or changed in system catalogs.
**		Files may be created.
*/
changedb()
{
	register struct reldes	*r;
	register int		i;
	struct tup_id		tid;
	struct relation		relk;
	struct relation		relt;

#	ifdef xSTR1
	AAtTfp(40, 0, "CHANGEDB: Dbson=0%o, Dbsoff=0%o\n", Dbson, Dbsoff);
#	endif

	/* check to see we aren't doing anything illegal */
	if (flagval('q') < 0)
		AAsyserr(0, " Flag '-q' ist nicht erlaubt");

	/* update the database status field */
	AAdmin.adhdr.adflags = (AAdmin.adhdr.adflags | Dbson) & ~Dbsoff;

	/* open the system catalog caches */
	opencatalog(AA_REL, 2);
	opencatalog(AA_ATT, 0);

	/* scan the relation list:- Rellist */
	for (r = Rellist; r->parmv[1]; r++)
	{
		/* see if this relation exists */
		AAam_clearkeys(&Reldes);
		AAam_setkey(&Reldes, &relk, r->parmv[1], RELID);
		if ((i = AAgetequal(&Reldes, &relk, &relt, &tid)) < 0)
			AAsyserr(21040);

		if (i > 0)
		{
			/* doesn't exist, create it */
			printf("Erzeuge Katalog '%s'\n", r->parmv[1]);
			makefile(r);
			makereln(r);
		}
		else
		{
			/* exists -- check to make sure it is the right one */
			if (!(relt.relstat & S_CATALOG))
			{
				/* exists as a user reln -- tough luck buster */
				AAsyserr(0, "Katalog '%s' existiert bereits als Nutzerrelation", r->parmv[1]);
			}

			/* it exists and is the right one -- update status */
			if (!r->bitson && !r->bitsoff)
				continue;

			/* actual work need be done */
			relt.relstat = (relt.relstat | r->bitson) & ~r->bitsoff;

			/* replace tuple in AA_REL relation */
			if (i = AAam_replace(&Reldes, &tid, &relt, FALSE))
				AAsyserr(21041, i);

			/* update AAdmin struct if AA_REL or AA_ATT */
			if (AAsequal(r->parmv[1], AA_REL))
				AAdmin.adreld.reldum.relstat = relt.relstat;
			else if (AAsequal(r->parmv[1], AA_ATT))
				AAdmin.adattd.reldum.relstat = relt.relstat;
		}
	}
}


/*
**  READADMIN -- read the AA_ADMIN file into the 'AAdmin' cache
**
**	This routine opens and reads the 'AAdmin' cache from the
**	AA_ADMIN file in the current directory.
**
**	This version of the routine is modified for creatdb --
**	the '-e' flag is checked, and nothing is performed
**	unless it is set.
**
**	If not set, the AA_REL and AA_ATT relations
**	are opened, and the descriptors for them in the AAdmin
**	struct are filled in with their file descriptors.
**
**	Side Effects:
**		The 'AAdmin' struct is filled in.
**		The AA_REL and AA_ATT files are
**			opened.
*/
AAreadadmin()
{
	register int		i;
	char			relname[MAXNAME + 4];

	/* read the stuff from the AA_ADMIN file */
	if (flagval('e'))
	{
		if ((i = open(AA_ADMIN, 0)) < 0)
			AAsyserr(21042, i);
		if (read(i, &AAdmin, sizeof AAdmin) != sizeof AAdmin)
			AAsyserr(21043);
		close(i);

		/* open the physical files for AA_REL and AA_ATT */
		AAdbname(AAdmin.adreld.reldum.relid, AAdmin.adreld.reldum.relowner, relname);
		if ((AAdmin.adreld.relfp = open(relname, 2)) < 0)
			AAsyserr(21044, AAdmin.adreld.relfp);
		AAdbname(AAdmin.adattd.reldum.relid, AAdmin.adattd.reldum.relowner, relname);
		if ((AAdmin.adattd.relfp = open(relname, 2)) < 0)
			AAsyserr(21045, AAdmin.adattd.relfp);
		AAdmin.adreld.relopn = RELWOPEN(AAdmin.adreld.relfp);
		AAdmin.adattd.relopn = RELROPEN(AAdmin.adattd.relfp);
		/* get the page size of data base */
		if ((i = (AAdmin.adhdr.adflags >> 8) + 1) > PGBLKS)
			AAsyserr(10010, i, PGBLKS);
# ifdef VPGSIZE
		AApgsize = BLKSIZ * i;
		/* get the linetab size, the difference */
		AAltsize = (PGSIZE - AApgsize) / sizeof (short);
# endif
	}

	return (0);
}
