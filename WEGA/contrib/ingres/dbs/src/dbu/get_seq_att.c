# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/access.h"

/*
** Seq_attributes - get all attributes of a relation in their correct order.
**
**	Seq_attributes() can be called when it is desired to get all
**	attribute tuples for a relation. They are guaranteed to be in
**	the same order as they were created in; that is "attid" order.
**
**	The calling convention is:
**
**		seq_init(Attribute_descriptor, Descriptor_for_relation)
**
**		while (seq_attribute(Att_desc, Des_for_rel, Tuple)
**		{
**		}
**
**	The first parameter is an OPEN descriptor for the attribute
**	relation. The second parameter is a descriptor for the relation.
**	It must have the relation-relation tuple (ie AArelopen mode -1).
**	The third parameter is a place to put the attribute. Its
**	dimension must be at least sizeof (attribute structure).
**
**	Seq_attribute attempts to optimize the retrieval of the
**	attributes. It assumes initially that the are physically
**	stored sequentially. If so it will retrieve them in the
**	minimum possible number of accesses.
**
**	If it finds one tuple out of sequence then all future
**	scans will start from that first out of sequence tuple.
*/

static struct tup_id	Seq_tdl;
static struct tup_id	Seq_tdh;
static struct tup_id	Seq_tdf;
static int		Seq_seqmode;
static int		Seq_next;



seq_init(atr, rel)
DESC			*atr;
DESC			*rel;
{
	register DESC			*a;
	register DESC			*r;
	register int			i;
	struct attribute		attkey;

	a = atr;
	r = rel;

	AAam_clearkeys(a);
	AAam_setkey(a, &attkey, r->reldum.relid, ATTRELID);
	AAam_setkey(a, &attkey, r->reldum.relowner, ATTOWNER);
	if (i = AAam_find(a, EXACTKEY, &Seq_tdl, &Seq_tdh, &attkey))
		AAsyserr(11031, i);

	Seq_seqmode = TRUE;
	Seq_next = 1;
}


seq_attributes(atr, rel, att_tuple)
DESC			*atr;
DESC			*rel;
struct attribute	*att_tuple;
{
	register DESC			*a;
	register DESC			*r;
	register struct attribute	*atup;
	int				i;
	int				nxttup;

	a = atr;
	r = rel;
	atup = att_tuple;

	if (Seq_next > r->reldum.relatts)
		return (0);	/* no more attributes */

	if (!Seq_seqmode)
	{
		/* attributes not stored sequencially, start from first attribute */
		AAbmove(&Seq_tdf, &Seq_tdl, sizeof (Seq_tdl));
		nxttup = FALSE;
	}
	else
		nxttup = TRUE;

	while (!(i = AAam_get(a, &Seq_tdl, &Seq_tdh, atup, nxttup)))
	{

		nxttup = TRUE;
		/* make sure this is a tuple for the right relation */
		if (AAkcompare(a, atup, r))
			continue;

		/* is this the attribute we want? */
		if (atup->attid != Seq_next)
		{
			if (Seq_seqmode)
			{
				/*
				** Turn off seq mode. Save the tid of
				** the current tuple. It will be the
				** starting point for the next call
				** to seq_attribute
				*/
				AAbmove(&Seq_tdl, &Seq_tdf, sizeof (Seq_tdf));
				Seq_seqmode = FALSE;
			}

			continue;
		}

		/* got current attribute. increment to next attribute */
		Seq_next++;
		return (1);
	}

	/* fell out of loop. either bad get return or missing attribute */
	AAsyserr(11032, i, Seq_next);
}
