## import "menue.x"

# define	MAX_AUSZUG		20

struct auszug
{
	long	a_datum;
	short	a_betrag;
	short	a_no_grund;
};

auszug()
{
	char			buf[36];

	while (!input(11, buf))
		pr_kunde();
	konto = atol(buf);
	pr_auszug();
}


auszuege()
{
	register int	cnt;

	for ( ; ; )
	{
		cnt = 0;
##		retrieve buchung
##		(
##			konto = $konto
##		)
##		{
			cnt++;
			break;
##		}
		if (!cnt)
			return;
		pr_auszug();
	}
}


pr_auszug()
{
	register struct auszug	**ap;
	register struct auszug	*a;
	register int		i;
	register int		j;
	struct auszug		abuf[MAX_AUSZUG];
	struct auszug		*apbuf[MAX_AUSZUG];
	extern int		cmpa();
	extern char		*ctime();

	if (get_konto())
	{
		error("ungueltige Kontonummer");
		return;
	}
	get_bank();
	get_kunde();
	get_name();
	get_vorname();
	get_str();
	get_ort();
	cls(1, 1, 25, 80);
	on_revers();
	cursor(1, 1);
	for (i = 0; i < 80; i++)
		putchar(' ');
	cursor(2, 1);
	for (i = 0; i < 80; i++)
		putchar(' ');
	cursor(1, 1);
	printf("KONTOAUSZUG #%ld# %s %s", konto, bank, ctime(&datum));
	cursor(2, 1);
	printf("NEUER SALDO  %ld", saldo);
	off_revers();
	cursor(3, 1);
	on_bold();
	printf("%s, %s aus %s %s, %d", name, vorname, str, ort, plz);
	off_bold();
	i = 0;
	a = abuf;
	ap = apbuf;
##	use (+buchung)
##	retrieve buchung $konto == konto
##	(
##		datum = $datum,
##		betrag = $betrag,
##		no_grund = $no_grund
##	)
##	{
		a->a_datum = datum;
		a->a_betrag = betrag;
		a->a_no_grund = no_grund;
		*ap++ = a++;
##		delete
		if (i++ == MAX_AUSZUG)
			break;
##	}
##	commit
	if (!i)
	{
		error("keine Kontoauszuege vorhanden");
		goto done;
	}
	qsort(apbuf, i, sizeof (struct auszug *), cmpa);
	on_revers();
	cursor(4, 1);
	for (j = 0; j < 80; j++)
		putchar(' ');
	cursor(4, 1);
	printf("UMSAETZE  %-35s  BUCHUNGSDATUM", "BUCHUNGSTEXT");
	off_revers();
	cursor(5, 1);
	ap = apbuf;
	while (i--)
	{
		a = *ap++;
		no_grund = a->a_no_grund;
		get_grund();
		if (!plus)
			on_blinking();
		printf("%8d", a->a_betrag);
		if (!plus)
			off_blinking();
		printf("  %-35s  %s", grund, ctime(&a->a_datum));
		a++;
	}
done:
	if (Batch)
		return;
	cursor(2, 60);
	on_revers();
	on_blinking();
	printf("continue<enter>");
	off_blinking();
	off_revers();
	fflush(stdout);
	while ((i = getchar()) > 0 && i != '\n')
		continue;
}


cmpa(a, b)
register struct auszug	*a;
register struct auszug	*b;
{
	if (a->a_datum == b->a_datum)
		return (0);
	return (a->a_datum < b->a_datum);
}
