##	shared	+1 Dt
##	i2	lnum;
amx(argc, argv)
int	argc;
char	**argv;
{
	lnum = (argc == 1)? 122: atoi(argv[1]);
##	open Teile
##	open Lieferung
##	use (shared Lieferung, shared Teile)
##	retrieve Lieferung $Lnum == lnum
##	{
##		retrieve Teile $Tnum == $Tnum { print ($Tname) }
##	}
##	commit
##	close Teile
##	close Lieferung
	fflush(stdout);
}
