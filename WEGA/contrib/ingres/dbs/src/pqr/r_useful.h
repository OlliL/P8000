/*
** ---  R_USEFUL.H ------ USEFUL STUFF  ---------------------------------------
**
**	Version: @(#)r_useful.h		4.0	02/05/89
**
*/


typedef		char		BOOL;	/* the boolean type */

# ifndef	TRUE
# define	FALSE		0	/* zero, false, no,  nop, etc. */
# define	TRUE		1	/* one,  true,  yes, ok,  etc. */
# endif

# ifndef	bitset
# define	bitset(b, s)	((b) & (s))
# define	setbit(b, s)	((s) |= (b))
# define	clrbit(b, s)	((s) &= ~(b))
# endif

# define	MIN_I1		-128
# define	MAX_I1		127
# define	MIN_I2		-32768
# define	MAX_I2		32767

# define	ASCII_MASK	0177
# define	BYTE_MASK	0377
# define	BYTE_LOG2	8

/* TRACE facilities */

# define	RC_TRACE
# define	RG_TRACE
/* # define	MEM_TRACE */

# ifdef	RC_TRACE
# define	TR_PARSE	(rc_debug & 0000001)
# define	TR_LEX		(rc_debug & 0000002)
# define	TR_INPUT	(rc_debug & 0000004)
# define	TR_OUTPUT	(rc_debug & 0000010)
# define	TR_R_CODE	(rc_debug & 0000020)
extern int	rc_debug;
# endif

# ifdef	RG_TRACE
# define	TR_GETUP	(rg_debug & 0000001)
# define	TR_INTERP	(rg_debug & 0000002)
# define	TR_EXPR		(rg_debug & 0000004)
# define	TR_PUSH		(rg_debug & 0000010)
# define	TR_LINE		(rg_debug & 0000020)
# define	TR_PAGE		(rg_debug & 0000040)
# define	TR_BATCH	(rg_debug & 0000100)
# define	TR_EXCEPTION	(rg_debug & 0000200)
# define	TR_FILE		(rg_debug & 0000400)
# define	TR_REPORT	(rg_debug & 0001000)
# define	TR_FORMAT	(rg_debug & 0002000)
# define	TR_MEM		(rg_debug & 0004000)
extern int	rg_debug;
# endif
