# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/access.h"
# include	"../h/symbol.h"
# include	"../h/aux.h"

AAicompare(ax, bx, frmt, frml)
char		*ax;
char		*bx;
int		frmt;
register int	frml;
{
	register char	*a;
	register char	*b;
	char		atemp[sizeof (double)];
	char		btemp[sizeof (double)];

	frml = ctou(frml);
	if (frmt == CHAR)
		return (AAscompare(ax, frml, bx, frml));
	a = atemp;
	b = btemp;
	AAbmove(ax, a, frml);
	AAbmove(bx, b, frml);
	if (AAbequal(a, b, frml))
		return (0);
	switch (frmt)
	{
	  case INT:
		switch (frml)
		{
		  case sizeof (char):
			return (ctoi(((I1TYPE *) a)->i1type) - ctoi(((I1TYPE *) b)->i1type));
		  case sizeof (short):
			return (((I2TYPE *) a)->i2type - ((I2TYPE *) b)->i2type);
		  case sizeof (long):
			return (((I4TYPE *) a)->i4type > ((I4TYPE *) b)->i4type? 1: -1);
		};
	  case FLOAT:
# ifndef NO_F4
		if (frml == sizeof (float))
			return (((F4TYPE *) a)->f4type > ((F4TYPE *) b)->f4type? 1: -1);
		else
# endif
			return (((F8TYPE *) a)->f8type > ((F8TYPE *) b)->f8type? 1: -1);
	};	/*NOTREACHED*/
}


/*
** compares all domains indicated by SETKEY in the tuple to the
** corressponding domains in the key.
** the comparison is done according to the format of the domain
** as specified in the descriptor.
**
** function values:
**
**	<0 tuple < key
** 	=0 tuple = key
**	>0 tuple > key
*/
AAkcompare(d, tuple, key)
register DESC	*d;		/*relation descriptor	*/
char		tuple[MAXTUP];	/*tuple to be compared	*/
char		key[MAXTUP];	/*second tuple or key	*/
{
	register int			i;
	register int			tmp;

	for (i = 1; i <= d->reldum.relatts; i++)
		if (d->relgiven[i])
			if (tmp = AAicompare(&tuple[d->reloff[i]],
			        &key[d->reloff[i]],
			        d->relfrmt[i],
				d->relfrml[i]))
			{
				return (tmp);
			}
	return (0);
}
