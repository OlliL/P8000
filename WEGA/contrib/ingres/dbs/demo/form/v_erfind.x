## shared dt

# define	NF		((int (*)()) 0)		/* null function */
# define	N		 8

# define	SATZ		 4
   int		satz;

/* vars of relation erfind */
# define	ERFINDUNG	36
## char		erfindung[37];

# define	ERFINDER	20
## char		erfinder[21];

# define	JAHR		 9
## char		jahr[10];

# define	LAND		12
## char		land[13];


f_retrieve(frame)
register char		*frame;
{
	register int		cnt;

   MMerror(" RECHERCHE LAEUFT ");
## use (shared erfind)
   satz = cnt = 0;
## retrieve erfind
## (
## 	erfindung = $erfindung,
## 	erfinder = $erfinder,
## 	jahr = $jahr,
## 	land = $land
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
	MMset_value(frame, erfindung, cnt);
	MMset_value(frame, erfinder, cnt);
	MMset_value(frame, jahr, cnt);
	MMset_value(frame, land, cnt);
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
   MMout_field(form, "----------------------------------------------------------- E R F I N D");

   form = MMalloc(&frame, 4, 1, 4, (char *) 0);
   MMout_field(form, "SATZ");

   form = MMalloc(&frame, 4, 7, 9, (char *) 0);
   MMout_field(form, "ERFINDUNG");

   form = MMalloc(&frame, 4, 19, 8, (char *) 0);
   MMout_field(form, "ERFINDER");

   form = MMalloc(&frame, 4, 54, 4, (char *) 0);
   MMout_field(form, "JAHR");

   form = MMalloc(&frame, 4, 68, 4, (char *) 0);
   MMout_field(form, "LAND");

   form = MMalloc(&frame, 5, 7, 65, (char *) 0);
   MMout_field(form, "-----------------------------------------------------------------");

   for (i = 6; i < 6 + (2 * N); i += 2)
   {
	form = MMalloc(&frame, i, 1, SATZ, (char *) 0);
	MMval_field(form, NF);
	MMvari2(form, &satz, "%4d");

	form = MMalloc(&frame, i, 7, ERFINDUNG, (char *) 0);
	MMval_field(form, NF);
	MMvarc(form, erfindung, (char *) 0);

	form = MMalloc(&frame, i + 1, 27, ERFINDER, (char *) 0);
	MMval_field(form, NF);
	MMvarc(form, erfinder, "%20s");

	form = MMalloc(&frame, i + 1, 49, JAHR, (char *) 0);
	MMval_field(form, NF);
	MMvarc(form, jahr, "%9s");

	form = MMalloc(&frame, i + 1, 60, LAND, (char *) 0);
	MMval_field(form, NF);
	MMvarc(form, land, "%12s");
   }

   form = MMalloc(&frame, i, 7, 65, (char *) 0);
   MMout_field(form, "-----------------------------------------------------------------");

   form = MMalloc(&frame, 24, 2, 6, (char *) 0);
   MMout_field(form, "SAETZE");

   form = MMalloc(&frame, 24, 13, 10, (char *) 0);
   MMmen_field(form, f_retrieve, NF, " ANZEIGEN ");

## open erfind

   MMform(frame);

## close erfind

   MMfree(&frame);
   MMexit_form(1);
   return (0);
}
