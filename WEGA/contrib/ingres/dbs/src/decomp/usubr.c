# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/access.h"
# include	"../h/aux.h"
# include	"../h/tree.h"
# include	"../h/symbol.h"
# include	"../h/pipes.h"
# include	"decomp.h"


/**
 **	usubr.c
 **
 **	utility routines to handle setting up params, etc for DBU calls
 **/



/*
 *	generate domain names, formats
 */
domnam(lnp, pre)
QTREE		 	**lnp;
char			*pre;
{
	register char 	suf;
	register char	*n;
	register char	**p;
	char		name[MAXNAME];
	extern char	*format();

	suf = '1';
	for (n = name; *n++ = *pre++; )
		continue;
	*n = '\0';
	n--;
	for (p = (char **) lnp; *p; p++)
	{
		*n = suf++;
		setp(name);
		setp(format(*p));
	}
	return;
}


/*
**	Parameter buffer for use in calling dbu routines.
**	The buffer is manipulated by setp() and initp().
**	It is interrogated by call_dbu().
*/

int		Pc;			/* number of parameters */
char		*Pv[PARGSIZE];		/* array of pointers to parameters */

static int	Pcount;			/* next free char in Pbuffer */
static char	Pbuffer[PBUFSIZE];	/* space for actual parameters */

initp()

/*
**	Init parameter buffer to have no params.
*/

{
	Pc = 0;
	Pcount = 0;
}


setp(s)
char	*s;

/*
**	Copy the paramer s into the parameter buffer.
*/

{
	register char	*r;
	register char	*p;
	register int	i;

	r = s;
	i = Pcount;
	p = &Pbuffer[i];

	/* copy parameter */
	Pv[Pc++] = p;
	while (*p++ = *r++)
		i++;

	Pcount = ++i;

	/* check for overflow */
	if (i > PBUFSIZE || Pc >= PARGSIZE)
		AAsyserr(12029, i, Pc);
}

/*
 *	gets format in ascii from RESDOM or AOP node
 */
char	*format(p)
QTREE		 *p;
{
	register char	*b;
	static char	buf[10];

	b = buf;

	*b++ = ((VAR_NODE *) p)->frmt;
	AA_itoa(ctou(((VAR_NODE *) p)->frml), b);
	return (buf);
}


/*
 *	makes list of nodes (depth first)
 */
lnode(nod, lnodv, count)
QTREE		 	*nod, *lnodv[];
int			count;
{
	register QTREE			*q;
	register int			i;

	i = count;
	q = nod;

	if (q && q->sym.type != TREE)
	{
		i = lnode(q->left, lnodv, i);
		lnodv[i++] = q;
	}
	return (i);
}




dstr_rel(relnum)
int	relnum;

/*
**	Immediately destroys the relation if it is an SYSNAME
*/

{
	initp();
	dstr_mark(relnum);
	dstr_flush(0);
}


dstr_mark(relnum)
int	relnum;

/*
**	Put relation on list of relations to be
**	destroyed. A call to initp() must be
**	made before any calls to dstr_mark().
**
**	A call to dstr_flush() will actually have
**	the relations exterminated
*/

{
	register char	*p;
	extern char	*rnum_convert();

	if (rnum_temp(relnum))
	{
		p = rnum_convert(relnum);
#		ifdef xDTR1
		AAtTfp(3, 4, "destroying %s\n", p);
#		endif
		setp(p);
		specclose(relnum);	/* guarantee that relation is closed and descriptor destroyed */
		rnum_remove(relnum);
	}
}


dstr_flush(errflag)
int	errflag;

/*
**	call destroy if any relations are
**	in the parameter vector
*/

{
	if (Pc)
		call_dbu(mdDESTROY, errflag);
}


mak_t_rel(tree, prefix, rnum)
QTREE			*tree;
char			*prefix;
int			rnum;

/*
**	Make a temporary relation to match
**	the target list of tree.
**
**	If rnum is positive, use it as the relation number,
**	Otherwise allocate a new one.
*/

{
	register int	relnum;
	char		*lnodv[MAXDOM + 1];

	initp();
	setp("0");	/* initial relstat field */
	relnum = (rnum < 0)? rnum_alloc(): rnum;
	setp(rnum_convert(relnum));
	lnodv[lnode(tree->left, lnodv, 0)] = 0;
	domnam(lnodv, prefix);

	call_dbu(mdCREATE, FALSE);
	return (relnum);
}


QTREE		 **mksqlist(tree, var)
QTREE			*tree;
int			var;
{
	register QTREE			**sq;
	register int			i;
	static QTREE			*sqlist[MAXRANGE];

	sq = sqlist;
	for (i = 0; i < MAXRANGE; i++)
		*sq++ = 0;

	sqlist[var] = tree;
	return (sqlist);
}




long rel_pages(tupcnt, width)
long	tupcnt;
int	width;
{
	register int	tups_p_page;

	tups_p_page = (AApgsize - PGHDR) / (width + sizeof (short));
	return ((tupcnt + tups_p_page - 1) / tups_p_page);
}
