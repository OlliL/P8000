# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"buf.h"			/* headers for buffer manip */

extern char	*AA_iocv();
extern char	*macplkup();

# define	ANYDELIM	'\020'		/* \| -- zero or more delims */
# define	ONEDELIM	'\021'		/* \^ -- exactly one delim */
# define	CHANGE		'\022'		/* \& -- token change */

# define	PARAMN		'\023'		/* $ -- non-preprocessed param */
# define	PARAMP		'\024'		/* $$ -- preprocessed param */

# define	PRESCANENABLE	'@'		/* character to enable prescan */
# define	LBRACE		'{'		/* left brace */
# define	RBRACE		'}'		/* right brace */
# define	BACKSLASH	'\\'		/* backslash */
# define	LQUOTE		'`'		/* left quote */
# define	RQUOTE		'\''		/* right quote */
# define	SPACE		' '
# define	TAB		'\t'
# define	NEWLINE		'\n'

# define	QUOTED		0200		/* pass right through bit */
# define	CHARMASK	0177		/* character part */

# define	ITERTHRESH	100		/* iteration limit */
# define	NPRIMS		(sizeof Macprims / sizeof Macprims[0])

/* token modes, used to compute token changes */
# define	NONE		0		/* guarantees a token change */
# define	ID		1		/* identifier */
# define	NUMBER		2		/* number (int or float) */
# define	DELIM		3		/* delimiter, guarantees a token change */
# define	QUOTEMODE	4		/* quoted construct */
# define	OP		5		/* operator */
# define	NOCHANGE	6		/* guarantees no token change */

/* macro definitions */
struct macro
{
	struct macro	*nextm;		/* pointer to next macro header */
	char		*template;	/* pointer to macro template */
	char		*substitute;	/* pointer to substitution text */
};

/* primitive declarations */
static struct macro	Macprims[]	= {
&Macprims[1],	"{define;\020\024t;\020\024s}",				(char *) 1,
&Macprims[2],	"{rawdefine;\020\024t;\020\024s}",			(char *) 2,
&Macprims[3],	"{remove;\020\024t}",					(char *) 3,
&Macprims[4],	"{dump}",						(char *) 4,
&Macprims[5],	"{type;\020\024m}",					(char *) 5,
&Macprims[6],	"{read;\020\024m}",					(char *) 6,
&Macprims[7],	"{readdefine;\020\024n;\020\024m}",			(char *) 7,
&Macprims[8],	"{ifsame;\020\024a;\020\024b;\020\023t;\020\023f}",	(char *) 8,
&Macprims[9],	"{ifeq;\020\024a;\020\024b;\020\023t;\020\023f}",	(char *) 9,
&Macprims[10],	"{ifgt;\020\024a;\020\024b;\020\023t;\020\023f}",	(char *) 10,
&Macprims[11],	"{same;\020\024a;\020\024b}",				(char *) 11,
&Macprims[12],	"{if;\020\024e;\020\023t;\020\023f}",			(char *) 12,
&Macprims[13],	"{expr;\020\024e}",					(char *) 13,
&Macprims[14],	"{exprdefine;\020\024n;\020\024e}",			(char *) 14,
&Macprims[15],	"{substr;\020\024f;\020\024t;\024s}",			(char *) 15,
&Macprims[16],	"{dnl}",						(char *) 16,
&Macprims[17],	"{remove}",						(char *) 3,
0,		"{dump;\020\024n}",					(char *) 4,
};

static struct macro	*Machead	= &Macprims[0];	/* head of macro list */

/* parameters */
struct param
{
	struct param	*nextp;
	int		mode;
	int		name;
	char		*paramt;
};

/* the environment */
struct env
{
	struct env	*nexte;		/* next environment */
	char		(*rawget)();	/* raw character get routine */
	char		**rawpar;	/* a parameter to that routine */
	int		prevchar;	/* previous character read */
	int		tokenmode;	/* current token mode */
	int		change;		/* token change flag */
	int		eof;		/* eof flag */
	int		newline;	/* set if bol */
	int		rawnewline;	/* same for raw input */
	struct buf	*pbuf;		/* peek buffer */
	struct buf	*mbuf;		/* macro buffer */
	int		endtrap;	/* endtrap flag */
	int		pass;		/* pass flag */
	int		pdelim;		/* current parameter delimiter */
	struct param	*params;	/* parameter list */
	int		itercount;	/* iteration count */
	int		quotelevel;	/* quote nesting level */
};

/* current environment pointer */
static struct env	*Macenv;

/*
**  MACRO PROCESSOR
**
**
**
**  MACINIT -- initialize for macro processing
**
**	*** EXTERNAL INTERFACE ***
**
**	The macro processor is initialized.  Any crap left over from
**	previous processing (which will never occur normally, but may
**	happen on an interrupt, for instance) will be cleaned up.  The
**	raw input is defined, and the 'endtrap' parameter tells whether
**	this is "primary" processing or not; in other words, it tells
**	whether to spring BEGINTRAP and ENDTRAP.
**
**	This routine must always be called prior to any processing.
*/
macinit(rawget, rawpar, endtrap)
char	(*rawget)();
char	**rawpar;
int	endtrap;
{
	static struct env	env;
	register struct env	*e;
	register struct env	*f;

	/* clear out old crap */
	for (e = Macenv; e; e = f)
	{
		bufpurge(&e->mbuf);
		bufpurge(&e->pbuf);
		macpflush(e);
		if (f = e->nexte)
			buffree(e);
	}

	/* set up the primary environment */
	Macenv = e = &env;
	bufclear(e, sizeof *e);

	e->rawget = rawget;
	e->rawpar = rawpar;
	e->endtrap = endtrap;
	e->newline = TRUE;

	if (endtrap)
		macspring(BEGINTRAP);
}


/*
**  MACGETCH -- get character after macro processing
**
**	*** EXTERNAL INTERFACE ROUTINE ***
**
**	The macro processor must have been previously initialized by a
**	call to macinit().
*/
macgetch()
{
	register struct env	*e;
	register int		c;

	e = Macenv;
	for ( ; ; )
	{
		/* get an input character and
		** check for end-of-file processing */
		if (!(c = macgch()))
		{
			/* check to see if we should spring ENDTRAP */
			if (e->endtrap)
			{
				e->endtrap = FALSE;
				macspring(ENDTRAP);
				continue;
			}

			/* don't spring endtrap -- real end of file */
			return (0);
		}

		/* not an end of file -- check for pass character through */
		if (e->pass)
		{
			e->pass = FALSE;
			e->change = FALSE;
		}
		if (c & QUOTED || !e->change || e->tokenmode == DELIM)
		{
			/* the character is to be passed through */
			/* reset iteration count and purge macro buffer */
			e->itercount = 0;
			bufflush(&e->mbuf);
			e->newline = (c == NEWLINE);
			return (c & CHARMASK);
		}

		/* this character is a candidate for macro processing */
		macunget(FALSE);
		bufflush(&e->mbuf);

		/* check for infinite loop */
		if (e->itercount > ITERTHRESH)
		{
			printf("Unendlicher Zyklus in der Makroexpansion\n");
			fflush(stdout);
			e->pass = TRUE;
			continue;
		}

		/* see if we have a macro match */
		if (macallscan())
			/* yep -- count iterations and rescan it */
			e->itercount++;
		else
			/* nope -- pass the next token through raw */
			e->pass = TRUE;
	}
}


/*
**  MACGCH -- get input character, knowing about tokens
**
**	The next input character is returned.  In addition, the quote
**	level info is maintained and the QUOTED bit is set if the
**	returned character is (a) quoted or (b) backslash escaped.
**	As a side effect the change flag is maintained.  Also, the
**	character is saved in mbuf.
*/
macgch()
{
	register int		c;
	register struct env	*e;

	e = Macenv;

	for ( ; ; )
	{
		/* get virtual raw character, save in mbuf, and set change
		** and test for magic frotz */
		switch (c = macfetch(e->quotelevel > 0))
		{
		  case 0:	/* end of file */
			return (0);

		  case LQUOTE:
			if (!e->quotelevel++)
				continue;
			break;

		  case RQUOTE:
			if (!e->quotelevel)
				return (c);
			if (!--e->quotelevel)
				continue;
			break;

		  case BACKSLASH:
			if (e->quotelevel > 0)
				break;
			/* handle special cases */
			if ((c = macfetch(TRUE)) == e->pdelim)
				break;
			/* do translations */
			switch (c)
			{
			  case SPACE:	/* space */
			  case TAB:	/* tab */
			  case NEWLINE:	/* newline */
			  case RQUOTE:
			  case LQUOTE:
			  case '$':
			  case LBRACE:
			  case RBRACE:
			  case BACKSLASH:
				break;

			  default:
				/* take character as is (unquoted) */
				c = 0;
				break;
			}
			if (c)
				break;
			/* not an escapable character -- treat it normally */
			macunget(TRUE);
			c = BACKSLASH;
			/* do default character processing on backslash */

		  default:
			if (e->quotelevel > 0)
				break;
			return (c);
		}

		/* the character is quoted */
		return (c | QUOTED);
	}
}


/*
**  MACFETCH -- fetch virtual raw character
**
**	A character is fetched from the peek buffer.  If that buffer is
**	empty, it is fetched from the raw input.  The character is then
**	saved away, and the change flag is set accordingly.
**	The QUOTED bit on the character is set if the 'quote' flag
**	parameter is set; used for backslash escapes.
**	Note that the QUOTED bit appears only on the character which
**	goes into the macro buffer; the character returned is normal.
*/
macfetch(quote)
int	quote;
{
	register struct env	*e;
	register int		c;
	register int		escapech;

	e = Macenv;
	escapech = 0;

	for ( ; ; )
	{
		/* get character from peek buffer */
		if (!(c = bufget(&e->pbuf)))
		{
			/* peek buffer is empty */
			/* check for already raw eof */
			if (!e->eof)
			{
				/* note that c must be int so that the QUOTED bit is not negative */
				if ((c = (*e->rawget)(e->rawpar)) <= 0)
				{
					c = 0;
					e->eof = TRUE;
				}
				else
				{
					if (e->rawnewline)
						e->prevchar = NEWLINE;
					e->rawnewline = (c == NEWLINE);
				}
			}
		}

		/* test for escapable character */
		if (escapech)
		{
			switch (c)
			{
			  case 't':	/* become quoted tab */
				c = TAB | QUOTED;
				break;

			  case 'n':	/* become quoted newline */
				c = NEWLINE | QUOTED;
				break;

			  default:
				bufput(c, &e->pbuf);
				c = BACKSLASH;
			}
			escapech = 0;
		}
		else
			if (c == BACKSLASH)
			{
				escapech++;
				continue;
			}
		break;
	}

	/* quote the character if appropriate to mask change flag */
	/* ('escapech' now becomes the maybe quoted character) */
	escapech = c;
	if (quote && c)
		escapech |= QUOTED;

	/* set change flag */
	macschng(escapech);

	if (c)	/* save the character in the macro buffer */
		bufput(escapech, &e->mbuf);

	return (c);
}


/*
**  MACSCHNG -- set change flag and compute token type
**
**	The change flag and token type is set.  This does some tricky
**	stuff to determine just when a new token begins.  Most notably,
**	notice that quoted stuff IS scanned, but the change flag is
**	reset in a higher level routine so that quoted stuff looks
**	like a single token, but any begin/end quote causes a token
**	change.
*/
macschng(c)
register int	c;
{
	register struct env	*e;
	register int		thismode;
	register int		changeflag;

	changeflag = 0;
	thismode = macmode(c);

	switch ((e = Macenv)->tokenmode)
	{
	  case NONE:
		/* always cause token change */
		break;

	  case QUOTEMODE:
		/* change only on initial entry to quotes */
		break;

	  case DELIM:
		changeflag++;
		break;

	  case ID:
		/* take any sequence of letters and numerals */
		if (thismode == NUMBER)
			thismode = ID;
		break;

	  case NUMBER:
		/* take string of digits and decimal points */
		if (c == '.')
			thismode = NUMBER;
		break;

	  case OP:
		switch (e->prevchar)
		{
		  case '<':
		  case '>':
		  case '!':
			if (c != '=')
				changeflag++;
			break;

		  case '*':
			if (c != '*' && c != '/')
				changeflag++;
			break;

		  case '/':
			if (c != '*')
				changeflag++;
			break;

		  case '.':
			if (thismode == NUMBER)
				e->tokenmode = thismode;
			break;

		  default:
			changeflag++;
			break;
		}
		break;

	  case NOCHANGE:	/* never cause token change */
		e->tokenmode = thismode;
		break;
	}

	e->prevchar = c;
	if (thismode != e->tokenmode)
		changeflag++;
	e->tokenmode = thismode;
	e->change = changeflag;
}


/*
**  MACMODE -- return mode of a character
*/
macmode(c)
register int	c;
{
	if (c & QUOTED)
		return (QUOTEMODE);
/*?EBCDIC?*/	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_'))
		return (ID);
	if (c >= '0' && c <= '9')
		return (NUMBER);
	if (c == SPACE || c == TAB || c == NEWLINE)
		return (DELIM);
	return (OP);
}


/*
**  MACALLSCAN -- scan to see if input matches a macro
**
**	Returns true if there was a match, false if not.  In any case,
**	the virtual raw input (i.e., the peek buffer) will contain
**	either the old raw input, or the substituted macro.
*/
macallscan()
{
	register struct macro	*m;

	for (m = Machead; m; m = m->nextm)
	{
		/* check to see if it matches this macro */
		if (macscan(m))
		{
			/* it does -- substituted value is in mbuf */
			macrescan();
			return (TRUE);
		}

		/* it doesn't match this macro -- try the next one */
		macrescan();
	}

	/* it doesn't match any of them -- tough luck */
	return (FALSE);
}


/*
**  MACSCAN -- scan a single macro for a match
**
**	As is scans it also collects parameters for possible future
**	substitution.  If it finds a match, it takes responsibility
**	for doing the substitution.
*/
macscan(m)
register struct macro	*m;
{
	register int		c;
	register char		*temp;
	register int		pname;
	register int		pdelim;

	/* check for anchored mode */
	if (*(temp = m->template) == ONEDELIM)
	{
		if (!Macenv->newline)
			return (FALSE);
		temp++;
	}

	/* scan the template */
	for ( ; c = *temp; temp++)
	{
		if (c == PARAMN || c == PARAMP)
		{
			/* we have a parameter */
			pname = *++temp;
			pdelim = *++temp;
			if (macparam(c, pname, pdelim))	/* parameter ok */
				continue;

			/* failure on parameter scan */
			return (FALSE);
		}

		if (!macmatch(c))	/* failure on literal match */
			return (FALSE);
	}

	/* it matches!!  substitute the macro */
	macsubs(m);
	return (TRUE);
}


/*
**  MACPARAM -- collect a parameter
**
**	The parameter is collected and stored away "somewhere" with
**	name 'name'.  The delimiter is taken to be 'delim'.  'Mode'
**	tells whether to prescan the parameter (done immediately before
**	substitute time to avoid side effects if the macro actually
**	turns out to not match).
*/
macparam(mode, name, delim)
int	mode;
int	name;
int	delim;
{
	register int		c;
	register struct env	*e;
	register struct param	*p;
	struct buf		*b;
	register int		bracecount;
	extern struct buf	*bufalloc();
	extern char		*bufcrunch();

	b = (struct buf *) 0;

	(e = Macenv)->pdelim = delim;
# ifdef xMTRM
	AAtTfp(70, 1, "MACPARAM(0%o, %c, %c)\n", mode, name, delim);
# endif
	if (mode == PARAMP)
	{
		/* check for REALLY prescan */
		if ((c = macgch()) != PRESCANENABLE)
		{
			mode = PARAMN;
			macunget(FALSE);
		}
	}

	bracecount = 0;
	e->tokenmode = NOCHANGE;
	while (!macmatch(delim))
		do
		{
			if (!(c = macgch()) || c == NEWLINE)
			{
				e->pdelim = 0;
				bufpurge(&b);
# ifdef xMTRM
				AAtTfp(70, 1, "MACPARAM: fails\n");
# endif
				return (FALSE);
			}
			bufput(c, &b);
			if (c == LBRACE)
				bracecount++;
			else if (c == RBRACE && bracecount > 0)
				bracecount--;
		} while (bracecount > 0);

	e->pdelim = 0;

	/* allocate and store the parameter */
	p = (struct param *) bufalloc(sizeof *p);
	p->mode = mode;
	p->name = name;
	p->nextp = e->params;
	e->params = p;
	p->paramt = bufcrunch(&b);
	bufpurge(&b);
# ifdef xMTRM
	AAtTfp(70, 1, "MACPARAM: |%s|\n", p->paramt);
# endif

	return (TRUE);
}


/*
**  MACMATCH -- test for a match between template character and input.
**
**	The parameter is the character from the template to match on.
**	The input is read.  The template character may be a meta-
**	character.  In all cases if the match occurs the input is
**	thrown away; if no match occurs the input is left unchanged.
**
**	Return value is true for a match, false for no match.
*/
macmatch(t)
register int	t;
{
	register int	c;

# ifdef xMTRM
	AAtTfp(70, 1, "MACMATCH(%c)\n", t);
# endif

	switch (t)
	{
	  case ANYDELIM:	/* match zero or more delimiters */
		/* chew and chuck delimiters */
		while (macdelim())
			continue;

		/* as a side effect, must match a token change */
		if (!macckch())
		{
# ifdef xMTRM
			AAtTfp(70, 1, "MACMATCH: fail\n");
# endif
			return (FALSE);
		}
# ifdef xMTRM
		AAtTfp(70, 1, "MACMATCH: succeed\n");
# endif
		return (TRUE);

	  case ONEDELIM:	/* match exactly one delimiter */
		return (macdelim());

	  case CHANGE:		/* match a token change */
	  case 0:		/* end of template */
		return (macckch());

	  default:		/* must have exact character match */
		c = macgch();
# ifdef xMTRM
		AAtTfp(70, 1, "MACMATCH: against %c\n", c);
# endif
		if (c == t)
		{
# ifdef xMTRM
			AAtTfp(70, 1, "MACMATCH: succeed\n");
# endif
			return (TRUE);
		}

		/* failure */
		macunget(FALSE);
# ifdef xMTRM
		AAtTfp(70, 1, "MACMATCH: fail\n");
# endif
		return (FALSE);
	}
}


/*
**  MACDELIM -- test for next input character a delimiter
**
**	Returns true if the next input character is a delimiter, false
**	otherwise.  Delimiters are chewed.
*/
macdelim()
{
	register int	c;

	c = macgch();
# ifdef xMTRM
	AAtTfp(70, 1, "MACDELIM: against %c\n", c);
# endif
	if (macmode(c) == DELIM)
	{
# ifdef xMTRM
		AAtTfp(70, 1, "MACDELIM: succeed\n");
# endif
		return (TRUE);
	}
	macunget(FALSE);
# ifdef xMTRM
	AAtTfp(70, 1, "MACDELIM: fail\n");
# endif
	return (FALSE);
}


/*
**  MACCKCH -- check for token change
**
**	Returns true if a token change occurs between this and the next
**	character.  No characters are ever chewed, however, the token
**	change (if it exists) is always chewed.
*/
macckch()
{
	register int		change;
# ifdef xMTRM
	register int		c;
# endif
	register struct env	*e;

	if ((e = Macenv)->tokenmode == NONE)
		/* then last character has been ungotten: take old change */
		change = e->change;
	else
	{
# ifdef xMTRM
		c = 
# endif
		    macgch();
		change = e->change;
		macunget(FALSE);
	}
# ifdef xMTRM
	AAtTfp(70, 1, "MACCKCH: got %c ret %d\n", c, change);
# endif

	/* chew the change and return */
	e->tokenmode = NOCHANGE;
	return (change);
}


/*
**  MACSUBS -- substitute in macro substitution
**
**	This routine prescans appropriate parameters and then either
**	loads the substitution into the macro buffer or calls the
**	correct primitive routine.
*/
macsubs(mac)
struct macro	*mac;
{
	register struct param	*p;
	register struct env	*e;
	register char		*s;
	extern char		*macprim();

	for (p = (e = Macenv)->params; p; p = p->nextp)
	{
		/* check to see if we should prescan */
		if (p->mode != PARAMP)
			continue;

		/* prescan parameter */
		macprescan(&p->paramt);
		p->mode = PARAMN;
	}

	s = mac->substitute;

	/* clear out the macro call */
	bufflush(&e->mbuf);

	if (s <= (char *) NPRIMS)	/* it is a primitive */
		macload(macprim((int) s));
	else	/* it is a user-defined macro */
		macload(s);
}


/*
**  MACPRESCAN -- prescan a parameter
**
**	The parameter pointed to by 'pp' is fed once through the macro
**	processor and replaced with the new version.
*/
macprescan(pp)
register char	**pp;
{
	register int		c;
	char			*p;
	struct buf		*b;
	extern int		macsget();
	extern char		*bufcrunch();

	b = (struct buf *) 0;
	p = *pp;

	/* set up a new environment */
	macnewev(macsget, &p);

	/* scan the parameter */
	while (c = macgetch())
		bufput(c, &b);

	/* free the old parameter */
	buffree(*pp);

	/* move in the new one */
	*pp = bufcrunch(&b);
	bufpurge(&b);

	/* restore the old environment */
	macpopev();
}


/*
**  MACNEWEV -- set up new environment
**
**	Parameters are raw get routine and parameter
*/
macnewev(rawget, rawpar)
char		(*rawget)();
register char	**rawpar;
{
	register struct env	*e;
	extern struct buf	*bufalloc();

	e = (struct env *) bufalloc(sizeof *e);
	e->rawget = rawget;
	e->rawpar = rawpar;
	e->nexte = Macenv;
	e->newline = TRUE;
	Macenv = e;
}


/*
**  MACPOPEV -- pop an environment
**
**	Makes sure all buffers and stuff are purged
*/
macpopev()
{
	register struct env	*e;

	bufpurge(&(e = Macenv)->mbuf);
	bufpurge(&e->pbuf);
	macpflush(e);
	Macenv = e->nexte;
	buffree(e);
}


/*
**  MACPFLUSH -- flush all parameters
**
**	Used to deallocate all parameters in a given environment.
*/
macpflush(e)
register struct env	*e;
{
	register struct param	*p;
	register struct param	*q;

	for (p = e->params; p; p = q)
	{
		buffree(p->paramt);
		q = p->nextp;
		buffree(p);
	}

	e->params = (struct param *) 0;
}


/*
**  MACSGET -- get from string
**
**	Works like a getchar from a string.  Used by macprescan().
**	The parameter is a pointer to the string.
*/
macsget(p)
register char	**p;
{
	register int	c;

	if (c = ctou(**p))
		(*p)++;
	return (c);
}


/*
**  MACLOAD -- load a string into the macro buffer
**
**	The parameters are a pointer to a string to be appended to
**	the macro buffer and a flag telling whether parameter substi-
**	tution can occur.
*/
macload(s)
register char	*s;
{
	register struct env	*e;
	register int		c;

	if (!s)
		return;

	e = Macenv;

	while (c = *s++)
		if (c == PARAMN)
			macload(macplkup(*s++));
		else
			bufput(c & CHARMASK, &e->mbuf);
}


/*
**  MACRESCAN -- rescan the macro buffer
**
**	Copies the macro buffer into the peek buffer so that it will be
**	reread.  Also deallocates any parameters which may happen to be
**	stored away.
*/
macrescan()
{
	register struct env	*e;
	register int		c;

	e = Macenv;

	while ((c = bufget(&e->mbuf) & CHARMASK))
		bufput(c, &e->pbuf);

	e->quotelevel = 0;
	e->tokenmode = NONE;
	macpflush(e);
}


/*
**  MACUNGET -- unget a character
**
**	Moves one character from the macro buffer to the peek buffer.
**	If 'mask' is set, the character has the quote bit stripped off.
*/
macunget(mask)
register int	mask;
{
	register struct env	*e;
	register int		c;

	if ((e = Macenv)->prevchar)
	{
		c = bufget(&e->mbuf);
		if (mask)
			c &= CHARMASK;
		bufput(c, &e->pbuf);
		e->tokenmode = NONE;
	}
}


/*
**  MACPLKUP -- look up parameter
**
**	Returns a pointer to the named parameter.  Returns null
**	if the parameter is not found ("cannot happen").
*/
char	*macplkup(name)
register int	name;
{
	register struct param	*p;

	for (p = Macenv->params; p; p = p->nextp)
		if (p->name == name)
			return (p->paramt);

	return ((char *) 0);
}


/*
**  MACSPRING -- spring a trap
**
**	The named trap is sprung, in other words, if the named macro is
**	defined it is called, otherwise there is no replacement text.
*/
macspring(trap)
register char	*trap;
{
	register struct env	*e;
	extern char		*macro();

	bufflush(&(e = Macenv)->mbuf);

	/* fetch the macro */
	trap = macro(trap);

	/* if not defined, don't bother */
	if (!trap)
		return;

	/* load the trap */
	macload(trap);

	/* insert a newline after the trap */
	bufput('\n', &e->mbuf);

	macrescan();
}


/*
**  MACPRIM -- do primitives
**
**	The parameter is the primitive to execute.
*/
char	*macprim(n)
register int	n;
{
	register char		*c;
	extern char		*bufcrunch();
	extern char		*macsstr();

	switch (n)
	{
	  case 1:	/* {define; $t; $s} */
	  case 2:	/* {rawdefine; $t; $s} */
		macdnl();
		macdefine(macplkup('t'), macplkup('s'), n == 2);
		break;

	  case 3:	/* {remove} */
			/* {remove; $t} */
		macdnl();
		macremove(macplkup('t'));
		break;

	  case 4:	/* {dump} */
			/* {dump; $n} */
		macdnl();
		macdump(macplkup('n'));
		break;

	  case 5:	/* {type; $m} */
		macdnl();
		c = macplkup('m');
		while (*c &= CHARMASK)
		{
			putchar(*c);
			c++;
		}
		putchar('\n');
		break;

	  case 6:	/* {read; $m} */
	  case 7:	/* {readdefine; $n; $m} */
		printf("%s ", macplkup('m'));
		fflush(stdout);
		macread();
		if (n == 6)
			break;

		macdefine(macplkup('n'), bufcrunch(&Macenv->mbuf), TRUE);
		return (READCOUNT);

	  case 8:	/* {ifsame; $a; $b; $t; $f} */
		return (macplkup(AAsequal(macplkup('a'), macplkup('b'))? 't': 'f'));

	  case 9:	/* {ifeq; $a; $b; $t; $f} */
		return (macplkup(macexpr(macplkup('a')) == macexpr(macplkup('b'))? 't': 'f'));

	  case 10:	/* {ifgt; $a; $b; $t; $f} */
		return (macplkup(macexpr(macplkup('a')) > macexpr(macplkup('b'))? 't': 'f'));

	  case 11:	/* {same; $a; $b} */
		return (AA_iocv(AAscompare(macplkup('a'), 0, macplkup('b'), 0)));

	  case 12:	/* {if; $e; $t; $f} */
		return (macplkup((macexpr(macplkup('e')) > 0)? 't': 'f'));

	  case 13:	/* {expr; $e} */
		return (AA_iocv(macexpr(macplkup('e'))));

	  case 14:	/* {exprdefine; $n; $e} */
		macdnl();
		macdefine(macplkup('n'), AA_iocv(macexpr(macplkup('e'))), FALSE);
		break;

	  case 15:	/* {substr; $f; $t; $s} */
		return (macsstr(macexpr(macplkup('f')), macexpr(macplkup('t')), macplkup('s')));

	  case 16:	/* {dnl} */
		macdnl();
		break;

	  default:
		AAsyserr(16013, n);
	}

	return ("");
}


/*
**  MACDNL -- delete to newline
**
**	Used in general after macro definitions to avoid embarrassing
**	newlines.  Just reads input until a newline character, and
**	then throws it away.
*/
macdnl()
{
	register int		c;

	while ((c = macgch()) && c != NEWLINE)
		continue;

	bufflush(&Macenv->mbuf);
}


/*
**  MACDEFINE -- define primitive
**
**	This function defines a macro.  The parameters are the
**	template, the substitution string, and a flag telling whether
**	this is a raw define or not.  Syntax checking is done.
*/
macdefine(template, subs, raw)
char	*template;
char	*subs;
int	raw;
{
	register struct env	*e;
	register int		c;
	register struct macro	*m;
	char			*p;
	struct buf		*b;
	char			paramdefined[128];
	extern int		macsget();
	extern struct buf	*bufalloc();
	extern char		*bufcrunch();
	extern char		*mactcvt();

# ifdef xMTRM
	AAtTfp(70, 0, "MACDEFINE: T='%s' S='%s'\n", template, subs);
# endif

	b = (struct buf *) 0;

	/* remove any old macro definition */
	macremove(template);

	/* get a new environment */
	macnewev(macsget, &p);

	/* undefine all parameters */
	bufclear(paramdefined, 128);

	/* avoid an initial token change */
	(e = Macenv)->tokenmode = NOCHANGE;

	/* allocate macro header and template */
	m = (struct macro *) bufalloc(sizeof *m);

	/* scan and convert template, collect available parameters */
	p = template;
	m->template = mactcvt(raw, paramdefined);
	if (!m->template)
	{
		/* some sort of syntax error */
		buffree(m);
		macpopev();
		return;
	}

	bufflush(&e->mbuf);
	bufflush(&e->pbuf);
	e->eof = FALSE;

	/* scan substitute string */
	for (p = subs; c = macfetch(FALSE); )
	{
		if (c != '$')
		{
			/* substitute non-parameters literally */
			bufput(c & CHARMASK, &b);
			continue;
		}

		/* it's a parameter */
		bufput(PARAMN, &b);

		/* check to see if name is supplied */
		if (!paramdefined[c = macfetch(FALSE)])
		{
			/* nope, it's not */
			printf("%sDer Parameter '%c' ist nicht definiert\n", ERROR, c);
			fflush(stdout);
			buffree(m->template);
			buffree(m);
			macpopev();
			bufpurge(&b);
			return;
		}
		bufput(c & CHARMASK, &b);
	}

	/* allocate substitution string */
	m->substitute = bufcrunch(&b);

	/* allocate it as a macro */
	m->nextm = Machead;
	Machead = m;

	/* finished... */
	macpopev();
	bufpurge(&b);
}


/*
**  MACTCVT -- convert template to internal form
**
**	Converts the template from external form to internal form.
**
**	Parameters:
**	raw -- set if only raw type conversion should take place.
**	paramdefined -- a map of flags to determine declaration of
**		parameters, etc.  If zero, no parameters are allowed.
**
**	Return value:
**	A character pointer off into mystic space.
**
**	The characters of the template are read using macfetch, so
**	a new environment should be created which will arrange to
**	get this.
*/
char	*mactcvt(raw, paramdefined)
int	raw;
char	paramdefined[128];
{
	register int		c;
	register int		escapech;
	register int		d;
	register char		*p;
	struct buf		*b;
	extern char		*bufcrunch();

	b = (struct buf *) 0;
	escapech = 1;

	while (c = macfetch(FALSE))
	{
		switch (c)
		{
		  case '$':		/* parameter */
			if (escapech < 0)
			{
				printf("%sJeder Parameter benoetigt ein Trennzeichen\n", ERROR);
				fflush(stdout);
				bufpurge(&b);
				return ((char *) 0);
			}

			/* skip delimiters before parameter in non-raw */
			if (Macenv->change && !escapech && !raw)
				bufput(ANYDELIM, &b);

			escapech = 0;
			d = PARAMN;
			if ((c = macfetch(FALSE)) == '$')
			{
				/* prescanned parameter */
				d = PARAMP;
				c = macfetch(FALSE);
			}

			/* process parameter name */
			if (!c)
			{
				/* no parameter name */
				printf("%sParameter ohne Name\n", ERROR);
				fflush(stdout);
				bufpurge(&b);
				return ((char *) 0);
			}

			bufput(d, &b);
			escapech = -1;

			/* check for legal parameter */
			if (!paramdefined)
				break;

			if (paramdefined[c])
			{
				printf("%sParameter '%c' redefiniert\n", ERROR, c);
				fflush(stdout);
				bufpurge(&b);
				return ((char *) 0);
			}
			paramdefined[c]++;

			/* get parameter delimiter */
			break;

		  case BACKSLASH:		/* a backslash escape */
			escapech = 1;
			switch (c = macfetch(FALSE))
			{
			  case '|':
				c = ANYDELIM;
				break;

			  case '^':
				c = ONEDELIM;
				break;

			  case '&':
				c = CHANGE;
				break;

			  default:
				escapech = 0;
				c = BACKSLASH;
				macunget(FALSE);
				break;
			}
			break;

		  case NEWLINE | QUOTED:
		  case TAB | QUOTED:
		  case SPACE | QUOTED:
			if (escapech < 0)
				c &= CHARMASK;
			escapech = 1;
			break;

		  default:
			/* change delimiters to ANYDELIM */
			if (macmode(c) == DELIM && !raw)
			{
				while (macmode(c = macfetch(FALSE)) == DELIM)
					continue;
				macunget(FALSE);
				c = c? ANYDELIM: ONEDELIM;
				escapech = 1;
			}
			else
			{
				if (Macenv->change && !escapech)
					bufput(ANYDELIM, &b);

				if (escapech < 0)
					/* parameter: don't allow quoted delimiters */
					c &= CHARMASK;
				escapech = 0;
			}
			break;
		}
		bufput(c, &b);
	}
	if (escapech <= 0)
		bufput(CHANGE, &b);

	p = bufcrunch(&b);
	bufpurge(&b);
# ifdef xMTRM
	AAtTfp(70, 1, "MACTCVT: '%s'\n", p);
# endif
	return (p);
}


/*
**  MACREMOVE -- remove macro
**
**	The named macro is looked up.  If it is found it is removed
**	from the macro list.
*/
macremove(name)
register char	*name;
{
	register struct macro	*m;
	register struct macro	**mp;
	register char		*cname;
	char			*p;
	extern int		macsget();
	extern char		*mactcvt();
	extern struct macro	*macmlkup();

# ifdef xMTRM
	AAtTfp(70, 0, "MACREMOVE: '%s'\n", name);
# endif

	cname = (char *) 0;
	if (name)
	{
		/* convert name to internal format */
		macnewev(macsget, &p);
		p = name;
		cname = mactcvt(FALSE, (char *) 0);
		macpopev();
		if (!cname)	/* some sort of syntax error */
			return;
	}

	/* find macro */
	while (name? ((m = macmlkup(cname)) != (struct macro *) 0): ((m = Machead)->substitute > ((char *) NPRIMS)))
	{
		/* remove macro from list */
		mp = &Machead;

		/* find it's parent */
		while (*mp != m)
			mp = &(*mp)->nextm;

		/* remove macro from list */
		*mp = m->nextm;
		buffree(m->template);
		buffree(m->substitute);
		buffree(m);
	}
	if (cname)
		buffree(cname);
}


/*
**  MACMLKUP -- look up macro
**
**	The named macro is looked up and a pointer to the macro header
**	is returned.  Zero is returned if the macro is not found.
**	The name must be in internal form.
*/
struct macro	*macmlkup(n)
register char	*n;
{
	register struct macro	*m;

	/* scan the macro list for it */
	for (m = Machead; m; m = m->nextm)
		if (macmmatch(n, m->template, FALSE))
			return (m);
	return ((struct macro *) 0);
}


/*
**  MACMMATCH -- check for macro name match
**
**	The given 'name' and 'temp' are compared for equality.  If they
**	match true is returned, else false.
**	Both must be converted to internal format before the call is
**	given.
**
**	"Match" is defined as two macros which might scan as equal.
**
**	'Flag' is set to indicate that the macros must match exactly,
**	that is, neither may have any parameters and must end with both
**	at end-of-template.  This mode is used for getting traps and
**	such.
*/
macmmatch(name, temp, flag)
char	*name;
char	*temp;
int	flag;
{
	register int	ac;
	register int	bc;
	char		*ap;
	char		*bp;

	ap = name;
	bp = temp;

	/* scan character by character */
	for ( ; ; ap++, bp++)
	{
		ac = *ap;
		bc = *bp;
# ifdef xMTRM
		AAtTfp(70, 1, "MACMMATCH: ac=%c/0%o, bc=%c/0%o\n", ac, ap, bc, bp);
# endif

		if (bc == ANYDELIM)
		{
			if (macmmchew(&ap))
				continue;
		}
		else
			switch (ac)
			{
			  case SPACE:
			  case NEWLINE:
			  case TAB:
				if (ac == bc || bc == ONEDELIM)
					continue;
				break;

			  case ONEDELIM:
				if (ac == bc || macmode(bc) == DELIM)
					continue;
				break;

			  case ANYDELIM:
				if (macmmchew(&bp))
					continue;
				break;

			  case PARAMP:
			  case PARAMN:
			  case 0:
				if (bc == PARAMN || bc == PARAMP || !bc ||
				    bc == ANYDELIM || bc == ONEDELIM ||
				    bc == CHANGE || macmode(bc) == DELIM)
					if (!flag || (!ac && !bc))	/* success */
						return (TRUE);
				break;

			  default:
				if (ac == bc)
					continue;
				break;
			}

		/* failure */
		return (FALSE);
	}
}


/*
**  MACMMCHEW -- chew nonspecific match characters
**
**	The pointer passed as parameter is scanned so as to skip over
**	delimiters and pseudocharacters.
**	At least one character must match.
*/
macmmchew(pp)
register char	**pp;
{
	register char	*p;
	register int	c;
	register int	matchflag;

	p = *pp;

	for (matchflag = 0; ; matchflag++)
	{
		if ((c = *p) != ANYDELIM && c != ONEDELIM && c != CHANGE &&  macmode(c) != DELIM)
			break;
		p++;
	}

	p--;
	if (!matchflag)
		return (FALSE);
	*pp = p;
	return (TRUE);
}


/*
**  MACREAD -- read a terminal input line
**
**	Reads one line from the user.  Returns the line into mbuf,
**	and a count of the number of characters read into the macro
**	READCOUNT (-1 for end of file).
*/
macread()
{
	register struct env	*e;
	register int		count;
	register int		c;

	e = Macenv;
	count = -1;

	while ((c = fgetchar()) > 0)
	{
		count++;
		if (c == NEWLINE)
			break;
		bufput(c, &e->mbuf);
	}

	macdefine(READCOUNT, AA_iocv(count), TRUE);
}
/*
**  macexpr -- return converted expr
**
*/
macexpr(p)
char	*p;
{
	extern int	macsget();

	return (expr(macsget, &p));
}


/*
**  MACSUBSTR -- substring primitive
**
**	The substring of 'string' from 'from' to 'to' is extracted.
**	A pointer to the result is returned.  Note that macsstr
**	in the general case modifies 'string' in place.
*/
char	*macsstr(from, to, string)
register int	from;
register int	to;
register char	*string;
{
	if (from < 1)
		from = 1;
	if (from > to || from > AAlength(string))
		return ("");
	if (to < AAlength(string))
		string[to] = 0;
	return (&string[--from]);
}


/*
**  MACDUMP -- dump a macro definition to the terminal
**
**	All macros matching 'name' are output to the buffer.  If
**	'name' is the null pointer, all macros are printed.
*/
macdump(n)
register char	*n;
{
	register struct macro	*m;
	char			*ptr;
	extern int		macsget();
	extern char		*mactcvt();
	extern char		*macmocv();

	if (n)
	{
		macnewev(macsget, &ptr);
		ptr = n;
		n = mactcvt(FALSE, (char *) 0);
		macpopev();
		if (!n)
			return;
	}

	for (m = Machead; m; m = m->nextm)
		if (!n || macmmatch(n, m->template, FALSE))
		{
			if (m->substitute <= (char *) NPRIMS)
				continue;
			printf("{rawdefine; %s;", macmocv(m->template));
			printf(" %s}\n", macmocv(m->substitute));
			fflush(stdout);
		}
	if (n)
		buffree(n);
}


/*
**  MACMOCV -- macro output conversion
**
**	This routine converts the internal format of the named macro
**	to an unambigous external representation.
**
**	Note that much work can be done to this routine to make it
**	produce cleaner output, for example, translate "\|" to " "
**	in most cases.
*/
char	*macmocv(m)
char	*m;
{
	register char		*p;
	register int		c;
	register int		pc;
	struct buf		*b;
	static char		*lastbuf;
	extern char		*bufcrunch();

	/* release last used buffer (as appropriate) */
	if (lastbuf)
	{
		buffree(lastbuf);
		lastbuf = (char *) 0;
	}

	if ((p = m) <= (char *) NPRIMS)
	{
		/* we have a primitive */
		p = "Primitive xxx";
		AA_itoa((int) m, &p[10]);
		return (p);
	}

	b = (struct buf *) 0;
	pc = 0;

	for ( ; c = *p++; pc = c)
	{
		switch (c)
		{
		  case BACKSLASH:
		  case '|':
		  case '&':
		  case '^':
			break;

		  case ANYDELIM:
			c = '\\' | ('|' << 8);
			break;

		  case ONEDELIM:
			c = '\\' | ('^' << 8);
			break;

		  case CHANGE:
			c = '\\' | ('&' << 8);
			break;

		  case PARAMN:
			c = '$';
			break;

		  case PARAMP:
			c = '$' | ('$' << 8);
			break;

		  case '$':
			c = '\\' | ('$' << 8);
			break;

		  case NEWLINE:
			c = ('\\' | QUOTED) | ('\n' << 8);
			break;

		  default:
			bufput(c, &b);
			continue;
		}

		if (pc == BACKSLASH)
			bufput(pc, &b);
		bufput(pc = c & CHARMASK, &b);
		if (pc = (c >> 8) & CHARMASK)
			bufput(c = pc, &b);
	}

	p = bufcrunch(&b);
	bufpurge(&b);
	lastbuf = p;
	return (p);
}


/*
**  MACRO -- get macro substitution value
**
**	***  EXTERNAL INTERFACE  ***
**
**	This routine handles the rather specialized case of looking
**	up a macro and returning the substitution value.  The name
**	must match EXACTLY, character for character.
**
**	The null pointer is returned if the macro is not defined.
*/
char	*macro(name)
char	*name;
{
	register struct macro	*m;
	register char		*n;
	char			*p;
	extern int		macsget();
	extern char		*mactcvt();

	/* convert macro name to internal format */
	p = name;
	macnewev(macsget, &p);
	n = mactcvt(FALSE, (char *) 0);
	macpopev();
	if (!n)	/* some sort of syntax error */
		return ((char *) 0);

	for (m = Machead; m; m = m->nextm)
		if (macmmatch(n, m->template, TRUE))
		{
			buffree(n);
			return (m->substitute);
		}

	buffree(n);
	return ((char *) 0);
}
