##	shared	Dt
##	char	*name;
##	i2	gehalt;
amx(argc, argv)
int	argc;
char	**argv;
{
	name = (argc == 1)? "Unfug, Ulli": argv[1];
	gehalt = (argc < 3)? 1000: atoi(argv[2]);
/*
**	Hier wird die Ausfuehrungsberechtigung des Programms
**	durch den aufrufenden Nutzer i.b.a. seine Zugehoerigkeit
**	zu der BS-Gruppe des Uebersetzers des AMX-Programms.
*/
##	permit group
##	open Personal
##	use (exclusive Personal)
##	append Personal ($Name = name, $Gehalt = gehalt)
##	commit
##	close Personal
	printf("Neuzugang: \"%s\" Gehalt %d,-M\n", name, gehalt);
	fflush(stdout);
}
