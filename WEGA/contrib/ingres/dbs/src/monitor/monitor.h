/*									    */
/*  MONITOR.H -- globals for the interactive terminal monitor		    */
/*									    */

/* various global names and strings					    */
extern int	Escape;		/* the command character		    */
extern char	Qbname[QRYNAME];/* pathname of query buffer		    */
extern char	AAversion[];	/* version number (with mod #)		    */
extern char	AArel_date[];	/* release date				    */
extern char	AAmod_date[];	/* release date of this mod		    */
extern char	*Fileset;	/* unique string			    */
extern char	*AApath;	/* pathname of DBS root			    */

/* flags								    */
extern int	Nodayfile;	/* suppress dayfile/prompts		    */
				/* 0 - print dayfile and prompts	    */
				/* 1 - suppress dayfile but not prompts	    */
				/* -1 - supress dayfile and prompts	    */
extern int	Userdflag;	/* same: user flag			    */
				/*  the Nodayfile flag gets reset by	    */
				/* include(); this is the flag that the	    */
				/* user actually specified (and what s/he   */
				/* gets when in interactive mode.	    */
extern int	Autoclear;	/* clear query buffer automatically if set  */
extern int	Notnull;	/* set if the query is not null		    */
extern int	Prompt;		/* set if a prompt is needed		    */
extern int	Nautoclear;	/* if set, disables the autoclear option    */
extern int	Newline;	/* set if last character was a newline	    */

/* query buffer stuff							    */
extern FILE	*Qryiop;	/* the query buffer			    */
# ifdef SETBUF
extern char	Qryxxx[BUFSIZ];
# endif

/* other stuff								    */
extern int	Xwaitpid;	/* pid to wait on - zero means none	    */
extern int	Error_id;	/* the error number of the last err	    */

/* \include support stuff						    */
extern FILE	*Input;		/* current input file			    */
extern int	Idepth;		/* include depth			    */
extern int	Oneline;	/* deliver EOF after one line input	    */
extern int	Peekch;		/* lookahead character for getch	    */

/* commands to monitor							    */
# define	C_APPEND	1
# define	C_BRANCH	2
# define	C_CHDIR		3
# define	C_EDIT		4
# define	C_GO		5
# define	C_INCLUDE	6
# define	C_MARK		7
# define	C_LIST		8
# define	C_PRINT		9
# define	C_QUIT		10
# define	C_RESET		11
# define	C_TIME		12
# define	C_EVAL		13
# define	C_WRITE		14
# define	C_SHELL		15
# define	C_S_HELP	16		/* short help */
# define	C_L_HELP	17		/*  long help */
# define	C_MONITOR	18

# ifdef MACRO
/* stuff for querytrap facility						    */
extern int	Trap;
extern FILE	*Trapfile;
# endif

/* internal retcode codes						    */
# define	RC_OK		0
# define	RC_BAD		1
