##	shared	Dt
##	f8	gehalt;
##	char	*name;
amx(argc, argv)
int	argc;
char	**argv;
{
	name = (argc == 1)? "*": argv[1];
/*
**	Hier wird ueberprueft, ob der Ausfuehrende des Programms
**	auch der Uebersetzende war.
*/
##	permit user
##	open Personal
##	use (exclusive Personal)
##	retrieve Personal $Name == name (gehalt = $Gehalt)
##	{
		gehalt *= 1.1;
##		replace ($Gehalt = gehalt)
##	}
##	commit
##	close Personal
}
