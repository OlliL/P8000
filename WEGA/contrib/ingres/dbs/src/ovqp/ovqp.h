/*	This header file contains the external (global) declarations	    */
/*	of variables and structures as well as the manifest constants	    */
/*	particular to OVQP.						    */
/*									    */
/*	By convention global variable identifiers are spelled with 	    */
/*	an initial capital letter; manifest constants are in caps	    */
/*	completely.							    */
/*									    */
/*	Manifest constants						    */

# define	AAtTFLAG		'O'	/* trace flag		    */

# define	LBUFSIZE	850	/* buffer size for holding qry list */
					/* and concat and ascii buffers     */
# define	NSIMP		15	/* maximum no. of "simple clauses"  */
					/* allowed in Qual list		    */
					/* (see "strategy" portion)	    */
# ifndef	STACKSIZ
# define	STACKSIZ	20	/* Stack size for interpreter	    */
# endif
# define	MAXNODES	(2 * MAXDOM) + 50
					/* max nodes in Qvect		    */

/* symbolic values for GETNXT parameter of fcn GET			    */
# define	CURTUP	0	/* get tuple specified by tid		    */

# ifndef	NXTTUP
# define	NXTTUP	1	/* get nxt tuple after one specified by tid */
# endif


/* symbolic values for CHECKDUPS param of fcn INSERT			    */
# define	DUPS	0	/* allow a duplicate tuple to be inserted   */
# define	NODUPS	1	/* check for and avoid inserting 	    */
				/* a duplicate (if possible)		    */


# define	TIDTYPE		INT
# define	TIDLEN		4

# define	CNTLEN 		4	/* counter for agg computations	    */
# define	CNTTYPE 	INT	/* counter type			    */

# define	ANYLEN		2	/* length for opANY		    */
# define	ANYTYPE		INT	/* type for opANY		    */

/* error codes for errors caused by user query ie. not AAsyserrs		    */

# define	LISTFULL	4100	/* postfix query list full	    */
# define	BADCONV		4101	/*				    */
# define	BADUOPC		4102	/* Unary operator not allowed on    */
					/* char fields			    */
# define	BADMIX		4103	/* can't assign, compare or operate */
					/* a numberic with a char	    */
# define	BADSUMC		4104	/* can't sum char domains (agg)	    */
# define	BADAVG		4105	/* can't avg char domains (agg)	    */
# define	STACKOVER	4106	/* interpreter stack overflow	    */
# define	CBUFULL		4107	/* not enough space for concat or   */
					/* ascii operation		    */
# define	BADCHAR		4108	/* arithmetic operation on two	    */
					/* character fields		    */
# define	NUMERIC		4109	/* numeric field in a character op  */
# define	FLOATEXCEP	4110	/* floating point exception	    */
# define	CHARCONVERT	4111	/* bad ascii to numeric conversion  */
# define	NODOVFLOW	4112	/* node vector overflow		    */
# define	BADSECINDX	4199	/* found a 6.0 sec index	    */


struct _stringp_
{
	char	*stringp;
};
typedef struct _stringp_	STRINGP;

struct stacksym
{
	char	s_type;
	char	s_len;
# ifdef BIT_32
	short	s_alignment;
# endif
	char	s_value[sizeof (double)];
};

extern char		Outtup[MAXTUP];
extern char		Intup[MAXTUP];
extern char		*Origtup;
extern long		Intid;
extern long		Uptid;
extern int		Ov_qmode;	/* flag set to indicate tuple mode  */
extern int		Eql;		/* EQL flag set by initproc	    */
extern int		Bopen;		/* TRUE if batch file is open	    */
extern DESC		*Scanr;		/* desc of reln to be scanned,      */
					/* (i.e. either Srcdesc or Indesc)  */
extern DESC		*Source;	/* 0 if no source for qry,	    */
					/* else points to Srcdesc	    */
extern DESC		*Result;	/* 0 if no result for qry,	    */
					/* else points to Reldesc	    */
extern long		*Counter;	/* cnts "gets" done in OVQP	    */
extern char		*Tend;		/* pts to end of data in Outtup	    */
extern struct symbol	**Tlist;
extern struct symbol	**Alist;
extern struct symbol	**Qlist;
extern struct symbol	**Bylist;
extern int		Newq;		/* flags new user query to OVQP	    */
extern long		Lotid;		/* lo limits of scan in OVQP	    */
extern long		Hitid;		/* hi limits of scan in OVQP	    */
extern struct stacksym	Stack[STACKSIZ];/* stack for OVQP interpreter	    */
extern int		Buflag;		/* flags a batch update situation   */
					/* (Ov_qmode != mdRETR)		    */
extern int		Targvc;		/* var count in Target list	    */
					/* (flags constant Targ. list)	    */
extern int		Qualvc;		/* var count in Qual list	    */
extern int		Userqry;	/* flags a query on the result rel  */
extern int		Retrieve;	/* true is a retrieve, else false   */
extern int		Diffrel;	/* true if			    */
					/* Source and Result are different  */
extern int		Agcount;	/* count # of aggs in the query	    */
extern long		Tupsfound;	/* counts # tuples		    */
					/* which satified the query	    */
extern int		R_decomp;	/* file for reading from decomp	    */
extern int		W_decomp;	/* file for writing to decomp	    */
extern struct pipfrmt	Inpipe;
extern struct pipfrmt	Outpipe;
extern int		Qvpointer;
