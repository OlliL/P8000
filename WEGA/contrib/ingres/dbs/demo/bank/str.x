## import "menue.x"

pr_str()
{
	pr_init();
##	retrieve str
##	(
##		str = $str
##	)
##	{
		if (pr_line())
			break;
		printf("%s", str);
##	}
	pr_end();
}


get_str()
{
	register int	rc;

	rc = 1;
##	retrieve str $no_str == no_str
##	(
##		str = $str
##	)
##	{
		rc = 0;
		break;
##	}
	return (rc);
}


new_str()
{
	while (!input(5, str))
		pr_str();
	no_str = 0;
##	use (+str)
##	retrieve str $str == str
##	(
##		no_str = $no_str
##	)
##	{
		break;
##	}
	if (!no_str)
	{
##		tuplecount str no_str
		no_str++;
##		append str
##		(
##			$no_str = no_str,
##			$str = str
##		)
	}
##	commit
}
