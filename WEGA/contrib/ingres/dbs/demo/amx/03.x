/*
** Es sollen die Namen aller Angestellten und ihre Leiter recherchiert werden.
**
** Jeder Angestellte besitzt eine Angestelltennummer `num' und einen
** Leiter `leiter'. Die spezielle Leiternummer `0' bedeuted, dass der
** Angestellte keinen Leiter besitzt, ansonsten verweist die Leiternummer
** `leiter' auf einen Angestellten, der diese Nummer als Angestellten-
** nummer `num' hat.
*/
##	shared	Dt
##	char	name[21];
##	i2	leiter;
##	char	*name_arg;
amx(argc, argv)
int	argc;
char	**argv;
{
	name_arg = (argc == 1)? "*": argv[1];
##	open Personal
/*
**	Das Feld `name' wird fuer jeden entsprechend der Bedingung gefundenen
**	Datensatz in die C-Variable `name' geladen, ebenso das Feld `leiter'
**	in die C-Variable `leiter'. Anschliessend wird die Aktion ausgefuehrt.
*/
##	retrieve Personal $Name == name_arg (name = $Name, leiter = $Leiter)
##	{
		printf("Angestellter %-20s ", name);
		if (leiter == 0)
		{
/*
**			Wenn die Leiternummer `0' ist, muss es ein
**			Direktor sein und ich finde keinen Leiter
**			fuer ihn.
*/
			printf("Direktor\n");
##			nextretrieve
		}
/*
**		Ich finde den Leiter des gerade gefundenen Angestellten,
**		indem ich die Nummer seines Leiters als Bedingung fuer
**		eine neue Recherche in der Relation `personal' nehme.
**		Gesucht wird der Angestellte der als Nummer die Nummer
**		hat, die der gefundene Angestellte als Leiternummer
**		hatte.
*/
##		retrieve Personal $Num == $Leiter (name = $Name)
##		{
			printf("Leiter %s\n", name);
##		}
##	}
##	close Personal
	fflush(stdout);
}
