# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/catalog.h"
# include	"../h/access.h"
# include	"../h/lock.h"

/*
**	This is the DBU routine INDEX
**
**  pc = # of parameters
**  pv[0] points to primary relation name
**  pv[1] points to index relation name
**  pv[2] points to domain1
**  pv[3] points to domain2
**  .
**  .
**  .
**  pv[pc] = -1
**
*/

struct dom
{
	int	id;
	int	off;
	int	frml;
	char	frm[5];
};

# ifdef STACK
static char		primtup[MAXTUP];
static char		systup[MAXTUP];
static DESC		desc;
static DESC		pridesc;
# endif

sec_index(pc, pv)
int	pc;
char	**pv;
{
	register int			i;
	int				j;
	register struct dom		*dom;
	register char			**p;
	char				*primary;
	char				*indx;
	int				ndoms;
	int				newpc;
	struct tup_id			tid;
	struct tup_id			hitid;
	struct tup_id			xtid;
	char				*newpv[MAXKEYS * 2 + 4];
# ifndef STACK
	char				primtup[MAXTUP];
	char				systup[MAXTUP];
	DESC				desc;
	DESC				pridesc;
# endif
	extern DESC			Reldes;
	extern DESC			Attdes;
	extern DESC			Inddes;
	struct relation			relkey;
	struct relation			reltup;
	struct attribute		attkey;
	struct attribute		atttup;
	struct index			indtup;
	struct dom			domain[MAXKEYS];

	primary = pv[0];
	indx = pv[1];
#	ifdef xZTR1
	AAtTfp(5, -1, "INDEX: (pri %s ind %s)\n", primary, indx);
#	endif
	i = AArelopen(&pridesc, 0, primary);
	if (i == AMOPNVIEW_ERR)
		return (error(5306, primary, (char *) 0));
	if (i > 0)
		return (error(5300, primary, (char *) 0));
	if (i < 0)
		AAsyserr(11040, primary, i);

	if (!AAbequal(pridesc.reldum.relowner, AAusercode, 2))
	{
		i = 5303;
	}
	else if (pridesc.reldum.relstat & S_CATALOG)
	{
		i = 5305;
	}
	else if (ctoi(pridesc.reldum.relindxd) == SECINDEX)
	{
		i = 5304;
	}

	if (i)
	{
		AArelclose(&pridesc);
		return (error(i, primary, (char *) 0));
	}
	/*
	**  GATHER INFO. ON DOMAINS
	*/
	opencatalog(AA_ATT, 2);
	AAam_setkey(&Attdes, &attkey, primary, ATTRELID);
	AAam_setkey(&Attdes, &attkey, pridesc.reldum.relowner, ATTOWNER);
	pc -= 2;
	p = &pv[2];
	dom = domain;
	for (i = 0; i < pc; i++)
	{
		if (i >= MAXKEYS)
		{
			AArelclose(&pridesc);
			return (error(5301, *p, primary, (char *) 0));	/* too many keys */
		}
		AAam_setkey(&Attdes, &attkey, *p, ATTNAME);
		j = AAgetequal(&Attdes, &attkey, &atttup, &tid);
		if (j < 0)
			AAsyserr(11041, j);
		if (j)
		{
			AArelclose(&pridesc);
			return (error(5302, *p, (char *) 0));	/* key not in relation */
		}
		dom->id = atttup.attid;
		dom->off = atttup.attoff;
		dom->frml = ctou(atttup.attfrml);
		dom->frm[0] = atttup.attfrmt;
		p++;
		dom++;
	}
	ndoms = i;
	AAnoclose(&Attdes);

	/*
	** The "order" of the steps have been altered to improve
	** recovery possibilities
	*/
	/*
	**  STEP 1 & 2: CREATE INDEX RELATION.
	*/
	newpc = 0;
	newpv[newpc++] = "0202";	/* S_NOUPDT | S_INDEX | */
	newpv[newpc++] = indx;
	p = &pv[2];
	dom = domain;
	for (i = 0; i < pc; i++)
	{
		newpv[newpc++] = *p;
		AA_itoa(dom->frml, &dom->frm[1]);
		newpv[newpc++] = dom->frm;
		dom++;
		p++;
	}
	newpv[newpc++] = "tidp";
	newpv[newpc++] = "i4";
	newpv[newpc] = (char *) -1;
#	ifdef xZTR1
	if (AAtTfp(5, 1, "INDEX: "))
		AAprargs(newpc, newpv);
#	endif
	if (create(newpc, newpv))
		return (-1);

	/*
	**  STEP 5: FILL UP THE SECONDARY INDEX FILE ITSELF
	*/

	/* set a exclusive use lock */
	if (AAsetusl(A_SLP, pridesc.reltid, M_EXCL) < 0)
		goto tabort;
	/* set a shared relation lock */
	if (AAsetrll(A_SLP, pridesc.reltid, M_SHARE) < 0)
	{
		AAunlus(pridesc.reltid);	/* release use lock */
tabort:
		return (error(5000, (char *) 0));
	}

	if (i = AArelopen(&desc, 2, indx))
		AAsyserr(11043, indx, i);
	AAam_find(&pridesc, NOKEY, &tid, &hitid);
	while (!(i = AAam_get(&pridesc, &tid, &hitid, primtup, TRUE)))
	{
		dom = domain;
		for (i = j = 0; j < ndoms; j++)
		{
			AAbmove(&primtup[dom->off], &systup[i], dom->frml);
			i += dom->frml;
			dom++;
		}
		AAbmove(&tid, &systup[i], sizeof tid);		/* move in pointer */
		if ((j = AAam_insert(&desc, &xtid, systup, TRUE)) < 0)
			AAsyserr(11044, indx, j);
	}
	if (i < 0)
		AAsyserr(11045, primary, i);
	AArelclose(&pridesc);
	AArelclose(&desc);

	/*
	**  STEP 3: ENTRIES TO INDEX-REL
	*/
	AApmove(primary, indtup.irelidp, MAXNAME, ' ');		/* mv in primary name  */
	AAbmove(pridesc.reldum.relowner, indtup.iownerp, 2);	/* primary owner */
	AApmove(indx, indtup.irelidi, MAXNAME, ' ');		/* index name */
	indtup.irelspeci = M_HEAP;
	for (i = 0; i < MAXKEYS; i++)
		indtup.idom[i] = (i < ndoms)? domain[i].id: 0;
	opencatalog(AA_INDEX, 2);
	if ((i = AAam_insert(&Inddes, &tid, &indtup, TRUE)) < 0)
		AAsyserr(11046, i);

	/*
	**  STEP 4: TURN BIT ON IN PRIMARY RELATION TO SHOW IT IS BEING INDEXED
	*/
	opencatalog(AA_REL, 2);
	AAam_setkey(&Reldes, &relkey, primary, RELID);
	AAam_setkey(&Reldes, &relkey, pridesc.reldum.relowner, RELOWNER);
	if (i = AAgetequal(&Reldes, &relkey, &reltup, &tid))
		AAsyserr(11047, i);
	reltup.relindxd = SECBASE;
	if ((i = AAam_replace(&Reldes, &tid, &reltup, TRUE)) < 0)
		AAsyserr(11048, i);

	/*
	** Flush all. This is necessary for recovery reasons.
	*/
	AApageflush((ACCBUF *) 0);
	AAunlrl(pridesc.reltid);	/* release relation lock */
	AAunlus(pridesc.reltid);	/* release use lock */
	return (0);
}
