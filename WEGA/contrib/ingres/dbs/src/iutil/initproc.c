# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/lock.h"
# include	"../h/bs.h"

# define	CLOSED		077

char		*Fileset;	/* unique string to append to temp file names */
char		*AAusercode;	/* code to append to relation names */
char		*AAdatabase;	/* database name */
char		**Xparams;	/* other parameters */
char		*AApath;	/* pathname of DBS subtree */
char		*AAdbpath;	/* pathname of DB (always empty) */
int		R_up;		/* pipe descriptor: read from above */
int		W_up;		/* pipe descriptor: write to above */
int		R_down;		/* pipe descriptor: read from below */
int		W_down;		/* pipe descriptor: write to below */
int		R_front;	/* pipe descriptor: read from front end */
int		W_front;	/* pipe descriptor: write to front end */
int		W_err;		/* pipe descriptor: write error message (usually W_up */
int		Eql;		/* flag set if running an EQL program */
int		Rublevel;	/* rubout level, set to -1 if turned off */
struct out_arg	AAout_arg;	/* output arguments */
int		Standalone;	/* not standalone */

/*
**  Initialize DBS Process
**
**	All initialization for a process is performed, including
**	initialization of trace info and global variables.  Typical
**	call is
**		initproc(procname, argv);
**	with procname being a string (e.g., "PARSER"),
**	argv is the parameter of main().
**
**	Calling this routine should be one of the first things done
**	in  main().
**
**	Information passed in argv is as follows:
**	argv[0] - line from process table
**	argv[1] - pipe vector
**		argv[1][0] - R_up
**		argv[1][1] - W_up
**		argv[1][2] - R_down
**		argv[1][3] - W_down
**		argv[1][4] - R_front
**		argv[1][5] - W_front
**		argv[1][6] - AAlockdes
**	argv[2] - Fileset - unique character string for temp file names.
**	argv[3] - AAusercode - two character user identifier
**	argv[4] - AAdatabase - database name
**	argv[5->n] - Xparams - additional parameters,
**		different for each process.  See .../files/proctab
**		for what each process actually gets.
**
**	The pipe descriptors are passed with the 0100 bit set, so that
**	they will show up in a readable fashion on a "ps".
**
**	A flag "Eql" is set if running an EQL program.  The flag
**	is set to 1 if the EQL program is a 6.0 or 6.0 EQL program,
**	and to 2 if it is a 6.2 EQL program.
**
**	User flags "[+-]x" are processed.  Note that the syntax of some of
**	these are assumed to be correct.  They should be checked in
**	src/support/proc_tab.c.
**
**	Signal 2 is caught and processed by 'rubcatch', which will
**	eventually call 'rubproc', a user supplied routine which cleans
**	up the relevant process.
*/
initproc(procname, argv)
char	*procname;
char	**argv;
{
	static int	reenter;
	register char	*p;
	register char	**q;
	register int	fd;
	register int	i;
	extern char	*AAproc_name;
	extern		rubcatch();	/* defined in rub.c */
	extern int	wronbrpipe();
	extern int	(*signal())();

	Standalone = FALSE;
	AAproc_name = procname;
	reenter = 0;
	setexit();
	if (reenter++)
	{
		fflush(stdout);
		exit(-1);
	}
	signal(13, wronbrpipe);
	if (signal(2, (int (*)()) 1) == (int (*)()) 0)
	{
		signal(2, rubcatch);	/* interrupt-signal */
		signal(3, rubcatch);	/* quit-signal */
	}
	else
		Rublevel = -1;
	p = argv[1];
	R_up = fd = *p++ & 077;
	if (fd == CLOSED)
		R_up = -1;
	W_up = fd = *p++ & 077;
	if (fd == CLOSED)
		W_up = -1;
	W_err = W_up;
	R_down = fd = *p++ & 077;
	if (fd == CLOSED)
		R_down = -1;
	W_down = fd = *p++ & 077;
	if (fd == CLOSED)
		W_down = -1;
	R_front = fd = *p++ & 077;
	if (fd == CLOSED)
		R_front = -1;
	W_front = fd = *p++ & 077;
	if (fd == CLOSED)
		W_front = -1;
	AAlockdes = fd = *p++ & 077;
	if (fd == CLOSED)
		AAlockdes = -1;

	q = &argv[2];
	Fileset = *q++;
	AAusercode = *q++;
	AAdatabase = *q++;
	AApath = *q++;
	Xparams = q;

	/* process flags */
	for ( ; (p = *q) != (char *) -1 && p; q++)
	{
		if (p[0] != '-')
			continue;
		switch (p[1])
		{
		  case '&':	/* EQL program */
			Eql = 1;
			if (p[6])
				Eql = 2;
			break;

		  case 'c':	/* c0 sizes */
			AA_atoi(&p[2], &AAout_arg.c0width);
			break;

		  case 'i':	/* iNsizes */
			i = p[2] - '0';
			switch (i)
			{
			  case sizeof (char):
				AA_atoi(&p[3], &AAout_arg.i1width);
				break;

			  case sizeof (short):
				AA_atoi(&p[3], &AAout_arg.i2width);
				break;

			  case sizeof (long):
				AA_atoi(&p[3], &AAout_arg.i4width);
				break;
			}
			break;

		  case 'f':	/* fN sizes */
			p = &p[3];
			fd = *p++;
			while (*p != '.')
				p++;
			*p++ = 0;
# ifndef NO_F4
			i = (*q)[2] - '0';
			if (i == sizeof (float))
			{
				AA_atoi(&(*q)[4], &AAout_arg.f4width);
				AA_atoi(p, &AAout_arg.f4prec);
				AAout_arg.f4style = fd;
			}
			else
# endif
			{
				AA_atoi(&(*q)[4], &AAout_arg.f8width);
				AA_atoi(p, &AAout_arg.f8prec);
				AAout_arg.f8style = fd;
			}
			*--p = '.';	/* restore parm for dbu's */
			break;

		  case 'v':	/* vertical seperator */
			AAout_arg.coldelim = p[2];
			break;

		  case 'l':	/* horizontal seperator */
			AAout_arg.linedelim = p[2];
			break;
		}
	}
}


wronbrpipe()
{
	extern int	errno;

	errno = 22;	/* EINVAL */
	AAsyserr(15003);
}
