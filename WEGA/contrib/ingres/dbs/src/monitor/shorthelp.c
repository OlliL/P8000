# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"monitor.h"

# define	HELPMSG		"\t? %s\n\tVersuchen Sie %s\n"

/*
**  SHORT (SYNTAX) ON-LINE HELP
*/
shorthelp(x)
register int	x;		/* x == 1:  short help */
				/*   == 0:   long help */
{
	register char		*help;
	extern char		*AAztack();
	extern char		*getfilename();

	help = getfilename();
	if (!*help)
		help = "toc";
	if (AAcat(AAztack(AAztack(AApath, x? "/sys/ref/": "/sys/man/"), help)))
		printf(HELPMSG, help, x? "help": "HELP");
}
