/*
** ---  RC.H ------------ BASIC DECLARATIONS  ---------------------------------
**
**	Report Compiler Definitions
**
**		contains the  global  structures and  variable
**		declarations  needed by the  lexical analyzer.
**		This includes  Manifest  constants and certain
**		variables for internal communication purposes.
**		Therefore,  extreme  care  should be exercised
**		when modifying this file.
**
**	Version: @(#)rc.h		4.0	02/05/89
**
*/


# include	"r_code.h"


/* SYSTEM VARIABLES */

# define	SYS_COL		1
# define	SYS_LINE	2
# define	SYS_ALL_LINE	3
# define	SYS_PAGE	4
# define	SYS_TUPL	5


/* CHARACTER TYPES */

# define	ALPHA		1
# define	NUMBR		2
# define	OPATR		3
# define	PUNCT		4
# define	CNTRL		5


/* character type mapping */

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


/* INTEGER CONSTANT */

# define	GOVAL		-1


/* SYMBOL TABLE ENTRY */

struct s_entry
{
	struct s_entry	*nextsym;
	char		symbl[1];
};



/* KEYWORD AND OPERATOR TABLE */

struct optab
{
	char		*term;
	int		token;
	int		opcode;
};
typedef struct optab		OPTAB;

					/* in rc_tables.y		     */
extern struct optab	Optab[];	/* operator table		     */
extern struct optab	Keyword[];	/* keyword table		     */
extern int		Keyent;		/* # of entries in the Keyword table */


/* SPECIAL TOKENS FOR SCANNER */

struct special
{
	int		name;
	int		sconst;
	int		bgncmnt;
	int		endcmnt;
	int		iconst;
	int		lconst;
	int		rconst;
};

					/* in rc_tables.y	      */
extern struct special	Tokens;		/* special tokens for scanner */


/* FORMAT STRUCTURE (formatted output) */

struct format
{
	int		data;
	int		length;
	double		scale;
	int		str_len;
	char		*str_adr;
	int		before;
	int		point;
	int		ignore;
	int		norm;
	char		*fr_to[2];
	int		batch_len;
	int		fr_to_v[2];
	double		fr_to_8[2];
};

/* PROCEDURE handling */

# define	MAX_PROCS	128

extern char		*Proc[];


/* CASE HANDLING */

# define	MAX_CASES	50

extern int		Case_no;
extern char		*Case[];


/* MACRO HANDLING */

# define	MAX_MACROS	500

struct macro
{
	char		*templt;
	int		subst;
};

extern int		Macro_no;
extern struct macro	Macro[];


/* DECLARATIONS */

extern struct desc_t	Desc[MAX_DOM];	/* domain desriptions		*/
extern int		Line;		/* source line counter		*/
extern int		Lcase;		/* UPPER->lower conversion flag */
extern int		Compile;	/* FALSE for only compilation	*/
extern int		Iflag;		/* read from standard input	*/
extern int		Oflag;		/* write to file "r_code"	*/
extern int		Sflag;		/* be silent if FALSE		*/
extern int		S_error;	/* # of errors found		*/
extern char		*S_name;	/* name of R_CODE file		*/
extern char		*S_desc;	/* name of DESC file		*/
