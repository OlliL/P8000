# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/catalog.h"
# include	"../h/access.h"

static char	MAN[] = "/sys/man/";
static char	TOC[] = "toc";
static int	DBA_flag;

/*
**  HELP - Provide Information to User
**
**	Arguments:
**		pv[i] - code
**			0 - print relation information
**			1 - print manual section
**			2 - print short relation list
**			3 - print long relation list
**		pv[i+1] - name of entity for modes 0 or 1
**
*/
help(parmc, parmv)
int	parmc;
char	**parmv;
{
	register char			**pv;
	register int			ret;
	short				mode;
	DESC				des;

	ret = 0;
	pv = parmv;
	getuser((char *) -1);	/* init getuser for modes 1 & 2 */
	DBA_flag = AAbequal(AAdmin.adhdr.adowner, AAusercode, 2);

	while (*pv != (char *) -1)
	{
		if (AA_atoi(*pv++, &mode))
			goto badmode;

#		ifdef xZTR1
		if (AAtTf(10, -1) || AAtTf(0, 10))
		{
			printf("HELP: %d", mode);
			if (mode != 2)
				printf(" %s", *pv);
			putchar('\n');
		}
#		endif

		switch (mode)
		{
		  case 0:	/* help relation */
			if (!AArelopen(&des, -1, *pv) && rel_fmt(&des))
				*pv = (char *) 0;
			goto next;

		  case 1:	/* help manual section */
			if (!man_fmt(*pv))
				*pv = (char *) 0;
next:
			pv++;
			break;

		  case 2:
		  case 3:
			relpr(mode);
			break;

		  default:
badmode:
			AAsyserr(11033, *--pv);
		}
	}
	getuser((char *) 0);	/* close getuser in case mode 1 or 2 */

	/* now rescan for error messages */
	pv = parmv;
	while (*pv != (char *) -1)
	{
		AA_atoi(*pv++, &mode);
		switch (mode)
		{
		  case 0:
		  case 1:
			if (*pv)
				ret = error(5401 + mode, *pv, (char *) 0);
			break;

		  case 2:
		  case 3:
			continue;
		}
		pv++;
	}

	return (ret);
}


/*
**  Nroff Manual Section
**
**	The manual section given by 'name' is runoff'ed.  Returns one
**	on success, zero if the manual section is not found.
*/
man_fmt(name)
register char	*name;
{
	extern char	*AAztack();

	/* a null manual name gives table of contents */
	if (!*name)
		name = TOC;
	else if (AAlength(name) > MAXNAME)
		return (TRUE);

	return (AAcat(AAztack(AAztack(AApath, MAN), name)));
}


/*
**  PRINT DATABASE INFORMATION
**
**	Prints a list of all the relations in the database, together
**	with their owner.
*/
relpr(mode)
int	mode;
{
	register DESC			*d;
	register int			i;
	struct tup_id			limtid;
	struct tup_id			tid;
	struct relation			rel;
	char				buf[MAXLINE + 1];
	char				lastuser[2];
	extern DESC			Reldes;
	extern char			*AAdatabase;
	extern char			*AAbmove();
	extern char			*AAcustomer();

	opencatalog(AA_REL, 0);
	d = &Reldes;
	if (i = AAam_find(d, NOKEY, &tid, &limtid))
		AAsyserr(11036, i);

	lastuser[0] = '\0';

	if (mode == 2)
	{
		i = AAdmin.adhdr.adflags;
		rel_owner(buf, AAdmin.adhdr.adowner);
		printf("\n%s / DBA %s\n\n", AAcustomer(), buf);
		printf("DB %s %sparallel verwendbar\n",
			AAdatabase, (i & A_DBCONCUR)? "": "nicht ");
		printf("%sQRYMOD-Unterstuetzung\n", (i & A_QRYMOD)? "": "keine ");
		printf("Seitengroesse %d Bytes\n\n", AApgsize);
		printf(" Relation          Besitzer          Typ\n");
		printf("----------------------------------------------------\n");
	}

	while (!(i = AAam_get(d, &tid, &limtid, &rel, 1)))
	{
		if (mode == 2)
		{
			if (!DBA_flag && !AAbequal(AAusercode, rel.relowner, 2))
				continue;

			if (!AAbequal(lastuser, rel.relowner, 2))
			{
				rel_owner(buf, rel.relowner);
				AAbmove(rel.relowner, lastuser, 2);
			}
			printf(" %.12s      %-18s", rel.relid, buf);
			if (rel.relstat & S_CATALOG)
				printf("Systemkatalog\n");
			else if (rel.relstat & S_VIEW)
				printf("Sicht\n");
			else if (ctoi(rel.relindxd) == SECINDEX)
				printf("Index\n");
			else
			{
				if (rel.relstat & S_DISTRIBUTED)
					printf("entfernte ");
				printf("Nutzerrelation\n");
			}
		}
		else if (AAbequal(AAusercode, rel.relowner, 2) && !(rel.relstat & S_CATALOG))
			rel_fmt(&rel);
	}

	if (i < 0)
		AAsyserr(11037, i);
	if (mode == 2)
		printf("----------------------------------------------------\n");
}


rel_owner(buf, own)
register char	*buf;
register char	*own;
{
	register char			*cp;
	extern char			*AAbmove();

	if (getuser(own, buf))
	{
		/* cant find user code */
		cp = AAbmove("  ", buf, 2);
		cp = AAbmove(own, cp, 2);
	}
	else
		for (cp = buf; *cp != ':'; cp++)
			continue;
	*cp = '\0';
}


/*
**  Print Relation Information
**
**	Prints detailed information regarding the relation.
*/
rel_fmt(r)
register struct relation	*r;
{
	register int			i;
	register int			j;
	struct tup_id			limtid;
	struct tup_id			tid;
	struct attribute		att;
	struct index			indkey;
	struct index			ind;
	char				buf[MAXLINE + 1];
	char				tempname[MAXNAME + 3];
	long				mainpg;
	long				ovflopg;
	extern DESC			Attdes;
	extern DESC			Inddes;
	extern char			*AAdbpath;
	extern long			lseek();
	extern char			*trim_relname();
	extern char			*ctime();

	if (AAbequal(r->relid, SYSNAME, s_SYSNAME))
		return (FALSE);

	if (!AAbequal(AAusercode, r->relowner, 2)
	   && (r->relstat & S_PROTRET) && (r->relstat & S_PROTALL))
			return (FALSE);

	printf("\nRelation:\t\t%s\n", trim_relname(r->relid));
	printf("---------\t\t%s\n", AAcustomer());
	if (i = getuser(r->relowner, buf))
	{
		AAsmove("(xx)", buf);
		AAbmove(r->relowner, &buf[1], 2);
	}
	else
	{
		for (i = 0; buf[i] != ':'; i++)
			continue;
		buf[i] = 0;
	}
	printf("Besitzer:\t\t%s\n", buf);
	if (AAdbpath)
		printf("Lokalisation:\t\t%s\n", AAdbpath);
	printf("Domaenanzahl:\t\t%d\n", r->relatts);
	printf("Tupellaenge:\t\t%d\n", r->relwid);
	if (r->relsave)
# ifdef ESER_VMX
		printf("Verfallsdatum:\t\t%s", ctime(r->relsave));
# else
		printf("Verfallsdatum:\t\t%s", ctime(&r->relsave));
# endif
	if (!(r->relstat & S_VIEW))
	{
		printf("Tupelanzahl:\t\t%ld\n", r->reltups);
		printf("Speicherstruktur:\t");
		if ((i = ctoi(r->relspec)) < 0)
		{
			printf("compressed ");
			i = -i;
		}

		switch (i)
		{
		  case M_HEAP:
			printf("HEAP\n");
			break;

		  case M_ISAM:
			printf("ISAM\n");
			break;

		  case M_HASH:
			printf("HASH\n");
			break;

		  default:
			printf("unbekannt %d\n", i);
			break;
		}

		AAdbname(r->relid, r->relowner, tempname);
		if ((j = open(tempname, 0)) >= 0)
		{
			if ((ovflopg = lseek(j, 0L, 2)) >= 0L)
			{
				mainpg = r->relprim;
				if (i == M_ISAM)
					mainpg++;
				ovflopg /= AApgsize;
				ovflopg -= mainpg;
				printf("Pages:\t\t\t%ld + %ld\n", mainpg, ovflopg);
			}
			close(j);
		}
	}

	printf("Relationentyp:\t\t");
	if (r->relstat & S_CATALOG)
		printf("Systemkatalog\n");
	else if (r->relstat & S_VIEW)
		printf("Sicht\n");
	else if (ctoi(r->relindxd) == SECINDEX)
	{
		printf("Index auf ");
		opencatalog(AA_INDEX, 0);
		AAam_setkey(&Inddes, &indkey, r->relowner, IOWNERP);
		AAam_setkey(&Inddes, &indkey, r->relid, IRELIDI);
		if (!AAgetequal(&Inddes, &indkey, &ind, &tid))
			printf("%s\n", trim_relname(ind.irelidp));
		else
			printf("unbekannte Relation\n");
	}
	else
	{
		if (r->relstat & S_DISTRIBUTED)
			printf("entfernte ");
		printf("Nutzerrelation\n");
	}
	printf("Zugriffsrechte:\t\t%s\n", (r->relstat & S_PROTUPS)? "definiert": "keine");
	if (!(r->relstat & S_PROTRET))
		printf("\t\t\tRETRIEVE TO ALL\n");
	if (!(r->relstat & S_PROTALL))
		printf("\t\t\tALL TO ALL\n");
	printf("Integritaeten:\t\t%s\n", (r->relstat & S_INTEG)? "definiert": "keine");
	if (ctoi(r->relindxd) == SECBASE)
	{
		printf("Indexrelationen:\t");
		opencatalog(AA_INDEX, 0);
		AAam_setkey(&Inddes, &indkey, r->relid, IRELIDP);
		AAam_setkey(&Inddes, &indkey, r->relowner, IOWNERP);
		if (i = AAam_find(&Inddes, EXACTKEY, &tid, &limtid, &indkey))
			AAsyserr(11038, i);
		j = FALSE;
		while (!(i = AAam_get(&Inddes, &tid, &limtid, &ind, 1)))
		{
			if (!AAbequal(&indkey, &ind, MAXNAME + 2))
				continue;
			if (j)
				printf("\t\t\t");
			j =TRUE;
			printf("%s\n", trim_relname(ind.irelidi));
		}
		if (i < 0)
			AAsyserr(11039, i);
		if (!j)
			printf("unbekannt\n");
	}

	opencatalog(AA_ATT, 0);
	printf("\n Nummer Domaenname        Typ   Laenge  Schluessel\n");
	printf("---------------------------------------------------\n");
	fflush(stdout);
	seq_init(&Attdes, r);
	while (seq_attributes(&Attdes, r, &att))
	{
		printf("  %3d   %.12s	    %c%8d", att.attid, att.attname,
			typeunconv(att.attfrmt), ctou(att.attfrml));
		if (att.attxtra)
			printf("%7d", att.attxtra);
		putchar('\n');
	}

	printf("---------------------------------------------------\n");
	fflush(stdout);
	return (TRUE);
}
