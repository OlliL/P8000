# include	<stdio.h>
# include	"gen.h"
# include	"../h/bs.h"
# include	"constants.h"
# include	"globals.h"
# include	"cmap.c"

# ifdef P8000
ret_buf		AAjmp_buf;
# else
jmp_buf		AAjmp_buf;
# endif

int		Opcode;			/* op code to distinguis tokens	    */
int		Newline;		/* less than one token has been	    */
int		Lineout;		/* output line count		    */
int		In_quote;		/* set if inside an IIwrite("...    */
char		*Input_file_name;	/* input file name		    */
int 		C_code_flag;		/* set to indicate C-code in parser */
int 		Pre_proc_flag;		/* set to indicate a 		    */
					/* pre-processor line 		    */
int		Block_level;		/* > 0 local, 0 global		    */
int		Indir_level;		/* holds indirection level	    */
					/* of a reference to a C-var	    */
int		In_string;		/* set if output is in a string	    */
int		Fillmode;		/* set if output line being filled  */
int		Charcnt;		/* # chars written onto output line */
int		Rtdb;			/* set by "-d" command line arg,    */
					/* supplies EQL runtime support     */
					/* with info to report file name    */
					/* and line number in case of an    */
					/* error			    */
FILE		*In_file;		/* input file			    */
FILE		*Out_file;		/* output file			    */
# ifdef SETBUF
char		*Ibuf;			/* input buffer			    */
char		Out_buf[BUFSIZ];	/* buffer for Out_file		    */
# endif
struct cvar	*C_globals;		/* Global C-variable tree	    */
struct cvar	*C_locals;		/* Local C-variable tree	    */
struct cvar	*F_globals;		/* Global structure fields	    */
struct cvar	*F_locals;		/* Local structure fields	    */

# ifdef xDEBUG
char	Lex_debug;			/* debugging info for lexemes in    */
					/* yylex() [yylex.c]		    */
char	Chardebug;			/* print debugging info for routine */
					/* in getch.c			    */
# endif

static int	Arg_error;		/* set in argproc() [main.c] on	    */
					/* error in prcessing command line  */
					/* arguments.			    */

/*
**  MAIN.C -- Start up routines
**
**	Usage:
**		eql {-d | -v | -c | -y | -f | -f<integer> | <name>.q}
**
**	Files:
**		standard output -- for diagnostics
**		<name>.q -- read
**		<name>.c -- created and written
**		any file appearing in a "#include" with a name
**		<name>.q.h -- read
**		<name>.c.h -- created and written
**
**	Flags:
**		possible arguments are:
**		-d -- enables run-time errors to have the file name
**		      and line number where they occurred reported
**		      Defaults to off.
**		-f -- specify the number of characters to fill
**		      an output line on QL commands
**		      as being very high (to get C code on the
**		      right line invariably).
**		-f<integer> -- fill output lines to integer chars
**		      (0 is like -f alone)
**		      Defaults to FILLCNT.
**		-y -- have the parser print debugging info
**		      Defaults to off.
**		-v -- (verbose) have the lexical analizer
**		      print the kind of token it sees.
**		      (only available if xDEBUG is defined)
**		      Defaults to off.
**		-c -- have every character read or backed up
**		      echoed (only if xDEBUG is defined)
**		      Defaults to off.
**		-r -- reset all previous flags to default
**		<name>.q -- name of a file to be EQL'ed
**
**	Defines:
**		main()
**		argproc()
**		eql()
*/

char	*AApath;

/*
**  MAIN --  invokes the pre-compiler on all the argument files
**
**	Parameters:
**		argc
**		argv
**
*/
main(argc, argv)
int	argc;
char	**argv;
{
	extern char	**argproc();

	argv[argc] = 0;

	for (argv++; *argv; )
	{
		argv = argproc(argv);
		if (!Arg_error)
		{
			printf("%s :\n", Input_file_name);
			eql(Input_file_name);
			sync();
		}
	}
	fflush(stdout);
	exit(0);
}


/*
**  ARGPROC -- process arguments on the command line
**	Arguments have effect on all the files following them
**	until a "-r" or an argument cancelling the first
**
**	Also performs global initializations.
**
**	Parameters:
**		argv -- a 0 terminated string vector with the
**		        command lines components.
**
**	Returns:
**		a new argv with all the leading arguments taken out
**
**	Side Effects:
**		sets certain variables for certain flags
**		  -d -- Rtdb
**		  -c -- Chardebug
**		  -v -- Lex_debug
**		  -y -- yydebug
**		  -f -- Fillcnt
**		  -r -- resets all variables to default values
**		Sets Arg_error on an argument error that should abort
**		the pre-processing of the file read.
*/
char	**argproc(argv)
register char	**argv;
{
	/* initializations for a new file */
	C_code_flag = Pre_proc_flag = 0;
	yyline = Newline = Lineout = 1;
	Block_level = Indir_level = In_string = Fillmode = 0;
	Charcnt = In_quote = 0;
	Arg_error = 0;

	/* free C variable trees, and symbol space */
	freecvar(&C_locals);
	freecvar(&F_locals);
	freecvar(&C_globals);
	freecvar(&F_globals);

	symspfree();

	for ( ; *argv && **argv == '-'; argv++)
	{
		switch (*++*argv)
		{
#		  ifdef xDEBUG
		  case 'V':
		  case 'v':
			Lex_debug = 'v';
			break;

		  case 'C':
		  case 'c':
			Chardebug = 'c';
			break;
#		  endif

		  case 'Y':
		  case 'y':
			yydebug = 1;
			break;

		  case 'D':
		  case 'd':
			Rtdb = 1;
			break;

		  case 'F':
		  case 'f':		/* line fill */
			if (AA_atoi(++*argv, &Fillcnt))
				printf("bad fill count \"%s\"\n",
				*argv);
			if (!Fillcnt)
				/* make SURE that C_CODE is put
				 * on line that it was typed in on
				 */
				Fillcnt = 30000;
			break;

		  case 'R':
		  case 'r':		/* reset all flags to
					 * their default values.
					 */
			yydebug = Rtdb = 0;
			Fillcnt = FILLCNT;
#			ifdef xDEBUG
			Lex_debug = Chardebug = 0;
#			endif
			break;

		  default :
			printf("invalid option : '-%c'\n", **argv);
		}
	}
	if (*argv)
		Input_file_name = *argv++;
	else
	{
		printf("missing file name after arguments\n");
		Arg_error++;
	}
	return (argv);
}


/*
**  EQL -- invokes the precompiler for a non-included file
**
**	Parameters:
**		filename -- the name of the file to pre-compile
**
**	Side Effects:
**		performs the preprocessing on <filename>
*/
eql(filename)
register char	*filename;
{
	register int	l;
# ifdef SETBUF
	char		in_buf[BUFSIZ];
# endif
	char		o_file[100];

	l = AAlength(filename);
	if (l > sizeof o_file - 1)
	{
		printf("filename \"%s\" too long\n",
		filename);
		return;
	}
	if (!AAsequal(".q", &filename[l - 2]))
	{
		printf("invalid input file name \"%s\"\n",
		filename);
		return;
	}
	AAbmove(".c", AAbmove(filename, o_file, l - 2), 3);
	Input_file_name = filename;
	In_file = Out_file = (FILE *) 0;
# ifdef SETBUF
	Ibuf = in_buf;
# endif


	if (!(In_file = fopen(filename, "r")))
		printf("can't read file \"%s\"\n", filename);
	else if (!(Out_file = fopen(o_file, "w")))
		printf("can't write file \"%s\"\n", o_file);
	else
	{
# ifdef SETBUF
		setbuf(In_file, in_buf);
		setbuf(Out_file, Out_buf);
# endif
		setexit();
		yyparse();
	}

	/* if a reset(3) is done while processing
	 * an included file, then this closes all
	 * files skipped.
	 */
	while (restoref())
		continue;

	if (Out_file)
		fclose(Out_file);
	if (In_file)
		fclose(In_file);
	In_file = Out_file = (FILE *) 0;
	Input_file_name = 0;
	if (Block_level)
		printf("unclosed block\n");
}


AAunlall()
{
}
