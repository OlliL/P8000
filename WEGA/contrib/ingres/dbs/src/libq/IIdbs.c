# include	<stdio.h>
# include	"gen.h"

# define	CLOSED		'?'

# include	"../h/dbs.h"
# include	"../h/symbol.h"
# include	"../h/pipes.h"
# include	"../h/bs.h"
# include	"../h/aux.h"
# include	"IIglobals.h"

# ifdef P8000
ret_buf		IIjmp_buf;
# else
jmp_buf		IIjmp_buf;
# endif

int		IIqlpid;		/* process id of DBS		    */
int		IIin_retrieve;		/* set if inside a retrieve	    */
int		IIndoms;		/* # of domains in this retrieve    */
int		IIdoms;			/* index into source field of buf   */
long		IItupcnt;		/* # tuples which satified last     */
struct retsym	IIretsym[MAXDOM];	/* retrieve table		    */
struct retsym	IIr_sym;		/* type, length fields used by new  */

int		IIw_down;		/* pipe descriptors for 	    */
int		IIr_down;		/* parser communcation 		    */
int		IIr_front;		/* pipe descriptor for 		    */
					/* ovqp ommunication 		    */
struct pipfrmt	IIeinpipe;		/* the data pipe structure	    */
struct pipfrmt	IIinpipe;		/* the control pipe struct 	    */
struct pipfrmt	IIoutpipe;		/* the pipe for writing queries	    */

static char	*IImainpr   = "/bin/iql";
# ifdef ESER_PSU
static char	II_PASSWD[] = "/SYS/PASSWD";
# else
static char	II_PASSWD[] = "/etc/passwd";
# endif

/*
**  IIGETPATH -- initialize the IIPathname variable
**
**	Sets IIPathname to the home directory of the USERDBS
**	[bs.h] user.
*/
# ifdef DBS_PATH
char		*IIPathname = DBS_PATH;
# else
static char	IIPathbuf[41];
char		*IIPathname = IIPathbuf;

IIgetpath()
{
	static char		reenter;
	register int		i;
	register int		c;
	register char		*lp;
	register FILE		*iop;
	char			line[MAXLINE + 1];
	char			*field[UF_NFIELDS];

	if (reenter)
		return;
	else
		reenter++;

	if (!(iop = fopen(II_PASSWD, "r")))
		IIsyserr("Passwordfile %s nicht lesbar", II_PASSWD);

	do
	{
		/* get a line from the II_PASSWD */
		i = 0;
		for (lp = line; (c = getc(iop)) != '\n'; )
		{
			if (c <= 0)
				IIsyserr("Nutzer %s im Passwordfile %s nicht verzeichnet", USERDBS, II_PASSWD);
			if (++i > sizeof line - 1)
			{
				*lp = '\0';
				IIsyserr("GETPATH: Zeilenpufferueberlauf bei \"%s\"", line);
			}
			*lp++ = c;
		}
		*lp = '\0';
		for (i = 0, lp = line; *lp; lp++)
		{
			if (*lp == ':')
			{
				*lp = '\0';
				field[++i] = lp + 1;
			}
		}
	} while (!IIsequal(line, USERDBS));
	fclose(iop);

	/* check for enough fields for valid entry */
	if (i < PF_HOME)
		IIsyserr("Zu wenig Felder in dem Eintrag \"%s\"", line);

	/* check that pathname won't overflow static buffer */
	if ((i = IIlength(field[PF_HOME]) + 1) > sizeof IIPathbuf)
		IIsyserr("DBS-Pfadname \"%s\" ist zu lang", field[PF_HOME]);

	/* move pathname into buffer */
	IIbmove(field[PF_HOME], IIPathname, i);
}
# endif


/*
**	IIdbs opens the needed pipes and
**	forks an DBS process.
**
**	DBS recognizes the EQL flag followed by
**	three control characters as being an EQL processes
**
**	parameters to DBS are passed directly. only
**	the first 9 are counted.
*/
IIdbs(p1, p2, p3, p4, p5, p6, p7, p8, p9)
char	*p1, *p2, *p3, *p4, *p5, *p6, *p7, *p8, *p9;
{
	int		pipes[6];		/* pipe vector buffer */
	char		eoption[10];	/* dummy var to hold -EQL option */
	register char	*cp;
	char		*argv[12];
	register char	**ap;
	extern char	*IIconcatv();
	extern		IIresync();
	extern		*(IIinterrupt)();
	extern		exit();
	extern int	(*signal())();

#	ifdef xETR1
	if (IIdebug)
		printf("DBS\n");
#	endif
	/* test if DBS is already invoked */
	if (IIqlpid)
		IIsyserr("Zweites ## DBS-Statement");

	IIgetpath();
	/* open DBS pipes */
	if (pipe(&pipes[0]) || pipe(&pipes[2]) || pipe(&pipes[4]))
		IIsyserr("DBS: PIPE ERROR");

	IIw_down = pipes[1];	/* file desc for EQL->parser */
	IIr_down = pipes[2];	/* file desc for parser->EQL */
	IIr_front = pipes[4];	/* file desc for ovqp->EQL */

	/* catch interupts if they are not being ignored */
	if (signal(2, (int (*)()) 1) != (int (*)()) 1)
	{
		signal(2, IIresync);
		signal(3, IIresync);
	}
	/* prime the control pipes */
	IIrdpipe(P_PRIME, &IIinpipe);
	IIwrpipe(P_PRIME, &IIoutpipe, IIw_down);

	/* set up EQL option flag */
	cp = eoption;
	*cp++ = '-';
	*cp++ = EQL;
	*cp++ = pipes[0] | 0100;
	*cp++ = pipes[3] | 0100;
	*cp++ = CLOSED;	/* this will be the EQL->ovqp pipe in the future */
	*cp++ = pipes[5] | 0100;
	/* put "6.2" at end of flag for OVQP to not do flush after
	 * every tuple
	 */
	*cp++ = '6';
	*cp++ = '.';
	*cp++ = '2';
	*cp = '\0';

	fflush(stdout);
	if ((IIqlpid = fork()) < 0)
		IIsyserr("DBS: FORK ERROR");
	/* if parent, close the unneeded files and return */
	if (IIqlpid)
	{
		close(pipes[0]);
		close(pipes[3]);
		close(pipes[5]);
#		ifdef xETR1
		if (IIdebug)
			printf("## DBS database %s\n", p1);
#		endif
		/* resync to child */
		IIrdpipe(P_INT, &IIinpipe, IIr_down);
		return;
	}
	/* the child overlays .../bin/iql */
	ap = argv;
	*ap++ = "iql";
	*ap++ = eoption;
	*ap++ = p1;
	*ap++ = p2;
	*ap++ = p3;
	*ap++ = p4;
	*ap++ = p5;
	*ap++ = p6;
	*ap++ = p7;
	*ap++ = p8;
	*ap++ = p9;
	*ap = (char *) 0;
	execv(cp = IIconcatv(IIPathname, IImainpr), argv);
	IIsyserr("DBS: EXEC ERROR \"%s\"", cp);
}

# ifndef SIGN_EXT
# include	"../gutil/i1toi2.c"
# endif
