## shared Dbsr

   int		count;
## char		ot[14];

amx()
{
	register int	i;
	long		old;
	long		new;
	char		buf[BUFSIZ];
	extern long	time();

   setbuf(stdout, buf);
   printf("DBS/R-DEMONSTRATIONSBEISPIEL:\n");
   printf("Konsistenzcheck Inf.Ot --> Teile.T\n\n");
   fflush(stdout);

## open Teile
## open Inf
   old = time((char *) 0);
## retrieve Inf (ot = $Ot)
## {
	count++;
	i = 0;
##	retrieve Teile $T == $Ot
##	{
		i++;
##	}
	if (!i)
	{
		printf("ot = %s LOST\n", ot);
		fflush(stdout);
	}
	if (i != 1)
	{
		printf("ot = %s has %d entries\n", ot, i);
		fflush(stdout);
	}
## }
   new = time((char *) 0);
   printf("\nEXECUTE %ld sec\n", new - old);
   printf("TUPLECOUNT %d\n", count);
   fflush(stdout);
## close Inf
## close Teile
}
