/*
** ---  RG_MAIN.C ------- REPORT INTERPRETER  ---------------------------------
**
**	Version: @(#)rg_main.c		4.0	02/05/89
**
*/

# include	<stdio.h>
# include	"rg_file.h"

# define	FLAG			'-'

# ifdef RG_TRACE
# define	DEBUG			'X'
# endif


# define	ALL_LINES		'a'
# define	BIG			'b'
# define	COLUMNS			'c'
# define	LINES			'l'
# define	FROM_PAGE		'p'
# define	TO_PAGE			'P'
# define	FROM_TUPLE		't'
# define	TO_TUPLE		'T'

# undef VAR
# define	VAR			'v'
# define	SWITCH			's'

# define	DESC_FILE		'd'
# define	TUPLE_FILE		'f'
# define	R_CODE_FILE		'i'
# define	REPORT_FILE		'o'

# ifdef STACK
static ANYTYPE	mem_var[MAX_VAR];
# endif

/*ARGSUSED*/
main(argc, argv)
int	argc;
char	**argv;
{
	register char		**pa;	/* argument pointer	*/
	register char		*ps;	/* string pointer	*/
	register FILETYPE	*file;
	int			fatal;	/* argument error flag	*/
	register char		*p;	/* string pointer	*/
	register int		assign;
	int			i;
# ifndef STACK
	ANYTYPE			mem_var[MAX_VAR];
# endif
	extern int		rg_flptexcep();
# ifndef RG_TRACE
# ifdef SETBUF
	char			buf[BUFSIZ];

	setbuf(stdout, buf);
# endif
# else
	rg_code();
# endif

	PQR_system = "generator";
	PQR();

	/* catch floating point signals */
	signal(8, rg_flptexcep);

	Var = mem_var;
	fatal = 1;
	pa = ++argv;		/* skip first argument */

	/* initialize i/o-files, skip but check all other arguments */
	while (ps = *pa++)
	{
		if (*ps++ != FLAG)
			break;

		switch (*ps++)
		{
# ifdef RG_TRACE
		  case DEBUG:
			ck_arg(ps, &rg_debug, &fatal);
			continue;
# endif

		  case BIG:
		  case 'B':
		  case VAR:
		  case 'V':
		  case SWITCH:
		  case 'S':
		  case ALL_LINES:
		  case 'A':
		  case COLUMNS:
		  case 'C':
		  case LINES:
		  case 'L':
		  case FROM_PAGE:
		  case TO_PAGE:
		  case FROM_TUPLE:
		  case TO_TUPLE:
			continue;

		  case DESC_FILE:
		  case 'D':
			file = &Rg_file[F_DESC - 1];
			goto file_name;

		  case TUPLE_FILE:
		  case 'F':
			file = &Rg_file[F_TUPLE - 1];
			goto file_name;

		  case R_CODE_FILE:
		  case 'I':
			file = &Rg_file[F_R_CODE - 1];
			goto file_name;

		  case REPORT_FILE:
		  case 'O':
			file = &Rg_file[F_REPORT - 1];
file_name:
			file->rf_name = ps;
			file->rf_open = FALSE;
			continue;

		  default:
			rg_error("%d: BAD flag '%c'",
				fatal++, ps[-1]);
			continue;
		}
	}

	if (--fatal)
		rg_exit(EXIT_ARG);

	/* open all needed files */
	rg_open();

	/* NOW read the R-CODE.				  */
	file = &Rg_file[F_R_CODE - 1];
	read(file->rf_fd, R_code, sizeof R_res - 1);
	read(file->rf_fd, (char *) &R_cols, sizeof (int));
	read(file->rf_fd, (char *) &R_lines, sizeof (int));
	read(file->rf_fd, (char *) &R_all_lines, sizeof (int));
	read(file->rf_fd, (char *) &R_for_all, sizeof (int));
	read(file->rf_fd, (char *) &R_first_page, sizeof (int));
	read(file->rf_fd, (char *) &R_last_page, sizeof (int));
	read(file->rf_fd, (char *) &R_from_tuple, sizeof (int));
	read(file->rf_fd, (char *) &R_to_tuple, sizeof (int));
	i = read(file->rf_fd, R_code, MAX_I2);

	/* assume successful read */
	rg_close(file);

	/* double check for length of R-CODE */
	if (i < MIN_R_CODE)
	{	/* very nice R-CODE */
		rg_error("R-CODE '%d' Bytes (too short)", i);
		goto abort;
	}
	if ( i > MAX_R_CODE)
	{	/* very nice R-CODE */
		rg_error("R-CODE '%d' Bytes (too long)", i);
abort:
		rg_exit(EXIT_R_CODE);
	}

	/* let's make the rest of initialization */
	fatal = 1;
	pa = argv;

	/* skip i/o-file arguments, check's are done */
	while (ps = *pa++)
	{
		ps++;	/* skip flag identifier */

		switch (*ps++)
		{
# ifdef RG_TRACE
		  case DEBUG:
# endif
		  case DESC_FILE:
		  case 'D':
		  case TUPLE_FILE:
		  case 'F':
		  case R_CODE_FILE:
		  case 'I':
		  case REPORT_FILE:
		  case 'O':
			continue;

		  case ALL_LINES:
		  case 'A':
			ck_arg(ps, &R_all_lines, &fatal);
			continue;

		  case BIG:
		  case 'B':
			R_big = TRUE;
			continue;

		  case COLUMNS:
		  case 'C':
			ck_arg(ps, &R_cols, &fatal);
			continue;

		  case LINES:
		  case 'L':
			ck_arg(ps, &R_lines, &fatal);
			continue;

		  case FROM_PAGE:
			ck_arg(ps, &R_first_page, &fatal);
			continue;

		  case TO_PAGE:
			ck_arg(ps, &R_last_page, &fatal);
			continue;

		  case FROM_TUPLE:
			ck_arg(ps, &R_from_tuple, &fatal);
			continue;

		  case TO_TUPLE:
			ck_arg(ps, &R_to_tuple, &fatal);
			continue;

		  case SWITCH:
		  case 'S':
			ck_arg(ps, &i, &fatal);
			if (i < 0 || i >= MAX_BVAR)
				rg_error("%d: ILLEGAL SWITCH '%d'", fatal++, i);
			else
				Bvar[i] = TRUE;
			continue;

		  case VAR:
		  case 'V':
			p = ps;
			while (*ps && *ps != '=')
				ps++;
			assign = *ps;
			*ps++ = 0;
			ck_arg(p, &i, &fatal);
			if (i < 0 || i >= MAX_VAR)
				rg_error("%d: ILLEGAL VAR '%d'\n", fatal++, i);
			else if (!assign)
				rg_error("%d: ASSIGMENT for VAR %d MISSING",
					fatal++, i);
			else if (AA_atof(ps, &Var[i].f8type))
				rg_error("%d: VALUE '%s' for VAR %d not numeric",
					fatal++, ps, i);
			continue;
		}
	}

	if (--fatal)
		rg_exit(EXIT_ARG);

	/* read tuple description, close description file */
	/* and read the first tuple			  */
	rg_desc();

	/* SETUP R-CODE HEADER */
	rg_init();

	/* REPORT DRIVER */
# ifdef RG_TRACE
	if (TR_INTERP)
		printf("----------   HEADER  ----------\n");
# endif
	rg_interp(/*HEADER*/);
	while (!Eof)
	{
		rg_getup();	/* read ahead tuple */
# ifdef RG_TRACE
	if (TR_INTERP)
		printf("----------   REPORT  ----------\n");
# endif
		rg_interp(/*REPORT*/);
	}
	R_rc = &R_code[R_for_all];
# ifdef RG_TRACE
	if (TR_INTERP)
		printf("----------   FOOTER  ----------\n");
# endif
	rg_interp(/*FOOTER*/);

	rg_exit(EXIT_END);
}



/*
**	Give a user error for a floating point exceptions
*/
rg_flptexcep()
{
	rg_error("Floating point exception");
	rg_exit(EXIT_FLP);
}
