# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/bs.h"
# include	"../h/lock.h"
# include	"../h/pipes.h"
# include	"monitor.h"

# ifdef P8000
ret_buf		AAjmp_buf;
# else
jmp_buf		AAjmp_buf;
# endif

char		Qbname[QRYNAME];/* pathname of query buffer		    */
int		Nodayfile;	/* suppress dayfile/prompts		    */
int		Userdflag;	/* same: user flag			    */
int		Prompt;		/* set if a prompt is needed		    */
int		Nautoclear;	/* if set, disables the autoclear option    */
int		Newline;	/* set if last character was a newline	    */
int		Oneline;	/* deliver EOF after one line input	    */
int		Idepth;		/* include depth			    */
FILE		*Input;		/* current input file			    */
FILE		*Qryiop;	/* the query buffer			    */
# ifdef SETBUF
char		Qryxxx[BUFSIZ];
# endif
int		Xwaitpid;	/* pid to wait on - zero means none	    */
int		Error_id;	/* the error number of the last err	    */

static int	Phase;		/* set if in processing phase		    */
static char	Versn[12];	/* version number (w/o mod #)		    */

extern char	*AAusercode;
extern char	*AAdatabase;

/*
**  INTERACTIVE TERMINAL MONITOR
**
**	The monitor gathers text from the standard input and performs
**	a variety of rudimentary editting functions.  This program
**	is the main setup.  Monitor() is then called, which does the
**	real work.
**
**	variables:
**	Nodayfile -- zero prints all messages; positive one suppresses
**		dayfile and logout but not prompts; negative one
**		suppresses all printed material except results from \p.
**	Newline -- set when the last character in the query buffer
**		is a newline.
**	Prompt -- set when a prompt character is needed.
**	Autoclear -- set when the query buffer should be cleared before
**		putting another character in.
**	Nautoclear -- if set, suppresses the autoclear function
**		entirely.
**
**	flags:
**	-M -- trace flag
**	-d -- suppress dayfile
**	-s -- suppress prompt (sets -d)
**	-a -- disable autoclear function
**
**	The last three options can be set by stating "+x".
*/
main(argc, argv, env)
int	argc;
char	*argv[];
char	*env[];
{
# ifdef xMTR3
	register int		ndx;
# endif
	register char		*p;
	char			buff[100];
	extern char		*valflag;
	extern char		**environ;
	extern int		Eql;
	extern char		*AAproc_name;
	extern char		**Xparams;
	extern int		quit();
	extern int		(*AAexitfn)();
	extern char		*getufield();
	extern char		*AAcustomer();
	extern char		*AAztack();
	extern long		time();
	extern char		*ctime();
# ifdef MACRO
	extern char		*mcall();
# endif

	environ = env;

	/* insure that permissions are ok */
	setuid(getuid());
	setgid(getgid());
	AALOCKREQ.l_pid = getpid();

	setexit();
	signal(13, quit);

# ifdef xMTR1
	AAtTrace(&argc, argv, 'M');
# endif

	/* now all looks better */
	set_so_buf();

	initproc("MONITOR", argv);
	AAexitfn = quit;
	set_si_buf();

	/* process arguments */
	if (!setflag(argv, 'd', 1))
		Nodayfile = 1;
	if (!setflag(argv, 's', 1))
		Nodayfile = -1;
	Nautoclear = !setflag(argv, 'a', 1);

# ifdef xMTR3
	if (AAtTf(9, 0))
	{
		close(ndx = dup(FALSE));
		printf("0 dups as %d\n", ndx);
	}
	if (AAtTf(9, 1))
		AAprargs(argc, argv);
	if (AAtTf(9, 2))
	{
		printf("R_up %d W_up %d R_down %d W_down %d\n",
			R_up, W_up, R_down, W_down);
		printf("\tR_front %d W_front %d Eql %d\n",
			R_front, W_front, Eql);
	}
# endif

	AAsmove(AAversion, Versn);
	for (p = Versn; *p != '/'; p++)
		if (!*p)
			break;
	*p = '\0';

# ifdef MACRO
	/* preinitialize macros */
	macinit((int (*)()) 0, (char **) 0, FALSE);
	macdefine(PATHNAME, AApath, TRUE);
	macdefine(DATABASE, AAdatabase, TRUE);
	macdefine(USERCODE, AAusercode, TRUE);
# endif

	/* print the dayfile */
	if (Nodayfile >= 0)
	{
		time(buff);
		printf(LOGIN, AAversion);
		printf("%s\n", AAcustomer(quit));
# ifdef ESER_VMX
		printf("%s", ctime(*((long *) buff)));
# else
		printf("%s", ctime(buff));
# endif
	}
	if (!Nodayfile)
		AAcat(AAztack(AAztack(AApath, DAYFILE), Versn));

	/* SET UP LOGICAL QUERY-BUFFER FILE */
	AAconcat(QRYBUF, Fileset, Qbname);
	if (!(Qryiop = fopen(Qbname, "w")))
		AAsyserr(16014, Qbname);
# ifdef SETBUF
	setbuf(Qryiop, Qryxxx);
# endif

	/* GO TO IT ... */
	Prompt = Newline = TRUE;
	Userdflag = Nodayfile;
	Nodayfile = -1;

	/* run the system initialization file */
	setexit();
	Phase++;
	include(Xparams[0]);

	/* find out what the user initialization file is */
	setexit();
	if (!getuser(AAusercode, buff))
	{
		p = getufield(buff, UF_IFILE);
		if (*p)
			include(p);
	}
	getuser((char *) 0, (char *) 0);

	Nodayfile = Userdflag;

	/* get user input from terminal */
	Input = stdin;
	setbuf(stdin, (char *) 0);
	if (setflag(argv, 'I', 1))
	{
		setexit();
		xwait();
		command();
	}
	else if (!setexit())
# ifdef MACRO
		include(mcall(valflag));
# else
		include(valflag);
# endif
	quit();
}


/*
**  CATCH SIGNALS
**
**	clear out pipes and respond to user
*/
rubproc()
{
	register int		i;
	extern long		lseek();

# ifdef xMTR3
	AAtTfp(10, 0, "caught sig\n");
# endif

	if (!Xwaitpid)
		printf("\nInterrupt\n");
	resyncpipes();
	for (i = 3; i < MAXFILES; i++)
	{
		if (i == Qryiop->_file
			|| i == R_down
			|| i == W_down
			|| i == AAlockdes
# ifdef MACRO
			|| (Trapfile && i == Trapfile->_file)
# endif
		   )
			continue;
		close(i);
	}
	lseek(stdin->_file, 0L, 2);
	Newline = Prompt = TRUE;
	Nodayfile = Userdflag;
	Oneline = FALSE;
	Idepth = 0;
	setbuf(stdin, (char *) 0);
	Input = stdin;
	sync();
	fflush(stdout);
}


# ifdef EBCDIC
# define	LAST_CHAR	0377
# else
# define	LAST_CHAR	0177
# endif

/*
**  PROCESS ERROR MESSAGE
**
**	This routine takes an error message off of the pipe and
**	processes it for output to the terminal.  This involves doing
**	a lookup in the AA_ERRFN? files, where ? is the thous-
**	ands digit of the error number.  The associated error message
**	then goes through parameter substitution and is printed.
**
**	In the current version, the error message is just printed.
*/
proc_error(s /*, rpipnum */)
register struct pipfrmt	*s;
/* int			rpipnum; */
{
	register int		c;
	register char		*p;
	register FILE		*iop;
	int			pc;
	register int		i;
	register int		err;
	short			ix;
	char			*pv[10];
	char			parm[512];
	char			buf[sizeof parm + 30];
# ifdef SETBUF
	char			iobufx[BUFSIZ];
# endif
	extern char		*AAerrfn();
# ifdef MACRO
	extern char		*mcall();
# endif

	Error_id = err = s->err_id;

	/* read in the parameters */
	pc = 0;
	p = parm;
	while (i = rdpipe(P_NORM, s, R_down, p, 0))
	{
		pv[pc] = p;
# ifdef xMTR3
		AAtTfp(11, 1, "pv[%d] = %s, i=%d\n", pc, p, i);
# endif
		p += i;
		pc++;
		if (pc >= 10 || (p - parm) >= 500)	/* buffer overflow */
			AAsyserr(16015, pc, err, p - parm);
	}
	pv[pc] = 0;
	rdpipe(P_PRIME, s);

# ifdef MACRO
	/* try calling the CATCHERROR macro -- maybe not print */
	macdefine(ERRORCOUNT, AA_iocv(err), TRUE);
	if (AAsequal(mcall(CATCHERROR), "0"))
		return;
# endif

	/* open the appropriate error file */
	p = AAerrfn(err / 1000);

# ifdef xMTR3
	AAtTfp(11, 0, "PROC_ERROR: %d, %s\n", err, p);
# endif

	if (!(iop = fopen(p, "r")))
		AAsyserr(16016, p);
# ifdef SETBUF
	setbuf(iop, iobufx);
# endif

	/* read in the code and check for correct */
	for ( ; ; )
	{
		p = buf;
		while ((c = getc(iop)) != '\t')
		{
			if (c <= 0)
			{
				/* no code exists, print the args */
				printf("%d:", err);
				for (i = 0; i < pc; i++)
					printf(" '%s'", pv[i]);
				goto error;
			}
			*p++ = c;
		}
		*p = 0;
		if (AA_atoi(buf, &ix))
			AAsyserr(16017, err, buf);
		if ((i = ix) != err)
		{
			while ((c = getc(iop)) != ERRDELIM)
				if (c <= 0)
					AAsyserr(16018, err);
			getc(iop);	/* throw out the newline */
			continue;
		}

		/* got the correct line, print it doing parameter substitution */
		printf("%d: ", err);
		c = '\n';
		for ( ; ; )
		{
			c = getc(iop);
			if (c <= 0 || c == ERRDELIM)
error:
			{
				putchar('\n');
				fclose(iop);
				return;
			}
			if (c == '%')
			{
				c = getc(iop);
				for (p = pv[c - '0']; c = *p; p++)
				{
					if (c < ' ' || c >= LAST_CHAR)
						printf("\\0%o", c);
					else
						putchar(c);
				}
				continue;
			}
			printf("%c", c);
		}
	}
}


AApageflush()
{
}


/*
**  END_JOB -- "cannot happen"
*/
end_job()
{
	AAsyserr(16019);
}
