/*								*/
/*  QRYMOD.H -- Query Modification header file.			*/
/*								*/
/*	Contains the manifest constants and global variables	*/
/*	used by the query modification process.			*/
/*								*/

/* check out match for this two in decomp.h */
# define	QBUFSIZ		2000		/* query buffer */

/* error message constants */
# define	QBUFFULL	3700		/* tree buffer oflo error */
# define	STACKFULL	3701		/* trbuild stack oflo error */

/* range table */
struct rngtab
{
	char	relid[MAXNAME];		/* relation name */
	int	rstat;			/* relation status */
	char	rowner[2];		/* relation owner */
	char	rused;			/* non-zero if this slot used */
};

extern struct rngtab	Rangev[MAXVAR + 1];
extern int		Remap[MAXVAR + 1];	/* remapping for range table */

extern char	*Qbuf;			/* tree buffer space */
extern int	Qmode;			/* type of query */
extern int	Resultvar;		/* if >= 0, result var name */


extern DESC	Treedes;		/* descriptor for AA_TREE catalog */




/********************************************************************/
/*								    */
/*  The following stuff is used by the protection algorithm only.   */
/*								    */
/********************************************************************/
/* maximum query mode for proopset (<--> sizeof Proopmap - 1) */
# define	MAXPROQM	4
