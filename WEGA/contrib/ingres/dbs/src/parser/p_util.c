# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/tree.h"
# include	"parser.h"
# include	"../h/catalog.h"
# include	"../h/symbol.h"

/*
**  P_UTIL -- parser utility functions
**
**	These functions are generally unrelated except that they are
**	needed to operate the parser and are too small to be considered
**	seperate modules.
**
**	Defines:
**		timeofday	-- convert arguments to minutes since midnight
**		tlprepend	-- attach two target list components
**		header		-- prints the header for a retrieve to terminal
**		patmat		-- converts pattern matching characters in a string
**		permcom		-- adds a command to the permit command vector
**
**
**
**  TIMEOFDAY -- convert 2 integers to minutes since midnight
**
**	Converts the hours and minutes parameters to minutes since midnight
**	performing some error (bounds) checking on the time.
**
**	To answer the question about what is midnight, both 0:00 and 24:00
**	are handled, but not the same way.  The former is zero minutes from
**	midnight and the latter is 1440 minutes from midnight.  (1440 is
**	24 hrs times 60 minutes, or 1 minute past the end of the day.)
**
**	Parameters:
**		hrs		-- an integer pointer to the hour
**		mins		-- an integer pointer to the minutes
**
**	Returns:
**		integer time since midnight
**
**	Side Effects:
**		may detect an error and call yyerror which never returns.
*/
timeofday(hrs, mins)
int	*hrs;
int	*mins;
{
	register int	h;
	register int	m;
	register int	rtval;

	h = *hrs;
	m = *mins;
	if (h > 24 || h < 0)
		/* no such hour */
		yyerror(BADHOURS, AA_iocv(h), (char *) 0);
	if (m > 59 || h < 0)
		/* no such minute */
		yyerror(BADMINS, AA_iocv(m), (char *) 0);
	if (h == 24)
	{
		h = 1440;
		if (m != 0)
			/* can only use 24:00 */
			yyerror(BAD24TIME, AA_iocv(m), (char *) 0);
	}
	rtval = (h * 60) + m;
	return (rtval);
}


/*
**  TLPREPEND -- combine two target list components
**
**	Attach two target list components to each other.
**	Neither component need be a single element.  The
**	'a' component will be attached at the extreme left
**	of the 'b' component.
**
**	Parameters:
**		a		-- tl component to attach
**		b		-- tl base for attaching
**
**	Side Effects:
**		this routine is a side effect.  It attaches a to b
**		and when it returns a is attached to b but the pointer
**		to b never changes (neither does the pointer to a)
*/
QTREE	*tlprepend(a, b)
QTREE	*a;
QTREE	*b;
{
	register QTREE			*q;

	/* scan to the left end of b */
	for (q = b; q->left; q = q->left)
		continue;	/* no action */

	/* attach a to the end of b */
	q->left = a;
	return (b);
}


/*
**  HEADER -- print header for retrieve to terminal
**
**	header scans the parameter list managed with "initp" and
**	"setp" to reconstruct the field names and types and passing
**	them to the normal AAprhdr etc.
*/
header()
{
	register int	i;
	short		j;

	AAbgnhdr();
	for (i = 0; i < Pc; i += 2)
	{
		AA_atoi(&Pv[i + 1][1], &j);
		AAprhdr(ctou(Pv[i + 1][0]), j, Pv[i]);
	}
	AApreol();
	AAendhdr();
	initp();
}


/*
**  PATMAT -- converts pattern matching characters in a string
**
**	Searches a string up to a null byte for one of the pattern
**	matching characters '*', '?', '[', and ']'. It then converts
**	these characters to their internal control character equivalents.
**
**	Parameters:
**		str		-- the string to search
**
**	Returns:
**		0		-- no pattern matching in string
**		1		-- at least one pattern matching character
*/
patmat(str)
char	*str;
{
	register char	*p;
	register char	*q;
	register int	flag;

	flag = 0;
	q = str;
	for (p = str; *p; p++)
	{
		if (*p == '\\')
		{
			*q++ = *++p;
			continue;
		}
		switch (*p)
		{
		  case '*':
			*q++ = PAT_ANY;
			flag = 1;
			continue;

		  case '?':
			*q++ = PAT_ONE;
			flag = 1;
			continue;

		  case '[':
			*q++ = PAT_LBRAC;
			flag = 1;
			continue;

		  case ']':
			*q++ = PAT_RBRAC;
			flag = 1;
			continue;
		}
		*q++ = *p;
	}
	*q = '\0';
	return (flag);
}


/*
**  PERMCOM -- map command allowed into AA_PROTECT catalog bits
**
**	translates the QMODE type symbols into the appropriate counterparts
**	for the permit statement allowed command vector.  The manifest
**	constants are designed to be inclusive or'd together to form a
**	composite bit map of OK actions.
**
**	Parameters:
**		a		-- the QMODE type symbol for the command to add
**
**	Side Effects:
**		changes the variable Permcomd to reflect the additional permission
*/
permcom(a)
register int	a;
{
	switch (a)
	{
	  case mdRETR:
		Permcomd |= PRO_RETR;
		break;

	  case mdAPP:
		Permcomd |= PRO_APP;
		break;

	  case mdREPL:
		Permcomd |= PRO_REPL;
		break;

	  case mdDEL:
		Permcomd |= PRO_DEL;
		break;

	  case -1:
		Permcomd |= 0177777;		/* all bits set */
		break;

	  default:
		AAsyserr(18018, a);
	}
}
