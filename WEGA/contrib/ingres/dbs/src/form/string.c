# include	"form.h"

char			*MM_cs[CS_SE + 1];

MM_ptis(string)
register char			*string;
{
	MM_pts(MM_cs[CS_SO]);
	MM_pts(string);
	MM_pts(MM_cs[CS_SE]);

	return (1);
}


MM_pts(string)
register char			*string;
{
	register int			time_out;
	register int			esc;
	register int			len;

	esc = *string != 033;
	len = 0;
	while (*string)
	{
		write(1, string++, 1);
		len++;
		if (esc)
			continue;
		for (time_out = 1000; time_out; --time_out)
			continue;
	}

	return (len);
}


MM_pta(string, length)
register char	*string;
register int	length;
{
	string[length] = 0;
	for (length -= MM_pts(string); length > 0; --length)
		write(1, " ", 1);

	return (1);
}
