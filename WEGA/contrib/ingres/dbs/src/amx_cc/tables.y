/*
**	SCANNER KEYWORD AND OPERATOR TABLES
**
**		Keywords, Tokens, and Opcode tuples are included
**		in this file the keyword table MUST be in sorted
**		order.  The  operator table does not need to be
**		sorted.
*/

OPTAB	Keyword[] =
{
/*	term		token		opcode
**	--------------------------------------		*/
# ifdef PAGE_LOCKING
	"abort",	ABORT,		0,
# endif
	"and",		COMMA,		0,
	"append",	APPEND,		0,
	"auto",		ALLOC,		opAUTO,
	"break",	GOTO,		1,
	"char",		TYPE,		opSTRING,
	"close",	CLOSE,		0,
	"closedb",	EXIT,		0,
	"commit",	COMMIT,		0,
	"continue",	GOTO,		0,
	"daba",		DBS,		M_EXCL,
# ifdef PAGE_LOCKING
	"deadlock",	DEADLK,		0,
# endif
	"delete",	DELETE,		0,
	"double",	TYPE,		opDOUBLE,
	"endretrieve",	GOTO,		1,
	"exclusive",	DBS,		M_EXCL,
# ifdef AMX_SC
	"export",	EXPORT,		0,
# endif
	"extern",	ALLOC,		opEXTERN,
# ifndef EBCDIC
	"f4",		TYPE,
# ifdef NO_F4
					opDOUBLE,
# else
					opFLT,
# endif
	"f8",		TYPE,		opDOUBLE,
# endif
	"float",	TYPE,
# ifdef NO_F4
					opDOUBLE,
# else
					opFLT,
# endif
# ifdef EBCDIC
	"f4",		TYPE,
# ifdef NO_F4
					opDOUBLE,
# else
					opFLT,
# endif
	"f8",		TYPE,		opDOUBLE,
# endif
	"get",		GET,		0,
	"group",	USER,		1,
# ifndef EBCDIC
	"i1",		TYPE,		opINT,
	"i2",		TYPE,		opINT,
	"i4",		TYPE,		opLONG,
# endif
# ifdef PAGE_LOCKING
	"if",		IF,		0,
# endif
# ifdef AMX_SC
	"import",	IMPORT,		0,
# endif
	"ing_db",	DBS,		M_EXCL,
	"insert",	INSERT,		0,
	"int",		TYPE,
# ifdef BIT_32
					opLONG,
# else
					opINT,
# endif
# ifdef EBCDIC
	"i1",		TYPE,		opINT,
	"i2",		TYPE,		opINT,
	"i4",		TYPE,		opLONG,
# endif
# ifdef PAGE_LOCKING
	"level",	LEVEL,		0,
# endif
	"long",		TYPE,		opLONG,
# ifdef PAGE_LOCKING
	"maxlocks",	MAXLK,		0,
# endif
	"nextretrieve",	GOTO,		0,
	"nolock",	DBS,		0,
	"open",		OPEN,		0,
	"over",		OVER,		0,
# ifdef PAGE_LOCKING
	"page",		RL_PG,		M_REL,
# endif
	"permit",	PERMIT,		0,
	"print",	PRINT,		0,
# ifdef PAGE_LOCKING
	"readlock",	READLK,		0,
# endif
	"replace",	REPLACE,	0,
	"retrieve",	SCAN,		0,
	"scan",		SCAN,		0,
	"shared",	DBS,		M_SHARE,
	"short",	TYPE,		opINT,
	"static",	ALLOC,		opSTATIC,
# ifdef PAGE_LOCKING
	"table",	RL_PG,		0,
# endif
	"tid",		TYPE,		opLONG,
# ifdef PAGE_LOCKING
	"timeout",	TIMEOUT,	0,
# endif
	"tuplecount",	COUNT,		0,
	"tuplelength",	LENGTH,		0,
	"use",		USE,		0,
	"user",		USER,		0,
# ifdef PAGE_LOCKING
	"with",		WITH,		0,
# endif
/*	--------------------------------------		*/
};
int	Keyent =	sizeof Keyword / sizeof Keyword[0];

OPTAB	Optab[] =
{
/*	PUNCTUATION
**	--------------------------------------
**	term		token		opcode
**	--------------------------------------		*/
	"(",		LPAREN,		0,
	")",		RPAREN,		0,
	"{",		LBRACE,		0,
	"}",		RBRACE,		0,
	"[",		LBRKT,		0,
	"]",		RBRKT,		0,
	"\/\*",		BGNCMNT,	0,
	"\*\/",		ENDCMNT,	0,
	"\"",		SCONST,		0,
	"$",		DOM,		0,
	"+",		PLUS,		0,
	"-",		MINUS,		0,
	",",		COMMA,		0,
	";",		SEMICOL,	0,
	"*",		STAR,		0,
/*	--------------------------------------
**
**	BOUNDS OPERATORS
**	--------------------------------------
**	term		token		opcode
**	--------------------------------------		*/
	">",		BDOP,		AP_GT,
	">=",		BDOP,		AP_GT | AP_GE,
	"<",		BDOP,		AP_LT,
	"<=",		BDOP,		AP_LT | AP_LE,
/*	--------------------------------------
**
**	EQUALITY OPERATORS
**	--------------------------------------
**	term		token		opcode
**	--------------------------------------		*/
	"=",		IS,		AP_EQ,
	"==",		IS,		AP_EQ,
	"<>",		BDOP,		AP_NE,
	"!=",		BDOP,		AP_NE,
/*	--------------------------------------		*/
	(char *) 0
};

/*
**	Tokens
**	A structur initialized here to contain the YACC
**	generated tokens for the indicated terminal symbols.
*/
struct special	Tokens =
{
	/* name    = */		NAME,
	/* sconst  = */		SCONST,
	/* bgncmnt = */		BGNCMNT,
	/* endcmnt = */		ENDCMNT,
	/* iconst  = */		ICONST,
	/* lconst  = */		LCONST,
	/* rconst  = */		RCONST
};
