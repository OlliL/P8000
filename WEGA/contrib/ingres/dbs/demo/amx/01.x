/*
** Es sollen alle Angaben zu allen Angestellten recherchiert werden.
**
**
** Datenbank `dt' mit dem C-Programm verbinden
*/
##	shared	Dt
/*
** Die C-Variable name wird als Austauschvariable zwischen
** dem C-Programm und der Datenbank deklariert
*/
##	char	*name;
/*
** Das Hauptprogramm muss `amx' und nicht `main' heissen !
*/
amx(argc, argv)
int	argc;
char	**argv;
{
/*
**	Wenn das Programm mit einem Argument gerufen wird,
**	wird das Argument als Bedingung fuer die auszuwaehlende
**	Saetze genommen, ansonsten `*', das heisst alle.
*/
	name = (argc == 1)? "*": argv[1];
/*
**	Relation `personal' fuer die Benutzung eroeffnen.
*/
##	open Personal
/*
**	Hole alle Saetze aus der Relation `personal', fuer die
**	das Feld `name' den Wert der C-Variablen `name' hat.
*/
##	retrieve Personal $Name == name
##	{
/*
**		Es folgt die Aktion, die fuer jeden entsprechend der
**		Bedingung gefunden Satz ausgefuert wird, hier das
**		Ausgeben des vollstaendigen Satzes.
*/
		printf("------------\n");
##		print ()
##	}
/*
**	Schliessen der  Relation `personal'.
*/
##	close Personal
	printf("------------\n");
	fflush(stdout);
}
