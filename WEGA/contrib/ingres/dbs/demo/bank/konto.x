## import "menue.x"

get_konto()
{
	register int	rc;

	rc = 1;
##	retrieve konto $konto == konto
##	(
##		konto = $konto,
##		saldo = $saldo,
##		datum = $datum,
##		no_bank = $no_bank
##	)
##	{
		rc = 0;
		break;
##	}
	return (rc);
}


new_konto()
{
	new_kunde();
	if (Batch)
	{
##		tuplecount bank no_bank
		no_bank = rand() % no_bank + 1;
		get_bank();
		printf("     Bank    %5d %s\n", no_bank, bank);
	}
	else
		new_bank();
	new_datum();
##	use (+konto)
##	append konto
##	(
##		$konto = konto,
##		$saldo = 0,
##		$datum = datum,
##		$no_bank = no_bank
##	)
##	commit
}
