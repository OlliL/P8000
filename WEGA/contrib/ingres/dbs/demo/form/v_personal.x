## shared dt

# define	NF		((int (*)()) 0)		/* null function */
# define	N		16

# define	SATZ		 4
   int		satz;

/* vars of relation personal */
# define	NUM		 4
## i2		num;

# define	NAME		20
## char		name[21];

# define	GEHALT		 6
## i2		gehalt;

# define	LEITER		 6
## i2		leiter;

# define	GEBDAT		 6
## i2		gebdat;

# define	EINSTDAT	 8
## i2		einstdat;


f_retrieve(frame)
register char		*frame;
{
	register int		cnt;

   MMerror(" RECHERCHE LAEUFT ");
## use (shared personal)
   satz = cnt = 0;
## retrieve personal
## (
## 	num = $num,
## 	name = $name,
## 	gehalt = $gehalt,
## 	leiter = $leiter,
## 	gebdat = $gebdat,
## 	einstdat = $einstdat
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
	MMset_value(frame, &num, cnt);
	MMset_value(frame, name, cnt);
	MMset_value(frame, &gehalt, cnt);
	MMset_value(frame, &leiter, cnt);
	MMset_value(frame, &gebdat, cnt);
	MMset_value(frame, &einstdat, cnt);
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
   MMout_field(form, "------------------------------------------------------- P E R S O N A L");

   form = MMalloc(&frame, 4, 1, 4, (char *) 0);
   MMout_field(form, "SATZ");

   form = MMalloc(&frame, 4, 8, 3, (char *) 0);
   MMout_field(form, "NUM");

   form = MMalloc(&frame, 4, 29, 4, (char *) 0);
   MMout_field(form, "NAME");

   form = MMalloc(&frame, 4, 35, 6, (char *) 0);
   MMout_field(form, "GEHALT");

   form = MMalloc(&frame, 4, 43, 6, (char *) 0);
   MMout_field(form, "LEITER");

   form = MMalloc(&frame, 4, 51, 6, (char *) 0);
   MMout_field(form, "GEBDAT");

   form = MMalloc(&frame, 4, 59, 8, (char *) 0);
   MMout_field(form, "EINSTDAT");

   form = MMalloc(&frame, 5, 7, 60, (char *) 0);
   MMout_field(form, "------------------------------------------------------------");

   for (i = 6; i < 6 + N; i++)
   {
	form = MMalloc(&frame, i, 1, SATZ, (char *) 0);
	MMval_field(form, NF);
	MMvari2(form, &satz, "%4d");

	form = MMalloc(&frame, i, 7, NUM, (char *) 0);
	MMval_field(form, NF);
	MMvari2(form, &num, "%4d");

	form = MMalloc(&frame, i, 13, NAME, (char *) 0);
	MMval_field(form, NF);
	MMvarc(form, name, "%20s");

	form = MMalloc(&frame, i, 35, GEHALT, (char *) 0);
	MMval_field(form, NF);
	MMvari2(form, &gehalt, "%6d");

	form = MMalloc(&frame, i, 43, LEITER, (char *) 0);
	MMval_field(form, NF);
	MMvari2(form, &leiter, "%6d");

	form = MMalloc(&frame, i, 51, GEBDAT, (char *) 0);
	MMval_field(form, NF);
	MMvari2(form, &gebdat, "%6d");

	form = MMalloc(&frame, i, 59, EINSTDAT, (char *) 0);
	MMval_field(form, NF);
	MMvari2(form, &einstdat, "%8d");
   }

   form = MMalloc(&frame, i, 7, 60, (char *) 0);
   MMout_field(form, "------------------------------------------------------------");

   form = MMalloc(&frame, 24, 2, 6, (char *) 0);
   MMout_field(form, "SAETZE");

   form = MMalloc(&frame, 24, 13, 10, (char *) 0);
   MMmen_field(form, f_retrieve, NF, " ANZEIGEN ");

## open personal

   MMform(frame);

## close personal

   MMfree(&frame);
   MMexit_form(1);
   return (0);
}
