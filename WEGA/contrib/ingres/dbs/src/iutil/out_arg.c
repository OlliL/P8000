# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"

/*
**  OUTPUT ARGUMENTS GLOBAL INITIALIZATION
*/
struct out_arg	AAout_arg =
{
	6,		/* c0width	*/
	6,		/* i1width	*/
	6,		/* i2width	*/
	13,		/* i4width	*/
# ifndef NO_F4
	10,		/* f4width	*/
# endif
	10,		/* f8width	*/
# ifndef NO_F4
	3,		/* f4prec	*/
# endif
	3,		/* f8prec	*/
# ifndef NO_F4
	'n',		/* f4style	*/
# endif
	'n',		/* f8style	*/
	66,		/* linesperpage	*/
	'|',		/* coldelim	*/
	'-',		/* linedelim	*/
};
