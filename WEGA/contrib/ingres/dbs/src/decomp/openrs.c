# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/tree.h"
# include	"../h/symbol.h"
# include	"../h/pipes.h"
# include	"decomp.h"



struct desc_tab
{
	int			relnum;	/* relation number */
	char			dtmode;	/* status of descriptor */
	char			dtpos;	/* position of last access */
	DESC			desc;	/* descriptor */
};

/* Defined constants for dtmode field above */
# define	DTALLOC		0	/* descriptor allocated */
# define	DTREL		1	/* has been openr'd -1 */
# define	DTATTS		2	/* has rel+atts but not opened */
# define	DTREAD		3	/* currently open for reading */
# define	DTWRITE		4	/* currently open for writing */



/* Allocation of descriptors */
struct desc_tab	Desc[MAXRELN];		/* descriptors available for use */

/* Globals which count #files open and maximum # of files which can be open */
static int	Dfiles;
static int	Dopnfiles;


/*
** OPENRS -- routines associated with maintaining the range table for decomp
**
**	openrs(root) -- fill range table info about each relation.
**
**	closers() -- close all variables in range table.
**
**	openr1(varno) -- fill range table for a particular relation.
**
**	closer1(varno) -- close a particular relation.
**
**	readopen(varno) -- open a variable for reading. returns descriptor.
**
**	writeopen(varno) -- open a variable for writing. returns descriptor.
**
**	initdesc()	-- initialize the descriptor cache.
**
**	reldescrip(varno) -- returns descriptor for var (has rel/atts but might not be open).
**
**	*desc_get(relnum, flag) -- finds a desc_tab & alloctes it for relnum.
**
**	desc_lru()  -- returns least recently used desc_tab.
**
**	desc_top(desc_tab) -- makes desc_tab most recently used.
**
**	desc_last(desc_tab) -- makes desc_tab the least recently used.
*/


initdesc(mode)
int	mode;

/*
** Initdesc -- initialize descriptors for range table
*/

{
	register struct desc_tab	*dt;
	register int			i;
	extern int			Eql;


	for (dt = Desc, i = 0; dt <= &Desc[MAXRELN - 1]; dt++, i++)
	{
		dt->dtmode = DTALLOC;
		dt->relnum = -2;	/* unused relnum value */
		dt->dtpos = i;		/* lru order */
	}

	/*
	** Determine number of available file descriptors.
	**	The answer will depend on whether this is decomp
	**	alone or decomp+ovqp, and whether it is an update
	**	and whether this is an EQL program.
	*/

	Dfiles = files_avail(mode);
	Dopnfiles = 0;
}


openrs(root)
QTREE			*root;

/*
**	Openrs -- open source relations for query. Fill values
**		in range table.
*/

{
	register QTREE			*r;
	register int			map;
	register int			i;
	extern DESC			*openr1();

	r = root;
	map =((ROOT_NODE *) r)->lvarm |((ROOT_NODE *) r)->rvarm;

#	ifdef xDTR1
	AAtTfp(2, 0, "OPENRS-root: 0%o, map: 0%o\n", r, map);
#	endif

	for (i = 0; i < MAXRANGE; i++)
		if (map & (01 << i))
			openr1(i);

}



closers()

/*
**	Close all open relations.
**	If any relations were created but never
**	opened, destroy them. The only
**	situation under which that can occure
**	is when a rub-out occures at an
**	in oportune moment or when an error
**	occures in ovqp.
*/

{
	register int			i;
	register struct desc_tab	*dt;


	for (dt = Desc; dt <= &Desc[MAXRELN - 1]; dt++)
		desc_close(dt);

	/* destroy any temps */
	initp();	/* init parameters vector for destroys */
	while (i = rnum_last())
		dstr_mark(i);
	dstr_flush(1);	/* call destroy with rel names */
}



DESC		 *openr1(var)
int	var;

/*
**	Openr1 -- open relation to get relation relation tuple
**
**	This will not open the relation for reading -- only
**	for getting the first part of the descriptor filled
*/

{
	register struct desc_tab	*dt;
	register struct rang_tab	*rp;
	register DESC			*d;
	int				i;
	extern struct desc_tab		*desc_get();

	rp = &Rangev[var];

#	ifdef xDTR1
	AAtTfp(2, 2, "OPENR1: var %d (%s)\t", var, rnum_convert(rp->relnum));
#	endif

	dt = desc_get(rp->relnum, TRUE);

	if (dt->dtmode == DTALLOC)
	{
		if (i = AArelopen(&dt->desc, -1, rnum_convert(rp->relnum)))
			AAsyserr(12012, i, rnum_convert(rp->relnum));
		dt->dtmode = DTREL;
	}

#	ifdef xDTR1
	AAtTfp(2, 2, "tups=%ld\n", dt->desc.reldum.reltups);
#	endif

	d = &dt->desc;

	rp->rtspec = ctoi(d->reldum.relspec);
	rp->rtstat = d->reldum.relstat;
	rp->rtwid = d->reldum.relwid;
	rp->rtcnt = d->reldum.reltups;

	return (d);
}


closer1(var)
int	var;

/*
*/

{
	register struct desc_tab	*dt;
	register struct rang_tab	*rp;
	register int			i;
	extern struct desc_tab		*desc_get();
	extern struct desc_tab		*desc_last();

	i = var;
	rp = &Rangev[i];

#	ifdef xDTR1
	AAtTfp(2, 4, "CLOSER1: var %d (%s)\n", i, rnum_convert(rp->relnum));
#	endif
	if (dt = desc_get(rp->relnum, FALSE))
	{

		/* currently a descriptor for rel */
		desc_close(dt);

		dt->relnum = -2;
		desc_last(dt);

	}
}


DESC		 *readopen(var)
int	var;

/*
*/

{
	register struct desc_tab	*dt;
	extern struct desc_tab		*desc_get();

	/* get descv for the relation */
	dt = desc_get(Rangev[var].relnum, TRUE);

	if (!(dt->dtmode == DTREAD || dt->dtmode == DTWRITE))
	{
		/* not open for reading or writing */
		openup(dt, var, 0);	/* open for reading */
	}

	return (&dt->desc);
}


DESC		 *writeopen(var)
int	var;

/*
*/

{
	register struct desc_tab	*dt;
	extern struct desc_tab		*desc_get();

	/* get descv for the relation */
	dt = desc_get(Rangev[var].relnum, TRUE);

	if (dt->dtmode != DTWRITE)
	{
		/* not open for writing */
		openup(dt, var, 2);	/* open for reading */
	}

	return (&dt->desc);
}


DESC		 *specopen(relnum)
int	relnum;

/*
** Specopen -- open for writing not associated with any variable
*/

{
	register struct desc_tab	*dt;
	extern struct desc_tab		*desc_get();

	dt = desc_get(relnum, TRUE);

	if (dt->dtmode != DTWRITE)
		openup(dt, -1, 2);

	return (&dt->desc);
}


specclose(relnum)
int	relnum;
{
	register struct desc_tab	*dt;
	extern struct desc_tab		*desc_get();
	extern struct desc_tab		*desc_last();

	if (dt = desc_get(relnum, FALSE))
	{
		desc_close(dt);
		desc_last(dt);
		dt->relnum = -2;
	}
}


openup(dt1, varno, mode)
struct desc_tab	*dt1;
int		varno;
int		mode;

/*
**	Openup -- make sure that the given descriptor is open
**		suitably for reading or writing.
*/

{
	register struct desc_tab	*dt;
	register int			md;
	register int			openmd;
	int				i;

	/* quick check to handle typical case of rel being already open */
	md = mode;
	dt = dt1;
	if ((md != 2 && dt->dtmode == DTREAD) || dt->dtmode == DTWRITE)
		return;

	/* relation not opened correctly */
	switch (dt->dtmode)
	{

	  case DTALLOC:
		/*
		** Descriptor allocated but nothing else. If this
		** is for a variable then use openr1 to get range table
		** info. Else open directly.
		*/
		if (varno < 0)
		{
			/* open unassociated with a range table variable */
			openmd = md ? 2 : 0;
			AAbmove(rnum_convert(dt->relnum), dt->desc.reldum.relid, MAXNAME);
			break;
		}

		/* open for range table variable */
		openr1(varno);

		/* now fall through to DTREL case */

	  case DTREL:
		/* relation relation tuple present but nothing else */
		openmd = md ? -3 : -2;	/* open -2 for read, -3 for write */
		break;

	  case DTATTS:
		/* relation & attributes filled but relation closed */
		openmd = md ? -5 : -4;
		break;
	  case DTREAD:
		/* relation open for reading but we need to write */
		desc_close(dt);

		openmd = -5;
		break;

	  default:
		AAsyserr(12013, dt->dtmode);
	}

	/* close a previous file if necessary */
	if (Dopnfiles == Dfiles)
		desc_victum();	/* close oldest file */

	/* now open relation */
	if (i = AArelopen(&dt->desc, openmd, dt->desc.reldum.relid))
		AAsyserr(12014,
			i, openmd, dt->desc.reldum.relid, rnum_convert(dt->relnum));
	Dopnfiles++;

	/* update mode of descriptor */
	dt->dtmode = md ? DTWRITE : DTREAD;
}



struct desc_tab	*desc_get(relnum, flag)
int	relnum;
int	flag;
/*
*/
{
	register struct desc_tab	*dt;
	register struct desc_tab	*ret;
	extern struct desc_tab		*desc_lru();

	ret = (struct desc_tab *) 0;

	/* search for one currently allocated */
	for (dt = &Desc[0]; dt <= &Desc[MAXRELN-1]; dt++)
	{
		if (dt->relnum == relnum)
		{
			ret = dt;
#			ifdef xDTR1
			AAtTfp(2, 3, "found desc for %d\n", relnum);
#			endif
			break;
		}
	}

	if (!ret && flag)
	{
		/* get a victum and deallocate desc */
		ret = desc_lru();

		/* deallocate */
#		ifdef xDTR1
		AAtTfp(2, 5, "trading %d for %d\n", ret->relnum, relnum);
#		endif
		desc_close(ret);

		/* allocate */
		ret->relnum = relnum;
		ret->dtmode = DTALLOC;
	}

	if (ret)
		desc_top(ret);

	return (ret);
}



/*
**	For text space reasons only, the close relation routine varies
**	between decomp and decomp70. In decomp, the relation is opened
**	only for reading and never for writing thus AAinpclose() can be
**	called. For decomp70 AArelclose() must be called. If there were no
**	text space shortage, then AArelclose() could always be called.
**	The routine init_decomp() assigned the value to Des_closefunc.
*/
extern int	(*Des_closefunc)();	/* either &AAinpclose or &AArelclose */

desc_close(dt1)
struct desc_tab	*dt1;
{
	register struct desc_tab	*dt;
	register int			i;

	dt = dt1;

	if (dt->dtmode == DTREAD || dt->dtmode == DTWRITE)
	{
		if (i = (*Des_closefunc)(&dt->desc))
			AAsyserr(12015, i, dt->desc.reldum.relid);
		Dopnfiles--;
		dt->dtmode = DTATTS;
	}
}




desc_top(dt1)
struct desc_tab	*dt1;

/*
** Desc_top -- make the desc_tab entry "dtx" the most recently used.
*/

{
	register struct desc_tab	*dt;
	register struct desc_tab	*dx;
	register int			oldpos;

	dt = dt1;

	if ((oldpos = dt->dtpos) != 0)
	{
		/* descriptor isn't currently top */
		for (dx = Desc; dx <= &Desc[MAXRELN-1]; dx++)
			if (dx->dtpos < oldpos)
				dx->dtpos++;

		/* make descriptor first */
		dt->dtpos = 0;
	}
}


struct desc_tab	*desc_last(dt1)
struct desc_tab	*dt1;

/*
** Desc_last -- make the desc_tab entry "dtx" the least recently used.
*/

{
	register struct desc_tab	*dt;
	register struct desc_tab	*dx;
	register int			oldpos;

	dt = dt1;

	oldpos = dt->dtpos;
	for (dx = Desc; dx <= &Desc[MAXRELN-1]; dx++)
		if (dx->dtpos > oldpos)
			dx->dtpos--;

	/* make descriptor last */
	dt->dtpos = MAXRELN - 1;
}



struct desc_tab	*desc_lru()
/*
** Desc_lru -- return least recently used descriptor
*/
{
	register struct desc_tab	*dx;

	for (dx = Desc; dx <= &Desc[MAXRELN - 1]; dx++)
	{
		if (dx->dtpos == MAXRELN - 1)
			return (dx);
	}
	AAsyserr(12016);
}


desc_victum()
{
	register struct desc_tab	*dt;
	register struct desc_tab	*old;

	old = (struct desc_tab *) 0;
	for (dt = &Desc[0]; dt <= &Desc[MAXRELN-1]; dt++)
	{
		if (dt->dtmode == DTWRITE || dt->dtmode == DTREAD)
		{
			if (!old || dt->dtpos > old->dtpos)
				old = dt;
		}
	}

	if (!old)
		AAsyserr(12017, Dopnfiles, Dfiles);
	desc_close(old);
}
