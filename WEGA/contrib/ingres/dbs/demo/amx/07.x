##	shared	Dt
##	char	*farbe;
amx(argc, argv)
int	argc;
char	**argv;
{
	farbe = (argc == 1)? "grau": argv[1];
##	open Teile
##	retrieve Teile $Farbe == farbe { print ($Tname, $Farbe) }
##	close Teile
	fflush(stdout);
}
