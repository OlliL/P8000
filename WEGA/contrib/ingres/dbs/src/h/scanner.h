/*	SCANNER.H							*/
/*	contains the global structures and variable declarations needed	*/
/*	by the lexical analyzer.  This includes Manifest Constants and	*/
/*	certain vars for internal communication purposes.  Therefore,	*/
/*	extreme care should be exercised when modifying this file.	*/


/* MANIFEST CONSTANTS */
# define	SBUFSIZ		2000	/* max size of symbol table for */
# define	MAXSTRING	255	/* max length of strings	*/
# define	GOVAL		-1	/* semantic value for cmd names */

/* CHARACTER TYPES */
# define	ALPHA		1
# define	NUMBR		2
# define	OPATR		3
# define	PUNCT		4
# define	CNTRL		5

/* Modes for input from EQL front end */
# define	CVAR_I2		'\1'	/* 2 byte integer		*/
# define	CVAR_F8		'\4'	/* 8 byte floating number	*/
# define	CVAR_S		'\3'	/* string with null byte	*/
# define	CVAR_I4		'\6'	/* 4 byte integer		*/

/* error number definitions */
# define	SYMERR		2500	/* syntactic error		*/
# define	STRTERM		2700	/* non term string		*/
# define	STRLONG		2701	/* string too long		*/
# define	BADOP		2702	/* can't find operator in table	*/
# define	NAMELONG	2703	/* name too long		*/
# define	SBUFOFLO	2704	/* over flow symbol table	*/
# define	COMMTERM	2705	/* non term comment		*/
# define	FCONSTERR	2707	/* float constant error		*/
# define	CNTRLCHR	2708	/* control char from EQL	*/
# define	NUMBUFOFLO	2709	/* buffer oflo in number.c	*/

/* KEYWORD and OPERATOR TABLE */
struct optab				/* key word/operator tables	*/
{
	char	*term;			/* key word/operator body	*/
	short	token;			/* associated parser token	*/
	short	opcode;			/* associated parser opcode	*/
};

/* SPECIAL TOKENS for scanner */
struct special
{
	short	sconst;
	short	bgncmnt;
	short	endcmnt;
	short	i2const;
	short	i4const;
# ifndef NO_F4
	short	f4const;
# endif
	short	f8const;
	short	name;
};

/* last token struct */
struct lastok
{
	short	toktyp;
	char	*tok;
	short	tokop;
};

/* declarations */
extern struct special	Tokens;		/* special tokens table		*/
extern struct optab	Optab[];	/* operator table		*/
extern struct optab	Keyword[];	/* keyword table		*/
extern struct lastok	Lastok;
extern int		Lcase;		/* UPPER->lower conversion flag	*/
extern int		Pars;		/* flag for call getcvar or not	*/
extern int		Keyent;	/* # of entries in the Keyword table	*/

