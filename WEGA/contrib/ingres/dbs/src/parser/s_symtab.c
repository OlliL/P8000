# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/scanner.h"

static char	Sbuf[SBUFSIZ];	/* symbol table buffer			*/

/*
** SYMENT
**	enter a symbol into the symbol table
*/
char	*syment(ptr, len1)
char	*ptr;
int	len1;
{
	register int	len;
	register char	*p;
	extern char	*need();

	len = len1;
	p = need(Sbuf, len);
	AAbmove(ptr, p, len);
	return (p);
}

/*
** FREESYM
**	free all entries in the symbol table
*/
freesym()
{
	extern int	neederr();

	initbuf(Sbuf, SBUFSIZ, SBUFOFLO, neederr);
}
