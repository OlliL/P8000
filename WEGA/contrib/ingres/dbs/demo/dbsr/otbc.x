## shared Dbsr

   int		count;
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
   printf("Ermittlung aller Endprodukte die keine Unterteile haben\n\n");
   fflush(stdout);

## open Teile
## open Inf
   old = time((char *) 0);
## retrieve Teile $Kenn != "a" (t = $T, m = $M)
## {
	cnt = 0;
##	retrieve Inf $Ut == $T
##	{
		cnt++;
		break;
##	}
	if (cnt)
		continue;
	printf("%s %4d\n", t, m);
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
