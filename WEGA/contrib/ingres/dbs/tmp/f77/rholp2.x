##		shared		f77
##		long		Vkey;
##		long		Vtug;
##		long		Vtog;

# define	MAX		10
# define	V4		3
# define	V8		3

struct rholp2
{
		long		key;
		double		v8[V8];
		float		v4[V4];
		long		quell_nr;
};


rholp2_(key, tu, to, v4, v8, quell_nr, i, lv4, lv8)
long	*key;
long	*tu;
long	*to;
float	*v4;
double	*v8;
long	*quell_nr;
long	*i;		/* or short	*i; */
long	*lv4;		/* length of v4     */
long	*lv8;		/* length of v8     */
{
	register int			ix;
	register struct rholp2		*r2;

	ix = -1;
	Vkey = *key;
	Vtug = *tu;
	Vtog = *to;
##	retrieve rholp2 $key = Vkey and $tug >= Vtug and $tog <= Vtog
##	{
		if (++ix < MAX)
		{
			r2 = (struct rholp2 *) AA_STUP;
			AAbmove(r2->v4, &v4[V4 * ix], sizeof r2->v4);
			AAbmove(r2->v8, &v8[V8 * ix], sizeof r2->v8);
			*quell_nr = r2->quell_nr;
		}
##	}
	ix++;
	*i = (ix <= MAX)? ix: -ix;
}
