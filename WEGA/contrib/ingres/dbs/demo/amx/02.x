/*
** Es sollen die Namen aller Angestellten recherchiert werden.
*/
##	shared	Dt
##	char	*name;
amx(argc, argv)
int	argc;
char	**argv;
{
	name = (argc == 1)? "*": argv[1];
##	open Personal
##	retrieve Personal $Name == name { print ($Name) }
##	close Personal
	fflush(stdout);
}
