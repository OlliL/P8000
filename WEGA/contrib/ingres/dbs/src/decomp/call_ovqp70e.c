# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/access.h"
# include	"../h/aux.h"
# include	"../h/catalog.h"
# include	"../h/tree.h"
# include	"../h/symbol.h"
# include	"../h/pipes.h"
# include	"../h/bs.h"
# include	"decomp.h"
# include	"../ovqp/ovqp.h"


/*
** Readagg_result -- read results from ovqp from a simple aggregate.
*/
readagg_result(r)
register QTREE		**r;
{
	register QTREE			*aop;
	register int			i;
# ifndef NO_F4
	register int			f4;
# endif
	register char			*val;
	extern char			*AAbmove();

	Tend = Outtup;

	while (aop = *r++)
	{
		i = ctou(aop->sym.len);

		if (aop->sym.type == CHAR)
			AApad(Tend, i);

# ifndef NO_F4
		if (f4 = aop->sym.type == FLOAT && i == sizeof (float))
			i = sizeof (double);
# endif

		AAbmove(Tend, aop->sym.value, i);
		Tend += i;

# ifndef NO_F4
		if (f4)
		{
			val = (char *) aop->sym.value;
			((F4TYPE *) val)->f4type = ((F8TYPE *) val)->f8type;
		}
# endif

#		ifdef xDTR1
		if (AAtTf(8, 3))
			writenod(aop);
#		endif
	}
}


ov_err(code)
int	code;
{
	derror(code);
}


DESC		 *openindex(name)
char	*name;
{
	register DESC			*d;
	register int			varno;
	extern DESC			*readopen();

	varno = SECINDVAR;
	Rangev[varno].relnum = rnum_assign(name);
	d = readopen(varno);
	return (d);
}


/*
**	Use "AArelclose()" for closing relations. See
**	desc_close in openrs.c for details.
*/
extern int	AArelclose();
int		(*Des_closefunc)() =	AArelclose;

init_decomp()
{
#	ifdef XTRA
	static ACCBUF		xtrabufs[XTRABUFS];

	AAam_addbuf(xtrabufs, XTRABUFS);
#	endif
}


startdecomp()
{
	/* called at the start of each user query */
	Retcode.rc_tupcount = 0;
	initrange();
	rnum_init();
	startovqp();
}



/*
** Files_avail -- returns how many file descriptors are available
**	for decomposition. For decomp combined with ovqp, the fixed
**	overhead for files is:
**
**		4 -- pipes
**		1 -- relation relation
**		1 -- attribute relation
**		1 -- standard output
**		1 -- concurrency
**		1 -- AA_INDEX relation
**
**	Optional overhead is:
**
**		1 -- for EQL data pipe
**		1 -- for batch file on updates
*/

files_avail(mode)
int	mode;
{
	register int	i;

	i = MAXFILES - 9;
	if (Eql)
		i--;
	if (mode != (int) mdRETR)
		i--;
	return (i);
}
