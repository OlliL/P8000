# include	<stdio.h>

# define	PR_BGN		15
# define	PR_END		25

static int	print;
static int	abort;

pr_init()
{
	register int	i;

	abort = 0;
	for (i = PR_BGN; i < PR_END; i++)
		cls(i, 1, i, 47);
	cursor(print = PR_BGN, 1);
	on_bold();
}

pr_line()
{
	register int	i;

	if (print == PR_END)
	{
		print = PR_BGN;
		if (go_on())
		{
			abort = 1;
			return (1);
		}
		for (i = PR_BGN; i < PR_END; i++)
			cls(i, 1, i, 47);
	}
	cursor(print, 1);
	print++;
	return (0);
}

pr_end()
{
	if (!abort)
		go_on();
	off_bold();
	cls(24, 48, 24, 80);
}

static go_on()
{
	register int	c;

	cursor(24, 48);
	on_revers();
	printf("continue<eof>/abort<enter>");
	off_revers();
	printf("  ");
	fflush(stdout);
	while ((c = getchar()) > 0 && c != '\n')
		continue;
	return (c > 0);
}
