# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"monitor.h"

/*
**  BRANCH
**
**	The "filename" following the \b op must match the "filename"
**	which follows some \k command somewhere in this same file.
**	The input pointer is placed at that point if possible.  If
**	the label does not exist, an error is printed and the next
**	character read is an EOF.
*/
branch()
{
	register int		c;
	int			i;
# ifdef MACRO
	register char		*def_mac;	/* name of macro DEFAULT */
	long			def_off;	/* offset for label DEFAULT */
	extern char		*macro();
	extern int		macgetch();
# else
	extern int		getch();
#	endif
	char			target[FILENAME + 1];
	register char		*fn;
	extern long		lseek();
	extern char		*getfilename();

#	ifdef xMTR2
	AAtTfp(16, -1, "BRANCH");
#	endif

	/* see if conditional */
	while ((c = getch()) > 0)
		if (c != ' ' && c != '\t')
			break;
	if (c == '?')
	{
		/* got a conditional; evaluate it */
		Oneline = TRUE;
# ifdef MACRO
		macinit(getch, (char *) 0, FALSE);
		i = expr(macgetch, 0);
# else
		i = expr(&getch, 0);
# endif
		Oneline = FALSE;
		if (i <= 0)
		{
#			ifdef xMTR2
			AAtTfp(16, 0, "no BRANCH\n");
#			endif
			getfilename();
			return;
		}
	}
	else
		Peekch = c;

	/* get the target label */
	AAsmove(getfilename(), target);
	if (isatty(Input->_file))
	{
		printf("%sKein Sprung am Terminal\n", ERROR);
		return;
	}

	rewind(Input);
# ifdef MACRO
	def_mac = macro(DEFAULT);
	def_off = -1L;
# endif

	/* search for the label */
	while ((c = getch()) > 0)
	{
		if ((c != Escape) || (getescape(FALSE) != C_MARK))
			continue;
		if (AAsequal(fn = getfilename(), target))
			return;
# ifdef MACRO
		if (AAsequal(fn, def_mac))
			def_off = lseek(Input->_file, 0L, 1) - Input->_cnt;
# endif
	}
# ifdef MACRO
	if (def_off < 0L)
	{
# endif
		Peekch = -1;
		printf("%s? Label `%s'\n", ERROR, target);
# ifdef MACRO
		if (def_mac)
			printf("\t\t? Label `%s' (%s)\n", def_mac, DEFAULT);
	}
	else
		fseek(Input, def_off, 0);
# endif
}
