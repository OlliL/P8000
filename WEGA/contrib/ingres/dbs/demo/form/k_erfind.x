## shared dt

# define	NF		((int (*)()) 0)		/* null function */
# define	FIELD		22

   int		key = 1;
   char		*text = " KEIN AKTUELLER SATZ VORHANDEN ";

/* vars of relation erfind */
# define	ERFINDUNG	36
## char		erfindung[37];
   char		v_erf[ERFINDUNG + 1];

# define	ERFINDER	20
## char		erfinder[21];
   char		v_erfinder[ERFINDER + 1];

# define	JAHR		 9
## char		jahr[10];
   char		v_jahr[JAHR + 1];

# define	LAND		12
## char		land[13];
   char		v_land[LAND + 1];


f_init(frame)
register char		*frame;
{
   erfinder[0] = 0;
   jahr[0] = 0;
   land[0] = 0;
   MMclr_value(frame);
   MMvalue(frame);
}

f_retrieve(frame)
register char		*frame;
{
	register int		cnt;

   key++;
   f_init(frame);
   cnt = 0;

## use (shared erfind)
## retrieve erfind $erfindung == erfindung
## (
## 	erfindung = $erfindung,
## 	erfinder = $erfinder,
## 	jahr = $jahr,
## 	land = $land
## )
## {
	cnt++;
	key = 0;
	MMset_value(frame, erfindung, 0);
	MMset_value(frame, erfinder, 0);
	MMset_value(frame, jahr, 0);
	MMset_value(frame, land, 0);
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

## use (exclusive erfind)
   cnt = 0;
## retrieve erfind $erfindung == erfindung
## {
	cnt++;
##	endretrieve
## }
   if (!cnt)
   {
	key = 0;
##	append erfind
##	(
## 		$erfindung = erfindung,
## 		$erfinder = erfinder,
## 		$jahr = jahr,
## 		$land = land
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

## use (exclusive erfind)
## retrieve erfind $erfindung == erfindung
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

## use (exclusive erfind)
   cnt = 0;
## retrieve erfind $erfindung == erfindung
## {
	cnt++;
##	replace
##	(
##		$erfinder = erfinder,
##		$jahr = jahr,
##		$land = land
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
   MMout_field(form, "----------------------------------------------------------- E R F I N D");

   form = MMalloc(&frame, 3, 1, 9, (char *) 0);
   MMout_field(form, "ERFINDUNG");

   form = MMalloc(&frame, 3, FIELD, ERFINDUNG, v_erf);
   MMval_field(form, NF);
   MMvarc(form, erfindung, (char *) 0);
   form = MMalloc(&frame, 3, FIELD, ERFINDUNG, v_erf);
   MMin_field(form, NF, NF, 0);
   MMvarc(form, erfindung, (char *) 0);

   form = MMalloc(&frame, 5, 1, 8, (char *) 0);
   MMout_field(form, "ERFINDER");

   form = MMalloc(&frame, 5, FIELD, ERFINDER, v_erfinder);
   MMval_field(form, NF);
   MMvarc(form, erfinder, (char *) 0);
   form = MMalloc(&frame, 5, FIELD, ERFINDER, v_erfinder);
   MMin_field(form, NF, NF, 0);
   MMvarc(form, erfinder, (char *) 0);

   form = MMalloc(&frame, 6, 1, 4, (char *) 0);
   MMout_field(form, "JAHR");

   form = MMalloc(&frame, 6, FIELD, JAHR, v_jahr);
   MMval_field(form, NF);
   MMvarc(form, jahr, (char *) 0);
   form = MMalloc(&frame, 6, FIELD, JAHR, v_jahr);
   MMin_field(form, NF, NF, 0);
   MMvarc(form, jahr, (char *) 0);

   form = MMalloc(&frame, 7, 1, 4, (char *) 0);
   MMout_field(form, "LAND");

   form = MMalloc(&frame, 7, FIELD, LAND, v_land);
   MMval_field(form, NF);
   MMvarc(form, land, (char *) 0);
   form = MMalloc(&frame, 7, FIELD, LAND, v_land);
   MMin_field(form, NF, NF, 0);
   MMvarc(form, land, (char *) 0);

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

## open erfind

   MMform(frame);

## close erfind

   MMfree(&frame);
   MMexit_form(1);
   return (0);
}
