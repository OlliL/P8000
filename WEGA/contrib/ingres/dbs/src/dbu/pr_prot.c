# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/tree.h"
# include	"../h/aux.h"
# include	"../h/bs.h"
# include	"../h/catalog.h"
# include	"../h/symbol.h"

extern FILE	*Dbs_out;
extern int	Standalone;
extern QTREE	*gettree();	/* gets a tree from AA_TREE catalog[readtree.c] */

static char	*Days[] =
{
	"sunday",
	"monday",
	"tuesday",
	"wednesday",
	"thursday",
	"friday",
	"saturday",
};

struct rngtab
{
	char	relid[MAXNAME];
	char	rowner[2];
	char	rused;
};

struct rngtab	Rangev[MAXVAR + 1];

/*
**  PR_PROT -- print out protection info on a relation
**
**	Prints out a "define permit" statement for
**	each permission on a relation.
**	First calls pr_spec_permit() to print permissions
**	indicated in the relation.relstat bits. Lower level pr_??
**	routines look for these bits, so in the calls to pr_permit
**	for tuples actually gotten from the AA_PROTECT catalog,
**	pr_prot sets the relstat bits, thereby suppressing their special
**	meaning (they are inverse bits: 0 means on).
**
**	Parameters:
**		relid -- non-unique relation name used by user in DBU call
**		rel -- relation tuple
**
**	Returns:
**		0 -- some permissions on rel
**		1 -- no permissions on rel
*/
pr_prot(r)
register struct relation		*r;
{
	extern DESC			Prodes;
	struct tup_id			hitid;
	struct tup_id			lotid;
	struct protect			key;
	struct protect			tuple;
	register int			i;
	int				flag;	/* indicates wether a special
						 * case occurred
						 */

#	ifdef xZTR1
	AAtTfp(11, 0, "PR_PROT: relation \"%s\" owner \"%s\"relstat 0%o\n",
		r->relid, r->relowner, r->relstat);
#	endif

	flag = 0;
	if (r->relstat & S_PROTUPS || !(r->relstat & S_PROTALL)
	   || !(r->relstat & S_PROTRET))
		if (!Standalone)
			fprintf(Dbs_out, "Permissions on %.12s are:\n\n", r->relid);
	/* print out special permissions, if any */
	flag += pr_spec_permit(r, S_PROTALL);
	flag += pr_spec_permit(r, S_PROTRET);

	if (!(r->relstat & S_PROTUPS))
		if (flag)
			return (0);
		else
			return (1);
	opencatalog(AA_PROTECT, 0);

	/* get AA_PROTECT catalog tuples for "r", "owner" */
	AAam_clearkeys(&Prodes);
	AAam_setkey(&Prodes, &key, r->relid, PRORELID);
	AAam_setkey(&Prodes, &key, r->relowner, PRORELOWN);
	if (i = AAam_find(&Prodes, EXACTKEY, &lotid, &hitid, &key))
		AAsyserr(11106, i);
	/* ready for pr_user call to getuser() */
	getuser((char *) -1);
	for ( ; ; )
	{
		if (i = AAam_get(&Prodes, &lotid, &hitid, &tuple, TRUE))
			break;
		/* print out protection info */
		if (!AAkcompare(&Prodes, &tuple, &key))
			/* permission from real protection tuple, concoct
			 * neutral relstat
			 */
			pr_permit(&tuple, r->relstat | S_PROTALL | S_PROTRET);
	}
	if (i != 1)
		AAsyserr(11107, i);

	/* close user file opened by pr_user call to getuser */
	getuser((char *) 0);
}


/*
**  PR_SPEC_PERMIT -- Print out special permissions
**	Prints out permissios indicated by the relation.relstat field bits.
**	Concocts a protection tuple for the permission and assigns a
**	propermid-like number to it for printing. Passes to pr_permit()
**	the concocted tuple, together with a relstat where the appropriate
**	bit is 0, so that the special printing at the lower level pr_???
**	routines takes place.
**
**	Parameters:
**		rel -- relation relation tuple for the permitted relation
**		relst_bit -- if this bit is 0 inthe relstat, prints the query
**				{S_PROTALL, S_PROTRET}
**
**	Returns:
**		1 -- if prints
**		0 -- otherwise
*/
pr_spec_permit(rel, relst_bit)
struct relation		*rel;
int			relst_bit;
{
	register struct relation	*r;
	register struct protect		*p;
	struct protect			prot;

	r = rel;
	if (r->relstat & relst_bit)
		return (0);
	p = &prot;
	p->protree = -1;
	if (relst_bit == S_PROTALL)
		p->propermid = 0;
	else if (relst_bit == S_PROTRET)
		p->propermid = 1;
	else
		AAsyserr(11108, relst_bit);

	AAbmove(r->relid, p->prorelid, MAXNAME);
	AAbmove("  ", p->prouser, 2);
	AAbmove(" ", p->proterm, 1);
	pr_permit(p, (r->relstat | S_PROTRET | S_PROTALL) & ~relst_bit);
	return (1);
}


/*
**  PR_PERMIT -- print out a DEFINE PERMIT query for a protection tuple
**
**	Parameters:
**		prot -- protection tuple
**		relstat -- relstat from relation
*/
pr_permit(prot, relstat)
struct protect	*prot;
int		relstat;
{
	register struct protect		*p;
	register QTREE			*t;
	extern int			Resultvar;
	extern DESC			Prodes;

	p = prot;
	/*
	 * if there is a qualification then
	 * clear range table, then read in protection tree,
	 * the print out range statements
	 * else create single entry range table.
	 */
	clrrange();
	if (p->protree >= 0)
		t = gettree(p->prorelid, p->prorelown, mdPROT, p->protree);
	else
	{
		t = 0;
		declare(0, p->prorelid, p->prorelown);
		p->proresvar = 0;
	}
	if (!Standalone)
		fprintf(Dbs_out, "Permission %d -\n\n", p->propermid);
	pr_range();

#	ifdef xZTR1
	if (AAtTfp(11, 1, "PR_PERMIT: prot="))
	{
		AAprtup(&Prodes, p);
		printf(", Resultvar=%d\n", Resultvar);
	}
#	endif

	/* print out query */
	fprintf(Dbs_out, "define permit ");
	pr_ops(ctoi(p->proopset), relstat);
	fprintf(Dbs_out, "on ");
	pr_rv(Resultvar = ctoi(p->proresvar));
	putc(' ', Dbs_out);
	pr_doms(p->prodomset, relstat);
	fprintf(Dbs_out, "\n\t");
	pr_user(p->prouser);
	pr_term(p->proterm);
	if ((relstat & S_PROTRET) && (relstat & S_PROTALL))
	{
		/* not special case */
		pr_time(p->protodbgn, p->protodend);
		pr_day(ctoi(p->prodowbgn), ctoi(p->prodowend));
	}
	putc('\n', Dbs_out);
	if (t && t->right->sym.type != QLEND)
	{
		fprintf(Dbs_out, "where ");
		pr_qual(t->right);
	}
	if (!Standalone)
		fprintf(Dbs_out, "\n\n\n");
}


/*
**  PR_OPS -- Prints the the operation list defined by a protection opset
**
**	Eliminates the appropriate bits from a copy of the opset while printing
**	out the appropriate operation list.
**
**	Parameters:
**		opset -- protection.opset for the relation
**		relstat
*/
pr_ops(opset, relstat)
int		opset;
int		relstat;
{
	register int	op;
	register int	j;

#	ifdef xZTR1
	AAtTfp(11, 2, "PR_OPS: 0%o\n", opset);
#	endif

	if (!(relstat & S_PROTALL) || opset == -1)
	{
		fprintf(Dbs_out, "all ");
		return;
	}
	if (!(relstat & S_PROTRET))
	{
		fprintf(Dbs_out, "retrieve ");
		return;
	}

	op = (opset & ~PRO_AGGR & ~PRO_TEST) & 077;
	for ( ; ; )
	{
		if (op & (j = PRO_RETR))
			fprintf(Dbs_out, "retrieve");
		else if (op & (j = PRO_REPL))
			fprintf(Dbs_out, "replace");
		else if (op & (j = PRO_DEL))
			fprintf(Dbs_out, "delete");
		else if (op & (j = PRO_APP))
			fprintf(Dbs_out, "append");
		op ^= j;
		if (op)
			fprintf(Dbs_out, ", ");
		else
			break;
	}
	putc(' ', Dbs_out);
}


/*
**  PR_DOMS -- Print domains in permit target list
**
**	Parameters:
**		doms -- an 8 byte integer array; a bit map of the domains
**			if all 8 integers are -1, then "all" is printed fo
**			for the target list
**		relstat
*/
pr_doms(doms, relstat)
short	doms[8];
int	relstat;
{
	register short		*d;
	register int		flag;
	register int		shift;
	int			word;
	char			*rel;
	extern int		Resultvar;

	word = shift = 0;
	d = doms;
	rel = Rangev[Resultvar].relid;

#	ifdef xZTR1
	if (AAtTfp(11, 3, "PR_DOMS: rel=\"%s\" ", rel))
	{
		for (word = 0; word < 8; )
			printf("0%o ", d[word++]);
		word = 0;
		putchar('\n');
	}
#	endif
	if (!(relstat & S_PROTALL) || !(relstat & S_PROTRET))
		return;
	flag = 1;
	for (word = 0; word < 8; word++)
		if (*d++ != -1)
		{
			flag = 0;
			break;
		}

	if (!flag)
	{
		putc('(', Dbs_out);
		for (d = doms, word = 0; word < 8; word++, d++)
		{
			for (shift = 0; shift < 16; shift++, *d >>= 1)
			{
				if (*d & 01)
				{
					if (flag++)
						fprintf(Dbs_out, ", ");
					pr_attname(rel, word * 16 + shift);
				}
			}
		}
		putc(')', Dbs_out);
	}
}


/*
**  PR_USER -- prints out permitted user's name
**
**	Parameters:
**		user -- 2 char array, user's usercode as in
**			AA_USERS
*/
pr_user(user)
char		user[2];
{
	char			buf[MAXFIELD + 1];
	register char		*c;
	register char		*u;

#	ifdef xZTR1
	AAtTfp(11, 4, "PR_USER: \"%c%c\"\n", user[0], user[1]);
#	endif

	c = buf;
	u = user;
	fprintf(Dbs_out, "to ");
	if (AAbequal(u, "  ", 2))
		fprintf(Dbs_out, "all ");
	else
	{
		if (getuser(u, c))
		{
			fprintf(Dbs_out, "%c%c ", u[0], u[1]);
			return;
		}
		while (*c != ':' && *c != '\n')
		{
			putc(*c, Dbs_out);
			c++;
		}
		putc(' ', Dbs_out);
	}
}


/*
**  PR_TIME -- Prints out clock time range access is allowed
**
**	Parameters:
**		bgn, end -- begin end times in seconds (if all day, returns)
*/
pr_time(bgn, end)
int		bgn;
int		end;
{
	char		time[3];
	register char	*t;
	register int	b;
	register int	e;

	t = time;
	b = bgn;
	e = end;
#	ifdef xZTR1
	AAtTfp(11, 5, "PR_TIME: bgn=%d, end=%d\n", b, e);
#	endif
	if (!b && e == 24 * 60)
		return;
	fprintf(Dbs_out, "from %d:", b / 60);
	AA_itoa(b % 60, t);
	if (!t[1])
		putc('0', Dbs_out);
	fprintf(Dbs_out, "%s to %d:", t, e / 60);
	AA_itoa(e % 60, t);
	if (!t[1])
		putc('0', Dbs_out);
	fprintf(Dbs_out, "%s ", t);
}


/*
**  PR_DAY -- Prints day range permitted
**
**	Parameters:
**		bgn, end -- bgn end days[0..6] (if all week returns)
*/
pr_day(bgn, end)
int		bgn;
int		end;
{
#	ifdef xZTR1
	AAtTfp(11, 6, "PR_DAY: bgn=%d, end=%d\n", bgn, end);
#	endif
	if (!bgn && end >= 6)
		return;
	fprintf(Dbs_out, "on %s to %s ", Days[bgn], Days[end]);
}


/*
**  PR_TERM -- Print terminal from which access permitted
**
**	Parameters:
**		term -- 1 char terminal id as in /etc/tty* (if ' ' the returns)
*/
pr_term(term)
char		*term;
{
#	ifdef xZTR1
# ifdef BIT_32
	AAtTfp(11, 7, "PR_TERM: '%.4s'\n", term);
# else
	AAtTfp(11, 7, "PR_TERM: '%.2s'\n", term);
# endif
#	endif

	if (*term != ' ')
# ifdef BIT_32
		fprintf(Dbs_out, "at tty%.4s", term);
# else
		fprintf(Dbs_out, "at tty%.2s", term);
# endif
}
