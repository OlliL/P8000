## shared Dbsr

   int		count;
## char		kenn[2];
## char		t[31];
## i2		m;

amx()
{
	register int	cnt;
	long		old;
	long		new;
	char		buf[BUFSIZ];
	extern long	time();

   setbuf(stdout, buf);
   printf("DBS/R-DEMONSTRATIONSBEISPIEL:\n");
   printf("Ermittlung aller Endprodukte\n\n");
   fflush(stdout);

## open Teile
## open Inf
   old = time((char *) 0);
## retrieve Teile (kenn = $Kenn, t = $T, m = $M)
## {
	cnt = 0;
##	retrieve Inf $Ut == $T
##	{
		cnt++;
		break;
##	}
	if (cnt)
		continue;
	printf("%s %s %4d\n", kenn, t, m);
	fflush(stdout);
	count++;
## }
   new = time((char *) 0);
   printf("\nEXECUTE %ld sec\n", new - old);
   printf("TUPLECOUNT %d\n", count);
   fflush(stdout);
## close Inf
## close Teile
}
