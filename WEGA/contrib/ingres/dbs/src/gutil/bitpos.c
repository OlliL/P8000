/*
**  FIND HIGH ORDER BIT POSITION
**
**	The position of the highest ordered one bit in `word' is
**	found and returned.  Bits are numbered 0 -> 15, from
**	right (low-order) to left (high-order) in word.
*/
bitpos(word)
register int	word;
{
	register int	i;
	register int	j;
	register int	pos;

	pos = -1;

	for (i = 1, j = 0; word; i <<= 1, j++)
		if (word & i)
		{
			pos = j;
			word &= ~i;
		}

	return (pos);
}
