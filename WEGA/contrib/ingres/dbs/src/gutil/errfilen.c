# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"

static char	AA_ERRFN[]	="/sys/etc/error";

/*
** Errfn() -- Returns the pathname where the error file can be found
**
**	It is assumed that the error errnum cannot be more than 999
*/
char	*AAerrfn(errnum)
register int	errnum;
{
	register char	*cp;
	char		ver[12];
	extern char	AAversion[];
	extern char	*AAbmove();
	extern char	*AAztack();
	extern char	*AA_iocv();

	AAbmove(AAversion, cp = ver, sizeof ver);
	/* make sure any mod number is removed */
	for ( ; *cp; cp++)
		if (*cp == '/')
			break;

	/* now insert the "_X" */
	*cp++ = '_';
	*cp++ = 0;
	return (AAztack(AAztack(AAztack(AApath, AA_ERRFN), ver), AA_iocv(errnum)));
}
