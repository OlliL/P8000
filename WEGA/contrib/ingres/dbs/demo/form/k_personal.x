## shared dt

# define	NF		((int (*)()) 0)		/* null function */
# define	FIELD		22

   int		key = 1;
   char		*text = " KEIN AKTUELLER SATZ VORHANDEN ";

/* vars of relation personal */
# define	NUM		 4
## i2		num;

# define	NAME		20
## char		name[21];
   char		v_name[NAME + 1];

# define	GEHALT		 4
## i2		gehalt;
   char		v_gehalt[GEHALT + 1];

# define	LEITER		 4
## i2		leiter;
   char		v_leiter[LEITER + 1];

# define	GEBDAT		 4
## i2		gebdat;
   char		v_gebdat[GEBDAT + 1];

# define	EINSTDAT	 4
## i2		einstdat;
   char		v_einstdat[EINSTDAT + 1];


f_init(frame)
register char		*frame;
{
   name[0] = 0;
   gehalt = 0;
   leiter = 0;
   gebdat = 0;
   einstdat = 0;
   MMclr_value(frame);
   MMvalue(frame);
}

f_num(frame)
register char		*frame;
{
   if (num <= 0 || num > 9999)
   {
	MMerror(" NUM FALSCH ");
	return (0);
   }
   return (1);
}

f_gehalt(frame)
register char		*frame;
{
   if (gehalt <= 0 || gehalt > 9999)
   {
	MMerror(" GEHALT FALSCH ");
	return (0);
   }
   return (1);
}

f_leiter(frame)
register char		*frame;
{
   if (leiter < 0 || leiter > 9999)
   {
	MMerror(" LEITER FALSCH ");
	return (0);
   }
   return (1);
}

f_gebdat(frame)
register char		*frame;
{
   if (gebdat <= 0 || gebdat > 9999)
   {
	MMerror(" GEBDAT FALSCH ");
	return (0);
   }
   return (1);
}

f_einstdat(frame)
register char		*frame;
{
   if (einstdat <= 0 || einstdat > 9999)
   {
	MMerror(" EINSTDAT FALSCH ");
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

## use (shared personal)
## retrieve personal $num == num
## (
## 	name = $name,
## 	gehalt = $gehalt,
## 	leiter = $leiter,
## 	gebdat = $gebdat,
## 	einstdat = $einstdat
## )
## {
	cnt++;
	key = 0;
	MMset_value(frame, name, 0);
	MMset_value(frame, &gehalt, 0);
	MMset_value(frame, &leiter, 0);
	MMset_value(frame, &gebdat, 0);
	MMset_value(frame, &einstdat, 0);
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

## use (exclusive personal)
   cnt = 0;
## retrieve personal $num == num
## {
	cnt++;
##	endretrieve
## }
   if (!cnt)
   {
	key = 0;
##	append personal
##	(
## 		$num = num,
## 		$name = name,
## 		$gehalt = gehalt,
## 		$leiter = leiter,
## 		$gebdat = gebdat,
## 		$einstdat = einstdat
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

## use (exclusive personal)
## retrieve personal $num == num
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

## use (exclusive personal)
   cnt = 0;
## retrieve personal $num == num
## {
	cnt++;
##	replace
##	(
##		$name = name,
##		$gehalt = gehalt,
##		$leiter = leiter,
##		$gebdat = gebdat,
##		$einstdat = einstdat
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
   MMout_field(form, "------------------------------------------------------- P E R S O N A L");

   form = MMalloc(&frame, 3, 1, 3, (char *) 0);
   MMout_field(form, "NUM");

   form = MMalloc(&frame, 3, FIELD, NUM, (char *) 0);
   MMin_field(form, f_num, NF, 0);
   MMvari2(form, &num, "%d");

   form = MMalloc(&frame, 5, 1, 4, (char *) 0);
   MMout_field(form, "NAME");

   form = MMalloc(&frame, 5, FIELD, NAME, v_name);
   MMval_field(form, NF);
   MMvarc(form, name, (char *) 0);
   form = MMalloc(&frame, 5, FIELD, NAME, v_name);
   MMin_field(form, NF, NF, 0);
   MMvarc(form, name, (char *) 0);

   form = MMalloc(&frame, 6, 1, 6, (char *) 0);
   MMout_field(form, "GEHALT");

   form = MMalloc(&frame, 6, FIELD, GEHALT, v_gehalt);
   MMval_field(form, NF);
   MMvari2(form, &gehalt, "%d");
   form = MMalloc(&frame, 6, FIELD, GEHALT, v_gehalt);
   MMin_field(form, f_gehalt, NF, 0);
   MMvari2(form, &gehalt, "%d");

   form = MMalloc(&frame, 7, 1, 6, (char *) 0);
   MMout_field(form, "LEITER");

   form = MMalloc(&frame, 7, FIELD, LEITER, v_leiter);
   MMval_field(form, NF);
   MMvari2(form, &leiter, "%d");
   form = MMalloc(&frame, 7, FIELD, LEITER, v_leiter);
   MMin_field(form, f_leiter, NF, 0);
   MMvari2(form, &leiter, "%d");

   form = MMalloc(&frame, 8, 1, 6, (char *) 0);
   MMout_field(form, "GEBDAT");

   form = MMalloc(&frame, 8, FIELD, GEBDAT, v_gebdat);
   MMval_field(form, NF);
   MMvari2(form, &gebdat, "%d");
   form = MMalloc(&frame, 8, FIELD, GEBDAT, v_gebdat);
   MMin_field(form, f_gebdat, NF, 0);
   MMvari2(form, &gebdat, "%d");

   form = MMalloc(&frame, 9, 1, 3, (char *) 0);
   MMout_field(form, "EINSTDAT");

   form = MMalloc(&frame, 9, FIELD, EINSTDAT, v_einstdat);
   MMval_field(form, NF);
   MMvari2(form, &einstdat, "%d");
   form = MMalloc(&frame, 9, FIELD, EINSTDAT, v_einstdat);
   MMin_field(form, f_einstdat, NF, 0);
   MMvari2(form, &einstdat, "%d");

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

## open personal

   MMform(frame);

## close personal

   MMfree(&frame);
   MMexit_form(1);
   return (0);
}
