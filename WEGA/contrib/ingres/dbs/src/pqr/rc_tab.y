/*
** ---  RC_TAB.Y -------- SCANNER TABLES  -------------------------------------
**
**	SCANNER KEYWORD AND OPERATOR TABLES
**
**		Keywords, Tokens, and Opcode tuples are included
**		in this file the keyword table MUST be in sorted
**		order.  The  operator table does not need to be
**		sorted.
**
**	Version: @(#)rc_tab.y		4.0	02/05/89
**
*/


OPTAB	Keyword[] =
{
/*	term		token		opcode
**	--------------------------------------		*/
	"all",		ALL,		0,
	"and",		LBOP,		(int) rcAND,
	"as",		AS,		0,
	"batch",	BATCH,		0,
	"batchline",	EXCEPTION,	(int) onBATCH,
	"before",	BEFORE,		0,
	"big",		BIG,		0,
	"bit",		FORMAT,		(int) fmtB_BIT,
	"byte",		BYTE,		0,
	"call",		CALL,		0,
	"case",		CASE,		0,
	"char",		F_CHAR,		CHAR,
	"clear",	CLEAR,		0,
	"col",		COL,		0,
	"column",	COLUMN,		0,
	"columns",	COLUMN,		0,
	"complement",	COMPLEMENT,	0,
	"date",		DATE,		(int) rcDATE,
	"day",		SYS_CONST,	M_DAY,
	"dec",		FORMAT,		(int) fmtB_DEC,
	"default",	OUT,		0,
	"define",	DEFINE,		0,
	"do",		DO,		0,
	"else",		ELSE,		0,
	"end",		END,		0,
	"enda",		ENDA,		0,
	"endc",		ENDC,		0,
	"endi",		ENDI,		0,
	"endo",		ENDO,		0,
	"endr",		ENDR,		0,
	"endw",		ENDW,		0,
	"exp",		EXP,		0,
	"false",	BCONST,		(int) pushFALSE,
	"for",		FOR,		0,
	"from",		FROM_TO,	0,
	"graphic",	GRAPHIC,	0,
	"groupbegin",	GROUPBEGIN,	0,
	"groupend",	GROUPEND,	0,
	"hex",		FORMAT,		(int) fmtB_HEX,
	"hour",		SYS_CONST,	HOUR,
	"if",		IF,		0,
	"ignore",	IGNORE,		0,
	"in",		IN,		0,
	"integer",	INTEGER,	INT,
	"is_dst",	SYS_CONST,	IS_DST,
	"last",		DOMAIN,		(int) pushLAST,
	"length",	LENGTH,		0,
	"line",		LINE,		0,
	"linebegin",	EXCEPTION,	(int) onB_LINE,
	"lineend",	EXCEPTION,	(int) onE_LINE,
	"lines",	LINE,		0,
	"m_day",	SYS_CONST,	M_DAY,
	"minute",	SYS_CONST,	MINUTE,
	"mod",		ABOP,		(int) rcMOD,
	"month",	SYS_CONST,	MONTH,
	"new",		NEW,		0,
	"next",		DOMAIN,		(int) pushNEXT,
	"norm",		NORM,		0,
	"not",		LUOP,		(int) rcNOT,
	"null",		ICONST,		0,
	"oct",		FORMAT,		(int) fmtB_OCT,
	"on",		ON,		0,
	"one",		ONE,		0,
	"or",		LBOP,		(int) rcOR,
	"out",		OUT,		0,
	"page",		PAGE,		0,
	"pagebegin",	EXCEPTION,	(int) onB_PAGE,
	"pageend",	EXCEPTION,	(int) onE_PAGE,
	"pic",		PIC,		0,
	"point",	POINT,		0,
	"proc",		PROC,		0,
	"real",		REAL,		FLOAT,
	"repeat",	REPEAT,		0,
	"report",	REPORT,		0,
	"return",	RETURN,		0,
	"scale",	SCALE,		0,
	"second",	SYS_CONST,	SECOND,
	"set",		SET,		0,
	"space",	SPACE,		0,
	"switch",	SWITCH,		0,
	"terminal",	TERMINAL,	0,
	"then",		THEN,		0,
	"time",		DATE,		(int) rcTIME,
	"to",		FROM_TO,	1,
	"true",		BCONST,		(int) pushTRUE,
	"tuple",	TUPLE,		0,
	"value",	DOMAIN,		(int) pushVALUE,
	"var",		NVAR,		0,
	"w_day",	SYS_CONST,	W_DAY,
	"while",	WHILE,		0,
	"with",		WITH,		0,
	"word",		WORD,		0,
	"y_day",	SYS_CONST,	Y_DAY,
	"year",		SYS_CONST,	YEAR
/*	--------------------------------------		*/
};
int	Keyent =	sizeof Keyword / sizeof Keyword[0];

OPTAB	Optab[] =
{
/*	PUNCTUATION
**	--------------------------------------
**	term		token		opcode
**	--------------------------------------		*/
	";",		SEMICOL,	0,
	"(",		LPAREN,		0,
	")",		RPAREN,		0,
	"'",		SCONST,		0,
	"\"",		SCONST,		0,
	"\/\*",		BGNCMNT,	0,
	"\*\/",		ENDCMNT,	0,
	"#",		DOMAIN,		(int) pushVALUE,
	"$",		NVAR,		0,
	"^",		SWITCH,		0,
	"@",		LENGTH,		0,
	"\\",		COL,		0,
	"\\\\",		SPACE,		0,
	"\\n",		NEWLINE,	0,
	"\\f",		NEWPAGE,	0,
/*	--------------------------------------
**
**	--------------------------------------
**	term		token		opcode
**	--------------------------------------		*/
	"+",		AUOP,		(int) rcADD,
	"-",		AUOP,		(int) rcMIN,
	"!",		LUOP,		(int) rcNOT,
/*	--------------------------------------
**
**	BINARY ARITHMETIC / LOGICAL OPERATORS
**	--------------------------------------
**	term		token		opcode
**	--------------------------------------		*/
	"*",		ABOP,		(int) rcMUL,
	":",		ABOP,		(int) rcDIV,
	"/",		ABOP,		(int) rcDIV_R,
	"%",		ABOP,		(int) rcMOD,
	"&",		LBOP,		(int) rcAND,
	"&&",		LBOP,		(int) rcAND,
	"|",		LBOP,		(int) rcOR,
	"||",		LBOP,		(int) rcOR,
/*	--------------------------------------
**
**	BINARY ASSIGNMENT OPERATORS
**	--------------------------------------
**	term		token		opcode
**	--------------------------------------		*/
	"+=",		ABIS,		(int) rcADD,
	"-=",		ABIS,		(int) rcSUB,
	"*=",		ABIS,		(int) rcMUL,
	":=",		ABIS,		(int) rcDIV,
	"/=",		ABIS,		(int) rcDIV_R,
	"%=",		ABIS,		(int) rcMOD,
/*	--------------------------------------
**
**	BOUNDS OPERATORS
**	--------------------------------------
**	term		token		opcode
**	--------------------------------------		*/
	">",		BDOP,		(int) bdGT,
	">=",		BDOP,		(int) bdGE,
	"<",		BDOP,		(int) bdLT,
	"<=",		BDOP,		(int) bdLE,
/*	--------------------------------------
**
**	EQUALITY OPERATORS
**	--------------------------------------
**	term		token		opcode
**	--------------------------------------		*/
	"=",		IS,		(int) bdEQ,
	"==",		IS,		(int) bdEQ,
	"<>",		EOP,		(int) bdNE,
	"!=",		EOP,		(int) bdNE,
/*	--------------------------------------		*/
	0
};

/*
**	Tokens
**	A structur initialized here to contain the YACC
**	generated tokens for the indicated terminal symbols.
*/
struct special	Tokens =
{
	/* name     = */	NAME,
	/* sconst   = */	SCONST,
	/* bgncmnt  = */	BGNCMNT,
	/* endcmnt  = */	ENDCMNT,
	/* iconst   = */	ICONST,
	/* lconst   = */	LCONST,
	/* rconst   = */	RCONST
};
