# include	<stdio.h>
# include	"gen.h"
# include	"../h/bs.h"

# ifdef P8000
ret_buf		AAjmp_buf;
# else
jmp_buf		AAjmp_buf;
# endif

main(argc, argv)
int	argc;
char 	*argv[];
{
	register char		**av;
	register int		x;
	register int		i;
	extern char		*AAproc_name;
	extern char		*Parmvect[];
	extern char		*Flagvect[];

	AAproc_name = (x = (*argv[0] == 'R') || (*argv[0] == 'r'))? "REFR": "MANR";
	switch (i = initucode(argc, argv, 0, (char *) 0, -1))
	{
	  case 0:
	  case 1:
	  case 2:
	  case 4:
	  case 5:
	  case 6:
		break;

	  case 3:
		AAsyserr(0, "Sie sind kein eingetragener DBS-Nutzer");

	  default:
		AAsyserr(22200, AAproc_name, i);
	}

	if (Flagvect[0])
	{
		printf("Es sind keine Flags gestattet\n");
		exit(1);
	}

	av = Parmvect;
	do shorthelp(x, *av); while (*av && *++av);

	fflush(stdout);
	exit(0);
# ifdef ESER_PSU
	AACCESS();
# endif
}


shorthelp(x, help)
register int	x;		/* x == 1:  short help */
				/*   == 0:   long help */
register char		*help;
{
	extern char		*AApath;
	extern char		*AAztack();

	if (!help)
		help = "toc";
	if (AAcat(AAztack(AAztack(AApath, x? "/sys/ref/": "/sys/man/"), help)))
		printf("Unbekanntes Manual %s\n", help);
}


rubproc()
{
	fflush(stdout);
	exit(1);
}
