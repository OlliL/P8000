##		shared		f77
##		long		Vkey;

# define	CA_NR		16
# define	NAME		32
# define	SUMFORM		16
# define	V8		24

struct basisdaten
{
		long		key;
		char		ca_nr[CA_NR];
		char		name[NAME];
		char		sumform[SUMFORM];
		double		v8[V8];
		long		quell_nr;
		long		version;
};

bdat_(key, ca_nr, name, sumform, v, quell_nr, version, i, cl, nl, sl, vl)
long	*key;
char	*ca_nr;
char	*name;
char	*sumform;
double	*v;
long	*quell_nr;
long	*version;
long	*i;		/* or short	*i;  */
long	*cl;		/* length of ca_nr   */
long	*nl;		/* length of name    */
long	*sl;		/* length of sumform */
long	*vl;		/* length of v-array */
{
	register int			ix;
	register struct basisdaten	*bd;

	*i = -1;
	Vkey = *key;
##	retrieve basisdaten $key = Vkey
##	{
		bd = (struct basisdaten *) AA_STUP;
		AAbmove(bd->ca_nr, ca_nr, CA_NR);
		AAbmove(bd->name, name, NAME);
		AAbmove(bd->sumform, sumform, SUMFORM);
		AAbmove(bd->v8, v, V8);
		*quell_nr = bd->quell_nr;
		*version = bd->version;
		*i = 0;
##	}
}
