# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/bs.h"
# include	"../h/catalog.h"
# include	"../h/access.h"
# include	"../h/batch.h"
# ifdef BC
# define	mdINTEG		23
# define	mdPROT		22
# define	mdVIEW		16
# else
# include	"../h/symbol.h"
# endif
# include	"../h/lock.h"

# ifdef P8000
ret_buf		AAjmp_buf;
# else
jmp_buf		AAjmp_buf;
# endif

/* first file to close on error */
# define	CLOSEFILES 	3

extern int	AAdbstat;
extern char	*AAusercode;
char		Utemp[2];
char		*Fileset;
char		Berror;		/* batch error */
char		Error;
extern char	Ask;
extern char	Superuser;
extern char	All;
extern char	Qrymod;
extern char	No_purge;
extern char	No_sysmod;
extern DESC	Reldes;
extern DESC	Attdes;
extern DESC	Inddes;
extern DESC	Treedes;
extern DESC	Prodes;
extern DESC	Intdes;
int		Direc =		CLOSEFILES - 1;
extern int	Wait_action;
int		Pass;
struct batchhd	Batchhd;
struct batchbuf	Batchbuf;
char		Header[] =	"**\t\t";
char		PURGE[]	=	"purge";
char		SYSMOD[] =	"sysmod";
char		WAIT[] =	"+w";
char		MODBATCH[] =	"_SYSmod";
int		s_MODBATCH =	sizeof MODBATCH - 1;

/*
** these two are used somewhere in some DBS library
** and need to be defined.  So do it here.
*/
int		Dburetflag;
struct retcode	Dburetcode;

struct direc
{
# ifndef NON_V7_FS
	short	inumber;
# endif
	char	fname[MAXNAME + 3];
};

/*
** DBS crash recovery processor
**	to recover a database you must be the dba or the DBS superuser
**	RESTORE attempts to complete updates from batch files left in a
**	database.  After finishing all the batch files
**	it calls PURGE and SYSMOD.
*/
main(argc, argv)
int	argc;
char	*argv[];
{
	register int		i;
	register char		*dbname;
	register char		**avp;
	register char		**fvp;
	int			stat;
	char			*nargv[20];
	extern char		*AAproc_name;
	extern char		*Flagvect[];
	extern int		exit();
	extern int		rubproc();
	extern char		*lookucode();
	extern int		(*AAexitfn)();
	extern char		*getnxtdb();

	AAproc_name = "RESTORE";

# 	ifdef xSTR1
	AAtTrace(&argc, argv, 'X');
#	endif

	initialize(argc, argv);

	signal(3, exit);
	while (dbname = getnxtdb())
	{
		AAexitfn = exit;
		Berror = Error = 0;
		Pass = 1;

		/* set exclusive lock on data base */
		db_lock(M_EXCL);

		/* do it to it */
		AAexitfn = rubproc;

		/* first restart point for this database */
		setexit();
		if (Error)	/* if set, will cause skip to next database */
			continue;

		AAam_start();

		printf("*************\n");
		printf("** RESTORE **\tDatenbank %s\n", dbname);
		printf("**   DBA   **\t%s\n", lookucode(AAdmin.adhdr.adowner));
		printf("*************\n");

		printf("** Phase %d **\tcheck unvollendete Update's\n", Pass++);
		restore();	/* recover batch update and modify files */

		/*
		** second restart point for this database
		**	the batch files are completed and now the
		**	system catalogs need checking
		*/
		setexit();
		if (Error)		/* again, may cause skipping to next database */
			continue;

		/*
		** check the catalog AA_REL
		**	this will mean checking for file existence,
		**	and whether the relstat bits are supported by
		**	the information in the other catalogs.
		*/
		printf("** Phase %d **\tcheck Katalog %s\n", Pass++, AA_REL);
		checkrel();

		/*
		** check the catalog AA_ATT
		**	for each tuple in the catalog AA_ATT, there must
		**	be a tuple in the catalog AA_REL.
		**	the catalog AA_INDEX doesn't need to be reverse checked
		**	into the catalog AA_REL since the order things are
		**	handled else where in the system is in the correct
		**	order.  All the other catalogs need to be reverse checked.
		*/
		printf("** Phase %d **\tcheck Katalog %s\n", Pass++, AA_ATT);
		checkatts();

		/* only check the qrymod catalogs if qrymod is turned on */
		if (Qrymod)
		{
			/* check the catalog AA_PROTECT */
			printf("** Phase %d **\tcheck Katalog %s\n", Pass++, AA_PROTECT);
			checkprotect();

			/* check the catalog AA_INTEG */
			printf("** Phase %d **\tcheck Katalog %s\n", Pass++, AA_INTEG);
			checkinteg();

			/*
			** check the catalog AA_TREE
			** must be done last since it depends upon
			** a state of the system catalogs provided
			** by the other check... routines.
			*/
			printf("** Phase %d **\tcheck Katalog %s\n", Pass++, AA_TREE);
			checktree();
		}

		/* close AA_USERS */
		getuser((char *) 0);

		/* finished, close up the database and go on to the next */
		closecatalog(TRUE);
		AAunldb();
		AAam_exit();

		/* call PURGE and SYSMOD if no errors */
		if (!Berror && !Error && !No_purge)
		{
			/* PURGE first */
			printf("** Phase %d **\tPURGE:  ", Pass++);
			fflush(stdout);
			if ((i = fork()) == -1)
				printf("fork-Fehler PURGE-Prozess\n");
			else if (!i)
			{
				avp = nargv;
				*avp++ = PURGE;
				for (fvp = Flagvect; *fvp; )
					*avp++ = *fvp++;
				if (Wait_action == A_SLP)
					*avp++ = WAIT;
				*avp++ = dbname;
				*avp++ = (char *) 0;
#				ifdef	xSTR2
				if (AAtTf(0, 1))
					for (avp = nargv, i = 0; *avp; avp++, i++)
						printf("%d %s\n", i, *avp);
#				endif
				for (i = 3; i <= MAXFILES; i++)
					close(i);
				fflush(stdout);
				execvp(PURGE, nargv);
				AAsyserr(0, "exec-Fehler PURGE-Programm %s", PURGE);
			}
			wait(&stat);
		}

			/* then SYSMOD */
		if (!Berror && !Error && !No_sysmod)
		{
			printf("** Phase %d **\tSYSMOD: ", Pass++);
			fflush(stdout);
			if ((i = fork()) == -1)
				printf("fork-Fehler SYSMOD-Prozess\n");
			else if (!i)
			{
				avp = nargv;
				*avp++ = SYSMOD;
				for (fvp = Flagvect; *fvp; fvp++)
					if ((*fvp)[1] == 's')
						*avp++ = *fvp;
				if (Wait_action == A_SLP)
					*avp++ = WAIT;
				*avp++ = dbname;
				*avp++ = (char *) 0;
#				ifdef	xSTR2
				if (AAtTf(0, 1))
					for (avp = nargv, i = 0; *avp; avp++, i++)
						printf("%d %s\n", i, *avp);
#				endif
				for (i = 3; i <= MAXFILES; i++)
					close(i);
				fflush(stdout);
				execvp(SYSMOD, nargv);
				AAsyserr(0, "exec-Fehler SYSMOD-Programm %s", SYSMOD);
			}
			wait(&stat);
		}
	}
}


/*
** RESTORE -- find the batch files and process them
*/
restore()
{
	register struct direc	*d;
# ifdef NON_V7_FS
	register char		*fn;
	extern char		*read_dir();
# else
	register int		dfd;
# endif
	struct direc		dir;
	DESC			descr;
	int			(*tmpfn)();
	extern char		*Fileset;
	extern int		uperr();
	extern int		(*AAexitfn)();

	d = &dir;
# ifdef NON_V7_FS
	if (!open_dir("."))
# else
	if ((dfd = open(".", 0)) < 0)
# endif
		AAsyserr(21080);
	d->fname[MAXNAME + 2] = 0;
	AAbmove(AAusercode, Utemp, 2);
	Batch_recovery = 1;
	tmpfn = AAexitfn;
	AAexitfn = uperr;

	/* restart point */
	setexit();
# ifdef NON_V7_FS
	while (fn = read_dir())
	{
		/* throw out legitimate files */
		if (AAsequal(fn, ".") || AAsequal(fn, ".."))
			continue;
		AAbmove(fn, d->fname, MAXNAME + 2);
# else
	while (read(dfd, d, 16) == 16)
	{
		if (!d->inumber)
			continue;
# endif
		if (AAbequal(SYSBATCH, d->fname, s_SYSBATCH))
		{
			Fileset = &d->fname[s_SYSBATCH];
			Batch_fp = open(batchname(), 0);
			Batch_cnt = BATCHSIZE;
			getbatch(&Batchhd, sizeof(Batchhd));
			printf("%s-UPDATE-\n", Header);
			printf("%sRelation %s\n", Header, Batchhd.rel_name);
			printf("%s  Nutzer %s\n", Header, lookucode(Batchhd.userid));
			close(Batch_fp);
			AAbmove(Batchhd.userid, AAusercode, 2);
			if (ask("vollenden"))
				update();
		}
		if (AAbequal(MODBATCH, d->fname, s_MODBATCH))
		{
			Fileset = &d->fname[s_MODBATCH];
			if ((Batch_fp = open(d->fname, 0)) < 0)
				AAsyserr(21081, d->fname);
			Batch_cnt = BATCHSIZE;
			getbatch(&descr, sizeof(descr));
			printf("%s-MODIFY-\n", Header);
			printf("%sRelation %.12s\n", Header, descr.reldum.relid);
			printf("%s  Nutzer %s\n", Header, lookucode(descr.reldum.relowner));

			AAbmove(descr.reldum.relowner, AAusercode, sizeof(descr.reldum.relowner));
			close(Batch_fp);
			if (ask("vollenden"))
				modupdate();
		}
	}
	AAbmove(Utemp, AAusercode, 2);
	AAexitfn = tmpfn;
# ifdef NON_V7_FS
	close_dir();
# else
	close(dfd);
# endif
}


/*
** handles AAsyserr's in the update processor
*/
uperr()
{
	if (Batch_fp)
		close(Batch_fp);
	Berror++;
	reset();
}


/*
** Catch errors in other places
*/
rubproc()
{
	register int		i;
	extern struct desxx	Desxx[];
	extern int		AAcc_init;

	Error++;
	printf("\n**Interrupt**\n");

	/* restore user code */
	AAbmove(Utemp, AAusercode, sizeof Utemp);

	/* close all possible files */
	if (AAcc_init)
	{
		closecatalog(TRUE);
		AAunldb();
		AAam_exit();
	}

	/* close AA_USERS */
	getuser((char *) 0);

	/* get everything else */
	for (i = Direc + 1; i <= MAXFILES; i++)
		close(i);
}


/*
** looks up user by usercode in AA_USERS
*/
char	*lookucode(ucode)
register char	*ucode;
{
	static char	buf[MAXLINE + 1];
	register char	*p;

	if (getuser(ucode, buf))
		AAsyserr(21083, ucode);
	for (p = buf; *p != ':'; p++)
		continue;
	*p = 0;
	return (buf);
}


/*
** CHECKATTS
**	Checks that all domains are in a catalog AA_ATT
*/
checkatts()
{
	register int			i;
	register int			once;
	register struct attribute	*a;
	struct tup_id			tid;
	struct tup_id			limtid;
	struct tup_id			reltid;
	struct attribute		atttup;
	struct relation			reltup;
	char				lastrel[MAXNAME + 2];
	char				key[MAXTUP];

	once = 0;
	opencatalog(AA_REL, 2);
	opencatalog(AA_ATT, 2);
	AAam_clearkeys(&Attdes);
	lastrel[0] = '\0';
	if (AAam_find(&Attdes, NOKEY, &tid, &limtid))
		AAsyserr(21084);

	while (!(i = AAam_get(&Attdes, &tid, &limtid, a = &atttup, TRUE)))
	{
		if (AAbequal(a->attrelid, lastrel, MAXNAME + 2))
			continue;

		AAam_clearkeys(&Reldes);
		AAam_setkey(&Reldes, key, a->attrelid, ATTRELID);
		AAam_setkey(&Reldes, key, a->attowner, ATTOWNER);

		if (i = AAgetequal(&Reldes, key, &reltup, &reltid))
		{
			if (i < 0)
				AAsyserr(21085);
			if (!once++)
				printf("%sEs gibt keine Relation fuer die Domaene:\n", Header);
			printf("%s", Header);
			AAprtup(&Attdes, a);
			if (ask("Domaene loeschen"))
				if (i = AAam_delete(&Attdes, &tid))
					AAsyserr(21086, i);
		}
		else
			AAbmove(a->attrelid, lastrel, MAXNAME + 2);
	}

	if (i < 0)
		AAsyserr(21087, i);
}


/*
** CHECKREL -- check catalog AA_REL against every thing else
**
**	Each tuple in the catalog AA_REL is read and each verifiable
**	characteristic is checked for accuracy.  Including the existence
**	of the physical file (if not a view), the qrymod definition if
**	appropriate and the secondary indexing.
*/
checkrel()
{
	register int			i;
	register int			j;
	register struct relation	*r;
	struct tup_id			rtid;
	struct tup_id			limtid;
	struct relation			rel;
	char				fname[MAXNAME + 3];

	/* setup for search of entire catalog AA_REL */
	opencatalog(AA_REL, 2);
	AAam_clearkeys(&Reldes);
	if (AAam_find(&Reldes, NOKEY, &rtid, &limtid))
		AAsyserr(21088);

	/* loop until all tuples checked */
	for (r = &rel; ; )
	{
		/* for each tuple in the rel-rel */
		if ((i = AAam_get(&Reldes, &rtid, &limtid, r, TRUE)) > 0)
			break;	/* have finished */
		if (i < 0)
			AAsyserr(21089, i);

		/* if not a view, check for the file */
		if ((r->relstat & S_VIEW) != S_VIEW)
		{
			AAdbname(r->relid, r->relowner, fname);
			if ((j = open(fname, 2)) < 0)
			{
				printf("%sEs gibt kein BS file fuer die Relation:\n", Header);
				printf("%s", Header);
				AAprtup(&Reldes, r);
				if (ask("Relation im Katalog loeschen"))
				{
					if (j = AAam_delete(&Reldes, &rtid))
						AAsyserr(21090, j);
					continue;
				}
				else	/* don't call purge the file might still be there */
					Error++;
			}
			else
				close(j);
		}

		/* does it think that it has a secondary index */
		if (ctoi(r->relindxd) == SECBASE)
		{
			/* does it really have an index? */
			if (!hasndx(r->relid, r->relowner))
			{
				/* no, should it be fixed */
				printf("%sEs gibt keinen Index fuer die Primaerrelation:\n", Header);
				printf("%s", Header);
				AAprtup(&Reldes, r);
				if (ask("INDEX-Status der Relation loeschen"))
				{
					/* fix up catalog AA_REL entry */
					r->relindxd = 0;
					if (i = AAam_replace(&Reldes, &rtid, r, FALSE))
						AAsyserr(21091, i);
				}
			}
		}

		/* does it think that it is a secondary index */
		if (ctoi(r->relindxd) == SECINDEX)
		{
			/* check to make sure */
			if (!isndx(r->relid, r->relowner))
			{
				/* none, what should be done? */
				printf("%sEs gibt keinen Eintrag im Katalog %s\n", Header, AA_INDEX);
				printf("%s\tfuer die Indexrelation:\n", Header);
				printf("%s", Header);
				AAprtup(&Reldes, r);
				if (ask("Indexrelation loeschen"))
				{
					/*
					** get rid of rel-rel tuple for
					** secondary index,
					** purge will do rest of
					** removal if necessary
					*/
					if (i = AAam_delete(&Reldes, &rtid))
						AAsyserr(21090, i);
					continue;	/* go on to next tuple */
				}
			}
		}

		/* if qrymod on in the database, check those catalogs too */
		if (Qrymod)
		{
			/*
			** cannot deal with S_VBASE since there is no way to
			** find the catalog AA_TREE entries without decoding the
			** 'treetree' fields.
			**
			** check to see if this is a view
			*/
			if ((r->relstat & S_VIEW) && !havetree(r->relid, r->relowner, mdVIEW))
			{
				/* no entry, should it be fixed? */
				printf("%sEs gibt keinen Eintrag im Katalog %s\n", Header, AA_TREE);
				printf("%s\tfuer die Sicht (view):\n", Header);
				printf("%s", Header);
				AAprtup(&Reldes, r);
				if (ask("Sicht (view) loeschen"))
				{
					/* delete catalog AA_REL entry */
					if (i = AAam_delete(&Reldes, &rtid))
						AAsyserr(21090, i);
					continue;	/* skip to next entry in rel-rel */
				}
			}

			/* check to see if has catalog AA_PROTECT entry */
			if ((r->relstat & S_PROTUPS) && !isprot(r->relid, r->relowner, -1))
			{
				/* no entry, should the bit be reset */
				printf("%sEs gibt keinen Eintrag im Katalog %s\n", Header, AA_PROTECT);
				printf("%s\tfuer die Relation:\n", Header);
				printf("%s", Header);
				AAprtup(&Reldes, r);
				if (ask("PROTECT-Status der Relation loeschen"))
				{
					/* fix the bit */
					r->relstat &= ~S_PROTUPS;
					if (i = AAam_replace(&Reldes, &rtid, r, FALSE))
						AAsyserr(21091, i);
				}
			}

			/* check to see if has AA_INTEG entry */
			if ((r->relstat & S_INTEG) && !isinteg(r->relid, r->relowner, -1))
			{
				/* no entry, should bit be reset */
				printf("%sEs gibt keinen Eintrag im Katalog %s\n", Header, AA_INTEG);
				printf("%s\tfuer die Relation:\n", Header);
				printf("%s", Header);
				AAprtup(&Reldes, r);
				if (ask("INTEG-Status der Relation loeschen"))
				{
					/* fix up the bit */
					r->relstat &= ~S_INTEG;
					if (i = AAam_replace(&Reldes, &rtid, r, FALSE))
						AAsyserr(21091, i);
				}
			}
		}
	}
}


/*
** HASNDX -- the relation indicated an index, check it out
**
**	will search the catalog AA_INDEX for all secondary indexes
**	and check to see that each secondary index named has an
**	entry in the catalog AA_REL.
*/
hasndx(id, own)
char	id[MAXNAME];
char	own[2];
{
	register int		hasindexes;
	register int		i;
	register int		j;
	struct tup_id		rtid;
	struct relation		rkey;
	struct relation		rel;
	struct tup_id		itid;
	struct tup_id		ihitid;
	struct index		ikey;
	struct index		ind;

	/* presume that answer is negative */
	hasindexes = FALSE;

	/* set search for all tuples with 'id' and 'own' in catalog AA_INDEX */
	opencatalog(AA_INDEX, 2);
	AAam_clearkeys(&Inddes);
	AAam_setkey(&Inddes, &ikey, id, IRELIDP);
	AAam_setkey(&Inddes, &ikey, own, IOWNERP);
	if (AAam_find(&Inddes, EXACTKEY, &itid, &ihitid, &ikey))
		AAsyserr(21096);

	/* for each possible tuple in the catalog AA_INDEX */
	for ( ; ; )
	{
		i = AAam_get(&Inddes, &itid, &ihitid, &ind, TRUE);

		/* check return values */
		if (i < 0)
			AAsyserr(21097, i);
		if (i > 0)
			break;	/* finished */

		/* if key doesn't match, skip to next tuple */
		if (AAkcompare(&Inddes, &ikey, &ind))
			continue;
		hasindexes = TRUE;

		/* verify that primary entry for sec index exists */
		opencatalog(AA_REL, 2);
		AAam_clearkeys(&Reldes);
		AAam_setkey(&Reldes, &rkey, ind.irelidi, RELID);
		AAam_setkey(&Reldes, &rkey, ind.iownerp, RELOWNER);
		if (j = AAgetequal(&Reldes, &rkey, &rel, &rtid, FALSE))
		{
			/* one doesn't exist, should we ignore it */
			if (j < 0)
				AAsyserr(21098, j);
			printf("%sEs gibt keinen Eintrag im Katalog %s\n", Header, AA_REL);
			printf("%s\tfuer die Indexrelation:\n", Header);
			printf("%s", Header);
			AAprtup(&Inddes, &ind);
			if (ask("Indexrelation loeschen"))
			{
				/* get rid of bad entry in catalog AA_INDEX */
				if (j = AAam_delete(&Inddes, &itid))
					AAsyserr(21099, j);
				hasindexes = FALSE;
			}
		}
	}
	return (hasindexes);
}


/*
** ISNDX -- so you think that you're a secondary index, I'll check it out.
**
**	searches the catalog AA_INDEX for the name of the primary relation
**	and check to see if the primary is real.  Will also update the
**	'relindxd' field of the primary if it isn't correct.
*/
isndx(id, own)
char	id[MAXNAME];
char	own[2];
{
	register int		isindex;
	register int		i;
	struct tup_id		itid;
	struct index		ind;
	struct index		ikey;
	struct tup_id		rtid;
	struct relation		rel;
	struct relation		rkey;

	/* search for tuple in catalog AA_INDEX, should only be one */
	opencatalog(AA_INDEX, 2);
	AAam_clearkeys(&Inddes);
	AAam_setkey(&Inddes, &ikey, id, IRELIDI);
	AAam_setkey(&Inddes, &ikey, own, IOWNERP);
	if (i = AAgetequal(&Inddes, &ikey, &ind, &itid))
	{
		/* there isn't a tuple in the catalog AA_INDEX */
		if (i < 0)
			AAsyserr(21100, i);
		isindex = FALSE;
	}
	else
	{
		isindex = TRUE;

		/* there is a tuple in the catalog AA_INDEX */
		opencatalog(AA_REL, 2);
		AAam_clearkeys(&Reldes);
		AAam_setkey(&Reldes, &rkey, ind.irelidp, RELID);
		AAam_setkey(&Reldes, &rkey, ind.iownerp, RELOWNER);

		/* see if the primary relation exists */
		if (i = AAgetequal(&Reldes, &rkey, &rel, &rtid))
		{
			/* no it doesn't */
			if (i < 0)
				AAsyserr(21100, i);

			/* what should be done about it */
			printf("%sEs gibt keine Primaerrelation fuer die Indexrelation:\n", Header);
			printf("%s", Header);
			AAprtup(&Inddes, &ind);
			if (ask("Indexrelation loeschen"))
			{
				/*
				** get rid of catalog AA_INDEX tuple,
				** a FALSE return will also get rid
				** of the catalog AA_REL tuple
				*/
				if (i = AAam_delete(&Inddes, &itid))
					AAsyserr(21102, i);
				isindex = FALSE;
			}
		}
		else if (!(ctoi(rel.relindxd) == SECBASE) || (rel.relstat & S_INDEX) == S_INDEX)
		{
			/*
			** the primary tuple exists but isn't marked correctly
			*/
			printf("%s%.12s ist ein Index der Primaerrelation:\n", Header, rel.relid);
			printf("%s", Header);
			AAprtup(&Reldes, &rel);
			if (ask("INDEX-Status der Primaerrelation setzen"))
			{
				rel.relstat |= S_INDEX;
				rel.relindxd = SECBASE;
				if (i = AAam_replace(&Reldes, &rtid, &rel, FALSE))
					AAsyserr(21103, i);
			}
		}
	}
	return (isindex);
}


/*
** HAVETREE -- check AA_TREE catalog for an entry with right name and owner
**
**	The 'id' and 'own' parameters are used to look in the AA_TREE catalog
**	for at least on tuple that also has a 'treetype' of 'mdvalue'.
**
**	If any tuples are found, havetree returns TRUE, else FALSE
*/
havetree(id, own, mdvalue)
char	id[MAXNAME];
char	own[2];
char	mdvalue;
{
	register int		i;
	register struct tree	*key;
	register struct tree	*ent;
	struct tree		trkey;
	struct tree		trent;
	struct tup_id		trtid;
	struct tup_id		trhitid;

	/* search catalog AA_TREE for tuple that matches */
	opencatalog(AA_TREE, 2);
	AAam_clearkeys(&Treedes);
	AAam_setkey(&Treedes, key = &trkey, id, TREERELID);
	AAam_setkey(&Treedes, key, own, TREEOWNER);
	AAam_setkey(&Treedes, key, &mdvalue, TREETYPE);

	/* set search limit tids from the key */
	if (i = AAam_find(&Treedes, EXACTKEY, &trtid, &trhitid, key))
		AAsyserr(21104, i);

	for (ent = &trent; ; )
	{
		if ((i = AAam_get(&Treedes, &trtid, &trhitid, ent, TRUE)) < 0)
			AAsyserr(21105, i);

		if (i > 0)
			break;	/* finished, didn't find one */

		if (!AAkcompare(&Treedes, key, ent))
			return (TRUE);
	}
	return (FALSE);
}


/*
** ISPROT -- check in the AA_PROTECT catalog for a tuple with right name, owner
**
**	search the AA_PROTECT catalog for at least on tuple with matches the
**	values in the parameters. If 'treeid' is >= 0 then it is not used as
**	a key.
**
**	if one is found, returns TRUE, otherwise, returns FALSE
*/
isprot(id, own, treeid)
char	id[MAXNAME];
char	own[2];
short	treeid;
{
	register int		i;
	register struct protect	*key;
	register struct protect	*ent;
	struct protect		prkey;
	struct protect		prent;
	struct tup_id		prtid;
	struct tup_id		prhitid;

	/* search the catalog AA_PROTECT for at least on matching tuple */
	opencatalog(AA_PROTECT, 2);
	AAam_clearkeys(&Prodes);
	AAam_setkey(&Prodes, key = &prkey, id, PRORELID);
	AAam_setkey(&Prodes, key, own, PRORELOWN);
	if (treeid >= 0)
		AAam_setkey(&Prodes, key, &treeid, PROTREE);

	/* set search limit tids from the keys */
	if (i = AAam_find(&Prodes, EXACTKEY, &prtid, &prhitid, key))
		AAsyserr(21106, i);

	for (ent = &prent; ; )
	{
		if ((i = AAam_get(&Prodes, &prtid, &prhitid, ent, TRUE)) < 0)
			AAsyserr(21107, i);

		if (i > 0)
			break;	/* finished, didn't find one */

		if (!AAkcompare(&Prodes, key, ent))
			return (TRUE);
	}
	return (FALSE);
}


/*
** ISINTEG -- check for a tuple in catalog AA_INTEG
**
**	searches the catalog AA_INTEG for 'id' and 'own'.
**
**	returns TRUE if one is found, else FALSE
*/
isinteg(id, own, treeid)
char	id[MAXNAME];
char	own[2];
int	treeid;
{
	register int			i;
	register struct integrity	*key;
	register struct integrity	*ent;
	struct integrity		inkey;
	struct integrity		integ;
	struct tup_id			intid;
	struct tup_id			inhitid;

	/* search the entire catalog AA_INTEG for a tuple that matches */
	opencatalog(AA_INTEG, 2);
	AAam_clearkeys(&Intdes);
	AAam_setkey(&Intdes, key = &inkey, id, INTRELID);
	AAam_setkey(&Intdes, key, own, INTRELOWNER);
	if (treeid >= 0)
		AAam_setkey(&Intdes, key, &treeid, INTTREE);

	/* set the search limit tids from the key */
	if (i = AAam_find(&Intdes, EXACTKEY, &intid, &inhitid, key))
		AAsyserr(21108, i);

	for (ent = &integ; ; )
	{
		if ((i = AAam_get(&Intdes, &intid, &inhitid, ent, TRUE)) < 0)
			AAsyserr(21109, i);

		if (i > 0)
			break;	/* finished, didn't find one */

		if (!AAkcompare(&Intdes, key, ent))
			return (TRUE);
	}
	return (FALSE);
}


/*
** CHECKTREE -- check the AA_TREE catalog against the others
*/
checktree()
{
	register int			i;
	register struct tree		*ent;
	register struct relation	*r;
	register struct relation	*key;
	struct tree			trent;
	struct tup_id			trtid;
	struct tup_id			trhitid;
	struct relation			rkey;
	struct relation			rel;
	struct tup_id			rtid;

	/* search the entire AA_TREE catalog */
	opencatalog(AA_TREE, 2);
	AAam_clearkeys(&Treedes);
	if (i = AAam_find(&Treedes, NOKEY, &trtid, &trhitid))
		AAsyserr(21110, i);

	/* for each tuple in catalog AA_TREE */
	for (ent = &trent, key = &rkey, r = &rel; ; )
	{
		if ((i = AAam_get(&Treedes, &trtid, &trhitid, ent, TRUE)) > 0)
			break;	/* finished */

		if (i < 0)
			AAsyserr(21111, i);

		/* verify that a tuple exists in the catalog AA_REL */
		opencatalog(AA_REL, 2);
		AAam_clearkeys(&Reldes);
		AAam_setkey(&Reldes, key, ent->treerelid, RELID);
		AAam_setkey(&Reldes, key, ent->treeowner, RELOWNER);

		/* fetch the tuple */
		if (i = AAgetequal(&Reldes, key, r, &rtid))
		{
			/*
			** Oops, a tuple doesn't exist in the catalog AA_REL.
			**
			** Maybe it's just a fatal error
			*/
			if (i < 0)
				AAsyserr(21112, i);

			/* not a fatal error, what to do about it? */
			printf("%sEs gibt keinen Eintrag im Katalog %s\n", Header, AA_REL);
			printf("%s\tfuer die Definition:\n", Header);
			printf("%s", Header);
			AAprtup(&Treedes, ent);
			if (ask("Definition loeschen"))
			{
				if (i = AAam_delete(&Treedes, &trtid))
					AAsyserr(21113, i);
				continue;	/* go on to next tuple */
			}
		}
		else
		{
			/*
			** Ah. A tuple does exist.
			**
			** If the relstat bits are correct then we can stop
			** here since elsewhere the catalog AA_PROTECT and AA_INTEG
			** entries were verified.
			*/
			switch (ent->treetype)
			{
			  case mdVIEW:
				/* mere existence is sufficient */
				break;

			  case mdPROT:
				if ((r->relstat & S_PROTUPS) != S_PROTUPS)
				{
					printf("%sDie Relation %.12s hat den PROTECT-Status nicht gesetzt\n", Header,
						r->relid);
deltup:
					printf("%s\tfuer die Definition:\n", Header);
					printf("%s", Header);
					AAprtup(&Treedes, ent);
					if (ask("Definition loeschen"))
					{
						if (i = AAam_delete(&Treedes, &trtid))
							AAsyserr(21113, i);
						continue;
					}
				}
				break;

			  case mdINTEG:
				if ((r->relstat & S_INTEG) != S_INTEG)
				{
					printf("%sDie Relation %.12s hat den INTEG-Status nicht gesetzt\n", Header,
						r->relid);
					goto deltup;
				}
				break;

			  default:
				AAsyserr(21115, ent->treetype);
			}
		}
	}
}


checkprotect()
{
	register int			i;
	register struct protect		*ent;
	register struct relation	*r;
	register struct relation	*key;
	struct protect			prent;
	struct tup_id			prtid;
	struct tup_id			prhitid;
	struct relation			rkey;
	struct relation			rel;
	struct tup_id			rtid;

	/* for each entry in the catalog AA_PROTECT */
	opencatalog(AA_PROTECT, 2);
	AAam_clearkeys(&Prodes);
	if (i = AAam_find(&Prodes, NOKEY, &prtid, &prhitid))
		AAsyserr(21116, i);

	for (ent = &prent, key = &rkey, r = &rel; ; )
	{
		if ((i = AAam_get(&Prodes, &prtid, &prhitid, ent, TRUE)) > 0)
			break;	/* finished */

		if (i < 0)
			AAsyserr(21117, i);

		/* verify that a tuple exists in catalog AA_REL */
		opencatalog(AA_REL, 2);
		AAam_clearkeys(&Reldes);
		AAam_setkey(&Reldes, key, ent->prorelid, RELID);
		AAam_setkey(&Reldes, key, ent->prorelown, RELOWNER);

		/* fetch the tuple if possible */
		if (i = AAgetequal(&Reldes, key, r, &rtid))
		{
			/*
			** Oops.  A tuple doesn't exits in catalog AA_REL
			**
			** Maybe it's just a fatal error.
			*/
			if (i < 0)
				AAsyserr(21118, i);

			/* not a fatal error, what to do? */
			printf("%sEs gibt keinen Eintrag im Katalog %s\n", Header, AA_REL);
			printf("%s\tfuer das Zugriffsrecht:\n", Header);
			printf("%s", Header);
			AAprtup(&Prodes, ent);
			if (ask("Zugriffsrechte loeschen"))
			{
				if (i = AAam_delete(&Prodes, &prtid))
					AAsyserr(21119, i);
				continue;	/* go on to next tuple */
			}
		}
		else
		{
			/* AA_REL entry exists, check for the AA_TREE entry */
			if (ent->protree >= 0)
			{
				if (!havetree(ent->prorelid, ent->prorelown, mdPROT))
				{
					/* no tuples in catalog AA_TREE */
					printf("%sEs gibt keinen Eintrag im Katalog %s\n", Header, AA_TREE);
					printf("%sfuer die Zugriffsrechte:\n", Header);
					printf("%s", Header);
					AAprtup(&Prodes, ent);
					if (ask("Zugriffsrechte und PROTECT-Status der Relation loeschen"))
					{
						if (i = AAam_delete(&Prodes, ent))
							AAsyserr(21119, i);
						r->relstat &= ~S_PROTUPS;
						if (i = AAam_replace(&Reldes, &rtid, r, FALSE))
							AAsyserr(21121, i);
						continue;	/* go on to next tuple */
					}
				}
			}
			if ((r->relstat & S_PROTUPS) != S_PROTUPS)
			{
				/* bits not set correctly */
				printf("%sPROTECT-Status der Relation nicht gesetzt\n", Header);
				printf("%s", Header);
				AAprtup(&Reldes, r);
				if (ask("PROTECT-Status der Relation setzen"))
				{
					r->relstat |= S_PROTUPS;
					if (i = AAam_replace(&Reldes, &rtid, r, FALSE))
						AAsyserr(21121, i);
					continue;	/* go on to next tuple */
				}
			}
		}
	}
}


checkinteg()
{
	register int			i;
	register struct integrity	*ent;
	register struct relation	*r;
	register struct relation	*key;
	struct integrity		inent;
	struct tup_id			intid;
	struct tup_id			inhitid;
	struct relation			rkey;
	struct relation			rel;
	struct tup_id			rtid;

	/* for each entry in catalog AA_INTEG */
	opencatalog(AA_INTEG, 2);
	AAam_clearkeys(&Intdes);
	if (i = AAam_find(&Intdes, NOKEY, &intid, &inhitid))
		AAsyserr(21123, i);

	for (ent = &inent, key = &rkey, r = &rel; ; )
	{
		if ((i = AAam_get(&Intdes, &intid, &inhitid, ent, TRUE)) > 0)
			break;	/* finished */

		if (i < 0)
			AAsyserr(21124, i);

		/* verify that a tuple exists in catalog AA_REL */
		opencatalog(AA_REL, 2);
		AAam_clearkeys(&Reldes);
		AAam_setkey(&Reldes, key, ent->intrelid, RELID);
		AAam_setkey(&Reldes, key, ent->intrelowner, RELOWNER);

		/* fetch the tuple if possible */
		if (i = AAgetequal(&Reldes, key, r, &rtid))
		{
			/*
			** Oops.  A tuple doesn't exits in catalog AA_REL
			**
			** Maybe it's just a fatal error.
			*/
			if (i < 0)
				AAsyserr(21125, i);

			/* not a fatal error, what to do? */
			printf("%sEs gibt keinen Eintrag im Katalog %s\n", Header, AA_REL);
			printf("%sfuer die Integritaetsdefinition:\n", Header);
			printf("%s", Header);
			AAprtup(&Intdes, ent);
			if (ask("Integritaetsdefinition loeschen"))
			{
				if (i = AAam_delete(&Intdes, &intid))
					AAsyserr(21126, i);
				continue;	/* go on to next tuple */
			}
		}
		else
		{
			/* AA_REL entry exists, check for the AA_TREE entry */
			if (ent->inttree >= 0)
			{
				if (!havetree(ent->intrelid, ent->intrelowner, mdINTEG))
				{
					/* no tuples in catalog AA_TREE */
					printf("%sEs gibt keinen Eintrag im Katalog %s\n", Header, AA_TREE);
					printf("%sfuer die Integritaetsdefinition:\n", Header);
					printf("%s", Header);
					AAprtup(&Intdes, ent);
					if (ask("Integritaetsdefinition und INTEG-Status der Relation loeschen"))
					{
						if (i = AAam_delete(&Intdes, ent))
							AAsyserr(21126, i);
						r->relstat &= ~S_INTEG;
						if (i = AAam_replace(&Reldes, &rtid, r, FALSE))
							AAsyserr(21128, i);
						continue;	/* go on to next tuple */
					}
				}
			}
			if ((r->relstat & S_INTEG) != S_INTEG)
			{
				/* bits not set correctly */
				printf("%sINTEG-Status der Relation nicht gesetzt\n", Header);
				printf("%s", Header);
				AAprtup(&Reldes, r);
				if (ask("INTEG-Status der Relation setzen"))
				{
					r->relstat |= S_INTEG;
					if (i = AAam_replace(&Reldes, &rtid, r, FALSE))
						AAsyserr(21128, i);
					continue;	/* go on to next tuple */
				}
			}
		}
	}
}
