## import "menue.x"

new_buchung()
{
	register int	i;
	register long	l;
	char		buf[36];
	extern long	atol();
	extern long	time();

	if (Batch)
	{
##		tuplecount konto konto
		konto = l = rand() % konto + 1;
		hash_konto(&konto);
		i = (rand() % l) * 7;
		if (l > i)
		{
			if (l > 500 || time((char *) 0) & 01)
				pr_auszug();
			else
				new_konto();
		}
		betrag = rand() % 1000 + 1;
		on_revers();
		printf("\n%d   Konto    ", Batch);
		off_revers();
		printf("%5ld\n", konto);
		printf("   Betrag    %5d\n", betrag);
	}
	else
	{
		while (!input(11, buf))
			pr_kunde();
		konto = atol(buf);
		input(12, buf);
		betrag = atoi(buf);
	}
	new_datum();
	new_grund();
	if (!plus)
		betrag = -betrag;
##	use (+buchung, +konto)
##	retrieve konto $konto == konto
##	(
##		saldo = $saldo
##	)
##	{
		saldo += betrag;
##		append buchung
##		(
##			$konto = konto,
##			$datum = datum,
##			$betrag = betrag,
##			$no_grund = no_grund
##		)
##		replace
##		(
##			$saldo = saldo,
##			$datum = datum
##		)
##	}
##	commit
	if (Batch && saldo < 0 && Mahnung++ == 10)
	{
		Mahnung = 0;
		mahnung();
	}
}
