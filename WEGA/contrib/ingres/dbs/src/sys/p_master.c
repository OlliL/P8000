# include	<stdio.h>

# define	TTYS		5

char		tty[] =		"/dev/tty_";

main(argc, argv)
int	argc;
char	*argv[];
{
	register int		i;
	register int		*pid;
	register int		run;
	register int		proc;
	int			code;
	int			want[TTYS];
	char			buf[BUFSIZ];

	setbuf(stdout, buf);
	proc = 0;
	for (run = 1; run < TTYS; run++)
	{
		tty[8] = run + '0';
		dup2((i = open(tty, 2)), 0);
		dup2(i, 1);
		close(i);
		if ((i = fork()) < 0)
		{
			printf("fork proc %d failed\n", run);
			fflush(stdout);
			exit(2 * run + 1);
		}
		printf("fork proc %d\n", run);
		fflush(stdout);
		if (!i)
		{
			argv[0][0] = run + '0';
			argv[0][1] = 0;
			argv[1] = (char *) 0;
			printf("exec proc %d\n", run);
			fflush(stdout);
			execvp("p_slave", argv);
			printf("exec proc %d failed\n", run);
			fflush(stdout);
			exit(2 * run + 2);
		}
		proc++;
	}

	run = 0;
	for (pid = &want[TTYS]; pid > want; )
		*(--pid) = 0;

	while ((i = wait(&code)) > 0)
	{
		printf("p_master %d=wait(%d)\n", i, code);
		fflush(stdout);
		pid = want;
		if ((code & 0377) == 0177)
		{
			if (!run)
				ptrace(7, run = i, 1, 0);
			else if (run == i)
			{
				ptrace(7, run = *pid, 1, 0);
				for (pid = want; pid < &want[TTYS]; pid++)
					*pid = pid[1];
			}
			else
			{
				while (*pid)
					pid++;
				*pid = i;
			}
		}
		else if (!--proc)
		{
			printf("p_master exit(0)\n");
			fflush(stdout);
			exit(0);
		}
	}
}
