##	shared	Dt
##	char	*name;
amx(argc, argv)
int	argc;
char	**argv;
{
	register int		c;
	name = (argc == 1)? "*": argv[1];
##	open Personal
##	use (exclusive Personal)
##	retrieve Personal $Name == name
##	{
##		print ()
		printf("STREICHEN   ?\t");
		fflush(stdout);
		if ((c = getchar()) == 'y')
##			delete
		while (c && c != '\n')
			c = getchar();
##	}
##	commit
##	close Personal
	fflush(stdout);
}
