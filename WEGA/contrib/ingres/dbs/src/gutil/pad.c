/*
**  PAD STRING OUT WITH BLANKS
**
**	This routine is an in-place AApmove which always pads
**	with blanks.
*/
AApad(s, n)
register char	*s;
register int	n;
{
	AApmove(s, s, n, ' ');
}
