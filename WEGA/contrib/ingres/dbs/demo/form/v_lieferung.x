## shared dt

# define	NF		((int (*)()) 0)		/* null function */
# define	N		16

# define	SATZ		 4
   int		satz;

/* vars of relation lieferung */
# define	BNUM		 4
## i2		bnum;

# define	TNUM		 4
## i2		tnum;

# define	LNUM		 4
## i2		lnum;

# define	LIEFDAT		  8
## char		liefdat[9];

# define	MENGE		 5
## i2		menge;


f_retrieve(frame)
register char		*frame;
{
	register int		cnt;

   MMerror(" RECHERCHE LAEUFT ");
## use (shared lieferung)
   satz = cnt = 0;
## retrieve lieferung
## (
## 	bnum = $bnum,
## 	tnum = $tnum,
## 	lnum = $lnum,
## 	liefdat = $liefdat,
## 	menge = $menge
## )
## {
	if (cnt == N)
	{
		if (MMnext())
##			endretrieve
		cnt = 0;
	}
	satz++;
	MMset_value(frame, &satz, cnt);
	MMset_value(frame, &bnum, cnt);
	MMset_value(frame, &tnum, cnt);
	MMset_value(frame, &lnum, cnt);
	MMset_value(frame, liefdat, cnt);
	MMset_value(frame, &menge, cnt);
	if (++cnt == N)
	{
		MMvalue(frame);
		MMclr_value(frame);
	}
## }
## commit

   MMerror(" RECHERCHE BEENDET ");
   if (cnt < N)
   {
	MMvalue(frame);
	MMclr_value(frame);
   }
}

amx(argc, argv)
int	argc;
char	*argv[];
{
	register int		i;
	register char		*form;
	char			*frame;
	extern char		*MMalloc();

   MMinit_form(1);

   frame = (char *) 0;
   form = MMalloc(&frame, 2, 1, 71, (char *) 0);
   MMout_field(form, "----------------------------------------------------- L I E F E R U N G");

   form = MMalloc(&frame, 4, 1, 4, (char *) 0);
   MMout_field(form, "SATZ");

   form = MMalloc(&frame, 4, 7, 4, (char *) 0);
   MMout_field(form, "BNUM");

   form = MMalloc(&frame, 4, 13, 4, (char *) 0);
   MMout_field(form, "TNUM");

   form = MMalloc(&frame, 4, 19, 4, (char *) 0);
   MMout_field(form, "LNUM");

   form = MMalloc(&frame, 4, 26, 7, (char *) 0);
   MMout_field(form, "LIEFDAT");

   form = MMalloc(&frame, 4, 35, 5, (char *) 0);
   MMout_field(form, "MENGE");

   form = MMalloc(&frame, 5, 7, 33, (char *) 0);
   MMout_field(form, "---------------------------------");

   for (i = 6; i < 6 + N; i++)
   {
	form = MMalloc(&frame, i, 1, SATZ, (char *) 0);
	MMval_field(form, NF);
	MMvari2(form, &satz, "%4d");

	form = MMalloc(&frame, i, 7, BNUM, (char *) 0);
	MMval_field(form, NF);
	MMvari2(form, &bnum, "%4d");

	form = MMalloc(&frame, i, 13, TNUM, (char *) 0);
	MMval_field(form, NF);
	MMvari2(form, &tnum, "%4d");

	form = MMalloc(&frame, i, 19, LNUM, (char *) 0);
	MMval_field(form, NF);
	MMvari2(form, &lnum, "%4d");

	form = MMalloc(&frame, i, 25, LIEFDAT, (char *) 0);
	MMval_field(form, NF);
	MMvarc(form, liefdat, "%8s");

	form = MMalloc(&frame, i, 35, MENGE, (char *) 0);
	MMval_field(form, NF);
	MMvari2(form, &menge, "%5d");
   }

   form = MMalloc(&frame, i, 7, 33, (char *) 0);
   MMout_field(form, "---------------------------------");

   form = MMalloc(&frame, 24, 2, 6, (char *) 0);
   MMout_field(form, "SAETZE");

   form = MMalloc(&frame, 24, 13, 10, (char *) 0);
   MMmen_field(form, f_retrieve, NF, " ANZEIGEN ");

## open lieferung

   MMform(frame);

## close lieferung

   MMfree(&frame);
   MMexit_form(1);
   return (0);
}
