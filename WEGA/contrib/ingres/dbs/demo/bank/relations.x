## import "menue.x"

short		Batch;
short		Mahnung;

amx(argc, argv)
int	argc;
char	*argv[];
{
	char			stdbuf[BUFSIZ];

	Batch = atoi(argv[1]);
	setbuf(stdout, stdbuf);
	relopen();
	if (Batch)
		for ( ; ; )
			new_buchung();
	else
	{
		off_cursor();
		for ( ; ; )
			menue();
	}
}


relopen()
{
##	open	vorname
##	open	name
##	open	ort
##	open	str
##	open	bank
##	open	kunde
##	open	konto
##	open	grund
##	open	buchung
}


relclose()
{
##	close	vorname
##	close	name
##	close	ort
##	close	str
##	close	bank
##	close	kunde
##	close	konto
##	close	grund
##	close	buchung
}
