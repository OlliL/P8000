/*
** Wir moechten die Betriebsstruktur (Unterstellungsverhaeltnisse)
** recherchieren.
*/
##	shared	Dt
##	i2	leiter;
##	char	name[21];
	int	niveau;
amx(argc, argv)
int	argc;
char	**argv;
{
/*
**	Wird kein Argument gegeben, liefert `atoi()' den Wert `0'.
*/
	leiter = atoi(argv[1]);
##	open Personal
/*
**	Ausgangspunkt der Betriebsstruktur ist der Direktor, also die
**	Leiternummer `0'.
*/
##	retrieve Personal $Leiter == leiter (name = $Name, leiter = $Num)
##	{
		suche_mitarbeiter();
##	}
##	close Personal
	fflush(stdout);
}
/*
** `suche_mitarbeiter' sucht rekursiv alle Unterstellten eines Angestellten.
*/
suche_mitarbeiter()
{
	register int		i;
/*
**	Die C-Variable `niveau' verwaltet die Unterstellungstiefe,
**	un optische Einrueckungen vornehmen zu koennen.
*/
	niveau++;
	for (i = niveau - 1; i; --i)
		putchar('\t');
	printf("%-20s %4d\n", name, leiter);
##	retrieve Personal $Leiter == leiter (name = $Name, leiter = $Num)
##	{
		suche_mitarbeiter();
##	}
	--niveau;
}
