# include	<stdio.h>
# include	"../conf/gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/bs.h"

/*
**  Initialize Users File From Passwd File
**
**	Everyone in AA_PASSWD is entered into the AA_USERS.  All
**	users can access all databases.
**
**	User codes are assigned sequentially.  This should therefore
**	probably be run once only, when DBS is first installed.
**	Otherwise, usercodes can change mysteriously.
**
**	The optional parameter replaces the root of the DBS subtree
**	as found in AA_PASSWD.  The DBS user must be installed
**	(with that name) when usersetup is run.  If this parameter
**	is a minus ("-"), output goes to the standard output.
**
**	The initialization file is initialized to "<home>/.ql",
**	where <home> is the home directory in the AA_PASSWD.
*/

char	*AApath;
char	AA_USERS[]	= "/sys/etc/users";
# ifdef ESER_PSU
char	AA_PASSWD[]	= "/SYS/PASSWD";
# else
char	AA_PASSWD[]	= "/etc/passwd";
# endif

main(argc, argv)
int	argc;
char	**argv;
{
	register int		i;
	char			buf[MAXLINE + 1];
	char			*pathname;
	register char		*code;
	char			*field[UF_NFIELDS];
# ifdef SETBUF
	char			iobuf[BUFSIZ];
# endif
	register FILE		*iop;
	extern char		*AAproc_name;

	AAproc_name = "USERSETUP";
	pathname = (char *) 0;

	code = "aa";
	if ((iop = fopen(AA_PASSWD, "r")) == (FILE *) 0)
		AAsyserr(0, "/ETC/PASSWD nicht lesbar");
# ifdef SETBUF
	setbuf(iop, iobuf);
# endif

	/* scan for DBS in AA_PASSWD */
	while (fgetline(buf, MAXLINE, iop))
	{
		i = decode(buf, field);
		if (!AAsequal(USERDBS, field[PF_NAME]))
			continue;

		/* check for enough fields for valid entry */
		if (i < PF_HOME)
			AAsyserr(0, "Zu wenig Felder in dem Eintrag \"%s\"", buf);

		pathname = field[PF_HOME];

		break;
	}

	/* test for DBS entry found */
	if (!pathname)
		AAsyserr(0, "%s ist kein eingetragener BS-Nutzer", USERDBS);

	/* get override pathname */
	if (argc > 1)
		pathname = argv[1];
	AApath = pathname;

	/* rewind AA_PASSWD */
	if (fclose(iop))
		AAsyserr(22137);
	if ((iop = fopen(AA_PASSWD, "r")) == (FILE *) 0)
		AAsyserr(22138);
# ifdef SETBUF
	setbuf(iop, iobuf);
# endif

	/* open output file as needed */
	if (pathname[0] != '-')
	{
		AAconcat(pathname, AA_USERS, buf);
		if ((i = open(buf, 0)) >= 0)
			AAsyserr(0, "Das Nutzerverzeichnis '%s' existiert bereits", buf);
		if ((i = creat(buf, 0644)) < 0)
			AAsyserr(22140, buf);
		close(i);
		if (freopen(buf, "w", stdout) == (FILE *) 0)
			AAsyserr(22141, buf);
	}

	while (fgetline(buf, MAXLINE, iop))
	{
		i = decode(buf, field);
		/* check for enough fields for valid entry */
		if (i < PF_HOME)
		{
			fprintf(stderr, "Zu wenig Felder in dem Eintrag \"%s\"\n", buf);
			continue;
		}
		/* print username & code */
		printf("%s:%s:%s:%s:%s:::%s/.ql::\n",
			field[PF_NAME],		/* user name */
			code,
			field[PF_UID],		/* user id */
			field[PF_GID],		/* user group */
			AAsequal(field[PF_NAME], USERDBS)? "177777": "000000",
			field[PF_HOME]);	/* working directory */
		next(code);
	}
	flush();
	sync();
}


decode(buf, field)
char	*buf;
char	*field[];
{
	register char	*cp;
	register int	c;
	register int	i;

	field[PF_NAME] = buf;
	for (i = 0, cp = buf; (c = *cp) != '\0'; cp++)
	{
		if (c == ':')
		{
			*cp = '\0';
			field[++i] = cp + 1;
		}
	}

	return (i);
}




next(code)
char	code[2];
{
	register char	*c;
	register int	a;
	register int	b;

	c = code;
	a = c[0];
	b = c[1];

	if (++b == 'z' + 1)
		b = '0';
# ifdef EBCDIC
	else if (b == 'i' + 1)
		b = 'j';
	else if (b == 'r' + 1)
		b = 's';
# endif
	else if (b == '9' + 1)
	{
		b = 'a';
		if (a++ == 'Z')
		{
			write(2, "Zu viele Nutzer\n", 16);
			exit(-1);
		}
		if (a == 'z' + 1)
			a = 'A';
# ifdef EBCDIC
		else if (a == 'i' + 1)
			a = 'j';
		else if (a == 'r' + 1)
			a = 's';
		else if (a == 'I' + 1)
			a = 'J';
		else if (a == 'R' + 1)
			a = 'S';
# endif
	}

	c[0] = a;
	c[1] = b;
}


AAunlall()
{
}
