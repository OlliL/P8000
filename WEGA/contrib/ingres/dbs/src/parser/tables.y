/*
**	SCANNER KEYWORD TABLES
**
**	Keywords, tokens, and opcode tuples are included in this file
**	The keyword table MUST be in sorted order.
**	The operator table does not need to be sorted
*/
struct optab	Keyword[] =
{
	"abs",			FOP,			(short) opABS,
	"all",			ALL,			0,
	"and",			LBOP,			AND,
	"any",			AGOP, 			(short) opANY,
	"append",		APPEND,			GOVAL,
	"ascii",		FOP,			(short) opASCII,
	"at",			AT,			0,
	"atan",			FOP,			(short) opATAN,
	"avg",			AGOP,			(short) opAVG,
	"avgu",			AGOP,			(short) opAVGU,
	"by",			BY,			(short) opEQ,
	"concat", 		FBOP,			(short) opCONCAT,
	"copy",			COPY,			GOVAL,
	"cos",			FOP,			(short) opCOS,
	"count",		AGOP,			(short) opCNT,
	"countu",		AGOP,			(short) opCNTU,
	"create",		CREATE,			GOVAL,
	"define",		DEFINE,			GOVAL,
	"delete",		DELETE,			GOVAL,
	"destroy",		DESTROY,		GOVAL,
#	ifdef	DISTRIB
	"distribute",		DISTRIBUTE,		0,
	"distributed",		DISTRD,			0,
#	endif
	"exp",			FOP,			(short) opEXP,
	"float4",		FOP,
# ifndef NO_F4
							(short) opFLT4,
# else
							opFLT8,
# endif
	"float8",		FOP,			(short) opFLT8,
	"from",			FROM,			0,
	"help",			HELP,			GOVAL,
	"in",			IN,			0,
	"index",		INDEX,			GOVAL,
# ifndef EBCDIC
	"int1",			FOP,			(short) opINT1,
	"int2",			FOP,			(short) opINT2,
	"int4",			FOP,			(short) opINT4,
# endif
	"integrity",		INTEGRITY,		0,
	"into",			INTO,			0,
# ifdef EBCDIC
	"int1",			FOP,			opINT1,
	"int2",			FOP,			opINT2,
	"int4",			FOP,			opINT4,
# endif
	"is",			IS,			(short) opEQ,
	"log",			FOP,			(short) opLOG,
	"max",			AGOP,			(short) opMAX,
	"min",			AGOP,			(short) opMIN,
	"mod",			FBOP,			(short) opMOD,
	"modify",		MODIFY,			GOVAL,
	"not",			LUOP,			(short) opNOT,
	"of",			OF,			0,
	"on",			ON,			0,
	"or",			LBOP,			OR,
	"permit",		PERMIT,			0,
	"print",		PRINT,			GOVAL,
	"range",		RANGE,			GOVAL,
	"replace",		REPLACE,		GOVAL,
	"retrieve",		RETRIEVE,		GOVAL,
	"save",			SAVE,			GOVAL,
	"sin",			FOP,			(short) opSIN,
	"sqrt",			FOP,			(short) opSQRT,
	"sum",			AGOP,			(short) opSUM,
	"sumu",			AGOP,			(short) opSUMU,
	"to",			TO,			0,
	"unique",		UNIQUE,			0,
	"until",		UNTIL,			0,
	"view",			VIEW,			0,
	"where",		WHERE,			0,
};
int	Keyent =	sizeof Keyword / sizeof Keyword[0];

struct optab	Optab[] =
{
	/* PUNCTUATION */
	",",			COMMA,			0,
	"(",			LPAREN,			0,
	".",			PERIOD,			0,
	")",			RPAREN,			0,
	"\"",			SCONST,			0,
	"\/\*",			BGNCMNT,		0,
	"\*\/",			ENDCMNT,		0,
	":",			COLON,			0,

	/* UNARY ARITHMETIC OPERATORS */
	"+",			UAOP,			(short) opADD,
	"-",			UAOP,			(short) opSUB,

	/* BINARY ARITHMETIC OPERATORS */
	"*",			BAOP,			(short) opMUL,
	"/",			BAOP,			(short) opDIV,
	"**",			BAOPH,			(short) opPOW,

	/* BOUNDS OPERATORS */
	">",			BDOP,			(short) opGT,
	">=",			BDOP,			(short) opGE,
	"<",			BDOP,			(short) opLT,
	"<=",			BDOP,			(short) opLE,

	/* EQUALITY OPERATORS */
	"!=",			EOP,			(short) opNE,
	"=",			IS,			(short) opEQ,
	(char *) 0
};

/* constant operators table for lookup */
struct constop	Coptab[] =
{
	"dba",		(short) opDBA,		CHAR,		2,
	"usercode",	(short) opUSERCODE,	CHAR,		2,
	(char *) 0,	0,			0,		0,
	"date",		(short) opDATE,		'?',		'?',
	"time",		(short) opDATE,		'?',		'?',
	(char *) 0,	0,			0,		0
};


/*
** Tokens
** a structure initialized here to contain the
** yacc generated tokens for the indicated
** terminal symbols.
*/
struct special	Tokens =
{
	SCONST,
	BGNCMNT,
	ENDCMNT,
	I2CONST,
	I4CONST,
# ifndef NO_F4
	F4CONST,
# endif
	F8CONST,
	NAME
};
