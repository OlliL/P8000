##	shared	Dt
##	i2	tnum;
##	i2	tnum_arg;
amx(argc, argv)
int	argc;
char	**argv;
{
	tnum_arg = (argc == 1)? 10: atoi(argv[1]);
##	open Teile
##	retrieve Teile $Tnum <= tnum_arg and $Tnum >= 2 (tnum = $Tnum)
##	{
		if (tnum == 2 || tnum == tnum_arg)
##			print ($Tname, $Tnum)
##	}
##	close Teile
	fflush(stdout);
}
