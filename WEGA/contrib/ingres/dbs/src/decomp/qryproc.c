# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/tree.h"
# include	"../h/symbol.h"
# include	"../h/lock.h"
# include	"../h/pipes.h"
# include	"decomp.h"

int	Qry_mode;	/* mode of original query (not nec same as Qmode) */
QTREE	*Qle;			/* ptr to QLEND node */
QTREE	*Tr;			/* ptr to TREE node */
char	Qbuf[QBUFSIZ];		/* buffer which holds symbol lists */

qryproc()
{
	register QTREE			*root;
	register QTREE			*q;
	register int			i;
	register int			mode;
	register int			result_num;
	register int			retr_uniq;
	extern long			AAccuread;
	extern long			AAccuwrite;
	extern long			AAccusread;
	extern int			derror();
	extern QTREE			*trbuild();
	extern QTREE			*readqry();

#	ifdef xDTM
	if (AAtTf(76, 1))
		timtrace(23, 0);
#	endif
#	ifdef xDTR1
	if (AAtTf(50, 0))
		AAccuread = AAccusread = AAccuwrite = 0;
#	endif

	/* initialize query buffer */
	initbuf(Qbuf, QBUFSIZ, QBUFFULL, derror);

	/* init various variables in decomp for start of this query */
	startdecomp();

	/* Read in query, range table and mode */
	root = readqry();
	mode = Qmode;

	/* Initialize relation descriptors */
	initdesc(mode);

	/* re-build the tree */
	root = trbuild(root);
	if (!root)
		derror(STACKFULL);


	/* locate pointers to QLEND and TREE nodes */
	for (q = root->right; q->sym.type != QLEND; q = q->right)
		continue;
	Qle = q;

	for (q = root->left; q->sym.type != TREE; q = q->left)
		continue;
	Tr = q;


	/* map the complete tree */
	mapvar(root, 0);

	/* set logical locks */
	lockit(root, Resultvar);

	/* If there is no result variable then this must be a retrieve to the terminal */
	Qry_mode = Resultvar < 0 ? (int) mdRETTERM : mode;

	/* if the mode is retrieve_unique, then make a result rel */
	retr_uniq = mode == (int) mdRET_UNI;
	if (retr_uniq)
	{
		mk_unique(root);
		mode = (int) mdRETR;
	}

	/* get id of result relation */
	if (Resultvar < 0)
		result_num = NORESULT;
	else
		result_num = Rangev[Resultvar].relnum;

	/* evaluate aggregates in query */
	aggregate(root);

	/* decompose and process aggregate free query */
	decomp(root, mode, result_num);

	/* If this is a retrieve unique, then retrieve results */
	if (retr_uniq)
		pr_unique(root, Resultvar);

	if (mode != (int) mdRETR)
		i = ACK;
	else
		i = NOACK;
	i = endovqp(i);

	/* call update processor if batch mode */
	if (i == UPDATE)
	{
		initp();
		call_dbu(mdUPDATE, -1);
	}


	/*
	** send eop back to parser to indicate completion
	** if UPDATE then return block comes from dbu else
	** return block comes from decomp
	*/
	writeback(i == UPDATE ? -1 : 1);

#	ifdef xDTM
	if(AAtTf(76, 1))
		timtrace(24, 0);
#	endif
#	ifdef xDTR1
	AAtTfp(50, 1, "DECOMP read %ld pages,%ld catalog pages,wrote %ld pages\n",
		AAccuread, AAccusread, AAccuwrite);
#	endif

	/* clean decomp */
	reinit();

	/* return */
}
