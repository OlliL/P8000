## import "menue.x"

# define	EINZAHLUNG		1000

mahnung()
{
	register int		i;
	extern char		*ctime();

	new_datum();
	box(1, 1, 25, 80);
	on_revers();
	on_blinking();
	cursor(1, 1);
	cursor(1, 10);
	printf("MAHNUNGEN     per     %s", ctime(&datum));
	off_blinking();
	off_revers();
	i = 2;
##	use(+buchung, +konto, vorname, name, str, ort, kunde, bank)
##	retrieve konto $saldo < 0
##	(
##		konto = $konto,
##		saldo = $saldo,
##		no_bank = $no_bank
##	)
##	{
		get_bank();
		get_kunde();
		get_name();
		get_vorname();
		get_str();
		get_ort();
		cls(i, 2, i, 79);
		cls(i + 1, 2, i + 1, 79);
		cursor(i, 3);
		printf("%s %ld", bank, saldo);
		cursor(i + 1, 8);
		printf("%s, %s aus %s %s, %d", name, vorname, str, ort, plz);
		betrag = EINZAHLUNG;
##		append buchung
##		(
##			$konto = konto,
##			$datum = datum,
##			$betrag = betrag,
##			$no_grund = 83
##		)
		saldo += betrag;
##		replace
##		(
##			$saldo = saldo,
##			$datum = datum
##		)
		i += 2;
		if (i == 24)
			i = 2;
##	}
##	commit
}
