/*
** ---  RG_FILE.C ------- FILE HANDLING  --------------------------------------
**
**		rg_open	| rg_close | rg_exit
**
**	Version: @(#)rg_file.c		4.0	02/05/89
**
*/

# include	<stdio.h>
# include	"rg_file.h"
# ifdef MSDOS
# include	<fcntl.h>
# endif

# ifdef MSDOS
# define	READ		O_BINARY
# else
# define	READ		0
# endif
# define	APPEND		1

FILETYPE	Rg_file[] =
{
/*	---------------------------------------------------
**	file	    open       fd	 name	   type
**	--------------------------------------------------- */
	F_TUPLE,    FALSE,     -1,    "tuple",	   READ,
	F_R_CODE,   FALSE,     -1,   "r_code",	   READ,
	F_DESC,	    FALSE,     -1,     "desc",	   READ,
	F_REPORT,    TRUE,	1,   "report",	 APPEND,
/*	--------------------------------------------------- */
	0
};


rg_open()
{
	register FILETYPE	*r;
	register char		*f;
	register int		i;
	extern int		errno;

	errno =	0;
	for (r = Rg_file; r->rf_file; r++)
	{
		if (!r->rf_open)
		{
			f = r->rf_name;
			if (r->rf_type == APPEND)
				i = creat(f, FILEMODE);
			else
				i = (*f == '-')? 0: open(f, READ);
			if (i < 0)
			{
				rg_error("`%s' OPEN ERROR", f);
				rg_exit(EXIT_OPEN);
			}
			r->rf_fd = i;
			r->rf_open = TRUE;
# ifdef RG_TRACE
			if (TR_FILE)
				printf("OPEN\tfile `%s' on fd %d\n", f, i);
# endif
		}
	}
}


rg_all_close()
{
	register FILETYPE	*r;

	for (r = Rg_file; r->rf_file; r++)
		rg_close(r);
}


rg_close(r)
register FILETYPE	*r;
{
	extern int	errno;

	if (r->rf_open)
	{
		errno =	0;
		if (r->rf_fd && close(r->rf_fd) < 0)
			rg_error("`%s' CLOSE ERROR", r->rf_name);
		r->rf_open = FALSE;
# ifdef RG_TRACE
		if (TR_FILE)
			printf("CLOSE\tfile `%s' on fd %d\n",
				r->rf_name, r->rf_fd);
# endif
	}
}


rg_exit(val)
register int	val;
{
	extern int	errno;


	if (!val)
		rg_flush();
	fflush(stdout);
	rg_all_close();
	errno =	0;
	if (val)
		rg_error("DYING	%d", val);
	fflush(stdout);
	sync();
	exit(val);
}
