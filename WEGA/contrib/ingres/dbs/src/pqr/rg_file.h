/*
** ---  RG_FILE.H ------- FILE DESCRIPTIONS  ----------------------------------
**
**	Version: @(#)rg_file.h		4.0	02/05/89
**
*/


# include	"rg.h"

# undef FILEMODE
# define	FILEMODE	0644

struct rg_file
{
	char		rf_file;	/* logical file	description	  */
					/* see above			  */
	BOOL		rf_open;	/* file	really open ?		  */
	int		rf_fd;		/* the file descriptor,if open	  */
	char		*rf_name;	/* the BS file name		  */
	int		rf_type;	/* type	for open, read/write	  */
};
typedef struct rg_file	FILETYPE;

extern FILETYPE		Rg_file[];


/* logical file descriptions */
# define	F_TUPLE		1	/* the tupel file        */
# define	F_R_CODE	2	/* the R-CODE file       */
# define	F_DESC		3	/* the tupel description */
# define	F_REPORT	4	/* the report file       */

