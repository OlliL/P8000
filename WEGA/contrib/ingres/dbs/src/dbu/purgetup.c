# include	"gen.h"
# include	"../h/dbs.h"

/*
**	Remove tuples from the specified system relation.
**
**	'Desa' is a descriptor for a system relation and
**	key[12] and dom[12] are the keys and domain numbers
**	to match on for the delete.
**	All the tuples in 'desa' with key1 and key2
**	are deleted from the relation.
*/

purgetup(desa, key1, dom1, key2, dom2)
DESC			*desa;
char			*key1;
int			dom1;
char			*key2;
int			dom2;
{
	struct tup_id			tid;
	struct tup_id			limtid;
	register int			i;
	char				tupkey[MAXTUP];
	char				tuple[MAXTUP];
	register DESC			*d;

	d = desa;

	AAam_setkey(d, tupkey, key1, dom1);
	AAam_setkey(d, tupkey, key2, dom2);
	if (i = AAam_find(d, EXACTKEY, &tid, &limtid, tupkey))
		AAsyserr(11118, i);
	while (!(i = AAam_get(d, &tid, &limtid, tuple, TRUE)))
	{
		if (!AAkcompare(d, tuple, tupkey))
			if (i = AAam_delete(d, &tid))
				AAsyserr(11119, i);
	}

	if (i < 0)
		AAsyserr(11120, d->reldum.relid, i);
}
