/*
**	Count the number of 1's in the integer var. As long
**	as left shift is zero fill this routine is machine
**	independent.
*/
bitcnt(var)
register int	var;
{
	register int	i;
	register int	ret;

	for (ret = 0, i = 1; i; i <<= 1)
		if (i & var)
			ret++;
	return (ret);
}
