## shared dt

# define	NF		((int (*)()) 0)		/* null function */
# define	FIELD		22

   int		key = 1;
   char		*text = " KEIN AKTUELLER SATZ VORHANDEN ";

/* vars of relation teile */
# define	TNUM		 3
## i2		tnum;

# define	TNAME		20
## char		tname[21];
   char		v_tname[TNAME + 1];

# define	FARBE		 8
## char		farbe[9];
   char		v_farbe[FARBE + 1];

# define	GEWICHT		 3
## i2		gewicht;
   char		v_gewicht[GEWICHT + 1];

# define	MENGE		 3
## i2		menge;
   char		v_menge[MENGE + 1];


f_init(frame)
register char		*frame;
{
   tname[0] = 0;
   farbe[0] = 0;
   gewicht = 0;
   menge = 0;
   MMclr_value(frame);
   MMvalue(frame);
}

f_tnum(frame)
register char		*frame;
{
   if (tnum <= 0 || tnum > 999)
   {
	MMerror(" TNUM FALSCH ");
	return (0);
   }
   return (1);
}

f_gewicht(frame)
register char		*frame;
{
   if (gewicht < 0 || gewicht > 999)
   {
	MMerror(" GEWICHT FALSCH ");
	return (0);
   }
   return (1);
}

f_menge(frame)
register char		*frame;
{
   if (menge <= 0 || menge > 999)
   {
	MMerror(" MENGE FALSCH ");
	return (0);
   }
   return (1);
}

f_retrieve(frame)
register char		*frame;
{
	register int		cnt;

   key++;
   f_init(frame);
   cnt = 0;

## use (shared teile)
## retrieve teile $tnum == tnum
## (
## 	tname = $tname,
## 	farbe = $farbe,
## 	gewicht = $gewicht,
## 	menge = $menge
## )
## {
	cnt++;
	key = 0;
	MMset_value(frame, tname, 0);
	MMset_value(frame, farbe, 0);
	MMset_value(frame, &gewicht, 0);
	MMset_value(frame, &menge, 0);
	MMvalue(frame);
##	endretrieve
## }
## commit

   if (!cnt)
	MMerror(" SATZ NICHT VORHANDEN ");
}

f_append(frame)
register char		*frame;
{
	register int		cnt;

## use (exclusive teile)
   cnt = 0;
## retrieve teile $tnum == tnum
## {
	cnt++;
##	endretrieve
## }
   if (!cnt)
   {
	key = 0;
##	append teile
##	(
## 		$tnum = tnum,
## 		$tname = tname,
## 		$farbe = farbe,
## 		$gewicht = gewicht,
## 		$menge = menge
##	)
## commit
   }

   MMerror(cnt? " SATZ VORHANDEN ": " SATZ ANGEFUEGT ");
}

f_delete(frame)
register char		*frame;
{
	register int		cnt;

   if (key)
   {
	MMerror(text);
	return;
   }

## use (exclusive teile)
## retrieve teile $tnum == tnum
## {
	cnt++;
##	delete
##	endretrieve
## }
## commit

   MMerror(cnt? " SATZ GELOESCHT ": " SATZ NICHT GELOESCHT ");
   key++;
   f_init(frame);
}

f_replace(frame)
register char		*frame;
{
	register int		cnt;
   if (key)
   {
	MMerror(text);
	return;
   }

## use (exclusive teile)
   cnt = 0;
## retrieve teile $tnum == tnum
## {
	cnt++;
##	replace
##	(
##		$tname = tname,
##		$farbe = farbe,
##		$gewicht = gewicht,
##		$menge = menge
##	)
##	endretrieve
## }
## commit

   MMerror(cnt? " SATZ ERSETZT ": " SATZ NICHT ERSETZT ");
}

amx(argc, argv)
int			argc;
char			*argv[];
{
	register char		*form;
	char			*frame;
	extern char		*MMalloc();


   MMinit_form(1);

   frame = (char *) 0;
   form = MMalloc(&frame, 2, 1, 71, (char *) 0);
   MMout_field(form, "------------------------------------------------------------- T E I L E");

   form = MMalloc(&frame, 3, 1, 4, (char *) 0);
   MMout_field(form, "TNUM");

   form = MMalloc(&frame, 3, FIELD, TNUM, (char *) 0);
   MMin_field(form, f_tnum, NF, 0);
   MMvari2(form, &tnum, "%d");

   form = MMalloc(&frame, 5, 1, 5, (char *) 0);
   MMout_field(form, "TNAME");

   form = MMalloc(&frame, 5, FIELD, TNAME, v_tname);
   MMval_field(form, NF);
   MMvarc(form, tname, (char *) 0);
   form = MMalloc(&frame, 5, FIELD, TNAME, v_tname);
   MMin_field(form, NF, NF, 0);
   MMvarc(form, tname, (char *) 0);

   form = MMalloc(&frame, 6, 1, 5, (char *) 0);
   MMout_field(form, "FARBE");

   form = MMalloc(&frame, 6, FIELD, FARBE, v_farbe);
   MMval_field(form, NF);
   MMvarc(form, farbe, (char *) 0);
   form = MMalloc(&frame, 6, FIELD, FARBE, v_farbe);
   MMin_field(form, NF, NF, 0);
   MMvarc(form, farbe, (char *) 0);

   form = MMalloc(&frame, 7, 1, 7, (char *) 0);
   MMout_field(form, "GEWICHT");

   form = MMalloc(&frame, 7, FIELD, GEWICHT, v_gewicht);
   MMval_field(form, NF);
   MMvari2(form, &gewicht, "%d");
   form = MMalloc(&frame, 7, FIELD, GEWICHT, v_gewicht);
   MMin_field(form, f_gewicht, NF, 0);
   MMvari2(form, &gewicht, "%d");

   form = MMalloc(&frame, 8, 1, 5, (char *) 0);
   MMout_field(form, "MENGE");

   form = MMalloc(&frame, 8, FIELD, MENGE, v_menge);
   MMval_field(form, NF);
   MMvari2(form, &menge, "%d");
   form = MMalloc(&frame, 8, FIELD, MENGE, v_menge);
   MMin_field(form, f_menge, NF, 0);
   MMvari2(form, &menge, "%d");

   form = MMalloc(&frame, 24, 2, 4, (char *) 0);
   MMout_field(form, "SATZ");

   form = MMalloc(&frame, 24, 13, 10, (char *) 0);
   MMmen_field(form, f_retrieve, NF, " ANZEIGEN ");

   form = MMalloc(&frame, 24, 28, 10, (char *) 0);
   MMmen_field(form, f_append, NF, " ANFUEGEN ");

   form = MMalloc(&frame, 24, 43, 10, (char *) 0);
   MMmen_field(form, f_delete, NF, " LOESCHEN ");

   form = MMalloc(&frame, 24, 58, 10, (char *) 0);
   MMmen_field(form, f_replace, NF, " ERSETZEN ");

## open teile

   MMform(frame);

## close teile

   MMfree(&frame);
   MMexit_form(1);
   return (0);
}
