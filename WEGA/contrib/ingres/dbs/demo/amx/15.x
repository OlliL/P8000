##	shared	Dt
##	i2	leiter;
amx(argc, argv)
int	argc;
char	**argv;
{
	leiter = (argc == 1)? 129: atoi(argv[1]);
##	open Personal
##	use (exclusive Personal)
##	retrieve Personal $Leiter == leiter { print ($Name); delete }
##	commit
##	close Personal
}
