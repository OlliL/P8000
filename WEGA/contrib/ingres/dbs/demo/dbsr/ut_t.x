## shared Dbsr

   int		count;
## char		ut[14];

amx()
{
	register int	i;
	long		old;
	long		new;
	char		buf[BUFSIZ];
	extern long	time();

   setbuf(stdout, buf);
   printf("DBS/R-DEMONSTRATIONSBEISPIEL:\n");
   printf("Konsistenzcheck Inf.Ut --> Teile.T\n\n");
   fflush(stdout);

## open Teile
## open Inf
   old = time((char *) 0);
## retrieve Inf (ut = $Ut)
## {
	count++;
	i = 0;
##	retrieve Teile $T == $Ut
##	{
		i++;
##	}
	if (!i)
	{
		printf("ut = %s LOST\n", ut);
		fflush(stdout);
	}
	if (i != 1)
	{
		printf("ut = %s has %d entries\n", ut, i);
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
