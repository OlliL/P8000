# include	"gen.h"
# include	"../h/symbol.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"

/*
**	convert  converts  numeric values of one type and length
**	to a different type and length.
**
**	The source numeric can be i1, i2, i4, f4, or f8.
**	The source number will be converted to the
**	type and length specified in the destination.
**	It also  must be one of i1, i2, i4, f4,  or f8.
**
**	convert returns 0 is no overflow occured,
**	  else it returns -1
*/
convert(inp, outp, sf, sl, df, dl)
char		*inp;	/* pointer to input */
char		*outp;	/* pointer to the output area */
int		sf;	/* format of the source number */
register int	sl;	/* length of the source number */
int		df;	/* format of the dest */
register int	dl;	/* length of the dest */
{
	char		number[sizeof (double)];	/* dummy buffer */
	register char	*num;

	num = number;
	AAbmove(inp, num,  sl);	/* copy number into buffer */

	if (sf != df)
	{
		/* if the source and destination formats are
		   different then the source must be converted
		   to i4 if the dest is int, otherwise to f8 */

		if (df == FLOAT)
		{
			switch (sl)
			{
			  case sizeof (char):
				((F8TYPE *) num)->f8type = ctoi(*num);		/* i1 to f8 */
				break;

			  case sizeof (short):
				((F8TYPE *) num)->f8type = ((I2TYPE *) num)->i2type;	/* i2 to f8 */
				break;

			  case sizeof (long):
				((F8TYPE *) num)->f8type = ((I4TYPE *) num)->i4type;	/* i4 to f8 */
			}
			sl = sizeof (double);
		}
		else
		{
			/* check if float >  2**31 */
# ifndef NO_F4
			if (sl == sizeof (float))	/* convert f4 to f8 */
				((F8TYPE *) num)->f8type = ((F4TYPE *) num)->f4type;
# endif

			if (((F8TYPE *) num)->f8type > 2147483647.0 || ((F8TYPE *) num)->f8type < -2147483648.0)
				return (-1);
			((I4TYPE *) num)->i4type = ((F8TYPE *) num)->f8type;
			sl = sizeof (long);
		}
	}

	/* source is now the same type as destination */
	/* convert lengths to match */

	if (sl != dl)
	{
		/* lengths don't match. convert. */
		if (df == INT)
		{
			switch (dl)
			{
			  case sizeof (char):
				if (sl == sizeof (short))	/* i2 to i1 */
				{
					if (((I2TYPE *) num)->i2type > 127 || ((I2TYPE *) num)->i2type < -128)
						return (-1);
					*num = ((I2TYPE *) num)->i2type;
				}
				else	/* i4 to i1 */
				{
					if (((I4TYPE *) num)->i4type > 127 || ((I4TYPE *) num)->i4type < -128)
						return (-1);
					*num = ((I4TYPE *) num)->i4type;
				}
				break;

			  case sizeof (short):
				if (sl == sizeof (char))	/* i1 to i2 */
					((I2TYPE *) num)->i2type = ctoi(*num);
				else	/* i4 to i2 */
				{
					if (((I4TYPE *) num)->i4type > 32767 || ((I4TYPE *) num)->i4type <-32768)
						return (-1);
					((I2TYPE *) num)->i2type = ((I4TYPE *) num)->i4type;
				}
				break;

			  case sizeof (long):
				if (sl == sizeof (char))	/* i1 to i4 */
					((I4TYPE *) num)->i4type = ctoi(*num);
				else	/* i2 to i4 */
					((I4TYPE *) num)->i4type = ((I2TYPE *) num)->i2type;
			}
		}
# ifndef NO_F4
		else
		{
			if (dl == sizeof (double))
				((F8TYPE *) num)->f8type = ((F4TYPE *) num)->f4type;	/* f4 to f8 */
			else
				((F4TYPE *) num)->f4type = ((F8TYPE *) num)->f8type;	/* f8 to f4 with rounding */
		}
# endif
	}

	/* conversion is complete */
	/* copy the result into outp */

	AAbmove(num, outp, dl);
	return (0);
}
