/*
** ---  RG.H ------------ BASIC DECLARATIONS  ---------------------------------
**
**	Version: @(#)rg.h		4.0	02/05/89
**
*/


# include	"r_code.h"


# define	MAX_LINE		512


/* ERROR and EXIT conditions */

# define	FATAL		0	/* fatal error message		*/
# define	WARNING		1

# define	EXIT_END	0	/* normal end, no error		*/
# define	EXIT_OPEN	1
# define	EXIT_READ	2
# define	EXIT_WRITE	3
# define	EXIT_DESC	4
# define	EXIT_ARG	5
# define	EXIT_R_CODE	6
# define	EXIT_VAR	7
# define	EXIT_MEM	8
# define	EXIT_FLP	9


/* TUPLE file	handling */

extern struct desc_t	*Desc;		/* domain desriptions		*/
extern char		*Last;		/* the tupel before		*/
extern char		*Value;		/* the actual tuple		*/
extern char		*Next;		/* the readahead tuple		*/
extern BOOL		Eof;		/* EOF flag			*/


/* the definition of variables */

union	anytype
{
	BOOL		booltype;
	char		i1type;
	short		i2type;
	long		i4type;
# ifndef NO_F4
	float		f4type;
# endif
	double		f8type;
	char		*cptype;
	char		c0type[sizeof (double)];
};
typedef union	anytype	ANYTYPE;

extern ANYTYPE		*Var;		/* the variables		*/
extern BOOL		Bvar[];		/* the switches			*/


# define	MAX_REPEAT		32

struct r_repeat
{
	char	*rr_code;	/* the pointer to R-CODE		*/
	int		rr_count;	/* the actual repeat count	*/
};
typedef struct r_repeat	REPEATYPE;


/* SYSTEM variables */

extern char		*SYS_col;	/* COLUMN			*/
extern int		SYS_line;	/* LINE				*/
extern int		SYS_page;	/* PAGE				*/
extern int		SYS_tuple;	/* TUPLE			*/


/* EXCEPTIONS						*/
/*	R-CODE addresses for		ON ......... DO	*/

extern char		*ONb_line;	/* LINEBEGIN			*/
extern char		*ONe_line;	/*  LINEEND			*/
extern char		*ONb_page;	/* PAGEBEGIN			*/
extern char		*ONe_page;	/*  PAGEEND			*/
extern char		*ONbatch;	/* BATCHLINE			*/


/* SWITCHES for running exceptions */

extern BOOL		RUN;		/* flag	to cancel interpreter	*/
extern BOOL		RUNoutput;	/* flag	to cancel output	*/
extern BOOL		RUNline;
extern BOOL		RUNpage;


/* some GLOBALS */

extern short		R_word;
extern short		R_val;		/* first argument of		*/
					/* most	R-CODES			*/
extern BOOL		R_expr;		/* result of expression		*/
					/* interpreter			*/
extern char		Line[];		/* line	buffer			*/
extern char		*Col_begin;	/* the actual			*/
extern char		*Col_end;	/*    line range		*/
extern int		Line_begin;	/* the actual			*/
extern int		Line_end;	/*    page range		*/
