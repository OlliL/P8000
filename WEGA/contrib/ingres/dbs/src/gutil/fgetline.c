# include	<stdio.h>

/*
**  FGETLINE -- get line from file
**
**	This routine reads a single newline-terminated line from
**	a file.
**
**	Parameters:
**		buf -- the buffer in which to place the result.
**		maxch -- the maximum number of characters the
**			buffer can hold.
**		iop -- the file from which to read the characters.
**
**	Returns:
**		0 -- end-of-file or error.
**		else -- 'buf'.
*/
char	*fgetline(buf, maxch, iop)
char		*buf;
int		maxch;
register FILE	*iop;
{
	register char	*p;
	register int	n;
	register int	c;

	p = buf;

	for (n = maxch; n > 0; n--)
	{
		c = getc(iop);

		/* test for end-of-file or error */
		if (c == EOF)
		{
			*p = 0;
			return ((char *) 0);
		}

		/* test for newline */
		if (c == '\n')
			break;

		/* insert character into buffer */
		*p++ = c;
	}

	/* null-terminate the string */
	*p = 0;
	return (buf);
}
