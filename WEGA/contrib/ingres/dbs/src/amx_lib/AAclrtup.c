# include	"AA_liba.h"

AAclrtup(d, rel, tuple)
register DESCXX		*d;
char			*rel;
register char		*tuple;
{
	register int	i;
	register int	len;
	register char	*off;
	register int	c;

# ifdef AMX_TRACE
	AAtTfp(18, 0,
# ifdef MSDOS
		"clear tuple\t`%.10s'\n",
# else
		"clear tuple\t`%.14s'\n",
# endif
		rel);
# endif

	errno = 0;
	if (!d)
		AAerror(106, rel);

	for (i = d->xx_desc.reldum.relatts; i; --i)
	{
		len = ctou(d->xx_desc.relfrml[i]);
		off = tuple + d->xx_desc.reloff[i];
		c = d->xx_desc.relfrmt[i] == CHAR? ' ': 0;
		do *off++ = c; while (--len);
	}
}
