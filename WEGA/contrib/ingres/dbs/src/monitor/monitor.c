# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"monitor.h"

int	Autoclear;	/* clear query buffer automatically if set  */
int	Notnull;	/* set if the query is not null		    */
int	Peekch;		/* lookahead character for getch	    */
int	Escape = ESCAPE;

/*
**  COMMAND TABLE
**	To add synonyms for commands, add entries to this table
*/
struct cntrlwd
{
	char	*name;
	int	code;
};

static struct cntrlwd	Controlwords[] =
{
# ifdef ESER
	"A",		C_APPEND,
	"B",		C_BRANCH,
	"D",		C_TIME,
	"E",		C_EDIT,
	"G",		C_GO,
	"H",		C_S_HELP,
	"H",		C_L_HELP,
	"I",		C_INCLUDE,
	"K",		C_MARK,
	"L",		C_LIST,
	"M",		C_MONITOR,
	"P",		C_PRINT,
	"Q",		C_QUIT,
	"R",		C_RESET,
	"S",		C_SHELL,
	"V",		C_EVAL,
	"W",		C_WRITE,
# endif
	"a",		C_APPEND,
	"b",		C_BRANCH,
	"d",		C_TIME,
	"e",		C_EDIT,
	"g",		C_GO,
	"h",		C_S_HELP,
	"H",		C_L_HELP,
	"i",		C_INCLUDE,
	"k",		C_MARK,
	"l",		C_LIST,
	"m",		C_MONITOR,
	"p",		C_PRINT,
	"q",		C_QUIT,
	"r",		C_RESET,
	"s",		C_SHELL,
	"v",		C_EVAL,
	"w",		C_WRITE,
# ifdef ESER
	"APPEND",	C_APPEND,
	"BRANCH",	C_BRANCH,
	"CD",		C_CHDIR,
	"CHDIR",	C_CHDIR,
	"DATE",		C_TIME,
	"ED",		C_EDIT,
	"EDIT",		C_EDIT,
	"EDITOR",	C_EDIT,
	"EVAL",		C_EVAL,
	"GO",		C_GO,
	"HELP",		C_S_HELP,
	"HELP",		C_L_HELP,
	"INCLUDE",	C_INCLUDE,
	"LIST",		C_LIST,
	"MARK",		C_MARK,
	"MONITOR",	C_MONITOR,
	"PRINT",	C_PRINT,
	"QUIT",		C_QUIT,
	"READ",		C_INCLUDE,
	"RESET",	C_RESET,
	"SH",		C_SHELL,
	"SHELL",	C_SHELL,
	"TIME",		C_TIME,
	"WRITE",	C_WRITE,
# endif
	"append",	C_APPEND,
	"branch",	C_BRANCH,
	"cd",		C_CHDIR,
	"chdir",	C_CHDIR,
	"date",		C_TIME,
	"ed",		C_EDIT,
	"edit",		C_EDIT,
	"editor",	C_EDIT,
	"eval",		C_EVAL,
	"go",		C_GO,
	"help",		C_S_HELP,
	"HELP",		C_L_HELP,
	"include",	C_INCLUDE,
	"list",		C_LIST,
	"mark",		C_MARK,
	"monitor",	C_MONITOR,
	"print",	C_PRINT,
	"quit",		C_QUIT,
	"read",		C_INCLUDE,
	"reset",	C_RESET,
	"sh",		C_SHELL,
	"shell",	C_SHELL,
	"time",		C_TIME,
	"write",	C_WRITE,
	(char *) 0
};

/*
**  MONITOR
**
**	This routine maintains the logical query buffer in
**	QRYBUFxxxx.  It in general just does a copy from input
**	to query buffer, unless it gets a backslash escape character.
**	It recognizes the following escapes:
**
**	\a -- force append mode (no autoclear)
**	\b -- branch (within an include file only)
**	\cd-- change working directory
**	\d -- print current time
**	\e -- enter editor
**	\g -- "GO": submit query to DBS
**	\h -- short help
**	\H --  long help
**	\i -- include (switch input to external file)
**	\k -- mark (for \b)
**	\l -- list: print query buffer after macro evaluation
**	\m -- set monitor command character
**	\p -- print query buffer (before macro evaluation)
**	\q -- quit DBS
**	\r -- force reset (clear) of query buffer
**	\s -- call shell
**	\v -- evaluate macros, but throw away result (for side effects)
**	\w -- write query buffer to external file
**	\\ -- produce a single backslash in query buffer
*/
command()
{
	register int		chr;
	register int		controlno;
	register char		*p;
	long			timevec;
	extern char		*getfilename();
	extern long		time();
	extern char		*ctime();

	while (chr = getch())
	{
		if (chr == Escape)
		{
			/* process control sequence */
			if (!(controlno = getescape(TRUE)))
				continue;

			switch (controlno)
			{
			  case C_S_HELP:
			  case C_L_HELP:
				shorthelp(controlno == C_S_HELP);
				cgprompt();
				continue;

			  case C_EDIT:
				edit();
				continue;

			  case C_LIST:
# ifdef MACRO
				eval(TRUE);
				continue;
# endif

			  case C_EVAL:
# ifdef MACRO
				eval(FALSE);
				Autoclear = TRUE;
				continue;
# endif

			  case C_PRINT:
				print(TRUE);
				continue;

			  case C_INCLUDE:
				include((char *) 0);
				cgprompt();
				continue;

			  case C_WRITE:
				writeout();
				cgprompt();
				continue;

			  case C_CHDIR:
				newdirec();
				cgprompt();
				continue;

			  case C_RESET:
				clear(TRUE);
				continue;

			  case C_GO:
				go();
				continue;

			  case C_QUIT:
				clrline(TRUE);
				quit();

			  case C_SHELL:
				shell();
				continue;

			  case C_TIME:
				time(&timevec);
# ifdef ESER_VMX
				printf("%s", ctime(timevec));
# else
				printf("%s", ctime(&timevec));
# endif
				clrline(FALSE);
				continue;

			  case C_APPEND:
				Autoclear = 0;
				clrline(FALSE);
				continue;

			  case C_MARK:
				getfilename();
				prompt((char *) 0);
				continue;

			  case C_MONITOR:
				p = getfilename();
				Escape = *p? *p: ESCAPE;
				prompt((char *) 0);
				continue;

			  case C_BRANCH:
				branch();
				prompt((char *) 0);
				continue;

			  default:
				AAsyserr(16020, controlno);
			}
		}
		putch(chr);
	}
	if (Input == stdin)
	{
		if (Nodayfile >= 0)
			putchar('\n');
	}
	else
		fclose(Input);
}


getescape(copy)
register int	copy;
{
	register struct cntrlwd		*cw;
	register char			*word;
	extern char			*getname();

	word = getname();
	for (cw = Controlwords; cw->name; cw++)
		if (AAsequal(cw->name, word))
			return (cw->code);

	/* not found -- pass symbol through and return failure */
	if (!copy)
		return (0);
	putch(Escape);
	while (*word)
	{
		putch(*word);
		word++;
	}
	return (0);
}


char	*getname()
{
	static char	buf[FILENAME + 1];
	register char	*p;
	register int	len;
	register int	c;

	for (p = buf, len = 0; len < FILENAME; len++)
		if (((c = getch()) >= 'a'
# ifdef EBCDIC
		   && c <= 'i') || (c >= 'j'
		   && c <= 'r') || (c >= 's'
# endif
		   && c <= 'z') || (c >= 'A'
# ifdef EBCDIC
		   && c <= 'I') || (c >= 'J'
		   && c <= 'R') || (c >= 'S'
# endif
		   && c <= 'Z'))
			*p++ = c;
		else
		{
			Peekch = c;
			break;
		}

	*p = 0;
	return (buf);
}


putch(c)
register int	c;
{
	Prompt = Newline = (c == '\n');
	if (c < ' ' && c != '\n' && c != '\t')
	{
		printf("%sZeichen '0%o' konvertiert in Blank\n", ERROR, c);
		c = ' ';
	}
	prompt((char *) 0);
	if (Autoclear)
		clear(FALSE);
	putc(c, Qryiop);
	Notnull++;
}
