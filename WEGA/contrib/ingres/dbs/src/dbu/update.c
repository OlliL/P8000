# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/catalog.h"
# include	"../h/symbol.h"
# include	"../h/access.h"
# include	"../h/batch.h"


/*
**	Update reads a batch file written by the
**	access method routines (openbatch, addbatch, closebatch)
**	and performs the updates stored in the file.
**
**	It assumes that it is running in the database. It
**	is driven by the data in the Batchhd struct (see ../batch.h).
**	If the relation has a secondary index then update calls
**	secupdate. As a last step the batch file is removed.
**
**	The global flag Batch_recovery is tested in case
**	of an error. It should be FALSE if update is being
**	run as the dbu deferred update processor. It should
**	be TRUE if it is being used as part of the recovery
**	procedure.
*/

update()
{
	register int		i;
	register int		mode;
	DESC			rel;
	long			oldtid;
	long			tupcnt;
	char			oldtup[MAXTUP];
	char			newtup[MAXTUP];
	extern char		*batchname();
	extern int		Dburetflag;
	extern struct retcode	Dburetcode;

#	ifdef xZTR1
	AAtTfp(15, -1, "UPDATE on %s\n", batchname());
#	endif
	/* set up to read batchhd */
	Batch_cnt = BATCHSIZE;	/* force a read on next getbatch */
	Batch_dirty = FALSE;
	if ((Batch_fp = open(batchname(), 2)) < 0)
		AAsyserr(11169, batchname());
	getbatch(&Batchhd, sizeof Batchhd);

	tupcnt = Batchhd.num_updts;
#	ifdef xZTR1
	AAtTfp(15, 0, "rel=%s tups=%ld\n", Batchhd.rel_name, tupcnt);
#	endif
	Dburetflag = TRUE;
	Dburetcode.rc_tupcount = 0;
	if (!tupcnt)
	{
		rmbatch();
		return (1);
	}

	/* update the primary relation */
	if (i = AArelopen(&rel, 2, Batchhd.rel_name))
		AAsyserr(11170, Batchhd.rel_name, i);
	mode = Batchhd.mode_up;

	while (tupcnt--)
	{
		getbatch(&oldtid, Batchhd.tido_size);	/* read old tid */
		getbatch(oldtup, Batchhd.tupo_size);	/* and portions of old tuple */
		getbatch(newtup, Batchhd.tupn_size);	/* and the newtup */

		switch (mode)
		{

		  case mdDEL:
			if ((i = AAam_delete(&rel, &oldtid)) < 0)
				AAsyserr(11171, i, Batchhd.rel_name);
			break;

		  case mdREPL:
			if (i = AAam_replace(&rel, &oldtid, newtup, TRUE))
			{
				/* if newtuple is a duplicate, then ok */
				if (i == 1)
					break;
				/* if this is recovery and oldtup not there, try to insert newtup */
				if (Batch_recovery && i == 2)
					goto upinsert;
				AAsyserr(11172, i, Batchhd.rel_name);
			}
			Dburetcode.rc_tupcount++;
			break;

		  case mdAPP:
		  upinsert:
			if ((i = AAam_insert(&rel, &oldtid, newtup, TRUE)) < 0)
				AAsyserr(11173, i, Batchhd.rel_name);
			break;

		  default:
			AAsyserr(11174, mode);
		}
		putbatch(&oldtid, Batchhd.tidn_size);	/* write new tid if necessary */
	}
	/* fix the tupchanged count if delete or append */
	if (mode != (int) mdREPL)
		Dburetcode.rc_tupcount = rel.reladds >= 0? rel.reladds: -rel.reladds;
	/* close the relation but secupdate will still use the decriptor */
	if (i = AArelclose(&rel))
		AAsyserr(11175, i, Batchhd.rel_name);
	batchflush();

	/* if this relation is indexed, update the indexes */
	if (ctoi(rel.reldum.relindxd) == SECBASE)
		secupdate(&rel);
	rmbatch();

#	ifdef xZTR1
	AAtTfp(15, 2, "%ld tups changed\n", Dburetcode.rc_tupcount);
#	endif
	return (0);
}
