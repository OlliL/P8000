## import "menue.x"

pr_vorname()
{
	pr_init();
##	retrieve vorname
##	(
##		vorname = $vorname
##	)
##	{
		if (pr_line())
			break;
		printf("%s", vorname);
##	}
	pr_end();
}


get_vorname()
{
	register int	rc;

	rc = 1;
##	retrieve vorname $no_vorname == no_vorname
##	(
##		vorname = $vorname
##	)
##	{
		rc = 0;
		break;
##	}
	return (rc);
}


new_vorname()
{
	while (!input(3, vorname))
		pr_vorname();
	no_vorname = 0;
##	use (+vorname)
##	retrieve vorname $vorname == vorname
##	(
##		no_vorname = $no_vorname
##	)
##	{
		break;
##	}
	if (!no_vorname)
	{
##		tuplecount vorname no_vorname
		no_vorname++;
##		append vorname
##		(
##			$no_vorname = no_vorname,
##			$vorname = vorname
##		)
	}
##	commit
}
