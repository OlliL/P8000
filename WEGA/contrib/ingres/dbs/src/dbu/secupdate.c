# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/catalog.h"
# include	"../h/symbol.h"
# include	"../h/access.h"
# include	"../h/batch.h"

struct key_pt
{
	char	*pt_old;
	char	*pt_new;
};

# ifdef STACK
static char		oldtup[MAXTUP];
static char		newtup[MAXTUP];
static char		oldkey[MAXTUP];
static char		newkey[MAXTUP];
static char		dumtup[MAXTUP];
static struct index	itup;
static DESC		si_desc;
static struct key_pt	keys[MAXKEYS + 1];
# endif


secupdate(rel)
DESC			*rel;
{
	register DESC			*r;
	register char			*p;
	register int			i;
	int				j;
	int				domcnt;
	int				mode;
	int				dom;
	long				tupcnt;
	long				oldtid;
	long				newtid;
	long				lotid;
	long				hitid;
	long				uptid;
# ifndef STACK
	char				oldtup[MAXTUP];
	char				newtup[MAXTUP];
	char				oldkey[MAXTUP];
	char				newkey[MAXTUP];
	char				dumtup[MAXTUP];
	struct index			itup;
	DESC				si_desc;
	struct key_pt			keys[MAXKEYS + 1];
# endif
	extern DESC			Inddes;
	extern long			lseek();

	r = rel;
	mode = Batchhd.mode_up;
	Batch_dirty = FALSE;
#	ifdef xZTR1
	AAtTfp(15, -1, "SECUPDATE\n");
#	endif
	opencatalog(AA_INDEX, 0);
	AAam_setkey(&Inddes, &itup, r->reldum.relid, IRELIDP);
	AAam_setkey(&Inddes, &itup, r->reldum.relowner, IOWNERP);
	if (i = AAam_find(&Inddes, EXACTKEY, &lotid, &hitid, &itup))
		AAsyserr(11159, i);

	/* update each secondary index */
	while (!(i = AAam_get(&Inddes, &lotid, &hitid, &itup, TRUE)))
	{
		/* check if the index is on the right relation */
#		ifdef xZTR1
		if (AAtTf(15, 7))
			AAprtup(&Inddes, &itup);
#		endif
		if (!AAbequal(itup.irelidp, r->reldum.relid, MAXNAME) ||
			!AAbequal(itup.iownerp, r->reldum.relowner, 2))
			continue;

		if (i = AArelopen(&si_desc, 1, itup.irelidi))
			AAsyserr(11160, itup.irelidi, i);
		/* reposition batch file to the beginning. */
		if (lseek(Batch_fp, 0L, 0) < 0L)
			AAsyserr(11161, Batch_fp);
		Batch_cnt = BATCHSIZE;
		getbatch(&Batchhd, sizeof Batchhd);	/* reread header */

		/* set up the key structure */
		p = itup.idom;
		for (domcnt = 0; domcnt < MAXKEYS; domcnt++)
		{
			if (!(dom = *p++))
				break;	/* no more key domains */
#			ifdef xZTR1
			AAtTfp(15, 15, "dom %d,tupo_off %d\n",
				dom, Batchhd.si[dom].tupo_off);
#			endif
			keys[domcnt].pt_old = &oldtup[Batchhd.si[dom].tupo_off];
			keys[domcnt].pt_new = &newtup[r->reloff[dom]];
		}

		/* the last domain is the "tidp" field */
		keys[domcnt].pt_old = (char *) &oldtid;
		keys[domcnt].pt_new = (char *) &newtid;

		/*
		** Start reading the batch file and updating
		** the secondary indexes.
		*/
		tupcnt = Batchhd.num_updts;
		while (tupcnt--)
		{
			getbatch(&oldtid, Batchhd.tido_size);
			getbatch(oldtup, Batchhd.tupo_size);
			getbatch(newtup, Batchhd.tupn_size);
			getbatch(&newtid, Batchhd.tidn_size);

			/* if this is a replace or append form the new key */
			if (mode != (int) mdDEL)
			{
				for (j = 0; j <= domcnt; j++)
					AAam_setkey(&si_desc, newkey, keys[j].pt_new, j + 1);
#				ifdef xZTR1
				if (AAtTf(15, 7))
					AAprkey(&si_desc, newkey);
#				endif
			}

			/* if this is delete or replace form the old key */
			if (mode != (int) mdAPP)
			{
				for (j = 0; j <= domcnt; j++)
					AAam_setkey(&si_desc, oldkey, keys[j].pt_old, j + 1);
#				ifdef xZTR1
				if (AAtTf(15, 8))
					AAprkey(&si_desc, oldkey);
#				endif
			}

			switch (mode)
			{

			  case mdDEL:
				if (i = AAgetequal(&si_desc, oldkey, dumtup, &uptid))
				{
					if (i > 0)
						break;
					AAsyserr(11162, i);
				}
				if ((i = AAam_delete(&si_desc, &uptid)) < 0)
					AAsyserr(11163, i);
				break;

			  case mdREPL:
				/* if the newtup = oldtup then do nothing */
				if (AAbequal(oldkey, newkey, si_desc.reldum.relwid))
					break;
				if (i = AAgetequal(&si_desc, oldkey, dumtup, &uptid))
				{
					if (Batch_recovery && i > 0)
						goto secinsert;
					AAsyserr(11164, i);
				}
				if (i = AAam_replace(&si_desc, &uptid, newkey, TRUE))
				{
					/* if newtuple is dup of old, ok */
					if (i == 1)
						break;
					/* if this is recovery and old tid not there, try an insert */
					if (Batch_recovery && i == 2)
						goto secinsert;
					AAsyserr(11165, i);
				}
				break;

			  case mdAPP:
			  secinsert:
				if ((i = AAam_insert(&si_desc, &uptid, newkey, TRUE)) < 0)
					AAsyserr(11166, i);
			}
		}
		if (i = AArelclose(&si_desc))
			AAsyserr(11167, si_desc.reldum.relid, i);
	}
	if (i < 0)
		AAsyserr(11168, i);
}
