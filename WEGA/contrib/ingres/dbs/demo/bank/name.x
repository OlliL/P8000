## import "menue.x"

pr_name()
{
	pr_init();
##	retrieve name
##	(
##		name = $name
##	)
##	{
		if (pr_line())
			break;
		printf("%s", name);
##	}
	pr_end();
}


get_name()
{
	register int	rc;

	rc = 1;
##	retrieve name $no_name == no_name
##	(
##		name = $name
##	)
##	{
		rc = 0;
		break;
##	}
	return (rc);
}


new_name()
{
	while (!input(4, name))
		pr_name();
	no_name = 0;
##	use (+name)
##	retrieve name $name == name
##	(
##		no_name = $no_name
##	)
##	{
		break;
##	}
	if (!no_name)
	{
##		tuplecount name no_name
		no_name++;
##		append name
##		(
##			$no_name = no_name,
##			$name = name
##		)
	}
##	commit
}
