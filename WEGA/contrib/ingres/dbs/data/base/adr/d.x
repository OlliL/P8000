## shared adr

## char		KURZ[51];
## short	STD;
## short	TAG;
## short	MON;
## short	JAHR;
## char		DBS[2];
## char		PQR[2];
## char		AMX[2];

amx()
{
##	open dis
	fprintf(stderr, "DRUCK dis\n\n");

##	retrieve dis
##	(
##		KURZ = $kurz,
## 		STD = $std,
## 		TAG = $tag,
## 		MON = $mon,
## 		JAHR = $jahr,
## 		DBS = $dbs,
## 		PQR = $pqr,
## 		AMX = $amx
##	)
##	{
		printf("%-50s: %2d.%2d.%4d %2d Uhr", KURZ, TAG, MON, JAHR, STD);
		if (*DBS == 'y')
			printf(" I");
		if (*PQR == 'y')
			printf(" P");
		if (*AMX == 'y')
			printf(" A");
		putchar('\n');
##	}
##	close dis
}
