# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/symbol.h"
# include	"../h/access.h"

/*
 * AArhash will perform a randomizing hash on the full key.
 */
long AArhash(d, key)
register DESC		*d;
char			key[MAXTUP];
{
	register int			i;
	register char			*cp;
	register int			tmp;
	register int			j;
	register int			knt;
	register int			numeric;
	long				bucket;

	bucket = 0;
	knt = 0;
	for (i = 1; i <= d->reldum.relatts; i++)
		if (d->relxtra[i])
		{
			/* form pointer to field */
			cp = &key[d->reloff[i]];
			numeric = d->relfrmt[i] != CHAR;
			for (j = 0; j < ctou(d->relfrml[i]); j++)
				if (((tmp = *cp++) != ' ') || numeric)
					AAaddabyte(tmp, &bucket, knt++);
		}

	/* remove sign bit from bucket the hard way */
	bucket &= 077777;

#	ifdef xATR3
	AAtTfp(92, 8, "RHASH: hval=%ld", bucket);
#	endif

	bucket %= d->reldum.relprim;

#	ifdef xATR3
	AAtTfp(92, 8, " ret %ld\n", bucket);
#	endif

	return (bucket);
}


/*
 * AAaddabyte is used to map a long key into a four byte integer.
 * As bytes are added, they are first rotated, then exclusive ored
 * into the existing key.
 */
AAaddabyte(ch, word, knt)
register int	ch;
register long	*word;
register int	knt;
{
	long		i;

	i = ctou(ch);	/*get rid of any sign extension*/
	knt += 8 * (knt & 3);	/*alternately add 0, 8, 16 or 24 to knt */
	knt &= 037;
	*word ^= (i << (knt) | i >> (32 - knt));
}
