/*	Accessparam structure -- this structure is filled by		*/
/*	the AAdparam() and AAiparam() routines. It gives a list of	*/
/*	key domains in their key sequence order.			*/

struct accessparam
{
	short	mode;		/* mode of relation			*/
	short	sec_index;	/* TRUE if relation is a sec index.	*/
	char	keydno[MAXDOM + 1];
};

/*  RETCODE STRUCTURE							*/
/*	This structure is passed back after each query to the front end	*/
/*	to give query status.						*/

struct retcode
{
	long	rc_tupcount;	/* count of tuples which satisfied	*/
	short	rc_status;	/* result status for the query		*/
	char	rc_siteid[2];	/* reserved for distributed decomp	*/
};




/*	Desxx structure -- This structure is used by opencatalog and	*/
/*	closecatalog. It is used to maintain a list of system relations	*/
/*	for caching.							*/

struct desxx
{
	char			*cach_relname;	/* name of the relation	*/
	struct descriptor	*cach_desc;	/* desc to use		*/
	struct descriptor	*cach_alias;	/* alias for above desc	*/
};


/*  Relation status bits						*/
/*									*/
/*	These bits are in the relation relation, in the "relstat"	*/
/*	field.  They define status information about the relation.	*/

# define	S_CATALOG	0000001	/* system catalog		*/
# define	S_NOUPDT	0000002	/* no update allowed		*/
# define	S_PROTUPS	0000004	/* tuples exist in AA_PROTECT	*/
# define	S_INTEG		0000010	/* integrity constrained	*/
/*define	S_CONCUR	0000020	** concurrency enforced		*/
# define	S_VIEW		0000040	/* relation is a view		*/
# define	S_VBASE		0000100	/* base rel for a view		*/
# define	S_INDEX		0000200	/* is a sec indx		*/
# define	S_BINARY	0000400	/* print char domains in binary	*/
# define	S_DISTRIBUTED	0001000	/* reserved for distr rels	*/
/*define	S_DISCRIT	0002000	** reserved for distr temp rels	*/
/*define	S_DISCOPY	0004000	** reserved for distr rels	*/
# define	S_PROTALL	0010000	/* if clear, all permission	*/
# define	S_PROTRET	0020000	/* if clear, read permission	*/

/*  Protection bits are defined as follows:				*/
/*									*/
/*	S_PROTUPS -- if set, there are tuples for this relation in the	*/
/*			AA_PROTECT catalog				*/
/*			Need to look in the AA_PROTECT catalog to tell.	*/
/*									*/
/*	S_PROTALL, S_PROTRET -- as shown from the following table:	*/
/*      PROTALL  PROTRET     meaning					*/
/*            1  1        No permits.					*/
/*            1  0        Permit RETRIEVE to ALL case.			*/
/*            0  1        Permit ALL to ALL case.			*/
/*            0  0        Permit ALL to ALL and RETRIEVE to ALL.	*/

/*  User status bits							*/
/*									*/
/*	These bits are in the status field of the AA_USERS.  They end	*/
/*	up in a variable "AAdbstat" after a call to initucode.		*/

# define	U_CREATDB	0000001	/* can create data bases	*/
# define	U_DRCTUPDT	0000002	/* can specify direct update	*/
# define	U_UPSYSCAT	0000004	/* can update catalogs directly	*/
# define	U_TRACE		0000020	/* can use trace flags		*/
/*define	U_QRYMODOFF	0000040	** can turn off qrymod		*/
# define	U_APROCTAB	0000100	/* can use arbitrary proctab	*/
# define	U_EPROCTAB	0000200	/* can use =proctab form	*/
# define	U_SUPER		0100000	/* DBS superuser		*/

/*	The following defines declare the field number in the 		*/
/*	AA_USERS for each field.					*/

# define	UF_NAME		0		/* login name		*/
# define	UF_UCODE	1		/* user code		*/
# define	UF_UID		2		/* BS user id		*/
/*define	UF_GID		3		** BS group id		*/
# define	UF_STAT		4		/* status bits		*/
# define	UF_FLAGS	5		/* default flags	*/
/*define	UF_PTAB		6		** default proctab	*/
# define	UF_IFILE	7		/* monitor init file	*/
# define	UF_DBLIST	9		/* list of valid DB's	*/

# define	UF_NFIELDS	10		/* TOTAL # of fields	*/

/*	AAusercode contains the current user's DBS user-id code.	*/
/*	AApath contains the name of the DBS subtree.			*/

extern char	*AAusercode;
extern char	*AApath;


/*	The following defines declare the field number in the 		*/
/*	AA_PASSWD for each field.					*/

# define	PF_NAME		0		/* login name		*/
/*define	PF_PASSWD	1		** password		*/
# define	PF_UID		2		/* BS user id		*/
# define	PF_GID		3		/* BS group id		*/
/*define	PF_COMMENT	4		** comment		*/
# define	PF_HOME		5		/* home directory	*/
/*define	PF_CMD		6		** default interpreter	*/


/*	The following structs are for use in type conversion.		*/

struct _i1type_
{
	char	i1type;
};
typedef struct _i1type_		I1TYPE;

struct _i2type_
{
	short	i2type;
};
typedef struct _i2type_		I2TYPE;

struct _i4type_
{
	long	i4type;
};
typedef struct _i4type_		I4TYPE;

# ifndef NO_F4
struct _f4type_
{
	float	f4type;
};
typedef struct _f4type_		F4TYPE;
# endif

struct _f8type_
{
	double	f8type;
};
typedef struct _f8type_		F8TYPE;



/*  PRINTED OUTPUT ARGUMENTS						*/
/*									*/
/*	The following struct describes the printed output available	*/
/*	to the user.							*/

struct out_arg
{
	short	c0width;	/* minimum width of "c" field		*/
	short	i1width;	/* width of "i1" field			*/
	short	i2width;	/* width of "i2" field			*/
	short	i4width;	/* width of "i4" field			*/
# ifndef NO_F4
	short	f4width;	/* width of "f4" field			*/
# endif
	short	f8width;	/* width of "f8" field			*/
# ifndef NO_F4
	short	f4prec;		/* number of decimal places on "f4"	*/
# endif
	short	f8prec;		/* number of decimal places on "f8"	*/
# ifndef NO_F4
	char	f4style;	/* "f4" output style			*/
# endif
	char	f8style;	/* "f8" output style			*/
	short	linesperpage;	/* number of lines per output page	*/
	char	coldelim;	/* column delimiter			*/
	char	linedelim;	/* line delimiter			*/
};

/* maximum width of any of the above fields				*/
# define	MAXFIELD	255

/*  Any text line read from a file (for example, USERFILES) can		*/
/*	be at most MAXLINE bytes long.  Buffers designed for holding	*/
/*	such info should be decleared as char buf[MAXLINE + 1] to	*/
/*	allow for the null terminator.					*/

# define	MAXLINE		256


/*  Assorted system stuff						*/
/*									*/
/*	FILEMODE is the file mode on a 'creat' call for all files	*/
/*		in the database and probably other external files.	*/

# define	FILEMODE	0600		/* db file mode		*/

/*	Defines to specify the execid of each process in the		*/
/*	current process structure.					*/
/*	The constants are named with "EXEC_" AAconcatenated with	*/
/*	the process name.						*/

# define	EXEC_DBU	'#'	/* db utilities (or overlays)	*/
# define	EXEC_DECOMP	'$'	/* decomposition process	*/
# define	EXEC_OVQP	'*'	/* one variable query processor	*/
# define	EXEC_QRYMOD	'*'	/* query modification for	*/
					/* view, protection, integrity	*/
# define	EXEC_PARSER	'@'	/* parser, scanner		*/
/*define	EXEC_FRONT		** EQL prog or terminal monitor	*/
# define	EXEC_ERROR	'%'	/* exec_id of an error block	*/


/* stuff giving information about the machine				*/

# ifdef S_ALIGN
# define	ALIGNMENT(x)	(((x) + 01) & ~01)
# endif
# ifdef L_ALIGN
# define	ALIGNMENT(x)	(((x) + 03) & ~03)
# endif
# ifdef D_ALIGN
# define	ALIGNMENT(x)	(((x) + 07) & ~07)
# endif
