# include	"amx.h"

/* LOOKUP NAME for syntactical correctness */
char	*name_lookup(name)
register char	*name;
{
	register int		i;
	register int		c;
	static char		name_buf[MAXNAME + 1];

	if ((i = AAlength(name)) > MAXNAME)
	{
		amxerror(81, name);
		return ((char *) 0);
	}
	AAbmove(name, name_buf, i);
	sym_free();
	for (name = name_buf; i; --i, name++)
	{
		if (((c = *name) >= 'A'
# ifdef EBCDIC
		   && c <= 'I') || (c >= 'J'
		   && c <= 'R') || (c >= 'S'
# endif
		   && c <= 'Z'))
			*name = c  + 'a' - 'A';
	}
	*name = '\0';
	return (name_buf);
}
