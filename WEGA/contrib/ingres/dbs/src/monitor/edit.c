# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/bs.h"
# include	"monitor.h"

/*
**  CALL TEXT EDITOR
**
**	The BS text editor is called.  The actual call is to
**	the macro EDITOR.  If that fails, /bin/ed is called.
**	This routine suppressed the autoclear function.
*/
edit()
{
	register int	i;
	register char	*p;
	register char	*editfile;
	extern int	exit();
	extern int	(*AAexitfn)();
	extern char	*getfilename();
	extern char	*macro();

	editfile = getfilename();
	if (!*editfile)
		editfile = Qbname;

	Autoclear = 0;
	fclose(Qryiop);

	/* FORK PROCESS & INVOKE THE EDITOR */
	fflush(stdout);
	if ((Xwaitpid = fork()) < 0)
	{
		printf("CANNOT FORK\n");
		return;
	}
	if (!Xwaitpid)
	{
		AAexitfn = exit;
		setuid(getuid());
		setgid(getgid());
		for (i = 3; i < MAXFILES; i++)
			close(i);
# ifdef MACRO
		if (!(p = macro(EDITOR)))
# endif
			p = BIN_ED;
		if (Nodayfile >= 0)
			printf(">>%s\n", p);
		fflush(stdout);
		execlp(p, p, editfile, (char *) 0);
		if (Nodayfile >= 0)
		{
			printf("CANNOT EXEC\n");
			fflush(stdout);
		}
		exit(-1);
	}

	/* WAIT FOR PROCESS TO DIE */
	xwait();
}
