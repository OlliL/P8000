/*
** Es sollen alle Angestellten und alle ihre Leiter recherchiert werden.
*/
##	shared	Dt
##	i2	leiter;
##	char	name[21];
##	char	*name_arg;
amx(argc, argv)
int	argc;
char	**argv;
{
	name_arg = (argc == 1)? "*": argv[1];
##	open Personal
##	retrieve Personal $Name == name_arg (name = $Name, leiter = $Leiter)
##	{
		suche_leiter();
##	}
##	close Personal
	fflush(stdout);
}
/*
** `suche_leiter' sucht rekursiv nach dem Leiter eines Angestellten,
** bis dieser der Direktor ist, das heisst, der Angestellte als
** Leiternummer eine `0' hat.
*/
suche_leiter()
{
	printf("%-20s ", name);
	if (leiter == 0)
	{
		putchar('\n');
		return;
	}
##	retrieve Personal $Num == leiter (name = $Name, leiter = $Leiter)
##	{
		suche_leiter();
##	}
}
