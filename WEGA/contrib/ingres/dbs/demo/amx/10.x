##	shared	Dt
##	i2	tnum;
amx(argc, argv)
int	argc;
char	**argv;
{
	tnum = (argc == 1)? 10: atoi(argv[1]);
##	open Teile
##	retrieve Teile $Tnum <= tnum and $Tnum > 5 { print ($Tname, $Tnum) }
##	close Teile
	fflush(stdout);
}
