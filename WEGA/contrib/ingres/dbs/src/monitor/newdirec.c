# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"monitor.h"

/*
**  CHANGE WORKING DIRECTORY
*/
newdirec()
{
	register char		*direc;
	extern char		*getfilename();

	if (chdir(direc = getfilename()))
		printf("%s? cd `%s'\n", ERROR, direc);
}
