# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/lock.h"
# include	"../h/bs.h"

# ifdef P8000
ret_buf		AAjmp_buf;
# else
jmp_buf		AAjmp_buf;
# endif

char		All;
char		Superuser;
char		Ask;
char		Purge;
char		Clean;
extern int	Wait_action;
extern int	AAdbstat;
extern char	*AAusercode;
long		Today;
char		SYSNAME[]	= "_SYS";
int		s_SYSNAME	= sizeof SYSNAME - 1;
extern DESC	Reldes;

struct directory
{
	short	inumber;
	char	fname[14];
	char	null;
};

/*
**  PURGE DATABASE
**
**	This stand-alone routine cleans up a database.  This includes:
**
**	- Destroy temporary relations, i.e., relations with names
**		beginning with SYSNAME.
**	- Destroy expired relations
**	- Clean out junk files, such as core, etc.
**	- As a suggested future expansion, reformat relations which
**		have many overflow pages, or heaps with lots of old
**		deleted tuples, etc.
**
**	It may be called by the DBS superuser or by the dba of
**	a database.  There are two modes.  The first is where databases
**	to be purged are explicitly named.  If none are named, then
**	all databases owned by the particular user (or all databases
**	if the DBS superuser) are purged.
**
**	Flags:
**	-p	enable the purge feature, i.e., clean out expired
**		relations as well as temporary relations.
**	-s	attempt to run in superuser mode.  The user must be
**		login USERDBS for this to succeed.
**	-a	ask the user before each database.
**	-f	clean out rather than report junk files.
*/
main(argc, argv)
int	argc;
char	*argv[];
{
	register char	*db;
	register int	ret;
	extern char	*getnxtdb();

#	ifdef xSTR1
	AAtTrace(&argc, argv, 'X');
#	endif

	/* set up arguments and operating modes */
	initialize(argc, argv);
	time(&Today);
#	ifdef	xSTR2
	AAtTfp(10, 2, "AAusercode: %.2s\n", AAusercode);
#	endif

	ret = 0;
	while (db = getnxtdb())
		ret += purgedb(db);
	fflush(stdout);
	exit(ret);
}


rubproc()
{
	AAunlall();
	fflush(stdout);
	exit(-1);
}


/*
**  PURGE DATABASE
**
**	The database is purged of temporaries, expired relations, and
**	junk.
*/
purgedb(db)
register char	*db;
{
	struct relation		rel;
	struct relation		key;
	struct tup_id		rtid;
	struct tup_id		rlimtid;
	register int		i;
# ifdef NON_V7_FS
	extern char		*read_dir();
	register char		*direc;
# else
	struct directory	direc;
	register int		fd;
	extern long		lseek();
# endif
	int			ret;
	char			*pv[2];
	char			pbuff[MAXNAME + 3];

	ret = 0;
#	ifdef	xSTR2
	AAtTfp(11, 0, "entered purgedb(%s)\n", db);
#	endif
	printf("Datenbank %s\n", db);
	AAam_start();

	/* set exclusive lock on data base */
#	ifdef	xSTR2
	AAtTfp(11, 1, "calling db_lock(%d)\n", M_EXCL);
#	endif
	db_lock(M_EXCL);

	/* open the relation relation for read-write */
	opencatalog(AA_REL, 2);

	if (AAam_find(&Reldes, NOKEY, &rtid, &rlimtid))
	{
		printf("Datenbank %s: Fehler beim Eroeffnen des Katalogs \"relation\"\n", db);
		ret++;
		goto go_away;
	}

	while (!AAam_get(&Reldes, &rtid, &rlimtid, &rel, 1))
	{
		i = 0;

		/* check for temp rel */
		if (AAbequal(rel.relid, SYSNAME, s_SYSNAME))
			i++;

		/* check for expired */
		else if (rel.relsave < Today && rel.relsave)
		{
			printf("**\t\tRelation %.14s: verfallen\n", rel.relid);
			if (Purge)
				if (ask("Relation loeschen"))
					i++;
		}

		/* if this relation should be purged -- call destroy */
		if (i)
		{
			printf("**\t\t purging %.14s\n", rel.relid);

			/* set up parameter vector for destroy */
			AAbmove(rel.relid, pbuff, MAXNAME + 2);
			pbuff[MAXNAME + 2] = '\0';
			pv[0] = pbuff;
			pv[1] = (char *) -1;
			if (destroy(1, pv))
				AAsyserr(21079, pv[0]);
			closecatalog(FALSE);	/* to flush */
		}
	}

	/* open the directory to check for extra files */
# ifdef NON_V7_FS
	if (!open_dir("."))
	{
		printf("Fehler beim Eroeffnen des directory %s/.\n");
		ret++;
		goto go_away;
	}

	/* scan the directory */
	while (direc = read_dir())
	{
		/* throw out legitimate files */
		if (AAsequal(direc, ".") || AAsequal(direc, "..") || AAsequal(direc, AA_ADMIN))
			continue;

		/* always purge SYSNAME files */
		if (!AAbequal(direc, SYSNAME, s_SYSNAME))
		{
			/* it might be a relation */
			AAam_clearkeys(&Reldes);
			AAam_setkey(&Reldes, &key, direc, RELID);
			AAam_setkey(&Reldes, &key, &direc[MAXNAME], RELOWNER);
			if (AAgetequal(&Reldes, &key, &rel, &rtid) <= 0)
			{
				/* it is a relation (or should be saved) */
				continue;
			}

			/* it is a funny file!!! */
			printf("**\t\tfile\t %s\n", direc);
			if (!Clean)
				continue;
		}

		/* purge the file */
		if (ask("file loeschen") && unlink(direc))
			printf("**\t\tFehler beim Loeschen des file %s\n", direc);
	}
	close_dir();
# else
	if ((fd = open(".", 0)) < 0)
	{
		printf("Fehler beim Eroeffnen des directory %s/.\n");
		ret++;
		goto go_away;
	}
	direc.null = 0;
	lseek(fd, 32L, 0);

	/* scan the directory */
	while (read(fd, &direc, 16) == 16)
	{
		/* throw out null entries */
		if (!direc.inumber)
			continue;

		/* throw out legitimate files */
		if (AAsequal(direc.fname, AA_ADMIN))
			continue;

		/* always purge SYSNAME files */
		if (!AAbequal(direc.fname, SYSNAME, s_SYSNAME))
		{
			if (direc.fname[13])
			{
# ifdef VENIX
				AAbmove(direc.fname, pbuff, MAXNAME + 3);
				for (i = 0; i < MAXNAME + 2; i++)
					if (direc.fname[i] == '#')
						direc.fname[i] = ' ';
# endif
				/* it might be a relation */
				AAam_clearkeys(&Reldes);
				AAam_setkey(&Reldes, &key, direc.fname, RELID);
				AAam_setkey(&Reldes, &key, &direc.fname[MAXNAME], RELOWNER);
				if (AAgetequal(&Reldes, &key, &rel, &rtid) <= 0)
					/* it is a relation (or should be saved) */
					continue;
# ifdef VENIX
				AAbmove(pbuff, direc.fname, MAXNAME + 3);
# endif
			}

			/* it is a funny file!!! */
			printf("**\t\tfile\t %s\n", direc.fname);
			if (!Clean)
				continue;
		}

		/* purge the file */
		if (ask("file loeschen") && unlink(direc.fname))
			printf("**\t\tFehler beim Loeschen des file %s\n",
				direc.fname);
	}
	close(fd);
# endif
go_away:
	closecatalog(TRUE);	/* close catalogs */
	AAunlall();		/* unlock the database */
	AAam_exit();
	return (ret);
}
