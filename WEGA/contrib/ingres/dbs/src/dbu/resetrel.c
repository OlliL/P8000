# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/access.h"
# include	"../h/batch.h"

/*
**	RESETREL -- will change a relation to an empty heap.  This is only
**		to be used on temporary relations and should only be called
**		by the DECOMP process.
*/

resetrel(pc, pv)
int	pc;
char	**pv;

{
	extern DESC			Reldes;
	DESC				desc;
	char				relname[MAXNAME + 4];
	long				lnum;

	opencatalog(AA_REL, 2);
	while (*pv != (char *) -1)
	{
		if (AArelopen(&desc, -1, *pv))
			AAsyserr(11133, *pv);
		if (!AAbequal(AAusercode, desc.reldum.relowner, sizeof desc.reldum.relowner))
			AAsyserr(11134, *pv);
		AAdbname(desc.reldum.relid, desc.reldum.relowner, relname);
		if ((desc.relfp = creat(relname, FILEMODE)) < 0)
			AAsyserr(11135, relname);
		lnum = 1;
		if (formatpg(&desc, lnum))
			AAsyserr(11136, relname);
		desc.reldum.reltups = 0;
		desc.reldum.relspec = M_HEAP;
		desc.reldum.relprim = 1;
		close(desc.relfp);
		if (AAam_replace(&Reldes, &desc.reltid, &desc, FALSE) < 0)
			AAsyserr(11137, relname);
		pv++;
	}
	return (0);
}
