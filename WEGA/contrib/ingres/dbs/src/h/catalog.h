/*	INDEX relation struct					*/
/*								*/
/*	The INDEX relation is used to determine what, if any,	*/
/*	secondary indicies exist for relations.  If a relation	*/
/*	has a secondary index, there will be one tuple in the	*/
/*	INDEX relation for each such index.  There may be one	*/
/*	or more domains indexed by one or many INDEX relations	*/
/*	depending on whether single or combined indicies are	*/
/*	being used.						*/
/*								*/
/*	Combined indices may use up to six domains to form the	*/
/*	index.							*/
/*								*/
/*	The simple existance of a secondary index is better	*/
/*	checked using the "relindxd" field in the RELATION	*/
/*	relation, since that is more efficient.			*/
/*								*/
/*	The two values SECINDEX and SECBASE are the values for	*/
/*	the relindxd field of the relation relation. Implicitly	*/
/*	SECINDEX must be < 0 and SECBASE must be > 0.		*/

# define	SECINDEX	-2	/* this in rel.relindxd indicates    */
					/* that the relation is a sec. index */
# define	SECBASE		1	/* this in rel.relindxd indicates    */
					/* has a sec. index		     */

struct index
{
	char	irelidp[MAXNAME];	/* unique name of primary relation   */
	char	iownerp[2];		/* owner of primary relation	     */
	char	irelidi[MAXNAME];	/* unique name of index relation     */
	char	irelspeci;		/* relspec of index relation	     */
	char	idom[MAXKEYS];		/* domain number of primary relation */
					/* which corresponds to each index   */
					/* attribute. In the index relation  */
					/* are stored as:		     */
					/*	idom1, idom2, ..,idom6	     */
};

/* field numbers for AAam_find() calls */
# define	IRELIDP		1
# define	IOWNERP		2
# define	IRELIDI		3



/*  TREE RELATION STRUCT						*/
/*									*/
/*	The TREE relation stores trees used by query modification and	*/
/*	for distribution criteria.					*/

struct tree
{
	char	treerelid[MAXNAME];	/* relation name		*/
	char	treeowner[2];		/* relation owner		*/
	short	treeid;			/* internal name of this tuple	*/
	short	treeseq;		/* sequence number in tree	*/
	char	treetype;		/* type info for this tree	*/
	char	treexxxx;
	char	treetree[100];		/* contents of tree		*/
};

/* field numbers for AAam_find() calls */
# define	TREERELID	1
# define	TREEOWNER	2
# define	TREEID		3
# define	TREESEQ		4
# define	TREETYPE	5



/*  STRUCT PROTECT -- AA_PROTECT catalog					*/
/*									*/
/*	This structure defines the format of the AA_PROTECT catalog.	*/
/*	One or two things should be noted.  First, the 'prodomset'	*/
/*	field is actually four domains in the physical relation,	*/
/*	since the best we know about is i4's, and we need an i16.	*/
/*	Second, both the proopset and the prodomset fields		*/
/*	are bit maps.							*/

/* bit values for proopset						*/
# define	PRO_RETR	0001	/* retrieve			*/
# define	PRO_REPL	0002	/* replace			*/
# define	PRO_DEL		0004	/* delete			*/
# define	PRO_APP		0010	/* append			*/
# define	PRO_TEST	0020	/* test in qualification	*/
# define	PRO_AGGR	0040	/* retrieve aggregate value	*/

# ifdef BIT_32
# define	PROTERM		4
# else
# define	PROTERM		2
# endif

struct protect
{
	char	prorelid[MAXNAME];	/* relation to which this applies */
	char	prorelown[2];		/* owner			  */
	short	propermid;		/* permission sequence number	  */
	char	prouser[2];		/* user code in PERMIT		  */
	char	proterm[PROTERM];	/* terminal id			  */
	char	proresvar;		/* result variable number	  */
	char	proopset;		/* operation set		  */
	short	protodbgn;		/* beginning time of day	  */
	short	protodend;		/* ending time of day		  */
	char	prodowbgn;		/* beginning day of week	  */
	char	prodowend;		/* ending day of week		  */
	short	prodomset[8];		/* domain set permitted		  */
	short	protree;		/* link to qualification	  */
};

/* field numbers for AAam_find() calls */
# define	PRORELID	1
# define	PRORELOWN	2
# define	PROPERMID	3
# define	PROTREE		16



/*  STRUCT INTEGRITY -- the descriptor for the integrity relation	*/

struct integrity
{
	char	intrelid[MAXNAME];	/* name of the relation		  */
	char	intrelowner[2];		/* owner of the relation	  */
# ifdef BIT_32
	short	inttree;		/* pointer into the tree catalog  */
	short	intdomset[8];		/* set of domains this applies to */
# else
	short	intdomset[8];		/* set of domains this applies to */
	short	inttree;		/* pointer into the tree catalog  */
# endif
	char	intresvar;		/* primary variable number	  */
};

/* field numbers for AAam_find() calls */
# define	INTRELID	1
# define	INTRELOWNER	2
# ifdef BIT_32
# define	INTTREE		3
# else
# define	INTTREE		7
# endif



/*	STRUCT REF    --   AA_REF catalog				*/
/*									*/
/*				declares the relations, which are	*/
/*				bases for views or permits		*/
/*									*/

struct refs
{
	char	refrelid[MAXNAME];
	char	refowner[2];
	char	refbrelid[MAXNAME];
	char	refbowner[2];
	char	reftype;
};

/* field numbers for AAam_find() calls */
/*define	REFRELID	1		**	*/
/*define	REFOWNER	2		**	*/
/*define	REFTYPE		5		**	*/
