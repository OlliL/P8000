## import "menue.x"

pr_grund()
{
	pr_init();
##	retrieve grund
##	(
##		plus = $plus,
##		grund = $grund
##	)
##	{
		if (pr_line())
			break;
		printf("%c%s", plus? ' ': '-', grund);
##	}
	pr_end();
}


get_grund()
{
	register int	rc;

	rc = 1;
##	retrieve grund $no_grund == no_grund
##	(
##		plus = $plus,
##		grund = $grund
##	)
##	{
		rc = 0;
		break;
##	}
	return (rc);
}


new_grund()
{
	char		buf[36];

	if (Batch)
	{
##		tuplecount grund no_grund
		no_grund = rand() % no_grund + 1;
		get_grund();
		printf("    Grund    %c%s\n", plus? ' ': '-', grund);
		return;
	}
	while (!input(8, grund))
		pr_grund();
	no_grund = 0;
##	use (+grund)
##	retrieve grund $grund == grund
##	(
##		no_grund = $no_grund,
##		plus = $plus
##	)
##	{
		break;
##	}
	if (!no_grund)
	{
##		tuplecount grund no_grund
		no_grund++;
##		commit
		input(9, buf);
		if (plus = atoi(buf))
			plus = 1;
##		use (+grund)
##		append grund
##		(
##			$no_grund = no_grund,
##			$plus = plus,
##			$grund = grund
##		)
	}
##	commit
}
