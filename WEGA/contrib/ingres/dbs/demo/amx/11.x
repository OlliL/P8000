##	shared	Dt
##	i2	lnum;
amx(argc, argv)
int	argc;
char	**argv;
{
	lnum = (argc == 1)? 122: atoi(argv[1]);
##	open Lieferung
##	retrieve Lieferung $Lnum == lnum { print ($Tnum) }
##	close Lieferung
	fflush(stdout);
}
