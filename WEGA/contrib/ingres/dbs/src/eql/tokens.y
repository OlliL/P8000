/*
**  TOKENS.Y -- operator and keyword tables
**
**	Defines:
**		Optab -- operator table
**		Kwrdtab -- keyword tables
**		Kwrdnum -- number of keywords (for binary search in getkey())
**		Tokens -- so lexical routines can know about yacc tokens
*/

struct optab	Kwrdtab[] =
{
	"all",			ALL,			0,
	"and",			LBOP,			0,
	"any",			AOP,			0,
	"append",		APPEND,			0,
	"ascii",		FOP,			0,
	"at",			AT,			0,
	"atan",			FOP,			0,
	"auto",			ALLOC,			opAUTO,
	"avg",			AOP,			0,
	"avgu",			AOP,			0,
	"by",			BY,			0,
	"char",			TYPE,			opSTRING,
	"concat",		FBOP,			0,
	"copy",			COPY,			0,
	"cos",			FOP,			0,
	"count",		AOP,			0,
	"countu",		AOP,			0,
	"create",		CREATE,			0,
	"daba",			DBS,			0,
	"define",		DEFINE,			0,
	"delete",		DELETE,			0,
	"destroy",		DESTROY,		0,
	"double",		TYPE,			opDOUBLE,
	"exit",			EXIT,			0,
	"extern",		ALLOC,			opEXTERN,
	"float",		TYPE,
# ifdef NO_F4
							opDOUBLE,
# else
							opFLT,
# endif
	"float4",		FOP,
# ifdef NO_F4
							sizeof (double),
# else
							sizeof (float),
# endif
	"float8",		FOP,			sizeof (double),
	"from",			FROM,			0,
	"gamma",		FOP,			0,
	"help",			HELP,			0,
	"in",			IN,			0,
	"index",		INDEX,			0,
	"ing_db",		DBS,			0,
# ifdef BIT_32
	"int",			TYPE,			opLONG,
# else
	"int",			TYPE,			opINT,
# endif
# ifndef EBCDIC
	"int1",			FOP,			sizeof (char),
	"int2",			FOP,			sizeof (short),
	"int4",			FOP,			sizeof (long),
# endif
	"integrity",		INTEGRITY,		0,
	"into",			INTO,			0,
# ifdef EBCDIC
	"int1",			FOP,			sizeof (char),
	"int2",			FOP,			sizeof (short),
	"int4",			FOP,			sizeof (long),
# endif
	"is",			IS,			0,
	"log",			FOP,			0,
	"long",			TYPE,			opLONG,
	"max",			AOP,			0,
	"min",			AOP,			0,
	"mod",			FBOP,			0,
	"modify",		MODIFY,			0,
	"not",			LUOP,			0,
	"of",			OF,			0,
	"on",			ON,			0,
	"or",			LBOP,			0,
	"param",		PARAM,			0,
	"permit",		PERMIT,			0,
	"print",		PRINT,			0,
	"rand",			FOP,			0,
	"range",		RANGE,			0,
	"register",		ALLOC,			opREGISTER,
	"replace",		REPLACE,		0,
	"retrieve",		RETRIEVE,		0,
	"save",			SAVE,			0,
	"short",		TYPE,			opINT,
	"sin",			FOP,			0,
	"sqrt",			FOP,			0,
	"static",		ALLOC,			opSTATIC,
	"struct",		STRUCT,			opSTRUCT,
	"sum",			AOP,			0,
	"sumu",			AOP,			0,
	"to",			TO,			0,
	"unique",		UNIQUE,			0,
	"until",		UNTIL,			0,
	"view",			VIEW,			0,
	"where",		WHERE,			0,
};

/* This decalaration must be here as getkey references it,
** and it must contain the number of elements in the
** Kwrdtab, which is only known here
*/

int	Kwrdnum =	sizeof Kwrdtab / sizeof Kwrdtab[0];

struct optab	Optab[] =
{
	/* PUNCTUATION */
	",",			COMMA,			0,
	"(",			LPAREN,			0,
	".",			PERIOD,			0,
	")",			RPAREN,			0,
	"\"",			QUOTE,			0,
	"\/\*",			BGNCMNT,		0,
	"\*\/",			ENDCMNT,		0,
	"#",			NONREF,			0,
	":",			COLON,			0,

	/* C LANUAGE PUNCTUATION */
	"{",			LBRACE,			0,
	"}",			RBRACE,			0,
	"[",			LBRKT,			0,
	"]",			RBRKT,			0,
	";",			SEMICOL,		0,
	"->",			POINTER,		0,

	/* UNARY ARITHMETIC OPERATORS */
	"+",			UOP,			0,
	"-",			UOP,			0,

	/* BINARY ARITHMETIC OPERATORS */
	"*",			BOP,			0,
	"/",			BOP,			0,
	"**",			BOP,			0,

	/* BOUNDS OPERATORS */
	">",			BDOP,			0,
	">=",			BDOP,			0,
	"<",			BDOP,			0,
	"<=",			BDOP,			0,

	/* EQUALITY OPERATORS */
	"!=",			EOP,			0,
	"=",			IS,			0,
	(char *) 0
};

struct special Tokens =
{
	NAME,
	SCONST,
	I2CONST,
	I4CONST,
	F8CONST,
	QUOTE,
	BGNCMNT,
	ENDCMNT,
	C_CODE,
	STRUCT_VAR,
};
