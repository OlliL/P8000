/*
** ---  RG_UTIL.C ------- UTILITIES  ------------------------------------------
**
**		CMOVE()		pad with some char's
**		SEPERATE()	find last blank for string batching
**		PAD()		pad string out with blanks
**		CONVERT()	convert anything to double
**
**	Version: @(#)rg_util.c		4.0	02/05/89
**
*/

# include	"rg_int.h"


char		*cmove(b, l, c)
register char	*b;
register int	l;
register int	c;
{
	while (--l >= 0)
		*b++ = c;

	return (b);
}


seperate(b, l)
register char	*b;
register int	l;
{
	register char	*p;

	b = p = &b[l];

	while (--l && *(--p) != ' ')
		continue;

	if (l)
		return (l);
	return (b - p + 1);
}


char		*AApad(s, l)
register char	*s;
register int	l;
{
	register int	c;

	c = ' ';

	while (--l >= 0)
		*s++ = c;

	return (s);
}


convert(s)
register SYMBOL		*s;
{
	register int		len;
	register union anytype	*any;
	register char		*p;
	register int		i;
	char			save;
	extern char		*print_type();

	len = ctou(s->sy_frml);
	any = &s->sy_val;

# ifdef RG_TRACE
	if (TR_FORMAT)
		printf("CONVERT\t%s%d\n", print_type(s->sy_frmt), len);
# endif

	switch (s->sy_frmt)
	/* assume correct types: "c..", "i1", "i2", "i4", "f4", "f8" */
	{
	  case CHAR:	/* try to convert */
		p = &any->cptype[len];
		save = *p;
		*p = 0;
		i = AA_atof(any->cptype, &any->f8type);
		*p = save;
		if (i)
			return (TRUE);
		break;

	  case INT:
		switch (len)
		{
		  case sizeof (char):
			any->f8type = ctoi(any->i1type);
			break;

		  case sizeof (short):
			any->f8type = any->i2type;
			break;

		  case sizeof (long):
			any->f8type = any->i4type;
			break;
		}
		break;

# ifndef NO_F4
	  case FLOAT:
		if (len == sizeof (float))
			any->f8type = any->f4type;
# endif
	}

	s->sy_frmt = FLOAT;
	s->sy_frml = sizeof (double);
	return (FALSE);
}


char	*print_type(type)
register int	type;
{
	static char	buf[2];
	switch (type)
	{
	  case INT:
		return ("i");

	  case CHAR:
		return ("c");

	  case FLOAT:
		return ("f");
	}

	buf[0] = type + 'A';
	buf[1] = 0;

	return (buf);
}
