/*  GLOBALS.H -- EQL declarations					    */
/*									    */
/*	Defines:							    */
/*		Structures and variables				    */
/*		used in EQL.						    */

/*	Structure declarations	    */

	/* parser keyword and operator table node */
struct optab
{
	char	*op_term;	/* pointer to terminal			    */
	int  	op_token;	/* lexical token			    */
	int  	op_code;	/* code to distinguish different op_terms   */
				/* with the same .op_token		    */
};


	/* C variable tree node */
struct cvar
{
	char		*c_id;		/* identifier			    */
	char		c_type;		/* type				    */
	char		c_indir;	/* indirection level		    */
					/* (- 1 for strings)		    */
	struct cvar	*c_left;	/* < sub-tree			    */
	struct cvar	*c_right;	/* > sub-tree			    */
};

	/* Display structures (linked list of strings) */
struct disp_node
{
	char			*d_elm;		/* display element	    */
	struct disp_node	*d_next;	/* next node		    */
	int			d_line;		/* for Symsp nodes, line    */
						/* where lexeme was seen    */
};

struct display
{
	struct disp_node	*disp_first;	/* first node in display    */
	struct disp_node	*disp_last;	/* last node in display	    */
};

	/* Retrieval list */
struct ret_var
{
	char		*r_elm;		/* string invoking variable	    */
					/* e.g. "*intpa [2]"		    */
	struct ret_var	*r_next;	/* next variable used in "tupret"   */
	char		r_type;		/* type of variable		    */
};

struct ret_list
{
	struct ret_var	*ret_first;	/* first node in ret_var chain	    */
	struct ret_var	*ret_last;	/* last node in chain		    */
};

	/* "# include" file processing stack (list) */
struct inc_file
{
	int		inc_yyline;	/* old file's yyline		    */
	int		inc_lineout;	/*  "    "     Lineout		    */
	char		*inc_fid;	/* name				    */
	FILE		*inc_infile;	/* In_file			    */
	FILE		*inc_outfile;	/* Out_file			    */
	struct inc_file	*inc_next;
# ifdef SETBUF
	char		*inc_ibuf;	/* Ibuf				    */
# endif
};

/* Structure for yacc generated terminal codes				    */
/*	This avoids having to include functions in			    */
/*	[grammar.y] for the function to know the 			    */
/*	parser generated token numbers.					    */
struct special
{
	int	sp_name;		/* NAME				    */
	int	sp_sconst;		/* SCONST			    */
	int	sp_i2const;		/* I2CONST			    */
	int	sp_i4const;		/* I4CONST			    */
	int	sp_f8const;		/* F8CONST			    */
	int	sp_quote;		/* QUOTE			    */
	int	sp_bgncmnt;		/* BGNCMNT			    */
	int	sp_endcmnt;		/* ENDCMNT			    */
	int	sp_c_code;		/* C_CODE			    */
	int	sp_struct_var;
};

/*	Global definitions 	    */
extern int	Opcode;			/* op code to distinguis tokens	    */
extern int	yychar;			/* yacc input token		    */
extern int	yyline;			/* yacc external line counter	    */
extern char	*yylval;		/* yacc external stack variable	    */
extern int	yydebug;		/* set by "-y" command line arg,    */
					/* has yacc parser give details	    */
					/* of parse			    */
extern int	Newline;		/* less than one token has been	    */
					/* read from the input line, and    */
					/*  yylex isn't sure yet whether it */
					/* is C_CODE or not		    */
extern int	Lineout;		/* output line count		    */
extern int	In_quote;		/* set if inside an IIwrite("...    */
extern char	*Input_file_name;	/* input file name		    */
extern int 	C_code_flag;		/* set to indicate C-code in parser */
extern int 	Pre_proc_flag;		/* set to indicate a 		    */
					/* pre-processor line 		    */
extern int	Block_level;		/* > 0 local, 0 global		    */
extern int	Indir_level;		/* holds indirection level	    */
					/* of a reference to a C-var	    */
extern int	In_string;		/* set if output is in a string	    */
extern int	Fillmode;		/* set if output line being filled  */
extern short	Fillcnt;		/* count to fill a line to	    */
extern int	Charcnt;		/* # chars written onto output line */
extern int	Rtdb;			/* set by "-d" command line arg,    */
					/* supplies EQL runtime support     */
					/* with info to report file name    */
					/* and line number in case of an    */
					/* error			    */
extern int	Kwrdnum;		/* # of entries in Kwrdtab tokens.y */
extern char	Line_buf[];		/* buffer for input line	    */
extern char	*Line_pos;		/* position in input line	    */

extern struct optab 	Kwrdtab[];	/* table of key words [tokens.y]    */
extern struct optab 	Optab[];	/* table of operators [tokens.y]    */
extern struct special	Tokens;		/* holds yacc generated lex codes   */
					/* [tokens.y]			    */
extern struct cvar	*C_globals;	/* Global C-variable tree	    */
extern struct cvar	*C_locals;	/* Local C-variable tree	    */
extern struct cvar	*F_globals;	/* Global structure fields	    */
extern struct cvar	*F_locals;	/* Local structure fields	    */
extern struct display	*Cv_display;	/* display into which "cvarx"'s     */
					/* get put 			    */
extern struct inc_file	*Inc_file;	/* file stack pushed by #includes   */

/*	I/O manipulation data structures	    */
extern FILE		*In_file;	/* input file			    */
extern FILE		*Out_file;	/* output file			    */
# ifdef SETBUF
extern char		*Ibuf;		/* input buffer			    */
extern char		Out_buf[];	/* buffer for Out_file		    */
# endif

/* character type mapping */
# define	ASCII_MASK	0177
# define	BYTE_MASK	0377
extern char			Cmap[];
# ifdef EBCDIC
# ifdef ESER_PSU
extern char			_etoa_[];
# define	ETOA(c)		_etoa_[(c) & BYTE_MASK]
# endif
# define	CMAP(c)		Cmap[(ETOA(c)) & ASCII_MASK]
# else
# define	CMAP(c)		Cmap[(c) & ASCII_MASK]
# endif

# ifdef xDEBUG
extern char		Lex_debug;	/* debugging info for lexemes in    */
					/* yylex() [yylex.c]		    */
extern char		Chardebug;	/* print debugging info for routine */
					/* in getch.c			    */
# endif
