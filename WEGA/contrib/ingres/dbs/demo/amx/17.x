##	shared	Dt
##	char	*name;
##	i2	num;
##	i2	leiter;
amx(argc, argv)
int	argc;
char	**argv;
{
	name = (argc == 1)? "Unfug, Ulli": argv[1];
	num = (argc < 3)? 12: atoi(argv[2]);
	leiter = (argc < 4)? 157: atoi(argv[3]);
##	open Personal
##	use (exclusive Personal)
##	retrieve Personal $Name == name
##	{ replace ($Num = num, $Leiter = leiter) }
##	commit
##	close Personal
}
