/*
** ---  RC_MAIN.C ------- MAIN  -----------------------------------------------
**
**	Version: @(#)rc_main.c		4.0	02/05/89
**
*/

# include	<stdio.h>
# include	"rc.h"

# define	FLAG		'-'

# define	COMPILE		'c'
# define	INPUT		'i'
# undef LCASE
# define	LCASE		'l'
# define	OUTPUT		'o'
# define	SILENT		's'
# define	VERBOSE		'v'

# ifdef RC_TRACE
# define	YYPARSE		'Y'
# define	YYLEX		'L'
# define	YYINPUT		'I'
# define	YYOUTPUT	'O'
# define	YYR_CODE	'R'
# endif

/*  SETEXIT | RESET  */
# ifdef P8000
ret_buf		AAjmp_buf;
# else
jmp_buf		AAjmp_buf;
# endif

extern FILE	*Input;
static	int	catch;

rc_catch()
{
	signal(2, rc_catch);
	signal(3, rc_catch);
	catch++;
	if (Sflag)
	{
		printf("\n----------------\n");
		printf("COMPILING aborted\n");
	}
	if (Iflag)
		Iflag = FALSE;
	else
		fclose(Input);
	reset();
}


main(argc, argv)
int	argc;
char	*argv[];
{
	register char		*p;
	register int		i;
	register int		files;
# ifdef SETBUF
	char			out_buf[BUFSIZ];
# endif

# ifdef SETBUF
	setbuf(stdout, out_buf);
# endif
	PQR_system = "compiler";
	signal(2, rc_catch);
	signal(3, rc_catch);
	files = 0;
	setexit();

	while (--argc > 0)
	{
		++argv;
		if (**argv == FLAG)
		{
			p = *argv;
			p++;
			while (*p)
			{
				switch (*p)
				{
# ifdef RC_TRACE
				  case YYPARSE:
					setbit(01, rc_debug);
					break;

				  case YYLEX:
					setbit(02, rc_debug);
					break;

				  case YYINPUT:
					setbit(04, rc_debug);
					break;

				  case YYOUTPUT:
					setbit(010, rc_debug);
					break;

				  case YYR_CODE:
					setbit(020, rc_debug);
					break;
# endif

				  case COMPILE:
				  case 'C':
					Compile = FALSE;
					break;

				  case INPUT:
					Iflag = TRUE;
					Input = stdin;
					goto compile;

				  case LCASE:
					Lcase = FALSE;
					break;

				  case OUTPUT:
					Oflag = TRUE;
					break;

				  case SILENT:
				  case 'S':
					Sflag = FALSE;
					break;

				  case VERBOSE:
				  case 'V':
					Sflag = TRUE;
					break;

				  default:
					printf("? Option: -%c\n", *p);
				}
				p++;
			}
			continue;
		}
		if (!(Input = fopen(*argv, "r")))
		{
			printf("Cannot open *** %s ***\n", *argv);
			continue;
		}

compile:
		Doms = 0;
		Line = 1;
		S_error = 0;

		R_cols = D_COLS;
		R_lines = D_LINES;
		R_all_lines = D_ALL_LINES;
		R_for_all = D_FOR_ALL;
		R_first_page = D_FIRST_PAGE;
		R_last_page = D_LAST_PAGE;
		R_from_tuple = D_FROM_TUPLE;
		R_to_tuple = D_TO_TUPLE;
		R_big = FALSE;
		R_rc = &R_code[1];

		Macro_no = 0;
		for (i = 0; i < MAX_PROCS; i++)
			Proc[i] = (char *) 0;
		if (Sflag)
		{
			if (!files)
				PQR();
			printf("\nCompiling REPORT ");
		}

		printf("%s:\n", *argv);

		if (Sflag)
		{
			printf("----------------\n");
			printf("%4d: ", Line);
		}
		files++;

		rc_parse();
		rc_freesym();
		if (Iflag)
			Iflag = FALSE;
		else
			fclose(Input);

		if (Sflag)
		{
			printf("\n----------------\n");
			printf("ERRORS\treported\t%d\n", S_error);
			printf("R_CODE\ton file\t\t\"%s\"\n", S_name);
			if (Doms && S_desc)
				printf("DESC\ton file\t\t\"%s\"\n", S_desc);
		}
		sync();
		Oflag = FALSE;
	}
	if (Sflag && files > 1)
	{
		printf("\nTOTAL\tfiles\t\t%d\n", files);
		printf("\tinterrupts\t%d\n", catch);
	}
	fflush(stdout);
	exit (S_error);
}
