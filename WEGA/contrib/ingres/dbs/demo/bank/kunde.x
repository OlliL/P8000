## import "menue.x"

pr_kunde()
{
	pr_init();
##	retrieve kunde
##	(
##		no_name = $no_name,
##		no_vorname = $no_vorname,
##		konto = $konto
##	)
##	{
		if (pr_line())
			break;
		get_name();
		get_vorname();
		printf("%-12.s %-12.s %ld", name, vorname, konto);
##	}
	pr_end();
}


get_kunde()
{
	register int	rc;

	rc = 1;
##	retrieve kunde $konto == konto
##	(
##		no_name = $no_name,
##		no_vorname = $no_vorname,
##		no_str = $no_str,
##		no_ort = $no_ort
##	)
##	{
		rc = 0;
		break;
##	}
	return (rc);
}


new_kunde()
{
	if (Batch)
	{
##		tuplecount name no_name
		no_name = rand() % no_name + 1;
		get_name();
		on_revers();
		printf("\n%d    Name    ", Batch);
		off_revers();
		printf("%5d %s\n", no_name, name);
##		tuplecount vorname no_vorname
		no_vorname = rand() % no_vorname + 1;
		get_vorname();
		printf("  Vorname    %5d %s\n", no_vorname, vorname);
##		tuplecount str no_str
		no_str = rand() % no_str + 1;
		get_str();
		printf("  Strasse    %5d %s\n", no_str, str);
##		tuplecount ort no_ort
		no_ort = rand() % no_ort + 1;
		get_ort();
		printf("      PLZ    %5d\n", plz);
		printf("      Ort    %5d %s\n", no_ort, ort);
	}
	else
	{
		new_name();
		new_vorname();
		new_str();
		new_ort();
	}
##	use (+kunde)
##	tuplecount kunde konto
	konto++;
	hash_konto(&konto);
##	append kunde
##	(
##		$konto = konto,
##		$no_name = no_name,
##		$no_vorname = no_vorname,
##		$no_str = no_str,
##		$no_ort = no_ort
##	)
##	commit
	if (Batch)
		printf("    Konto    %5ld\n", konto);
}
