## import "menue.x"

pr_bank()
{
	pr_init();
##	retrieve bank
##	(
##		bank = $bank
##	)
##	{
		if (pr_line())
			break;
		printf("%s", bank);
##	}
	pr_end();
}


get_bank()
{
	register int	rc;

	rc = 1;
##	retrieve bank $no_bank == no_bank
##	(
##		no_str = $no_str,
##		no_ort = $no_ort,
##		bank = $bank
##	)
##	{
		rc = 0;
		break;
##	}
	return (rc);
}


new_bank()
{
	while (!input(10, bank))
		pr_bank();
	no_bank = 0;
##	use (+bank)
##	retrieve bank $bank == bank
##	(
##		no_bank = $no_bank,
##		no_str = $no_str,
##		no_ort = $no_ort
##	)
##	{
		break;
##	}
	if (!no_bank)
	{
##		tuplecount bank no_bank
		no_bank++;
##		commit
		new_str();
		new_ort();
##		use (+bank)
##		append bank
##		(
##			$no_bank = no_bank,
##			$no_str = no_str,
##			$no_ort = no_ort,
##			$bank = bank
##		)
	}
##	commit
}
