## import "menue.x"

pr_ort()
{
	pr_init();
##	retrieve ort
##	(
##		plz = $plz,
##		ort = $ort
##	)
##	{
		if (pr_line())
			break;
		printf("%4d  %s", plz, ort);
##	}
	pr_end();
}


get_ort()
{
	register int	rc;

	rc = 1;
##	retrieve ort $no_ort == no_ort
##	(
##		plz = $plz,
##		ort = $ort
##	)
##	{
		rc = 0;
		break;
##	}
	return (rc);
}


new_ort()
{
	char		buf[36];

	while (!input(6, ort))
		pr_ort();
	input(7, buf);
	plz = atoi(buf);
	no_ort = 0;
##	use (+ort)
##	retrieve ort $plz == plz and $ort == ort
##	(
##		no_ort = $no_ort
##	)
##	{
		break;
##	}
	if (!no_ort)
	{
##		tuplecount ort no_ort
		no_ort++;
##		append ort
##		(
##			$no_ort = no_ort,
##			$plz = plz,
##			$ort = ort
##		)
	}
##	commit
}
