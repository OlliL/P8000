# include	<stdio.h>

# define	SIG		15

char		c;

catch()
{
	putchar(c);
	fflush(stdout);
}

main(argc, argv)
int	argc;
char	*argv[];
{
	register int		i;
	register int		pid;
	char			buf[BUFSIZ];

	setbuf(stdout, buf);
	c = *argv[1];
	pid = getpid();
	printf("\nstart %c of child %d\n", c, pid);
	fflush(stdout);

	if (ptrace(0) < 0)
	{
		printf("child %d dies via ptrace\n", pid);
		fflush(stdout);
		exit(1);
	}

	if (signal(SIG, catch) < 0)
	{
		printf("child %d dies via signal\n", pid);
		fflush(stdout);
		exit(2);
	}

	for (i = 0; ; )
	{
		kill(pid, SIG);
		printf("\ngo %c | rc = %d\n", c, i);
		fflush(stdout);
		for (i = 1; i < 2000; i++)
			c = c;
		kill(pid, SIG);
	}
}
