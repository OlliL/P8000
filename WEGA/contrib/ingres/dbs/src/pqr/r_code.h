/*
** ---  R_CODE.H -------- BASIC DECLARATIONS  ---------------------------------
**
**		INTERFACE between REPORT
**			compiler and
**			interpreter (generator)
**
**	Version: @(#)r_code.h		4.0	02/05/89
**
*/


# include	"r_useful.h"
# include	"r_dbs.h"


/* conversion stuff */

# define	BIT			2
# define	OCT			8
# define	DEC			10
# define	HEX			16


/* TIME / DATE stuff */

# define	SECOND			0
# define	MINUTE			1
# define	HOUR			2
# define	M_DAY			3	/* month day */
# define	MONTH			4
# define	YEAR			5
# define	W_DAY			6	/* week day */
# define	Y_DAY			7	/* year day */
# define	IS_DST			8	/* day light saving flag */


/* variables */

# define	MAX_VAR			500	/* user var's */
# define	MAX_BVAR		200
# define	VAR_VAR_MASK		0140000
# define	VAR_MASK		0100000
# define	VAR_VALUE		0037777


/* DOMAIN handling */

struct desc_t
{
	char		domfrmt;	/* format type	 */
	char		domfrml;	/* format length */
	short		domoff;		/* domain offset */
};
typedef struct desc_t	DESC_T;

extern int		Doms;		/* # of	actual domains */
extern int		Tup_len;	/* actual tupel	length */


/* R-CODE ARGUMENT defaults */

# define	A_BATCH			0
# define	A_BEFORE		0
# define	A_BIT			0
# define	A_BVAR			0
# define	A_CHAR			0
# define	A_COL			0
# define	A_DOM			0
# define	A_FROM			0
# define	A_GRAPHIC		50
# define	A_HEX			0
# define	A_INTEGER		0
# define	A_NEWLINE		1
# define	H_NEWLINE		512	/* high */
# define	L_NORM			-99	/* low */
# define	A_NORM			100
# define	H_NORM			99	/* high */
# define	A_OCT			0
# define	A_POINT			6
# define	A_REAL			0
# define	A_REPEAT		1
# define	A_SCALE			(double) 1
# define	A_SPACE			1
# define	A_SWITCH		0
# define	A_TO			MAX_FIELD
# define	A_VAR			0
# define	B_IGNORE		01
# define	B_NORM		02


/* R-CODE HEADER defaults */

# define	D_COLS			132
# define	D_LINES			66
# define	D_ALL_LINES		D_LINES
# define	D_FOR_ALL		0
# define	D_FIRST_PAGE		0
# define	D_LAST_PAGE		MAX_I2
# define	D_FROM_TUPLE		0
# define	D_TO_TUPLE		MAX_I2


/* R-CODE BUFFER */

# define	MIN_R_CODE		8
# define	MAX_R_CODE		8192

	/* header */
# define		R_res		"*  reserved  for  further  extensions  *"
extern int		R_cols;
extern int		R_lines;
extern int		R_all_lines;
extern int		R_for_all;
extern int		R_first_page;
extern int		R_last_page;
extern int		R_from_tuple;
extern int		R_to_tuple;
# define		R_big		*R_code

	/* r-code */
extern char		R_code[];
extern char		*R_rc;		/* r-code pointer */


/* R-CODE-VALUES */
enum R_CODE_VAL
{

/* BOUND OPERATORS */
	rcEXPR,			/* 0			*/
	bdLT,			/* <			*/
	bdEQ,			/* ==			*/
	bdLE,			/* <=			*/
	bdGT,			/* >			*/
	bdNE,			/* !=			*/
	bdGE,			/* >=			*/

/* PUSH VALUES */
/**/		L_PUSH,		/* LOW push code	*/
	pushFALSE,		/* COLUMN		*/
	pushTRUE,
	pushNULL,
	push1INT,
	push2INT,
	push4INT,
	pushREAL,
	pushSTRING,
	pushCONST,		/* TIME/DATE const	*/
	pushCOL,
	pushLINE,
	pushALLINES,
	pushPAGE,
	pushTUPLE,
	pushLAST,
	pushVALUE,
	pushNEXT,
	pushVAR,
	pushSWITCH,
/**/		H_PUSH,		/* HIGH push code	*/

/* LOGICAL OPERATORS */
	rcNOT,
	rcOR,
	rcAND,
	rcEND_SET,		/* end of expression	*/

/* ARITHMETIC OPERATORS	*/
	rcMIN,			/* - (unary)		*/
	rcADD,			/* +			*/
	rcSUB,			/* - (binary)		*/
	rcMUL,			/* *			*/
	rcDIV,			/* /			*/
	rcDIV_R,		/* :			*/
	rcMOD,			/* %			*/


/* ONE BYTE CODE */
	rcRC_END,
	rcRETURN,
	rcRTTS,
	rcEOTUP,
	rcALL,
	rcONE,
	rcDATE,
	rcTIME,
	prCOL,
	prLINE,
	prALLINES,
	prNEWPAGE,
	prPAGE,
	prTUPLE,
/* THE ANACHRONISM CODE					*/
/*	prVAR has a one byte argument, but the var	*/
/*	value is not expected				*/
	prVAR,

/* TWO Bytes CODES					*/
/*	the one byte argument is an integer value, or	*/
/*	a variable,  which contains the integer value	*/
/**/		L_2_BYTES,	/* LOW code		*/
	prCONST,
	prSPACE,
	prNEWLINE,
	prNEWCOL,
	prLAST,
	prVALUE,
	prNEXT,
	fmtB_BIT,
	fmtW_BIT,
	fmtB_OCT,
	fmtW_OCT,
	fmtB_DEC,
	fmtW_DEC,
	fmtB_HEX,
	fmtW_HEX,
/* THE ANACHRONISM CODE					*/
/*	codes have more than two bytes, let		*/
/*	them read the rest self				*/
	prSTRING,
	prINPUT,
	fmtINT,
	fmtREAL,
	fmtCHAR,
	fmtGRAPH,

/* THREE BYTES CODES					*/
/*	the two bytes argument is a relative address	*/
/**/		L_3_BYTES,	/* LOW code		*/
/* THE ANACHRONISM CODE					*/
/*	codes expects a one and a two byte argument,	*/
/*	give him both					*/
	rcCASE,
	rcREPEAT,
/**/		H_2_BYTES,	/* HIGH code		*/
/*	exception codes					*/
	onBATCH,
	onB_LINE,
	onE_LINE,
	onB_PAGE,
	onE_PAGE,
/*	jump / call					*/
	rcJUMP,
	rcCALL,
/*	expression / conditional			*/
	setVAR,
	setSWITCH,
	rcIF,
	rcTHEN,
/**/		H_3_BYTES	/* HIGH code		*/
};
