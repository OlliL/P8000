##	shared	Dt
##	i2	menge;
##	i2	gewicht;
##	char	*tname;
amx(argc, argv)
int	argc;
char	**argv;
{
	tname = (argc == 1)? "*": argv[1];
##	open Teile
##	retrieve Teile $Tname == tname (menge = $Menge, gewicht = $Gewicht)
##	{
##		print ($Tname, $Tnum)
		printf("%12s:\t%d\n", "tot", menge * gewicht);
##	}
##	close Teile
	fflush(stdout);
}
