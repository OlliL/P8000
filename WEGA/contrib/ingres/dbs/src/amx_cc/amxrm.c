# include	<stdio.h>

/*
**  CALL BS RM
**
**	The BS rm is called with DBS permission.
**	Only files near the current directory can be removed !!!
**	Perhaps: my/dir/file, ../other_dir/file or /tmp/file
*/
main(argc, argv)
int	argc;
char	**argv;
{
	register char		**av;
	register char		*arg;
	register int		a;
	register int		slash;
	register int		ok;

	ok = 1;
	for (av = &argv[1]; arg = *av; av++)
	{
		if (*arg == '-' && arg[1] == 'r')
		{
			printf("amxrm: illegal flag %s\n", *av);
			ok = 0;
			break;
		}
		slash = 0;
		while (a = *arg++)
		{
			if (a == '/')
				slash++;
		}
		if (slash > 2)
		{
			printf("amxrm: illegal file %s\n", *av);
			ok = 0;
		}
	}
	if (ok)
		execvp("rm", argv);
	return (-1);
}
