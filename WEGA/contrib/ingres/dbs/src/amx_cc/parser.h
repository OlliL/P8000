/*
**	Compiler Definitions
**
**		contains the  global  structures and  variable
**		declarations  needed by the  lexical analyzer.
**		This includes  Manifest  constants and certain
**		variables for internal communication purposes.
**		Therefore,  extreme  care  should be exercised
**		when modifying this file.
**
*/

/* CHARACTER TYPES */
# define	ALPHA		1
# define	NUMBR		2
# define	OPATR		3
# define	PUNCT		4
# define	CNTRL		5

/* C-VAR SYMBOLS */
# define	MAXCVAR		255
# if L_ALIGN > 0
# define	ODD		03
# endif
# if D_ALIGN > 0
# define	ODD		07
# endif
# ifndef ODD
# define	ODD		01
# endif
# define	NOTYPE		-1
# define	C_VAR		-2
# define	C_CONST		-4

# define	opCHAR		0
# define	opINT		1
# define	opLONG		2
# ifndef NO_F4
# define	opFLT		3
# endif
# define	opDOUBLE	4
# define	opSTRING	5

# define	opAUTO		0
# define	opSTATIC	1
# define	opEXTERN	2

extern char		*TYPE_SPEC[];
extern char		*SC_SPEC[];
extern char		*CONVERT[];

/* SYMBOL TABLE ENTRY */
struct s_entry
{
	struct s_entry	*nextsym;
	double		symbl[1];
};
extern struct s_entry	*Symtab;

/* C-VAR TABLE ENTRY */
struct cvartbl
{
	int		vartype;
	char		varname[MAXNAME + 1];
};

/* KEYWORD AND OPERATOR TABLE */
struct optab
{
	char		*term;
	int		token;
	int		opcode;
};
typedef struct optab		OPTAB;

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
extern struct special	Tokens;		/* special tokens for scanner */

/* DECLARATIONS */
extern int		Newline;	/* flag to indicate line end	*/
extern int		Line;		/* source line counter		*/
extern int		Type_spec;	/* type of C-variable		*/
extern int		Lcase;		/* UPPER->lower conversion flag */
extern int		Compile;	/* FALSE for only compilation	*/
extern int		Cvarindex;
extern struct cvartbl	Cvartbl[];
extern int		Type_spec;
