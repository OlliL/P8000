# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/symbol.h"

/*
**	AAam_clearkeys - reset all key indicators in descriptor
**
**	AAam_clearkeys is used to clear the key supplied
**	flags before calls to setkey
*/
AAam_clearkeys(d)
register DESC	*d;
{
	register int			i;

	for (i = 0; i <= d->reldum.relatts; i++)
		d->relgiven[i] = 0;
	return (0);
}


/*
**	AAam_setkey - indicate a partial key for find
**
**	AAam_setkey is used to set a key value into place
**	in a key. The key can be as large as the entire
**	tuple. AAam_setkey moves the key value into the
**	proper place in the key and marks the value as
**	supplied
**
**	If the value is a null pointer, then the key is
**	cleared.
**
**	AAam_clearkeys should be called once before the
**	first call to AAam_setkey.
*/
AAam_setkey(d, key, value, dom)
register DESC		*d;
char			*key;
char			*value;
register int		dom;
{
	register int			len;
	register char			*cp;

#	ifdef xATR1
	AAtTfp(96, 0, "SETKEY: %.14s, %d\n", d->reldum.relid, dom);
#	endif

	/* check validity of domain number */
	if (dom < 1 || dom > d->reldum.relatts)
		AAsyserr(10026, dom, d->reldum.relatts, d->reldum.relid);

	/* if value is null, clear key */
	if (!value)
	{
		d->relgiven[dom] = 0;
		return;
	}

	/* mark as given */
	d->relgiven[dom] = 1;

	len = ctou(d->relfrml[dom]);
	cp = &key[d->reloff[dom]];

	if (d->relfrmt[dom] == CHAR)
		AApmove(value, cp, len, ' ');
	else
		AAbmove(value, cp, len);
}
