/*
** ---  R_CODE.C -------- BASIC VARIABLES  ------------------------------------
**
**		INTERFACE between REPORT
**			compiler and
**			interpreter (generator)
**
**	Version: @(#)r_code.c		4.0	02/05/89
**
*/


# include	"r_code.h"


/* DOMAIN handling */
int		Doms = -1;
int		Tup_len;		/* = 0	       */


/* R-CODE BUFFER */

	/* header */
int	R_cols = D_COLS;
int	R_lines = D_LINES;
int	R_all_lines = D_ALL_LINES;
int	R_for_all = D_FOR_ALL;
int	R_first_page = D_FIRST_PAGE;
int	R_last_page = D_LAST_PAGE;
int	R_from_tuple = D_FROM_TUPLE;
int	R_to_tuple = D_TO_TUPLE;

	/* r-code */
char	R_code[MAX_R_CODE] = {FALSE};
char	*R_rc = 0;
