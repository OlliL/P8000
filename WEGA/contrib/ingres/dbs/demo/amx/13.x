##	shared	+1 Dt
##	char	*tname;
amx(argc, argv)
int	argc;
char	**argv;
{
	tname = (argc == 1)? "Zentraleinheit": argv[1];
##	open Teile
##	open Lieferung
##	use (shared Lieferung, shared Teile)
##	retrieve Teile $Tname == tname
##	{
##		retrieve Lieferung $Tnum == $Tnum { print ($Lnum) }
##	}
##	commit
##	close Teile
##	close Lieferung
	fflush(stdout);
}
