# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/bs.h"
# include	"monitor.h"

/*
**  CALL BS SHELL
**
**	The BS shell is called.  Shell() first tries to call an
**	alternate shell defined by the macro SHELL, and if it fails
**	calls BIN_SH.
**
**	If an argument is supplied, it is a shell file which is
**	supplied to the shell.
*/
shell()
{
	register int		i;
	register char		*shellfile;
	register char		*sh_arg;
	extern int		exit();
	extern int		(*AAexitfn)();
	extern char		*macro();
	extern char		*getfilename();

	sh_arg = "-c";
	shellfile = getfilename();
	if (!*shellfile)
	{
# ifdef MACRO
		if (!(shellfile = macro(SHELL)))
# endif
			sh_arg = "-i";
	}

	fclose(Qryiop);

	fflush(stdout);
	if ((Xwaitpid = fork()) == -1)
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
		if (Nodayfile >= 0)
			printf(">>%s\n", shellfile? shellfile: BIN_SH);
		fflush(stdout);
		execlp(BIN_SH, BIN_SH, sh_arg, shellfile, (char *) 0);
		if (Nodayfile >= 0)
		{
			printf("CANNOT EXEC\n");
			fflush(stdout);
		}
		exit(-1);
	}

	/* wait for shell to complete */
	xwait();
}
