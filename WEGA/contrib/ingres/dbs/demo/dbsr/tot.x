## shared Dbsr

## int		lev;
## char		ot[14];
## char		ut[14];
## char		name[31];
## char		me[3];
## int		m;
## int		menge;
## int		meng;

   int		count;
   int		mengen[20];
   char		titname[31];
   char		titnr[13];

amx()
{
	long		old;
	long		new;
	char		buf[BUFSIZ];
	extern long	time();

   setbuf(stdout, buf);
   printf("DBS/R-DEMONSTRATIONSBEISPIEL:\n");
   printf("Strukturstueckliste fuer das Teil Nr. 10 (20 Stueck)\n");
   fflush(stdout);

## open Total
## open Teile
## open Inf
   old = time((char *) 0);

## use (exclusive Total, shared Teile, shared Inf)
## retrieve Total { delete }
## retrieve Teile $T == "0000000000010" (ot = $T, name = $Name, me = $Me)
## {
	lev = 0;
	menge = 20;
	mengen[lev] = 20; 
	printf("\nStueckliste %-s %-s\n", ot, name);
	printf("\n\tAufl.St.\t  Gegenst.Nr.  Gegenst.Bez.\t\t       ME  Menge\n");
	fflush(stdout);
	AAsmove(ot, ut);
	AAsmove(ut, titnr);
	AAsmove(name, titname);
	app();
	AAsmove(ut, ot),

	stl_structure();
## }
   printtot();
## commit
## close Inf 
## close Total
## close Teile
   new = time((char *) 0);
   printf("\nEXECUTE %ld sec\n", new - old);
   fflush(stdout);
}


stl_structure()
{
	register int	i;

   lev++;
## retrieve Inf $Ot == ot (ut = $Ut, m = $M)
## {
	menge = mengen[--lev] * m;
	mengen[++lev] = menge;
##	retrieve Teile $T == ut (name = $Name, me = $Me)
##	{
		putchar('\t');
		for (i = lev; i; --i)
			putchar('.');
		printf("%d\t\t", lev);
		printf("%13s  %-30s  %-2s %5d\n", ut, name, me, menge);
		fflush(stdout);
		app();
##	}
	AAsmove(ut, ot);
	stl_structure();
## }
   --lev;
}


app()
{
	register int	count;
	register int	i;

   count = 0;
   i++;
## retrieve Total $T == ut (meng = $Menge)
## {
	count++;
	menge += meng;
##	replace ($Menge = menge)	
## }
   if (!count)
##	append Total
##	($Lev = lev, $T = ut, $Name = name, $Menge = menge, $Me = me)
}


printtot()
{
   printf("\n\tDBS/R-DEMONSTRATIONSBEISPIEL:\n");
   printf("\tTotale Stueckliste fuer das Teil Nr. 10 (20 Stueck)\n");
   printf("\n\tStueckliste %-s %-s\n", titnr, titname);
   printf("\n\t Gegenst.Nr.   Gegenst.Bez.                  ME  Menge\n");
   fflush(stdout);
## retrieve Total $T != "0000000000010"
## (ut = $T, name = $Name, menge = $Menge, me = $Me)
## { 
	printf("\t%13s %-30s %-2s %5d\n", ut, name, me, menge);
	fflush(stdout);
## }
}
