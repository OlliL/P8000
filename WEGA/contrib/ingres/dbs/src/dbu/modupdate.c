#
/*
** MODUPDATE
**	This routine is used to exicute the updates
**	for modifies so they are recoverable.
**	It is also used by restore to complete an aborted modify.
**	During a restore the Batch_recover flag should be set to 1;
*/

# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/catalog.h"
# include	"../h/access.h"
# include	"../h/batch.h"
# include	"../h/bs.h"

int	Batch_recovery;	/* TRUE is this is recovery, else FALSE */
char	MODTEMP[]	= "_SYSnewr";

modupdate()
{
	char				batchname[MAXNAME + 3];
	char				temprel[MAXNAME+ 3];
	char				relfile[MAXNAME + 3];
	register int			i;
	register int			j;
	struct stat			sbuf;
	char				aflag;
	struct tup_id			tid;
	DESC				desx;
	struct attribute		atttup;
	struct relation			oldreltup;
	struct index			ikey;
	struct index			itup;
	char				*newpv[2];
	extern DESC			Inddes;
	extern DESC			Attdes;
	extern DESC			Reldes;
	register DESC			*desp;
	char				*trim_relname();
	long				ltemp1;
	long				ltemp2;
	extern long			lseek();


	desp =  &desx;
	AAconcat(MODBATCH, Fileset, batchname);
	AAconcat(MODTEMP, Fileset, temprel);

#	ifdef xZTR1
	AAtTfp(20, 8, "MODUPDATE: %s, %s\n",batchname, temprel);
#	endif
	if ((Batch_fp = open(batchname, 0)) < 0)
		AAsyserr(11090, batchname);
	Batch_cnt = BATCHSIZE;
	Batch_dirty = FALSE;
	getbatch(desp, sizeof *desp);
	AAdbname(desp->reldum.relid, desp->reldum.relowner, relfile);

	/* don't loose old file before verifying new file */
	if (stat(temprel, &sbuf) != -1)
	{
		unlink(relfile);	/* Ok if failure */
		if (link(temprel, relfile) == -1)
			AAsyserr(11091, temprel, relfile);
		unlink(temprel);
	}

	else
		if (stat(relfile, &sbuf) == -1 || !Batch_recovery)
			AAsyserr(11092, relfile);


	/* Update admin if this is relation or atribute relations */
	/* Should only happen in Sysmod				  */
	if ((aflag = AAbequal(desp->reldum.relid, AA_ATT, MAXNAME)) ||
		AAbequal(desp->reldum.relid, AA_REL, MAXNAME))
	{
		AAdbname(desp->reldum.relid, desp->reldum.relowner, temprel);
		if ((i = open(AA_ADMIN, 2)) < 0)
			AAsyserr(11093);
		ltemp1 = sizeof AAdmin.adhdr;
		ltemp2 = sizeof *desp;
		if (lseek(i, ltemp1, 0) < 0L || (aflag && lseek(i, ltemp2, 1) < 0L))
			AAsyserr(11094);
		if (write(i, desp, sizeof *desp) != sizeof *desp)
			AAsyserr(11095);
		close(i);

		if (aflag)
		{
			AArelclose(&Attdes);
			cleanrel(&AAdmin.adattd);
			close(AAdmin.adattd.relfp);
			AAbmove(desp, &AAdmin.adattd, sizeof *desp);
			AAdbname(AAdmin.adattd.reldum.relid,
				AAdmin.adattd.reldum.relowner, temprel);
			if ((AAdmin.adattd.relfp = open(temprel, 2)) < 0)
				AAsyserr(11096, AAdmin.adattd.relfp);
			AAdmin.adattd.relopn = RELWOPEN(AAdmin.adattd.relfp);
		}
		else
		{
			AArelclose(&Reldes);
			cleanrel(&AAdmin.adreld);
			close(AAdmin.adreld.relfp);
			AAbmove(desp, &AAdmin.adreld, sizeof *desp);
			if ((AAdmin.adreld.relfp = open(temprel, 2)) < 0)
				AAsyserr(11097, AAdmin.adreld.relfp);
			AAdmin.adreld.relopn = RELWOPEN(AAdmin.adreld.relfp);
		}
	}

	if (i = AAam_get(&AAdmin.adreld, &desp->reltid, &desp->reltid, &oldreltup, FALSE))
		AAsyserr(11098, i);

	/* update relation relation */
	if ((i = AAam_replace(&AAdmin.adreld, &desp->reltid, desp, FALSE)) < 0)
		AAsyserr(11099, i);
	if (i = cleanrel(&AAdmin.adreld))
		AAsyserr(11100, i);

	/* update attribute relation */
	AAdmin.adattd.relopn = RELWOPEN(AAdmin.adattd.relfp);
	for (i = desp->reldum.relatts; i > 0; i--)
	{
		getbatch(&tid, sizeof tid);
		getbatch(&atttup, sizeof atttup);
		if ((j = AAam_replace(&AAdmin.adattd, &tid, &atttup, FALSE)) < 0)
			AAsyserr(11101, j);
	}

	if (i = cleanrel(&AAdmin.adattd))
		AAsyserr(11102, i);

	/* make the admin readonly */
	AAdmin.adattd.relopn = RELROPEN(AAdmin.adattd.relfp);

	close(Batch_fp);

	/* if this is an AA_INDEX, change the relspec in the index catalog */
	if (ctoi(oldreltup.relindxd) == SECINDEX)
	{
		opencatalog(AA_INDEX, 2);
		AAam_setkey(&Inddes, &ikey, desp->reldum.relid, IRELIDI);
		AAam_setkey(&Inddes, &ikey, desp->reldum.relowner, IOWNERP);
		if (!(i = AAgetequal(&Inddes, &ikey, &itup, &tid)))
		{
			itup.irelspeci = desp->reldum.relspec;
			if ((i = AAam_replace(&Inddes, &tid, &itup, 0)) < 0)
				AAsyserr(11103, i);
		}
	}

	else if (ctoi(desp->reldum.relindxd) == SECBASE)
	{
		/* destroy any secondary indexes on this primary */
		opencatalog(AA_INDEX, 2);
		AAam_setkey(&Inddes, &ikey, desp->reldum.relid, IRELIDP);
		AAam_setkey(&Inddes, &ikey, desp->reldum.relowner, IOWNERP);
		while (!(i = AAgetequal(&Inddes, &ikey, &itup, &tid)))
		{
			newpv[0] = itup.irelidi;
			newpv[1] = (char *) -1;
			printf("Destroy index %s\n", trim_relname(itup.irelidi));
			if (destroy(1, newpv))
				AAsyserr(11104, itup.irelidi);
		}
	}
	if (i < 0)
		AAsyserr(11105, i);

	/* clean things up and exit */
	unlink(batchname);
#	ifdef xZTR1
	AAtTfp(20, 8, "Leaving MODUPDATE\n");
#	endif
	sync();
	return (0);
}
