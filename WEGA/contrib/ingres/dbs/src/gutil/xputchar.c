# include	<stdio.h>

static short	AAprtable[16] =
{
# ifdef EBCDIC
	0x3820, 0x0060, 0x8020, 0x0000,
	0xf801, 0x7c01, 0xf803, 0xfc00,
	0x03fe, 0x03fe, 0x03fc, 0x0000,
	0x03fe, 0x03fe, 0x03fc, 0x03ff
# else
	0x3780, 0x0000, 0xffff, 0xffff,
	0xffff, 0xffff, 0xffff, 0x7fff,
	0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000
# endif
};

/*
**  XPUTCHAR -- put character to standard output
**
**	The character is put directly to standard output, unless it is
**	a control character, in which case it is translated to a back-
**	slash escape so that it can be read.  If this translation is
**	not wanted, use putc().
**
**	The determination of printability is done by looking at the
**	bit vector 'AAprtable' -- if the corresponding bit is set,
**	the character is printed directly.
**
**	Parameter:
**		c -- character to output
*/
AAxputchar(c)
register int	c;
{
	register int	i;
	register int	mask;

	i = (c >> 4) & 017;
	mask = 1 << (c & 017);

	if (!(mask & AAprtable[i]))
	{
		/* output the octal */
		putchar('\\');
		i = c;
		putchar(((i & 0300) >> 6) + '0');
		putchar(((i & 0070) >> 3) + '0');
		return (putchar((i & 0007) + '0'));
	}
	else
		return (putchar(c));
}
