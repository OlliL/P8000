# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/access.h"
# include	"../h/aux.h"
# include	"../h/catalog.h"
# include	"../h/tree.h"
# ifdef BC
# define	BOP		 3	/* BINARY OPERATORS		*/
# define	QLEND		21	/* NULL branch at end of QUAL	*/
# define	TREE		25	/* SIGNALS BEGINNING of TREE	*/
# define	VAR		12	/* VARIABLE			*/
# define	mdCREATE	 6
# define	mdMODIFY	10
# define	opEQ		 5
# else
# include	"../h/symbol.h"
# endif
# include	"../h/pipes.h"
# include	"decomp.h"

extern DESC	*openr1();

/*
**	REFORMAT.C -- "reformat" module of DECOMPOSITION
**
**	reformat() examines each relation which will be
**	involved in a one variable subquery to see if it
**	is cost effective to modify the relation to a more
**	usefull structure. Included in this file are all the
**	routines associated with reformat:
**
**		reformat -- reformats each relation if cost effective
**
**		findlinks -- determines what one variable clauses (if any)
**				are associated with the current variable
**				and the substitution variable.
**
**		rangrf    -- decides whether to actually reformat and does it.
**
**		primrf    -- performs a projection on a user relation
**
**		ckpkey    -- determines if relation is already structured usefully.
**
**		findwid   -- determines width of target list.
**
**		rel_tup   -- returns how many tuples fit on one page
**
**
**
** REFORMAT -- Examine each variable to see if it should be reformated.
**
**	The algorithm is:
**	(1) Find all variables for which, after tuple substitution,
**	    will have one variable equality clauses on them.
**	(2) Estimate how much it will cost to execute them.
**	(3) Ignore those relations which are already keyed usefully.
**	(4) If it is a user relation, determine if it will be less costly
**	    to first project (and possibly) modify the relation.
**	(5) If it is a SYSNAME relation, determine if it will be less costly
**	    to modify the relation to hash on the linking domains.
*/
reformat(var, sqlist, locrang, buf, tree)
int			var;
QTREE			*sqlist[];
int			locrang[];
char			buf[];
QTREE			*tree;
{
	register QTREE			*sq;
	register char			*lmap;
	register int			mainvar;
	register int			i;
	register int			j;
	char				linkmap[MAXDOM];

#	ifdef xDTR1
	AAtTfp(13, -1, "REFORMAT: subvar=%d\n", var);
#	endif

	/* if the main tree is single var then put it in sq list */
	mainvar = -1;
	if (((ROOT_NODE *) tree)->tvarc == 1)
	{
		mainvar = bitpos(((ROOT_NODE *) tree)->lvarm | ((ROOT_NODE *) tree)->rvarm);
		if (sqlist[mainvar])
			AAsyserr(12020);
		sqlist[mainvar] = tree;
#		ifdef xDTR1
		AAtTfp(13, 0, "including var %d\n", mainvar);
#		endif
	}
	for(i = 0; i < MAXRANGE; i++)
		if (sq = sqlist[i])
		{
#			ifdef xDTR1
			AAtTfp(13, 0, "Sqlist[%d]:\n", i);
#			endif
			lmap = linkmap;
			for (j = 0; j < MAXDOM; j++)
				*lmap++ = 0;
			if (findlinks(sq->right, var, i, linkmap))
			{
#				ifdef xDTR1
				if (AAtTf(13, 1))
					prlinks("Findlinks found", linkmap);
#				endif
				rangrf(i, var, sq, buf, linkmap, locrang);
			}
		}
	if (mainvar >= 0)
		sqlist[mainvar] = 0;
}


/*
** FINDLINKS -- Determine whether there are any equalify clauses
**	between the "linkv" and the variable selected for tuple
**	substitution "selv".
**
**	Return:
**		TRUE if there is a linking variable
**		FALSE if not
**
**	Side Effects:
**		The linkmap is set to 1 for each linking domains.
**		ie. if domains 3 is a linking domains then
**		linkmap[3] = 1.
*/
findlinks(node, selv, linkv, linkmap)
QTREE		 	*node;
int			selv;
int			linkv;
char			linkmap[];
{
	register QTREE		 	*b;
	register QTREE		 	*a;
	register int 			s;
	register QTREE			*temp;
	register int 			i;
	extern QTREE			*ckvar();

	a = node;
#	ifdef xDTR1
	if (AAtTfp(13, 2, "FINDLINKS:"))
		writenod(a);
#	endif
	if (a->sym.type == QLEND)
		return (0);
	s = findlinks(a->right, selv, linkv, linkmap);

	/*
	** This mess is looking for a clause of the form:
	**		EQ
	**	Var		Var
	**	linkv		selv
	** Where the VARS can be in either order
	*/
	if ((b = a->left)->sym.type != BOP || *b->sym.value != (int) opEQ ||
		b->left->sym.type != VAR || b->right->sym.type != VAR)
			return (s);

	a = ckvar(b->left);
	b = ckvar(b->right);
	if (((VAR_NODE *) b)->varno == linkv)
	{
		temp = a;
		a = b;
		b = temp;
	}
	if (((VAR_NODE *) a)->varno != linkv || (selv >= 0 && ((VAR_NODE *) b)->varno != selv))
		return (s);

	i = ctou(((VAR_NODE *) a)->attno);
	linkmap[i] = 1;
#	ifdef xDTR1
	AAtTfp(13, 3, "found: attno=%d\n", i);
#	endif
	return (1);
}


/*
** RANGRF -- reformat the variable "var" if it is cost effective.
**
**	Rangrf does the actual work of reformat. If the relation is
**	already keyed usefully then rangrf does nothing.
**	Otherwise rangrf is split into two decisions:
**	A user relation must first be projected and then modified;
**	a SYSNAME relation can be modified directly.
**
**	It may be cost effective to just project a user relation.
*/
rangrf(var, substvar, sq, buf, linkmap, locrang)
int		var;
int		substvar;
QTREE		*sq;
char 		buf[];
char 		linkmap[];
int		locrang[];
{
	register struct rang_tab	*r;
	register struct rang_tab	*rs;
	register int			j;
	char				nums[2];
	register int			i;
	register int			newwid;
	register QTREE			*pq;
	long				npages;
	long				newpages;
	long				origcost;
	long				modcost;
	long				projcost;
	extern long			hashcost();
	extern char			*rangename();
	extern long			rel_pages();
	extern QTREE			*makroot();

	r = &Rangev[var];
	rs = &Rangev[substvar];
	npages = rel_pages(r->rtcnt, r->rtwid);

	/* calculate original cost of substitution */

	origcost = rs->rtcnt * npages;

#	ifdef xDTR1
	AAtTfp(13, 5, "eval of mod for var %d. orig cost=%ld\n", var, origcost);
#	endif

	/* if relation is already keyed usefully, just return */
	if (i = ckpkey(linkmap, var))
	{
#		ifdef xDTR1
		AAtTfp(13, 4, "already keyed ok %d\n", i);
#		endif
		return;
	}

	/* if this is a primary relation, a projection must be done
	** before any modify can be performed */

	if (!rnum_temp(r->relnum))
	{
		/* evaluation for primary, user relation */

		/* to save some time, don't evaluate if origcost is very small */
		if (origcost < OVHDP + 1 + npages)
			return;

		j = markbuf(buf);

		/* build a projection tree but don't actually create the projection */
		pq = makroot(buf);
		dfind(sq, buf, mksqlist(pq, var));

		newwid = findwid(pq);
		newpages = rel_pages(r->rtcnt, newwid);

		/*
		** Calculate cost of projection + new cost of substitution
		** projcost = readoldpages + writenewpages + runquery + overhead
		*/

		projcost = npages + newpages + rs->rtcnt * newpages + OVHDP;


		/*  CALCULATE COST OF MODIFY = COST OF PROJECTION + COST OF MODIFY
		 *  	AND NEW COST OF SUBSTITUTION AFTER MODIFY    */

		modcost = (npages + newpages) +
				hashcost(newpages) +
				rs->rtcnt +
				OVHDP + OVHDM;

#		ifdef xDTR1
		AAtTfp(13, 5, "primary rel: proj cost=%ld mod cost=%ld\n",
			projcost, modcost);
#		endif

		if (origcost <= modcost)
			if (origcost <= projcost)
			{
				freebuf(buf, j);
				return;
			}

#		ifdef xDTR1
		AAtTfp(13, 5, "doing projection\n");
#		endif

		/* i will be TRUE if the proj was aborted */
		i = primrf(var, pq, locrang);
		freebuf(buf, j);
		if ((projcost <= modcost) || i)
			return;
	}

	/*  IF THIS IS A TEMPORARY RELATION, A MODIFY CAN BE DONE DIRECTLY  */

	else
	{

		/*  CALCULATE MODIFY COST (which does not include a projection)
		 *  AND NEW COST OF SUBSTITUTION				  */

		modcost = hashcost(npages)
		          + rs->rtcnt + OVHDM;

#		ifdef xDTR1
		AAtTfp(13, 5, "temp rel: mod cost=%ld\n", modcost);
#		endif

		if (origcost <= modcost)
			return;
	}

#	ifdef xDTR1
	AAtTfp(13, 5, "doing modify\n");
#	endif

	initp();
	setp(rangename(var));
	setp("hash");	/* modify to hash */
	setp("num");	/* passing domain numbers - not names */

	nums[1] = '\0';
	for (j = 0; j < MAXDOM; j++)
		if (linkmap[j])
		{
			*nums = j;
			setp(nums);
		}

	/* fill relation completely */
	setp("");
	setp("fillfactor");
	setp("100");
	setp("minpages");
	setp("1");
	closer1(var);
	call_dbu(mdMODIFY, FALSE);

	/* re-open modified range to get accurate descriptor */
	openr1(var);
}


/*
** PRIMRF -- Replace a user relation with a projection of the needed domains.
**
**	Primrf retrieves into a temporary relation, the domains
**	specified by the "pq" tree. The range table is updated to
**	reflect the new range.
**
**	In fact a projection is not an accurate way to describe what
**	happens. In order to avoid changing any attribute numbers in
**	the query, the needed domains are projected and the domains
**	inbetween are created as type "c0". This way they occupy
**	no space and the attribute numbering is unchanged. Of course,
**	any attributes beyond the last one needed are simply discarded.
**
**	In previous versions attempts were made to project only the needed
**	domains and to renumber the query tree. This proved to be very
**	expensive and difficult and was not always as optimal as this
**	method.
**
**	The routines newquery() and endquery() will undo the effects
**	of primrf and restore the range table back to its original state.
*/
primrf(var, pq, locrang)
int			var;
QTREE			*pq;
int			locrang[];
{
	register QTREE		*q;
	register QTREE		**np;
	register int		i;
	register int		maxresno;
	register int		rnum;
	QTREE			*node1[MAXDOM + 1];
	QTREE			*node2[MAXDOM + 1];
	char			czero[QT_HDR_SIZ + RES_SIZ];	/* a dummy resdom */

#	ifdef xDTR1
	AAtTfp(13, 6, "PRIMRF:\n");
#	endif

	/* renumber RESDOMs to match their VARs */
	for (q = pq->left; q->sym.type != TREE; q = q->left)
		((RES_NODE *) q)->resno = ctou(((VAR_NODE *) q->right)->attno);

	/* form list of RESDOMs from outermost inward */
	node1[lnode(pq->left, node1, 0)] = 0;

	/* form a dummy RESDOM with type CHAR and length 0 */
	q = (QTREE *) czero;
	((VAR_NODE *) q)->frmt = CHAR;
	((VAR_NODE *) q)->frml = 0;

	/* zero node2 */
	for (np = node2, i = 0; i < MAXDOM + 1; i++)
		*np++ = 0;

	/* sort RESDOMs into node2 */
	maxresno = 0;
	for (np = node1; q = *np++; )
	{
		if (!(i = ((RES_NODE *) q)->resno))
			return (1);	/* abort. Tid is referenced */
		if (i > maxresno)
			maxresno = i;
		node2[i - 1] = q;
	}

	/* fill missing RESDOMs with czero */
	for (np = node2, i = 0; i < maxresno; i++, np++)
		if (!*np)
			*np = (QTREE *) czero;


	/* set up params for the create */
	initp();
	setp("0");	/* initial relstat field */
	rnum = rnum_alloc();
	setp(rnum_convert(rnum));
	domnam(node2, "f");
	call_dbu(mdCREATE, FALSE);

	/* now run projection */
	i = var;
	execsq1(pq, i, rnum);
	new_range(i, rnum);
	/* save the name of the new relation */
	savrang(locrang, i);
	openr1(i);
	return (0);
}


/*
** CKPKEY -- determine if a relation is already keyed usefully.
**
**	Ckpkey gets the key structure from AAdparam() and compares
**	it to the linkmap. If the relation is ISAM and the first keyed
**	domain is in linkmap, or if it is HASH and all keyed domains
**	are in the linkmap, then ckpkey() returns >0, else
**	ckpkey looks for secondary indexes which are usable.
**	if none, then ckpkey() returns 0.
**
**	The value returned is an estimate of the number of
**	pages which must be read to satisfy a query given
**	equality clauses on the linkmap domains and the
**	structure of the relation. The (crude) estimates are:
**		hash	1 page
**		isam	2 pages
**		hash sec index	2 pages
**		isam sec index	3 pages
*/
ckpkey(linkmap, var)
char	linkmap[];
int	var;
{
	register DESC			*d;
	register int			i;
	struct index			itup;
	struct accessparam 		ap;
	struct tup_id			lotid;
	struct tup_id			hitid;
	extern DESC			Inddes;
	extern DESC			*readopen();


#	ifdef  xDTR1
	AAtTfp(13, 11, "CKPKEY:%s\n", rangename(var));
#	endif

	/* if relation is an unindexed heap, then it cannot be keyed usefully */
	d = openr1(var);
	if (abs(ctoi(d->reldum.relspec)) != M_HEAP ||
	ctoi(d->reldum.relindxd) == SECBASE)
	{
		d = readopen(var);	/* open relation if it hasn't been already */
		AAdparam(d, &ap);
		if (!ckpkey1(linkmap, &ap))
			return (ap.mode == EXACTKEY? 1: 2);	/* success */

		if (ctoi(d->reldum.relindxd) == SECBASE)
		{
			opencatalog(AA_INDEX, 0);
			AAam_setkey(&Inddes, &itup, d->reldum.relid, IRELIDP);
			AAam_setkey(&Inddes, &itup, d->reldum.relowner, IOWNERP);
			if (i = AAam_find(&Inddes, EXACTKEY, &lotid, &hitid, &itup))
				AAsyserr(12021, i);

			while (!(i = AAam_get(&Inddes, &lotid, &hitid, &itup, TRUE)))
			{
				if (!AAbequal(&itup, d->reldum.relid, MAXNAME + 2))
					continue;

				AAiparam(&itup, &ap);
				if (!ckpkey1(linkmap, &ap))
					return (ap.mode == EXACTKEY? 2: 3);	/* success */
			}
		}
	}
	return (0);	/* failure. no usefull structure */
}


ckpkey1(linkmap, ap)
char			linkmap[];
struct accessparam	*ap;
{
	register int	i;
	register int	k;
	register int	anykey;

	if (ap->mode == NOKEY)
		return (1);
	anykey = 0;
	for (i = 0; k = ap->keydno[i]; i++)
	{
		if (!linkmap[k])
		{
			if (ap->mode == EXACTKEY)
				return (1);
			else
				break;
		}
		anykey++;
	}
	return (!anykey);
}


/*
** FINDWID -- scan the target list of the projection tree
**	to determine the resultant tuple size.
**
**	Return:
**		Size of projected tuple.
*/
findwid(tree)
QTREE		*tree;
{
	register QTREE			*nod;
	register QTREE			*t;
	register int			wid;


	wid = 0;
	t = tree;

	for (nod = t->left; nod && nod->sym.type != TREE; nod = nod->left)
	{
		wid += ctou(((VAR_NODE *) nod)->frml);
	}

	return (wid);
}


/*
**  HASHCOST -- determine cost to modify to hash
**
**	Estimates the cost to modify the relation to hash.
**	The estimate is crude since it assumes that there
**	are no duplicates and that a BS page will be
**	the same size as an DBS page.
**
**	The cost is based on the parameters which signify
**	the size of the in-core sort buffer and the n-way
**	merge sort plus the cost to read the sorted file
**	and write the new relation twice (that's the was it works!).
**
**	Parameters:
**		npages - the number of pages (estimate) which the
**				relation currently occupies.
**
**	Returns:
**		the cost to hash the relation.
*/
long	hashcost(npages)
long	npages;
{
	long		sortpages;
	long		total;
	register int	nfiles;

	nfiles = npages / (COREBUFSIZE / AApgsize);
	sortpages = 2 * npages;

	while (nfiles > 1)
	{
		nfiles = (nfiles + MERGESIZE - 1) / MERGESIZE;
		sortpages += 2 * npages;
	}

	total = sortpages + npages + npages + npages;
#	ifdef xDTR1
	AAtTfp(13, 5, "hashcost is %ld\n", total);
#	endif

	return (total);
}
