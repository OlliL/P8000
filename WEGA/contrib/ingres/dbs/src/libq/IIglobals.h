/*  IIGLOBALS.H -- EQL run-time library globals				    */
/*									    */
/*	In this file are defined the global variables,			    */
/*	and manifest constants used in the EQL run-time			    */
/*	routines. All globals must start with "II".			    */
/*									    */
/*	Requires:							    */
/*		The "pipfrmt" structure is the dbs interprocess		    */
/*		pipe buffer structure defined in ".../pipes.h".		    */
/*		MAXDOM is defined in h/dbs.h.				    */

# define	opINT		1
# ifndef NO_F4
# define	opFLT		2
# endif
# define	opSTRING	3
# define	opDOUBLE	4
# define	opCHAR		5
# define	opLONG		6

# define	EQL		'&'

extern char	*IIproc_name;		/* file name			    */
extern int	IIline_no;		/* line no			    */
extern int	IIdebug;		/* debug flag			    */
extern int	IIqlpid;		/* process id of dbs		    */
extern int	IIin_retrieve;		/* set if inside a retrieve	    */
extern int	IIndoms;		/* # of domains in this retrieve    */
extern int	IIdoms;			/* index into source field of buf   */
extern long	IItupcnt;		/* # tuples which satified last     */
					/* update query 		    */
extern int	IIw_down;		/* pipe descriptors for 	    */
extern int	IIr_down;		/* parser communcation 		    */
extern int	IIr_front;		/* pipe descriptor for 		    */
					/* ovqp ommunication 		    */
extern int	IIerrflag;		/* error flag.  Set in IIerror,	    */
					/* cleared in IIsync 		    */
extern int	(*IIprint_err)();	/* wether or not error messgs	    */
					/* should be printed		    */
extern int	IIret_err();		/* returns its integer arg for 	    */
					/* (*IIprint_err)()		    */
extern int	IIno_err();		/* returns 0 for (*IIprint_err)()   */
extern int	(*IIo_print)();		/* a one value stack for 	    */
					/* temporarily turned off printing  */
					/* of errors. Done in [IIw_left.c   */
					/* and IIw_right.c].		    */

struct retsym
{
	char		type;
	char		len;
	char		*addr;
};
extern struct retsym	IIretsym[];	/* retrieve table		    */
extern struct retsym	IIr_sym;	/* type, length fields used by new  */
					/* EQL in IIn_ret and IIn_get	    */

extern struct pipfrmt	IIeinpipe;	/* the data pipe structure	    */
extern struct pipfrmt	IIinpipe;	/* the control pipe struct 	    */
extern struct pipfrmt	IIoutpipe;	/* the pipe for writing queries	    */
