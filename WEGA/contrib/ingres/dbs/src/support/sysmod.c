# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/lock.h"
# include	"../h/access.h"
# include	"../h/batch.h"
# include	"../h/bs.h"

# ifdef P8000
ret_buf		AAjmp_buf;
# else
jmp_buf		AAjmp_buf;
# endif

extern int	AAdbstat;

short		Qrymod;
char		*Fileset;
int		Noupdt =	FALSE;
char		*Dummy;
char		**Xparams =	&Dummy;
struct batchbuf	Batchbuf;
char		Header[] =	"**\t\t";

char	KSORT[]	=	 "/sys/bin/dbu___s";

struct modtabl
{
	char	*rname;
	char	**parvec;
	short	qmcatalogs;
	short	goahead;
	short	normgo;
	short	optn;
};

char	*Relpar[] =
{
	AA_REL,
	"hash",
	"name",
	"relid",
	(char *) -1
};

char	*Attpar[] =
{
	AA_ATT,
	"hash",
	"name",
	"attrelid",	"attowner",	"#attid",
	(char *) -1
};

char	*Indpar[] =
{
	AA_INDEX,
	"hash",
	"name",
	"irelidp",	"iownerp",
	"",
	"minpages",	"5",
	(char *) -1
};

char	*Trepar[] =
{
	AA_TREE,
	"hash",
	"name",
	"treerelid",	"treeowner",	"treetype",
	(char *) -1
};

char	*Propar[] =
{
	AA_PROTECT,
	"hash",
	"name",
	"prorelid",	"proowner",
	"",
	"minpages",	"5",
	(char *) -1
};

char	*Intpar[] =
{
	AA_INTEG,
	"hash",
	"name",
	"intrelid",	"intrelowner",
	"",
	"minpages",	"5",
	(char *) -1
};

char	*Refpar[] =
{
	AA_REF,
	"hash",
	"name",
	"refrelid",	"refowner",	"reftype",
	"",
	"minpages",	"5",
	(char *) -1
};

struct modtabl Modtabl[] =
{
	    AA_REL,  Relpar,  FALSE,  FALSE,   TRUE,  FALSE,
	    AA_ATT,  Attpar,  FALSE,  FALSE,   TRUE,  FALSE,
	  AA_INDEX,  Indpar,  FALSE,  FALSE,   TRUE,  FALSE,
	   AA_TREE,  Trepar,   TRUE,  FALSE,   TRUE,   TRUE,
	AA_PROTECT,  Propar,   TRUE,  FALSE,   TRUE,   TRUE,
	  AA_INTEG,  Intpar,   TRUE,  FALSE,   TRUE,   TRUE,
	    AA_REF,  Refpar,   TRUE,  FALSE,   TRUE,   TRUE,
	(char *) 0
};

/*
**	SYSMOD -- Modify system catalogs to a predetermined
**		storage structure with predetermined keys.
*/
main(argc, argv)
int	argc;
char	*argv[];
{
	register int	i;
	register int	j;
	register char	**av;
	register char	*p;
	register int	retval;
	register int	superuser;
	char		fileset[10];
	char		proctab[100];
	extern char	*Parmvect[];
	extern char	*Flagvect[];
	extern char	*AAdbpath;
	extern char	*AApath;
	extern char	*AAproc_name;

	AAproc_name = "SYSMOD";

#	ifdef xSTR1
	AAtTrace(&argc, argv, 'X');
#	endif

	AA_itoa(getpid(), fileset);
	Fileset = fileset;
	i = initucode(argc, argv, TRUE, (char *) 0, M_EXCL);
	printf("Datenbank %s\n", Parmvect[0]);
	switch (i)
	{
	  case 0:
	  case 5:
		break;

	  case 1:
	  case 6:
		AAsyserr(0, "Die Datenbank %s existiert nicht", Parmvect[0]);

	  case 2:
		AAsyserr(0, "Sie duerfen auf die Datenbank %s nicht zugreifen", Parmvect[0]);

	  case 3:
		AAsyserr(0, "Sie sind kein eingetragener DBS-Nutzer");

	  case 4:
		printf("Sie haben keinen Datenbanknamen angegeben\n");
	usage:
		AAsyserr(0, "HINWEIS: sysmod [-s] [+-w] db_name {relation ...}");

	  default:
		AAsyserr(22200, AAproc_name, i);
	}

	AAconcat(AApath, KSORT, proctab);
	Dummy = proctab;

	superuser = FALSE;
	for (av = Flagvect; p = *av; av++)
	{
		if (p[0] != '-')
		{
		badflag:
			printf("Unbekanntes Flag %s\n", p);
			goto usage;
		}
		switch (p[1])
		{
		  case 's':
			if (!(AAdbstat & U_SUPER))
				AAsyserr(0, "Sie duerfen das -s Flag nicht verwenden");
			superuser = TRUE;
			break;

		  default:
			goto badflag;
		}
	}
	if (chdir(AAdbpath) < 0)
		AAsyserr(0, "Die Datenbank %s existiert nicht", Parmvect[0]);
	if (superuser)
		AAbmove(AAdmin.adhdr.adowner, AAusercode, 2);
	if (!AAbequal(AAusercode, AAdmin.adhdr.adowner, 2))
		AAsyserr(0, "Sie sind nicht der DBA der Datenbank %s", Parmvect[0]);

	Qrymod = ((AAdmin.adhdr.adflags & A_QRYMOD) == A_QRYMOD);

/*
**	if there are any arguments, verify that they are valid
**	names of relations which can be modified by this program.
**	if there are no arguments, assume all system relations are to be
**	modified.
*/
	if (Parmvect[1])
		if (!(Parmvect[2]) && AAsequal(Parmvect[1], "all"))
			for (i = 0; Modtabl[i].rname; i++)
				Modtabl[i].goahead = TRUE;
		else
			for (av = &Parmvect[1]; p = *av; av++)
			{
				for (j = 0; Modtabl[j].rname; j++)
				{
					if (!AAscompare(p, MAXNAME, Modtabl[j].rname, MAXNAME))
					{
						if (Modtabl[j].goahead)
							AAsyserr(0, "Doppelt angegebener Relationsname %s", p);
						Modtabl[j].goahead = TRUE;
						break;
					}
				}
				if (!Modtabl[j].rname)
					AAsyserr(0, "%s ist kein Katalogname", p);
			}
	else
		for (i = 0; Modtabl[i].rname; i++)
			Modtabl[i].goahead = Modtabl[i].normgo;
	for (i = 0; Modtabl[i].rname; i++)
	{
		if (!Modtabl[i].goahead || optn_rel(&Modtabl[i]))
			continue;

		if (!Qrymod && Modtabl[i].qmcatalogs)
			continue;

		printf("%sKatalog %s\n", Header, Modtabl[i].rname);
		av = Modtabl[i].parvec + 3;
		j = 0;
		while (*av != (char *) -1)
		{
			j++;
			av++;
		}
		j += 3;
		AAsmove(Fileset, Batchbuf.file_id);
		if (retval = modify(j, Modtabl[i].parvec))
			AAsyserr(0, "%sFehlernummer *%d*", Header, retval);
	}
	AAunldb();
	fflush(stdout);
	exit(0);
}


optn_rel(m)
register struct modtabl		*m;
{
	DESC			des;

	return (m->optn & AArelopen(&des, -1, m->rname));
}


rubproc()
{
	printf("%sSYSMOD interrupted\n", Header);
	AAunlall();
	fflush(stdout);
	exit(1);
}
