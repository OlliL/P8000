/*
** Es soll die Stellung aller Angestellten bestimmt werden.
**
** Die Stellung kann sein:
**	Direktor	Der Angestellte hat keinen Leiter.
**	Leiter		Der Angestellte hat einen Leiter und Unterstellte.
**	Angestellter	Der Angestellte hat keine Unterstellte.
**
** Gleichzeitig soll die Anzahl der Direktoren, Leiter und
** Angestellten gezaehlt werden.
*/
##	shared	Dt
##	i2	leiter;
##	char	name[21];
##	char	*name_arg;
amx(argc, argv)
int	argc;
char	**argv;
{
	register int		unterstellte;
	register int		direktor_zaehler;
	register int		leiter_zaehler;
	register int		angestellten_zaehler;
	name_arg = (argc == 1)? "*": argv[1];
	direktor_zaehler = leiter_zaehler = angestellten_zaehler = 0;
##	open Personal
##	retrieve Personal $Name == name_arg (name = $Name, leiter = $Leiter)
##	{
		printf("%-20s ", name);
		if (leiter == 0)
		{
/*
**			Wir haben einen Direktor gefunden !
*/
			printf("Direktor\n");
			direktor_zaehler++;
##			nextretrieve
		}
/*
**		Mit der Variable `unterstellte' soll bestimmt werden,
**		ob wir den eben gefundenen Angestellten auch als Leiter
**		bei einem beliebigen anderen Angestellten finden.
*/
		unterstellte = 0;
##		retrieve Personal $Leiter == $Num
##		{
/*
**			Ein gefundener Unterstellter reicht uns, wir
**			brechen die Suche nach weiteren ab !
*/
			unterstellte++;
##			endretrieve
##		}
		if (unterstellte)
		{
/*
**			Wir haben Unterstellte gefunden, als muss der
**			Angestellte ein Leiter sein, die Direktoren
**			haben wir ja schon abgehandelt.
*/
			printf("Leiter\n");
			leiter_zaehler++;
		}
		else
		{
/*
**			Die Angestellten bleiben uebrig.
*/
			printf("Angestellter\n");
			angestellten_zaehler++;
		}
##	}
##	close Personal
	printf("TOTAL:\t%3d\n", direktor_zaehler + leiter_zaehler + angestellten_zaehler);
	printf("\t%3d Direktoren\n", direktor_zaehler);
	printf("\t%3d Leiter\n", leiter_zaehler);
	printf("\t%3d Angestellte\n", angestellten_zaehler);
	fflush(stdout);
}
