/*
** ---  RG_INT.H -------- INTERPRETER DECLARATIONS  ---------------------------
**
**	Version: @(#)rg_int.h		4.0	02/05/89
**
*/

# include	"rg.h"

# define	MAX_STACK		50

struct symbol
{
	char		sy_frmt;	/* format */
	char		sy_frml;	/* length */
	union anytype	sy_val;	/* value  */
};
typedef struct symbol	SYMBOL;

extern struct symbol	Stack[];
extern struct symbol	*Print;
extern char		*rc_word();

/* inline procedure to read R-CODE */
# define	RC_BYTE		((short) ctou(*rc++))
# define	RC_WORD		rc = rc_word(rc)

# define	RANGE(l, x, h)	((x) >= ((int) (l)) && (x) <= ((int) (h)))
