## shared dt

# define	NF		((int (*)()) 0)		/* null function */
# define	FIELD		22

   int		key = 1;
   char		*text = " KEIN AKTUELLER SATZ VORHANDEN ";

/* vars of relation lieferung */
# define	BNUM		 4
## i2		bnum;

# define	TNUM		 3
## i2		tnum;

# define	LNUM		 3
## i2		lnum;
   char		v_lnum[LNUM + 1];

# define	LIEFDAT		 8
## char		liefdat[21];
   char		v_liefdat[LIEFDAT + 1];

# define	MENGE		 4
## i2		menge;
   char		v_menge[MENGE + 1];


f_init(frame)
register char		*frame;
{
   lnum = 0;
   liefdat[0] = 0;
   menge = 0;
   MMclr_value(frame);
   MMvalue(frame);
}

f_bnum(frame)
register char		*frame;
{
   if (bnum < 0 || bnum > 9999)
   {
	MMerror(" BNUM FALSCH ");
	return (0);
   }
   return (1);
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

f_lnum(frame)
register char		*frame;
{
   if (lnum <= 0 || lnum > 999)
   {
	MMerror(" LNUM FALSCH ");
	return (0);
   }
   return (1);
}

f_menge(frame)
register char		*frame;
{
   if (menge <= 0 || menge > 9999)
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

## use (shared lieferung)
## retrieve lieferung $bnum == bnum and $tnum == tnum
## (
## 	lnum = $lnum,
## 	liefdat = $liefdat,
## 	menge = $menge
## )
## {
	cnt++;
	key = 0;
	MMset_value(frame, &lnum, 0);
	MMset_value(frame, liefdat, 0);
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

## use (exclusive lieferung)
   cnt = 0;
## retrieve lieferung $bnum == bnum and $tnum == tnum
## {
	cnt++;
##	endretrieve
## }
   if (!cnt)
   {
	key = 0;
##	append lieferung
##	(
## 		$bnum = bnum,
## 		$tnum = tnum,
## 		$lnum = lnum,
## 		$liefdat = liefdat,
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

## use (exclusive lieferung)
## retrieve lieferung $bnum == bnum and $tnum == tnum
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

## use (exclusive lieferung)
   cnt = 0;
## retrieve lieferung $bnum == bnum and $tnum == tnum
## {
	cnt++;
##	replace
##	(
##		$lnum = lnum,
##		$liefdat = liefdat,
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
   MMout_field(form, "----------------------------------------------------- L I E F E R U N G");

   form = MMalloc(&frame, 3, 1, 4, (char *) 0);
   MMout_field(form, "BNUM");

   form = MMalloc(&frame, 3, FIELD, BNUM, (char *) 0);
   MMin_field(form, f_bnum, NF, 0);
   MMvari2(form, &bnum, "%d");

   form = MMalloc(&frame, 4, 1, 4, (char *) 0);
   MMout_field(form, "TNUM");

   form = MMalloc(&frame, 4, FIELD, TNUM, (char *) 0);
   MMin_field(form, f_tnum, NF, 0);
   MMvari2(form, &tnum, "%d");

   form = MMalloc(&frame, 6, 1, 4, (char *) 0);
   MMout_field(form, "LNUM");

   form = MMalloc(&frame, 6, FIELD, LNUM, v_lnum);
   MMval_field(form, NF);
   MMvari2(form, &lnum, "%d");
   form = MMalloc(&frame, 6, FIELD, LNUM, v_lnum);
   MMin_field(form, f_lnum, NF, 0);
   MMvari2(form, &lnum, "%d");

   form = MMalloc(&frame, 7, 1, 7, (char *) 0);
   MMout_field(form, "LIEFDAT");

   form = MMalloc(&frame, 7, FIELD, LIEFDAT, v_liefdat);
   MMval_field(form, NF);
   MMvarc(form, liefdat, (char *) 0);
   form = MMalloc(&frame, 7, FIELD, LIEFDAT, v_liefdat);
   MMin_field(form, NF, NF, 0);
   MMvarc(form, liefdat, (char *) 0);

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

## open lieferung

   MMform(frame);

## close lieferung

   MMfree(&frame);
   MMexit_form(1);
   return (0);
}
