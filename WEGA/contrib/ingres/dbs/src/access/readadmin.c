# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/access.h"
# include	"../h/aux.h"

# ifdef VPGSIZE
int		AApgsize = PGSIZE;
int		AAmaxtup = MAXTUP;
int		AAltsize = 0;
# endif

/*
**  AAreadadmin -- read AA_ADMIN file into 'AAdmin' cache
**
**	The AA_ADMIN file in the current directory is opened and read
**	into the 'AAdmin' cache.  The AA_ADMIN file contains the following
**	information:
**
**	A header block, containing the owner of the database (that is,
**	the DBA), and a set of status bits for the database as a whole.
**	These bits are defined in h/aux.h.
**
**	Descriptors for the AA_REL and AA_ATT relations.  These
**	descriptors should be completely correct except for the
**	relfp and relopn fields.  These are required so that the
**	process of opening a relation is not recursive.
**
**	After the AA_ADMIN file is read in, the relation and attribute
**	files are opened, and the relfp and relopn fields in both
**	descriptors are correctly initialized.  Both catalogs are
**	opened read/write.
**
**	Finally, the AA_REL catalog is scanned for the entries
**	of both catalogs, and the reltid field in both descriptors
**	are filled in from this.  This is because of a bug in an
**	earlier version of creatdb, which failed to initialize
**	these fields correctly.  In fact, it is critical that these
**	fields are correct in this implementation, since the reltid
**	field is what uniquely identifies the cache buffer in the
**	low-level routines.
**
**	WARNING:  This routine is redefined by creatdb. If this
**		routine is changed, check these program also!!
**
**	Side Effects:
**		The 'AAdmin' struct is filled in from the AA_ADMIN file
**			in the current directory.
**		The 'relation....xx' and 'attribute...xx' files are
**			opened.
**
**	Files:
**		./admin
**			The bootstrap description of the database,
**			described above.
*/
AAreadadmin()
{
	register int		i;
	register int		retval;
	char			relname[MAXNAME + 4];
	extern char		*AArelpath();

	/* read the stuff from the AA_ADMIN file */
	if ((i = open(AArelpath(AA_ADMIN), 0)) < 0)
		AAsyserr(10020);
	if ((retval = read(i, &AAdmin, sizeof AAdmin)) != sizeof AAdmin)
		AAsyserr(10021, retval, sizeof AAdmin);
	close(i);

	/* open the physical files for AA_REL and AA_ATT */
	AAdbname(AAdmin.adreld.reldum.relid, AAdmin.adreld.reldum.relowner, relname);
	if ((AAdmin.adreld.relfp = open(AArelpath(relname), 2)) < 0)
		AAsyserr(10022);

	AAdbname(AAdmin.adattd.reldum.relid, AAdmin.adattd.reldum.relowner, relname);
	if ((AAdmin.adattd.relfp = open(AArelpath(relname), 2)) < 0)
		AAsyserr(10023);
	AAdmin.adreld.relopn = RELWOPEN(AAdmin.adreld.relfp);
	/* we just want to read here create, modify and destroy fix it up */
	AAdmin.adattd.relopn = RELROPEN(AAdmin.adattd.relfp);

	/* get the page size of data base */
	if ((i = (AAdmin.adhdr.adflags >> 8) + 1) > PGBLKS)
		AAsyserr(10010, i, PGBLKS);

# ifdef VPGSIZE
	AApgsize = BLKSIZ * i;

	/* get the linetab size, the difference */
	AAltsize = (PGSIZE - AApgsize) / sizeof (short);
# endif
}
