# ifdef TRACE
# include	"../h/trace.h"
# endif

/*	DBS manifest constants						    */
/*									    */
/*	These constants are manifest to the operation of the entire	    */
/*	system.  If anything						    */
/*	is changed part or all of the system will stop working.		    */
/*	The values have been carefully chosen and are not intended	    */
/*	to be modifiable by users.					    */

# ifdef MEM_E
# define	MAXDOM		128	/* max. number + 1 of domains	    */
# else
# define	MAXDOM		50	/* in a relation		    */
# endif
# define	MINTUP		498	/* min size of MAXTUP		    */
# ifdef MEM_N
# define	MAXTUP		MINTUP	/* max size (in bytes) of a tuple   */
# else
# define	MAXTUP		1010
# endif

# define	INT		30	/* INTEGER CONSTANT		    */
# define	FLOAT		31	/* FLOATING POINT CONSTANT	    */
# define	CHAR		32	/* CHARACTER CONSTANT		    */

# define	MAXNAME		12	/* max size of a name (in bytes)    */
# define	MAXVAR		10	/* max # of variables		    */
# define	MAXKEYS		6	/* max # of keys in secondary index */
# define	MAXAGG		50	/* max number of aggs in a qry	    */
# define	MAXPARMS	(MAXDOM * 2 + 20)
					/* max number of parameters	    */
					/* to the DBU controller	    */
					/* (allows for 2 per domain for	    */
					/* create which is the worst case   */
# define	STACKSIZ	20	/* max depth for arith. expr.stacks */

# ifndef	TRUE
# define	TRUE		1	/* logical  one,  true, yes,  ok    */
# define	FALSE		0	/* logical zero, false,  no, nop    */
# endif
# ifndef	NULL
# define	NULL		0	/* the null pointer or null char    */
# endif

# define	i_1		char
# define	i_2		short
# define	i_4		long
# define	c_1		char
# define	c_2		char
# define	c_12		char



/*	RELATION relation struct					*/
/*									*/
/*	The RELATION relation contains one tuple for each relation	*/
/*	in the database.  This relation contains information which	*/
/*	describes how each relation is actually stored in the		*/
/*	database, who the owner is, information about its size,		*/
/*	assorted operation information, etc.				*/

struct relation
{
	c_12	relid[MAXNAME];	/* relation name			   */
	c_2	relowner[2];	/* code of relation owner		   */
	i_1	relspec;	/* storage mode of relation		   */
				/* M_HEAP  unsorted paged heap		   */
				/* -M_HEAP compressed heap		   */
				/* M_ISAM  isam				   */
				/* -M_ISAM compressed isam		   */
				/* M_HASH  hashed			   */
				/* -M_HASH compressed hash		   */
	i_1	relindxd;	/* -1 rel is an index			   */
				/*  0	     not indexed		   */
				/*  1	     indexed			   */
# ifndef BIT_32
	i_2	relstat;	/* relation status bits			   */
# endif
	i_4	relsave;	/* bs time until which relation is saved   */
	i_4	reltups;	/* number of tuples in relation		   */
	i_2	relatts;	/* number of attributes in relation	   */
	i_2	relwid;		/* width (in bytes) of relation		   */
	i_4	relprim;	/* no. of primary pages in relation	   */
	i_2	relspare;	/* count for relation references	   */
				/* via VIEW(QL) and PERMIT(QL)		   */
	c_2	relock[2];	/* locking area				   */
# define	P_SHR		001	/*	shared page lock	   */
# define	P_EXCL		002	/*	exclusive page lock	   */
# define	R_SHR		010	/*	shared relation lock	   */
# define	R_EXCL		020	/*	exclusive relation lock	   */
# define	R_LOCKED	030	/*	relation lock		   */
# ifdef BIT_32
	i_2	relstat;	/* relation status bits			   */
# endif
};

/* field numbers for AAam_find() calls */
# define	RELID		1
# define	RELOWNER	2



/*	ATTRIBUTE relation struct					*/
/*									*/
/*	The ATTRIBUTE relation contains one tuple for each domain	*/
/*	of each relation in the database.  This relation describes	*/
/*	the position of each domain in the tuple, its format,		*/
/*	its length, and whether or not it is used in part of the key.	*/

struct attribute
{
	c_12 	attrelid[MAXNAME];	/* relation name of which this is */
					/* an attr			  */
	c_2	attowner[2];		/* code of relation owner	  */
	c_12	attname[MAXNAME];	/* alias for this domain	  */
	i_2	attid;			/* domain # (from 1 to relatts)	  */
	i_2	attoff;			/* offset in tuple (no. of bytes) */
	i_1	attfrmt;		/* INT, FLOAT, CHAR (in symbol.h) */
	i_1	attfrml;		/* unsigned integer no of bytes	  */
	i_1	attxtra;		/* flag indicating whether this	  */
					/* dom is part of a key		  */
};

/* field numbers for AAam_find() calls */
# define	ATTRELID	1
# define	ATTOWNER	2
# define	ATTNAME		3
# define	ATTID		4



/*	DESCRIPTOR struct						*/
/*									*/
/*	The DESCRIPTOR is initialized by AArelopen to describe any	*/
/*	open relation.  The first part of the descriptor is the tuple	*/
/*	from the catalog AA_REL.  The remainder contains some magic	*/
/*	numbers and a template initialized from the catalog AA_ATT.	*/

struct descriptor
{
	struct relation		reldum;
		/* The above part of the descriptor struct is identical	    */
		/* to the relation struct and the inormation in this	    */
		/* part of the struct is read directly from the		    */
		/* relation tuple by AArelopen. The rest of the descriptor  */
		/* struct is calculated by AArelopen.			    */
	i_2	relfp;			/* filep for relation , if open	    */
	i_2	relopn;			/* indicates if rel is really open  */
	i_4	reltid;			/* when relation is open, this	    */
					/* indicates the tid in the catalog */
					/* AA_REL for this relation	    */
	i_4	reladds;		/* no. of additions of tuples	    */
					/* during this open		    */
	i_2	reloff[MAXDOM];		/* reloff[i] is offset to domain i  */
	c_1	relfrmt[MAXDOM];	/* format of domain i		    */
					/* INT, FLOAT, or CHAR		    */
	c_1	relfrml[MAXDOM];	/* relfrml[i] is an unsigned int    */
					/* which indicates length	    */
					/* in bytes of domain		    */
	c_1	relxtra[MAXDOM];	/* relxtra[i] is none zero if dom i */
					/* is a key domain for the relation */
	c_1	relgiven[MAXDOM];	/* cleared by AArelopen and set     */
					/* before call to find to indicate  */
					/* value of this domain has been    */
					/* supplied in the key		    */
};
typedef struct descriptor	DESC;

/* functions to fill relopn field from relfp field */
# define	RELROPEN(fp)	((fp + 1) *  5)
# define	RELWOPEN(fp)	((fp + 1) * -5)

# define	NOKEY		1	/* scan entire relation	*/
# define	EXACTKEY	2
# define	LRANGEKEY	3	/* low range key		*/
# define	FULLKEY		4	/* forces full key comparison	*/
# define	HRANGEKEY	5	/* high range key		*/


/*	tuple id struct		*/
# ifdef		REV_BYTES
# ifdef		REV_WORDS
struct tup_id { char	line_id, pg2, pg1, pg0; };
struct lpage  { char	lpg2, lpg1, lpg0, lpgx; };
# else
struct tup_id { char	pg1, pg0, line_id, pg2; };
struct lpage  { char	lpg0, lpgx, lpg2, lpg1; };
# endif
# else
# ifdef		REV_WORDS
struct tup_id { char	pg2, line_id, pg0, pg1; };
struct lpage  { char	lpg1, lpg2, lpgx, lpg0; };
# else
struct tup_id { char	pg0, pg1, pg2, line_id; };
struct lpage  { char	lpgx, lpg0, lpg1, lpg2; };
# endif
# endif
