# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/catalog.h"
# include	"../h/symbol.h"
# include	"../h/access.h"
# include	"../h/batch.h"

struct batchhd	Batchhd;
struct batchbuf	Batchbuf;

/*
** Open batch prepares for batch processing.
** 1. If the batch is already open, return an error
** 2. Create the batch file.
** 3. clear domain flags.
** 4. If the relation is indexed, Identify all the domains
** 	which must be saved to speed the index update.
** 5. Set up specifics for each type of update.
** 6. Write out the batch structure.
**
** The following itemizes what is saved (in bytes):
** f(si) means it's a function of the secondary index keys
** space for newtid is saved only if there is a sec. index.
**
** 		mdDEL	mdREPL	mdAPP
** ------------------------------------
** oldtid	4	4	0
** oldtuple	f(si)	f(si)	0
** newtuple	0	tupwid	tupwid
** newtid	0	4	4
*/
openbatch(rel, indx, mode)
register DESC		*rel;
register DESC		*indx;
int			mode;
{
	register struct si_doms		*sp;
	register int			i;
	register int			saveoff;
	register int			dom;
	char				*p;
	long				lotid;
	long				hitid;
	struct index			itup;
	extern char			*AAdatabase;
	extern char			*batchname();

	if (Batchhd.mode_up)
		return (-1);	/* batch already open */

	p = batchname();	/* form batch name */

#	ifdef xATR1
	AAtTfp(89, -1, "OPENBATCH %s\n", p);
#	endif

	if ((Batch_fp = creat(p, FILEMODE)) < 0)
		AAsyserr(10001, p);

	Batch_cnt = 0;
	Batchhd.num_updts = 0;
	Batchhd.si_dcount = 0;
	Batchhd.mode_up = mode;

	/* copy the important info */
	AAsmove(Fileset, Batchbuf.file_id);
	AAsmove(AAdatabase, Batchhd.db_name);
	AAbmove(rel->reldum.relid, Batchhd.rel_name, MAXNAME);
	Batchhd.rel_name[MAXNAME] = 0;
	AAbmove(rel->reldum.relowner, Batchhd.userid, 2);

	/* clear out the secondary index domain flags */
	for (sp = Batchhd.si, i = 1; i <= MAXDOM; i++, sp++)
		sp->dom_size = 0;

	/* set up the tid and tuple sizes by type of update */
	/* assume size of tido, tidn, and tupn */
	Batchhd.tido_size = sizeof (long);	/* assume old tid is needed */
	Batchhd.tupo_size = 0;			/* assume old tuple isn't needed */
	Batchhd.tupn_size = rel->reldum.relwid;	/* assume new tuple is needed */
	Batchhd.tidn_size = sizeof (long);	/* assume space is needed for new tid */
	switch (Batchhd.mode_up)
	{
	  case mdDEL:
		Batchhd.tupn_size = 0;		/* assume new tuple isn't needed */
		Batchhd.tidn_size = 0;		/* assume new tid isn't needed */

	  case mdREPL:
		break;

	  case mdAPP:
		Batchhd.tido_size = 0;		/* assume old tid isn't needed */
		break;

	  default:
		AAsyserr(10002, Batchhd.mode_up);
	}
	/* if there are no secondary indexes then tidn isn't needed */
	if (!(i = ctoi(rel->reldum.relindxd)) || i == SECINDEX)
		Batchhd.tidn_size = 0;

	/* if this relation has a secondary index, figure out what to save */
	if (ctoi(rel->reldum.relindxd) == SECBASE && mode != (int) mdAPP)
	{
		AAam_setkey(indx, &itup, rel->reldum.relid, IRELIDP);
		AAam_setkey(indx, &itup, rel->reldum.relowner, IOWNERP);

		if (i = AAam_find(indx, EXACTKEY, &lotid, &hitid, &itup))
			AAsyserr(10003, i, rel->reldum.relid);

		/* check each entry in "index" relation for useful index */
		while (!(i = AAam_get(indx, &lotid, &hitid, &itup, TRUE)))
		{
			if (!AAbequal(itup.irelidp, rel->reldum.relid, MAXNAME) ||
				!AAbequal(itup.iownerp, rel->reldum.relowner, 2))
				continue;
			/* found one. copy the used domains */
			p = itup.idom;		/* get address of first */
			i = MAXKEYS;
			while (i--)
			{
				if (!(dom = *p++))
					break;	/* no more domains */
				sp = &Batchhd.si[dom];
				if (sp->dom_size)
					continue;	/* domain has already been done once */
				Batchhd.si_dcount++;
				Batchhd.tupo_size += ctou(rel->relfrml[dom]);
				sp->rel_off = rel->reloff[dom];
				sp->dom_size = ctou(rel->relfrml[dom]);
			}
		}
		if (i < 0)
			AAsyserr(10004, i, rel->reldum.relid);
		/* compute offsets of domains in saved "oldtuple" */
		saveoff = 0;
		sp = Batchhd.si;
		i = Batchhd.si_dcount;
		while (i--)
		{
			/* skip to next domain */
			while (!sp->dom_size)
				sp++;
			sp->tupo_off = saveoff;
			saveoff += sp->dom_size;
			sp++;
		}
	}
	wrbatch(&Batchhd, sizeof Batchhd);
	return (0);
}


addbatch(oldtid, newtuple, old)
long		*oldtid;
char		*newtuple;
register char	*old;
{
	long			newtid;
	register int		i;
	register struct si_doms	*sp;

#	ifdef xATR1
	AAtTfp(89, 3, "ADDBATCH\n");
#	endif

	if (!Batchhd.mode_up)
		return (-1);

	Batchhd.num_updts++;	/* increment the number of add batches */

	/* write out the old tid */
	wrbatch(oldtid, Batchhd.tido_size);

	/* write out each of the old tuple domains */
	sp = Batchhd.si;	/* sp points to the domain structures */
	i = Batchhd.si_dcount;	/* i get the number of domains */
	while (i--)
	{
		/* skip to the next domain */
		while (!sp->dom_size)
			sp++;

		wrbatch(&old[sp->rel_off], sp->dom_size);
		sp++;
	}

	/* write out the new tuple */
	wrbatch(newtuple, Batchhd.tupn_size);

	/* reserve space for the new tid. Init to -1 */
	newtid = -1;
	wrbatch(&newtid, Batchhd.tidn_size);

	return (0);
}


closebatch()
{
	extern long	lseek();

	if (!Batchhd.mode_up)
		return (-1);
	flushbatch();	/* write out any remainder */
	if (lseek(Batch_fp, 0L, 0) < 0L)
		AAsyserr(10005);
	wrbatch(&Batchhd, sizeof Batchhd);	/* update num_updts */
	flushbatch();
	if (close(Batch_fp))
		AAsyserr(10006);
	Batchhd.mode_up = 0;
	sync();
	return (0);
}
