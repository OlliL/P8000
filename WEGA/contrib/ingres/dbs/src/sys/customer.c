# include	<stdio.h>

main(argc, argv)
int	argc;
char	*argv[];
{
	register FILE		*fd;


	if ((fd = fopen("sys/etc/customer", "w")) == (FILE *) 0)
	{
		printf("can't create sys/etc/customer\n");
		return (1);
	}
	fprintf(fd, "%s", argv[1]);
	putc(0, fd);
	return (0);
}
