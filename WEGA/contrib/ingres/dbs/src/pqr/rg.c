/*
** ---  RG.C ------------ BASIC VARIABLES  ------------------------------------
**
**	Version: @(#)rg.c		4.0	02/05/89
**
*/


# include	"rg.h"


/* RG_TRACE facilities */
# ifdef	RG_TRACE
int		rg_debug;	/* RG_TRACE flag = 0		*/
# endif

/* TUPLE file	handling */
struct desc_t	*Desc;		/* domain desriptions		*/
char		*Last;		/* pointer to last tuple	*/
char		*Value;		/* pointer to last tuple	*/
char		*Next;		/* pointer to last tuple	*/
BOOL		Eof;			/* = FALSE		*/

/* the variables */
ANYTYPE		*Var;			/* 0.0			*/
BOOL		Bvar[MAX_BVAR];		/* FALSE		*/

/* SYSTEM variables */
char		*SYS_col = Line;
int		SYS_line;		/* = 0			*/
int		SYS_page;		/* = 0			*/
int		SYS_tuple;		/* = 0			*/

/* EXCEPTIONS	*/
char		*ONb_line;		/* = (char *) 0		*/
char		*ONe_line;		/* = (char *) 0		*/
char		*ONb_page;		/* = (char *) 0		*/
char		*ONe_page;		/* = (char *) 0		*/
char		*ONbatch;		/* = (char *) 0		*/

/* SWITCHES for running exceptions */
BOOL		RUN;			/* = FALSE		*/
BOOL		RUNoutput;		/* = FALSE		*/
BOOL		RUNline;		/* = FALSE		*/
BOOL		RUNpage;		/* = FALSE		*/

/* some GLOBALS */
short		R_word;			/* = 0			*/
short		R_val;			/* = 0			*/
BOOL		R_expr;			/* = FALSE		*/
char		Line[2 * MAX_LINE];
char		*Col_begin = Line;
char		*Col_end;		/* in rg_init()		*/
int		Line_begin = 1;
int		Line_end;		/* in rg_init()		*/
