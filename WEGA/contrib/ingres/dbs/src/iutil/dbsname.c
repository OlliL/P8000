# include	"gen.h"
# include	"../h/dbs.h"

/*
**  MAKE DBS FILE NAME
**
**	The null-terminated string 'iname' is converted to a
**	file name as used by the dbs relations.  The name
**	of the relation is padded out to be MAXNAME bytes long,
**	and the two-character id 'id' is appended.  The whole
**	thing will be null-terminated and put into 'outname'.
**
**	'Outname' must be at least MAXNAME + 3 bytes long.
*/
AAdbname(iname, id, p)
register char	*iname;
register char	*id;
register char	*p;
{
	extern char	*AApmove();

# ifdef VENIX
	p = AApmove(iname, p, MAXNAME, '#');
# else
	p = AApmove(iname, p, MAXNAME, ' ');
# endif
	AAbmove(id, p, 2);
	p[2] = '\0';
}
