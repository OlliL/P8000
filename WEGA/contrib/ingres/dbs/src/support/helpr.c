# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/access.h"
# include	"../h/lock.h"
# include	"../h/bs.h"

# ifdef P8000
ret_buf		AAjmp_buf;
# else
jmp_buf		AAjmp_buf;
# endif

FILE		*Dbs_out = stdout;
char		*AAdatabase;
extern int	AAdbstat;
char		SYSNAME[]	= "_SYS";
int		s_SYSNAME	= sizeof SYSNAME - 1;

main(argc, argv)
int	argc;
char 	*argv[];
{
	register char		**av;
	register int		i;
	register char		*ii;
	register char		*p;
	register int		nc;
	register char		**nv;
	char			*qm;
# ifdef SETBUF
	char			stdbuf[BUFSIZ];
# endif
	char			*newpv[MAXPARMS];
	extern char		*Parmvect[];
	extern char		*Flagvect[];
	extern char		*AAdbpath;
	extern char		*AAproc_name;
	extern char		*qmtest();

	AAproc_name = "HELPR";

#	ifdef xSTR1
	AAtTrace(&argc, argv, 'X');
#	endif

	i = initucode(argc, argv, TRUE, (char *) 0, M_SHARE);
	AAdatabase = Parmvect[0];
	switch (i)
	{
	  case 0:
	  case 5:
		break;

	  case 1:
	  case 6:
		AAsyserr(0, "Die Datenbank '%s' existiert nicht", AAdatabase);

	  case 2:
		AAsyserr(0, "Sie duerfen auf die Datenbank '%s' nicht zugreifen", AAdatabase);

	  case 3:
		AAsyserr(0, "Sie sind kein eingetragener DBS-Nutzer");

	  case 4:
		printf("Sie haben keinen Datenbanknamen angegeben\n");
	usage:
		AAsyserr(0, "HINWEIS: helpr db_name [{relname}]");

	  default:
		AAsyserr(22200, AAproc_name, i);
	}

	if (Flagvect[0])
	{
		printf("Es sind keine Flags gestattet\n");
		goto usage;
	}

	if (chdir(AAdbpath) < 0)
		AAsyserr(22201, AAproc_name, AAdbpath);

# ifdef SETBUF
	setbuf(stdout, stdbuf);
# endif

	/* initialize access methods (and AAdmin struct) for user_ovrd test */
	AAam_start();

	av = &Parmvect[1];	/* get first param after datbase name */
	p = *av;
	if (!p)
	{
		/* special case of no relations specified */
		nv = newpv;
		*nv++ = "2";
		*nv = (char *) -1;
		help(1, newpv);
	}
	else
	{
		do
		{
			nc = 0;
			nv = newpv;

			if (qm = qmtest(p))
			{
				/* either help view, integrity or protect */
				av++;
				while (p = *av++)
				{
					if (ii = qmtest(p))
					{
						/* change of qmtest result */
						qm = ii;
						continue;
					}
					*nv++ = qm;
					*nv++ = p;
					nc += 2;
				}
				*nv = (char *) -1;
				display(nc, newpv);
			}
			else
			{
				/* help relname */
				while ((p = *av++) && !qmtest(p))
				{
					if (AAsequal("all", p))
					{
						*nv++ = "3";
						nc++;
					}
					else
					{
						*nv++ = "0";
						*nv++ = p;
						nc += 2;
					}
				}
				*nv = (char *) -1;
				help(nc, newpv);
				/* this backs av up one step, so
				 * that it points at the keywords (permit,
				 * integrity, view) or the (char *) 0
				 */
				--av;
			}
		} while (p);
	}
	AAunldb();
	fflush(stdout);
	exit(0);
}



char	*qmtest(parm)
register char	*parm;
{
	if (AAsequal("view", parm))
		return ("4");

	if (AAsequal("permit", parm))
		return ("5");

	if (AAsequal("integrity", parm))
		return ("6");

	return ((char *) 0);
}


rubproc()
{
	AAunlall();
	fflush(stdout);
	exit(1);
}
