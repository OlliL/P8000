# define	PARBUFSIZ	2000	/* size of buffer for dbu commands */
# define	TREEMAX		2500	/* max number of bytes for tree */
# define	MAXATT		150	/* max number of attributes in the att stash */

/* mode parameters for range table manipulation */
# define	LOOKREL		1
# define	LOOKVAR		2
# define	R_INTERNAL	3
# define	R_EXTERNAL	4

/* error numbers */
# define	TREEOFLO	2118	/* over flow tree buffer */
# define	PBUFOFLO	2106	/* over flow dbu arg buffer */

# define	NOATTRIN	2100	/* attrib not in relation */
# define	CANTUPDATE	2107	/* can't update rel */
# define	NOVBLE		2109	/* vble not declared */
# define	NOPATMAT	2120	/* no pattern matching in tl */
# define	RNGEXIST	2117	/* can't find rel for var */
# define	REPALL		2123	/* x.all on replace */
# define	BADCONSTOP	2134	/* bad constant operator */

# define	INDEXTRA	2111	/* too many atts in key */
# define	RESXTRA		2130	/* too many resdoms in tl */
# define	TARGXTRA	2131	/* tl larger than MAXTUP */
# define	AGGXTRA		2132	/* too many aggs */

# define	MODTYPE		2119	/* type conflict for MOD */
# define	CONCATTYPE	2121	/* type conflict for CONCAT */
# define	AVGTYPE		2125	/* type conflict for AVG(U) */
# define	SUMTYPE		2126	/* type conflict for SUM(U) */
# define	FOPTYPE		2127	/* type conflict for func ops */
# define	UOPTYPE		2128	/* type conflict for unary ops */
# define	NUMTYPE		2129	/* type conflict for numeric ops */
# define	RELTYPE		2133	/* type conflict for relatv op */

# define	RESTYPE		2103	/* result type mismatch w/expr */
# define	RESAPPEX	2108	/* append res rel not exist */
# define	RESEXIST	2135	/* result rel already exists */

# define	NXTCMDERR	2501	/* misspelt where problem */
# define	NOQRYMOD	2139	/* no qrymod in database */

# define	BADHOURS	2136	/* no such hour */
# define	BADMINS		2137	/* no such minute */
# define	BAD24TIME	2138	/* only 24:00 can be used */

/* -- ASSORTED DATA STRUCTURES -- */
struct atstash					/* attribute table */
{
	char		atbid;			/* attribute number */
	char		atbfrmt;		/* attribute form type */
	char		atbfrml;		/* attribute form length */
	char		atbname[MAXNAME];	/* attribute name */
	struct atstash	*atbnext;		/* pointer to next entry in chain */
};

struct rngtab				/* range table */
{
	short		rentno;			/* variable number of this table position */
	char		rmark;			/* was it used in this command */
	char		ractiv;			/* is entry empty */
	short		rposit;			/* lru position of entry */
	char		varname[MAXNAME + 1];	/* variable name */
	char		relnm[MAXNAME + 1];	/* relation name */
	char		relnowner[2];		/* relation owner */
	short		rstat;			/* relstat field of relation relation */
	short		ratts;			/* number of attributes in relation */
	struct atstash	*attlist;		/* head of attrib list for this reln */
};

struct constop				/* constant operator lookup table */
{
	char	*copname;		/* string name for identification */
	short	copnum;			/* op number */
	char	coptype;		/* op result type for formating */
	char	coplen;			/* op result length for formatting */
};

extern struct rngtab	*Resrng;	/* ptr to result reln entry */
extern struct atstash	*Freeatt;	/* free list of attrib stash */
extern struct querytree	*Lastree;	/* pointer to root node of tree */
extern DESC		Reldesc;	/* descriptor for range table lookup */
extern DESC		Desc;		/* descriptor for attribute relation */
extern struct atstash	Faketid;	/* atstash structure for TID node */
#ifdef	DISTRIB
extern struct atstash	Fakesid;	/* atstash structure for SID node */
#endif

extern short		Rsdmno;		/* result domain number */
extern int		Opflag;		/* operator flag contains query mode */
extern char		*Relspec;	/* ptr to storage structure of result relation */
extern char		*Indexspec;	/* ptr to stor strctr of index */
extern char		*Indexname;	/* ptr to name of index */
extern char		Trfrmt;		/* format for type checking */
extern char		Trfrml;		/* format length for type checking */
extern char		*Trname;	/* pointer to attribute name */
extern int		Agflag;		/* how many aggs in this qry */
extern int		Eql;		/* indicates EQL preprocessor on */
extern int		Qrymod;		/* qrymod on in database flag */
extern int		Dbserr;		/* set if a query returns an error from processes below */
extern int		Patflag;	/* signals a pattern match reduction */
extern int		Qlflag;		/* set when processing a qual */
extern int		Noupdt;		/* DBS user override of no update restriction */
extern int		yypflag;	/* disable action stmts if = 0 */
extern int		yyline;		/* line counter */
extern int		Dcase;		/* default case mapping */
extern char		**Pv;		/* ptr to list of dbu params */
extern int		Pc;		/* number of dbu commands */
extern char		*Qbuf;		/* buffer which holds tree allocated on stack in main.c to get data space */
extern short		Permcomd;
extern char		*AA_iocv();

# ifdef ESER_PSU
# define CAST(x)	((char *) ((unsigned short) (x)))
# else
# define CAST(x)	((char *) (x))
# endif

/* character type mapping */
# define	ASCII_MASK	0177
# define	BYTE_MASK	0377
extern char			Cmap[];
# ifdef EBCDIC
# ifdef ESER_PSU
extern char			_etoa_[];
# define	ETOA(c)		_etoa_[(c) & BYTE_MASK]
# endif
# define	CMAP(c)		Cmap[(ETOA(c)) & ASCII_MASK]
# else
# define	CMAP(c)		Cmap[(c) & ASCII_MASK]
# endif
