# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/symbol.h"
# include	"../h/tree.h"
# include	"../h/aux.h"

extern FILE		*Dbs_out;
extern char		*pr_trim();
extern char		*resultres();

/* This structure must be IDENTICAL to that in readtree.c */
struct rngtab
{
	char	relid[MAXNAME];
	char	rowner[2];
	char	rused;
};

extern struct rngtab	Rangev[];
int			Resultvar;
DESC			Attdes;
static int		Tl_elm;
static int		Dom_num;
extern char		*Resrel;

struct tab
{
	int	t_opcode;
	char	*t_string;
};

static struct tab	Uop_tab[] =
{
	(int) opPLUS,		"+ ",
	(int) opMINUS,		"- ",
	(int) opNOT,		"not[ERROR]",
	(int) opATAN,		"atan",
	(int) opCOS,		"cos",
	(int) opGAMMA,		"gamma",
	(int) opLOG,		"log",
	(int) opASCII,		"ascii",
	(int) opSIN,		"sin",
	(int) opSQRT,		"sqrt",
	(int) opABS,		"abs",
	(int) opEXP,		"exp",
	(int) opINT1,		"int1",
	(int) opINT2,		"int2",
	(int) opINT4,		"int4",
	(int) opFLT4,		"float4",
	(int) opFLT8,		"float8",
};

static struct tab	Bop_tab[] =
{
	(int) opADD,		"+",
	(int) opSUB,		"-",
	(int) opMUL,		"*",
	(int) opDIV,		"/",
	(int) opPOW,		"**",
	(int) opEQ,		"=",
	(int) opNE,		"!=",
	(int) opLT,		"<",
	(int) opLE,		"<=",
	(int) opGT,		">",
	(int) opGE,		">=",
	(int) opMOD,		"%",
};

static struct tab	Cop_tab[] =
{
	(int) opDBA,		"dba",
	(int) opUSERCODE,	"usercode",
	(int) opDATE,		"date",
	(int) opTIME,		"time",
};

static struct tab	Aop_tab[] =
{
	(int) opCNT,		"count",
	(int) opCNTU,		"countu",
	(int) opSUM,		"sum",
	(int) opSUMU,		"sumu",
	(int) opAVG,		"avg",
	(int) opAVGU,		"avgu",
	(int) opMIN,		"min",
	(int) opMAX,		"max",
	(int) opANY,		"any",
};

/*
**  PR_TREE.C -- Query tree printing routines
**
**	Defines:
**		pr_tree() -- print out query tree
**		pr_qual() -- print out qualofocation tree
**		pr_attname() -- print out attribute name of domain
**
**
**
** PR_TREE:	tl_clause ROOT tl_clause
**
**	prints out a tree assuming a mdVIEW-like mode
*/
pr_tree(root)
QTREE			*root;
{

#	ifdef xZTR1
	AAtTfp(10, -1, "PR_TREE: root 0%o Resultvar %d Resrel %s\n",
		root, Resultvar, Resrel);
#	endif

	fprintf(Dbs_out, "%s ", pr_trim(resultres(), MAXNAME));

	pr_dom_init();
	Tl_elm = 0;

	/* print target list */
	fprintf(Dbs_out, "\n(\n");
	pr_tl_clause(root->left, TRUE);
	fprintf(Dbs_out, "\n)\n");

	/* print qualification */
	if (root->right->sym.type != QLEND)
	{
                fprintf(Dbs_out, "where ");
		pr_qual(root->right);
	}
}


/*
** tl_clause:	TREE
**	|	tl_clause RESDOM expr
**
** target_flag = "in a target list (as opposed to in a by list)"
*/
pr_tl_clause(t_l, target_flag)
QTREE			*t_l;
int			target_flag;
{
	register int		fl;

	fl = target_flag;

#	ifdef xZTR1
	AAtTfp(10, 1, "tl_clause target %d Tl_elm %d\n", fl, Tl_elm);
#	endif

	if (t_l->sym.type != TREE)
	{
		pr_tl_clause(t_l->left, fl);
		if (Tl_elm)
		{
			fprintf(Dbs_out, ", ");
			if (fl)
				putc('\n', Dbs_out);
		}
		/* print out info on result variable */
		pr_resdom(t_l, fl);
		pr_expr(t_l->right);
		Tl_elm++;
	}
}


/*
** print out info on a result attribute.
** this will be done only if the RESDOM node
** is inside a target_list and if the Resultvar >= 0.
** Resultvar == -1 inside a target list indicates that this is
** a retrieve to terminal.
*/
pr_resdom(resdom, target_flag)
QTREE			*resdom;
int			target_flag;
{

#	ifdef xZTR1
	AAtTfp(10, 2, "PR_RESDOM: target_flag %d\n", target_flag);
#	endif

	if (target_flag)
	{
		fprintf(Dbs_out, "\t");
		pr_attname(resultres(), ((RES_NODE *) resdom)->resno);
		fprintf(Dbs_out, " = ");
	}
}


/*
** give a relation name, and the attribute number of that
** relation, looks in the attribute relation for the name of the
** attribute.
*/
pr_attname(rel, attno)
char		*rel;
short		attno;
{
	struct tup_id		tid;
	struct attribute	key;
	struct attribute	tuple;
	register int		i;

#	ifdef xZTR1
	AAtTfp(10, 3, "PR_ATTNAME: rel %s attno %d\n", rel, attno);
#	endif

	if (!attno)
	{
		fprintf(Dbs_out, "tid");
		return;
	}
	opencatalog(AA_ATT, 0);
	AAam_clearkeys(&Attdes);
	AAam_setkey(&Attdes, &key, rel, ATTRELID);
	AAam_setkey(&Attdes, &key, &attno, ATTID);
	i = AAgetequal(&Attdes, &key, &tuple, &tid);
	if (i)
		AAsyserr(11111, i, rel, attno);
	fprintf(Dbs_out, "%s", pr_trim(tuple.attname, MAXNAME));
}


/*
** expr:	VAR
**	|	expr BOP expr
**	|	expr UOP
**	|	AOP AGHEAD qual
**		  \
**		   expr
**	|	BYHEAD	  AGHEAD qual
**	        /    \
**	tl_clause     AOP
**			\
**			 expr
**	|	INT
**	|	FLOAT
**	| 	CHAR
**	|	COP
**
*/
pr_expr(ex)
QTREE			*ex;
{
	register int			op;
	register int			tl_elm;
	register QTREE			*e;

	e = ex;
	switch (e->sym.type)
	{
	  case VAR:
		pr_var(e);
		break;

	  case BOP:
		if (((OP_NODE *) e)->opno == (int) opCONCAT)
		{
			fprintf(Dbs_out, "AAconcat(");
			pr_expr(e->left);
			fprintf(Dbs_out, ", ");
			pr_expr(e->right);
			putc(')', Dbs_out);
		}
		else
		{
			putc('(', Dbs_out);
			pr_expr(e->left);
			pr_op(BOP, ((OP_NODE *) e)->opno);
			pr_expr(e->right);
			putc(')', Dbs_out);
		}
		break;

	  case UOP:
		if ((op = ((OP_NODE *) e)->opno) == ((int) opMINUS) || op == ((int) opPLUS) || op == ((int) opNOT))
		{
			pr_op(UOP, ((OP_NODE *) e)->opno);
			pr_expr(e->left);
			putc(')', Dbs_out);
		}
		else
		{
			/* functional operators */
			pr_op(UOP, ((OP_NODE *) e)->opno);
			pr_expr(e->left);
			putc(')', Dbs_out);
		}
		break;

	  case AGHEAD:
		if (e->left->sym.type == AOP)
		{
			/* simple aggregate */
			pr_op(AOP, ((OP_NODE *) e->left)->opno);
			pr_expr(e->left->right);
			if (e->right->sym.type != QLEND)
			{
				fprintf(Dbs_out, "\where ");
				pr_qual(e->right);
			}
			putc(')', Dbs_out);
		}
		else
		{
			/* aggregate function */
			pr_op(AOP, ((OP_NODE *) e->left->right)->opno);
			pr_expr(e->left->right->right);
			fprintf(Dbs_out, " by ");
			/* avoid counting target list elements
			 * in determining wether to put out
			 * commas after list's elements
			 */
			tl_elm = Tl_elm;
			Tl_elm = 0;
			pr_tl_clause(e->left->left, FALSE);
			Tl_elm = tl_elm;
			if (e->right->sym.type != QLEND)
			{
				fprintf(Dbs_out, "\n\t\twhere ");
				pr_qual(e->right);
			}
			putc(')', Dbs_out);
		}
		break;

	  case INT:
	  case FLOAT:
	  case CHAR:
		pr_const(e);
		break;

	  case COP:
		pr_op(COP, ((OP_NODE *) e)->opno);
		break;

	  default:
		AAsyserr(11112, e->sym.type);
	}
}


pr_const(ct)
QTREE			*ct;
{
	register QTREE			*c;
	register char			*cp;
	register int			i;
	char				ch;
	double				d;

	c = ct;
	switch (c->sym.type)
	{
	  case INT:
		if (c->sym.len == sizeof (char))
			fprintf(Dbs_out, "%d", ctoi(((I1TYPE *) c->sym.value)->i1type));
		else if (c->sym.len == sizeof (short))
			fprintf(Dbs_out, "%d", c->sym.value[0]);
		else	/* i4 */
			fprintf(Dbs_out, "%d", ((I4TYPE *) c->sym.value)->i4type);
		break;

	  case FLOAT:
# ifndef NO_F4
		if (c->sym.len == sizeof (float))
			d = ((F4TYPE *) c->sym.value)->f4type;
		else
# endif
			d = ((F8TYPE *) c->sym.value)->f8type;
		fprintf(Dbs_out, "%-10.3f", d);
		break;

	  case CHAR:
		fprintf(Dbs_out, "\"");
		cp = (char *) c->sym.value;
		for (i = c->sym.len; i--; cp++)
		{
			if (any(*cp, "\"\\[]*?") == TRUE)
				putc('\\', Dbs_out);

			if (*cp >= ' ')
			{
				putc(*cp, Dbs_out);
				continue;
			}
			/* perform pattern matching character replacement */
			switch (*cp)
			{
			  case PAT_ANY:
				ch = '*';
				break;

			  case PAT_ONE:
				ch = '?';
				break;

			  case PAT_LBRAC:
				ch = '[';
				break;

			  case PAT_RBRAC:
				ch = ']';
				break;

			  default:
				ch = *cp;
			}
			putc(ch, Dbs_out);
		}
		putc('"', Dbs_out);
		break;

	  default:
		AAsyserr(11113, c->sym.type);
	}
}


/*
** PR_OP: print out operator of a certain type
*/
pr_op(op_type, op_code)
int		op_type;
int		op_code;
{
	register struct tab	*s;
	register int		opc;

	opc = op_code;
	switch (op_type)
	{
	  case UOP:
		s = &Uop_tab[opc];
		fprintf(Dbs_out, "%s(", s->t_string);
		break;

	  case BOP:
		s = &Bop_tab[opc];
		fprintf(Dbs_out, " %s ", s->t_string);
		break;

	  case AOP:
		s = &Aop_tab[opc];
		fprintf(Dbs_out, "%s(", s->t_string);
		break;

	  case COP:
		s = &Cop_tab[opc];
		fprintf(Dbs_out, "%s", s->t_string);
		break;
	}
	if (opc != s->t_opcode)
		AAsyserr(11114, op_type,
		s->t_opcode);
}


/*
** print a VAR node: that is, a var.attno pair
** at present the var part is the relation name over which var
** ranges.
*/
pr_var(v)
register QTREE		*v;
{
	register int		i;

#	ifdef xZTR1
	AAtTfp(10, 4, "pr_var(var=%d)\n", v);
#	endif

	pr_rv(((VAR_NODE *) v)->varno);
	putc('.', Dbs_out);
	i = ((VAR_NODE *) v)->varno;
	pr_attname(Rangev[i].relid, ((VAR_NODE *) v)->attno);
}


/*
** qual:	QLEND
**	|	q_clause AND qual
**
*/
pr_qual(q)
QTREE			*q;
{
	pr_q_clause(q->left);
	putc('\n', Dbs_out);
	if (q->right->sym.type != QLEND)
	{
		fprintf(Dbs_out, "and ");
		pr_qual(q->right);
	}
}


/*
** q_clause:	q_clause OR q_clause
**	|	expr
*/
pr_q_clause(qc)
QTREE			*qc;
{
	register QTREE			*q;


	q = qc;
	if (q->sym.type == OR)
	{
		pr_q_clause(q->left);
		fprintf(Dbs_out, " or ");
		pr_q_clause(q->right);
	}
	else
		pr_expr(q);
}


char	*pr_trim(string, len)
char	*string;
int	len;
{
	static char	buf[30];
	register int	l;
	register char	*s;

	s = string;
	l = len;
	AAbmove(s, buf, l);
	for (s = buf; l && *s != ' ' && *s; --l)
		s++;
	*s = '\0';
	return (buf);
}


pr_dom_init()
{
	Dom_num = 0;
}


pr_ddom()
{
	fprintf(Dbs_out, "d%d = ", Dom_num++);
}


pr_range()
{
	register int	i;

	for (i = 0; i <= MAXVAR; i++)
	{
		if (Rangev[i].rused)
		{
			fprintf(Dbs_out, "range of ");
			pr_rv(i);
			fprintf(Dbs_out, " is %s\n",
			  pr_trim(Rangev[i].relid, MAXNAME));
		}
	}
}


pr_rv(re)
int	re;
{
	register int	i;
	register int	j;
	register int	ch;

	i = re;
	ch = Rangev[i].relid[0];

#	ifdef xZTR1
	AAtTfp(10, 6, "pr_rv(%d) ch '%c'\n", i, ch);
#	endif

	for (j = 0; j <= MAXVAR; j++)
	{
		if (!Rangev[j].rused)
			continue;
		if (ch == Rangev[j].relid[0])
			break;
	}
	if (j < i)
		fprintf(Dbs_out, "rv%d", i);
	else
		fprintf(Dbs_out, "%c", ch);
}


char	*resultres()
{
#	ifdef xZTR1
	AAtTfp(10, 5, "RESULTRES: Resultvar %d, Resrel %s\n", Resultvar, Resrel);
#	endif
	if (Resultvar > 0)
		return (Rangev[Resultvar].relid);
	if (!Resrel)
		AAsyserr(11115);

	return (Resrel);
}


any(c, s)
register int	c;
register char	*s;
{
	for ( ; *s; )
		if (*s++ == c)
			return (TRUE);

	return (FALSE);
}
