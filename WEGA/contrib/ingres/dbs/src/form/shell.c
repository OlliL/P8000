# include	"form.h"

/*
**	SHELL	CALL BS SHELL
**		return (0) = ok
**		return (1) = error
*/
MMshell(frame, shell, quit, command)
struct MM_form			*frame;
int				shell;
int				quit;
register char			*command;
{
	register int			i;
	register int			pid;
	register int			line;
	register int			col;
	int				status;

	if (!command || !*command)
		return (0);

	line = MMline;
	MMline = 1;
	col = MMcol;
	MMcol = 1;
	MMclr_line(1, 1);
	MMclr_screen(2, 1);

	if ((pid = fork()) < 0)
	{
		return (1);
	}
	if (!pid)
	{
		setuid(getuid());
		setgid(getgid());
		for (i = 3; i < 20; i++)
			close(i);
		if (shell)
			execlp("sh", "sh", "-c", command, (char *) 0);
		else
			execlp(command, command, (char *) 0);
		exit(1);
	}

	/* wait for shell to complete */
	while ((i = wait(&status)) != -1)
		if (i == pid)
			break;

	i = status >> 8;
	if (!i && quit)
	{
		MMclr_line(1, 1);
		MM_ptis(" ENDE ");
		MM_getchar();
		MMclr_line(1, 1);
	}
	MMframe(frame, line, col);
	return (i);
}
