# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/access.h"

/*
**	SYSTEM RELATION DESCRIPTOR CACHE DEFINITION
**
**	***  TOOLED FOR QUERY MODIFICATION PROCESS  ***
*/

DESC			Reldes;
DESC			Prodes;
DESC			Intdes;
DESC			Treedes;

struct desxx	Desxx[] =
{
	AA_REL,		&Reldes,	&AAdmin.adreld,
	AA_PROTECT,	&Prodes,	(DESC *) 0,
	AA_INTEG,	&Intdes,	(DESC *) 0,
	AA_TREE,		&Treedes,	(DESC *) 0,
	(char *) 0
};
