## shared dt

# define	NF		((int (*)()) 0)		/* null function */
# define	N		16

# define	SATZ		 4
   int		satz;

/* vars of relation teile */
# define	TNUM		 4
## i2		tnum;

# define	TNAME		20
## char		tname[21];

# define	FARBE		 8
## char		farbe[9];

# define	GEWICHT		 7
## i2		gewicht;

# define	MENGE		 5
## i2		menge;


f_retrieve(frame)
register char		*frame;
{
	register int		cnt;

   MMerror(" RECHERCHE LAEUFT ");
## use (shared teile)
   satz = cnt = 0;
## retrieve teile
## (
## 	tnum = $tnum,
## 	tname = $tname,
## 	farbe = $farbe,
## 	gewicht = $gewicht,
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
	MMset_value(frame, &tnum, cnt);
	MMset_value(frame, tname, cnt);
	MMset_value(frame, farbe, cnt);
	MMset_value(frame, &gewicht, cnt);
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
   MMout_field(form, "------------------------------------------------------------- T E I L E");

   form = MMalloc(&frame, 4, 1, 4, (char *) 0);
   MMout_field(form, "SATZ");

   form = MMalloc(&frame, 4, 7, 4, (char *) 0);
   MMout_field(form, "TNUM");

   form = MMalloc(&frame, 4, 28, 5, (char *) 0);
   MMout_field(form, "TNAME");

   form = MMalloc(&frame, 4, 38, 5, (char *) 0);
   MMout_field(form, "FARBE");

   form = MMalloc(&frame, 4, 45, 7, (char *) 0);
   MMout_field(form, "GEWICHT");

   form = MMalloc(&frame, 4, 54, 5, (char *) 0);
   MMout_field(form, "MENGE");

   form = MMalloc(&frame, 5, 7, 52, (char *) 0);
   MMout_field(form, "----------------------------------------------------");

   for (i = 6; i < 6 + N; i++)
   {
	form = MMalloc(&frame, i, 1, SATZ, (char *) 0);
	MMval_field(form, NF);
	MMvari2(form, &satz, "%4d");

	form = MMalloc(&frame, i, 7, TNUM, (char *) 0);
	MMval_field(form, NF);
	MMvari2(form, &tnum, "%4d");

	form = MMalloc(&frame, i, 13, TNAME, (char *) 0);
	MMval_field(form, NF);
	MMvarc(form, tname, "%20s");

	form = MMalloc(&frame, i, 35, FARBE, (char *) 0);
	MMval_field(form, NF);
	MMvarc(form, farbe, "%8s");

	form = MMalloc(&frame, i, 45, GEWICHT, (char *) 0);
	MMval_field(form, NF);
	MMvari2(form, &gewicht, "%7d");

	form = MMalloc(&frame, i, 54, MENGE, (char *) 0);
	MMval_field(form, NF);
	MMvari2(form, &menge, "%5d");
   }

   form = MMalloc(&frame, i, 7, 52, (char *) 0);
   MMout_field(form, "----------------------------------------------------");

   form = MMalloc(&frame, 24, 2, 6, (char *) 0);
   MMout_field(form, "SAETZE");

   form = MMalloc(&frame, 24, 13, 10, (char *) 0);
   MMmen_field(form, f_retrieve, NF, " ANZEIGEN ");

## open teile

   MMform(frame);

## close teile

   MMfree(&frame);
   MMexit_form(1);
   return (0);
}
