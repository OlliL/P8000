# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/symbol.h"
# include	"../h/tree.h"
# include	"../h/pipes.h"
# include	"ovqp.h"

int			Qvpointer;

static SYMBOL		*Qvect[MAXNODES];


/*
**	Reads one symbol from the pipe
**	and inserts it in the next slot
**	on the Ovqpbuf.
**
**	returns address of list element.
*/
char	*rdsym()
{
	register SYMBOL	*next;
	register int	i;
	register int	j;
	extern char	*Ovqpbuf;
	extern char	*need();

	next = (SYMBOL *) need(Ovqpbuf, SYM_HDR_SIZ);	/* get two bytes for type and length */

	if (j = rdpipe(P_NORM, &Inpipe, R_decomp, next, TYP_LEN_SIZ) != TYP_LEN_SIZ)	/* get type and length */
		AAsyserr(17010, j);
	i = ctou(next->len);	/* get length of symbol */
#	ifdef xOTR1
	AAtTfp(29, 0, "RDSYM: sym %2.d  len=%3.d\n", next->type, i);
#	endif

	if (i)
	{
		/* if i is odd round up and allocate enought space. */
		/* alloc will guarantee an even byte adress */

		need(Ovqpbuf, i);		/* get space for value */
		if (next->type == INT && i == sizeof (char))
			i++;
		if (j = rdpipe(P_NORM, &Inpipe, R_decomp, next->value, i) != i)
			AAsyserr(17011, j);
	}

#	ifdef xOTR1
	if (AAtTf(29, 1))
		if (next->type != VAR)
			prsym(next);
#	endif

	if (Qvpointer >= MAXNODES)
		ov_err(NODOVFLOW);
	Qvect[Qvpointer++] = (SYMBOL *) next;
	return ((char *) next);
}

/*
**  Sym_ad -- reasonable way of getting the address
**	of the last symbol read in.
**
*/

SYMBOL	**sym_ad()
{
	return (&Qvect[Qvpointer - 1]);
}



/*
**	putvar is called to insert a tuple
**	pointer into the list. Desc is a
**	descriptor struc of an open relation.
**	Tup is the tuple buffer for the relation.
*/

putvar(sym, d, tup)
register SVAR_NODE		*sym;
char				tup[];
register DESC			*d;
{
	register int			attnum;
	extern char			*Ovqpbuf;

	attnum = sym->vattno;
	need(Ovqpbuf, SVAR_SIZ);

	if (attnum)
	{
		/* attnum is a real attribute number */
		if (attnum > d->reldum.relatts)
			AAsyserr(17012, attnum, d->reldum.relid);
		sym->vtype = d->relfrmt[attnum];
		sym->vlen = d->relfrml[attnum];
		sym->vpoint = (short *) (&tup[0] + d->reloff[attnum]);	/* address within tuple buffer location */
	}
	else
	{
		/* attnum refers to the tuple id */
		sym->vtype = TIDTYPE;
		sym->vlen = TIDLEN;	/* tids are longs */
		sym->vpoint = (short *) &Intid;	/* address of tid */
	}
#	ifdef xOTR1
	if (AAtTfp(29, 3, "PUTVAR: "))
		prsym(sym);
#	endif
}
