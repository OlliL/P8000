# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/symbol.h"
# include	"../h/tree.h"
# include	"../h/pipes.h"
# include	"ovqp.h"

/*
**	This file contains the string
**	manipulation routines
*/


extern char	*Ovqpbuf;


/*
**	Concat takes the two character strings in
**	s1 and s2 and concatenates them together
**	into a new location.
**
**	trailing blanks are removed from the first symbol.
**	The size of the concatenation equals the sum of
**	the two original strings.
*/
concatsym(s1, s2)
register SYMBOL		*s1;
register SYMBOL		*s2;
{
	register char		*p;
	int			size1;
	int			size2;
	int			i;
	char			*px;
	char			*ps;
	extern char		*AAbmove();
	extern char		*need();

	size1 = size(s1);	/* compute size without trailing blanks */
	if (!size1 && s1->len)
		size1++;	/* don't allow string to be trunc to zero length */
	size2 = ctou(s2->len);	/* size of second string remains the same */
	i = ctou(s1->len) + size2;	/* i equals sum of original sizes */
	if (i > MAXFIELD)
		i = MAXFIELD;	/* a string can't exceed this size */
	if (size2 + size1 > MAXFIELD)
		size2 = MAXFIELD - size1;	/* adjust size2 to not exceed MAXFIELD */

	px = p = need(Ovqpbuf, i);	/* request the needed space */
	ps = (char *) s1->value;
	p = AAbmove(((STRINGP *) ps)->stringp, p, size1);	/* copy first string */
	((STRINGP *) ps)->stringp = px;
	ps = (char *) s2->value;
	p = AAbmove(((STRINGP *) ps)->stringp, p, size2);
	s1->len = i;
	/* pad with blanks if necessary */
	i -= size1 - size2;
	while (i--)
		*p++ = ' ';

#	ifdef xOTR1
	if (AAtTfp(32, 1, "CONCAT:"))
		prstack(s1);
#	endif
}


/*
**	Size determines the size of a character symbol
**	without trailing blanks.
*/
size(s)
register SYMBOL		*s;
{
	register char		*c;
	register int		i;

	c = (char *) s->value;
	c = ((STRINGP *) c)->stringp;
	i = ctou(s->len);

	for (c += i; i; i--)
		if(*--c != ' ')
			break;

	return (i);
}


/*
**	Converts the numeric symbol to
**	ascii. Formats to be used are determined
**	by AAout_arg.
*/
ascii(s)
register SYMBOL		*s;
{
	register int		i;
	register char		*p;
	register char		*val;
	char			temp[MAXFIELD];
	extern struct out_arg	AAout_arg;	/* used for float conversions */
	extern char		*AA_locv();
	extern char		*need();

	p = temp;
	val = (char *) s->value;
	switch (s->type)
	{
	  case INT:
		if (s->len == sizeof (long))
		{
			i = AAout_arg.i4width;
			p = AA_locv(((I4TYPE *) val)->i4type);
		}
		else
		{
			AA_itoa(((I2TYPE *) val)->i2type, p);
			if (s->len == sizeof (short))
				i = AAout_arg.i2width;
			else
				i = AAout_arg.i1width;
		}
		break;

	  case CHAR:
		return;

	  case FLOAT:
# ifndef NO_F4
		if (s->len == sizeof (float))
		{
			i = AAout_arg.f4width;
			AA_ftoa(((F4TYPE *) val)->f4type, p, i, AAout_arg.f4prec, AAout_arg.f4style);
		}
		else
# endif
		{
			i = AAout_arg.f8width;
			AA_ftoa(((F8TYPE *) val)->f8type, p, i, AAout_arg.f8prec, AAout_arg.f8style);
		}
	}
	((STRINGP *) val)->stringp = need(Ovqpbuf, i);
	AApmove(p, ((STRINGP *) val)->stringp, i, ' ');	/* blank pad to fixed length i */
	s->type = CHAR;
	s->len = i;
}


/*
**	LEXCOMP performs character comparisons between the two
**	strings s1 and s2. All blanks and null are ignored in
**	both strings. In addition pattern matching is performed
**	using the "shell syntax". Pattern matching characters
**	are converted to the pattern matching symbols PAT_ANY etc.
**	by the scanner.
**
**	Pattern matching characters can appear in either or
**	both strings. Since they cannot be stored in relations,
**	pattern matching chars in both strings can only happen
**	if the user types in such an expression.
**
**	examples:
**
**	"Smith, Homer" = "Smith,Homer"
**
**	"abcd" < "abcdd"
**
**	"abcd" = "aPAT_ANYd"
**
**	returns	<0 if s1 < s2
**		 0 if s1 = s2
**		>0 if s1 > s2
*/

lexcomp(s1, l1, s2, l2)
register char	*s1;
register char	*s2;
register int	l1;
register int	l2;
{
	register int	c1;
	register int	c2;

loop:
	while (l1--)
	{
		switch (c1 = *s1++)
		{
		  case ' ':
		  case '\0':
			break;

		  case PAT_ANY:
			return (pmatch(s1, l1, s2, l2));

		  case PAT_LBRAC:
			return (lmatch(s1, l1, s2, l2));

		  default:
			while (l2--)
			{
				switch (c2 = *s2++)
				{
				  case ' ':
				  case '\0':
					continue;

				  case PAT_ANY:
					return (pmatch(s2, l2, --s1, ++l1));

				  case PAT_LBRAC:
					return (lmatch(s2, l2, --s1, ++l1));

				  default:
					if (c1 == c2)
						goto loop;
					if (c1 == PAT_ONE || c2 == PAT_ONE)
						goto loop;
					return (c1 - c2);
				}
			}
			return (1);	/* s1 > s2 */
		}
	}

	/* examine remainder of s2 for any characters */
	while (l2--)
		if ((c1 = *s2++) != ' ' && c1 && (c1 != PAT_ANY))
			return (-1);	/* s1 < s2 */
	return (0);
}


pmatch(pat, plen, s, slen)
char		*pat;	/* the string holding the pattern matching char */
register char	*s;	/* the string to be checked */
int		plen;
register int	slen;	/* the lengths */
{
	register int	d;
	register int	c;

	if (plen == 0)
		return (0);	/* a match if no more chars in p */

	/*
	** If the next character in "pat" is not another
	** pattern matching character, then scan until
	** first matching char and continue comparison.
	*/
	if ((c = *pat) != PAT_ANY && c != PAT_LBRAC && c != PAT_ONE)
	{
		while (slen--)
		{
			if ((d = *s) == c || d == PAT_ANY || d == PAT_LBRAC && d != PAT_ONE)
			{
				if (!lexcomp(pat, plen, s, slen + 1))
					return (0);
			}
			s++;
		}
	}
	else
	{
		while (slen)
			if (!lexcomp(pat, plen, s++, slen--))
				return (0);	/* match */
	}
	return (-1);	/* no match */
}


lmatch(p, plen, s, slen)
register char	*p;	/* the string holding the pattern matching char */
register char	*s;	/* the other string */
int		plen;
int		slen;	/* their respective sizes */
{
	register int	cc;
	register int	oldc;
	register int	c;
	int		found;

	/* find a non-blank, non-null char in s */
	while (slen--)
	{
		if ((c = *s++) != ' ' && c)
		{
			/* search for a match on 'c' */
			found = 0;	/* assume failure */
			oldc = 0777;	/* make previous char large */

			while (plen--)
			{
				switch (cc = *p++)
				{
				  case PAT_RBRAC:
					if (found)
						return (lexcomp(p, plen, s, slen));
					return (-1);

				  case '-':
					if (!plen--)
						return (-1);	/* not found */
					if (oldc <= c && c <= (cc = *p++))
						found++;
					break;

				  default:
					if (c == (oldc = cc))
						found++;
				}
			}
			return (-1);	/* no match */
		}
	}
	return (1);
}
