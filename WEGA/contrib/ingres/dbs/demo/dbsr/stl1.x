## shared Dbsr

   int		count;
   int		lev;
## char		ot[14];
## char		ut[14];
## char		name[31];
## char		me[3];

amx()
{
	long		old;
	long		new;
	char		buf[BUFSIZ];
	extern long	time();

   setbuf(stdout, buf);
   printf("DBS/R-DEMONSTRATIONSBEISPIEL:\n");
   printf("einstufige Strukturstueckliste\n");
   fflush(stdout);

## open Teile
## open Inf
   old = time((char *) 0);
## retrieve Teile $Kenn == "a" (ot = $T, name = $Name)
## {
	lev = 0;
	printf("\n*** einstufige STRUKTURLISTEN ***\n");
	printf("\nStueckliste %s %s\n", ot, name);
	printf("\n\tAufl.St.\t  Gegenst.Nr.                    Gegenst.Bez.  ME\n\n");
	fflush(stdout);
	stl_structure();
## }
## close Inf
## close Teile
   new = time((char *) 0);
   printf("\nEXECUTE %ld sec\n", new - old);
}

stl_structure()
{
   register int		i;

   lev++;
## retrieve Inf $Ot == ot (ut = $Ut)
## {
##	retrieve Teile $T == ut (name = $Name, me = $Me)
##	{
		putchar('\t');
		for (i = lev; i; --i)
			putchar('.');
		printf("%d\t\t", lev);
		printf("%13s  %30s  %2s\n", ut, name, me);
		fflush(stdout);
##	}
## }
   --lev;
}
