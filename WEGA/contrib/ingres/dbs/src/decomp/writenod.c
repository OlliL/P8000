# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/symbol.h"
# include	"../h/tree.h"
# include	"../h/pipes.h"
# include	"decomp.h"

writenod(q)
register QTREE		*q;
{
	register int			t;
	register SYMBOL			*s;
	register char			*cp;
	register int			l;

	s = &q->sym;
	t = s->type;
	l = ctou(s->len);

	printf("0%o/\t0%o:\t0%o:\t", q, q->left, q->right);
	printf("%2d, %2d\t", t, l);

	cp = (char *) s->value;
	switch (t)
	{
	  case VAR:
		printf("VAR\t%d,%d,%d,%d,0%o/", ((VAR_NODE *) q)->varno,
			ctou(((VAR_NODE *) q)->attno), ((VAR_NODE *) q)->frmt,
			ctou(((VAR_NODE *) q)->frml), ((VAR_NODE *) q)->valptr);
		if (ctoi(((VAR_NODE *) q)->varno) < 0)
			writenod(((VAR_NODE *) q)->valptr);
		else
			putchar('\n');
		return;

	  case AND:
		printf("AND");
		goto root_node;

	  case ROOT:
		printf("ROOT");
		goto root_node;

	  case AGHEAD:
		printf("AGHEAD");
root_node:
		printf("\t%d,%d,0%o,0%o", ((ROOT_NODE *) q)->tvarc,
			((ROOT_NODE *) q)->lvarc, ((ROOT_NODE *) q)->lvarm,
			((ROOT_NODE *) q)->rvarm);
		if (t != AND)
			printf(",(%d)", ((ROOT_NODE *) q)->rootuser);
		break;

	  case AOP:
		printf("AOP");
		goto resdom_node;

	  case RESDOM:
		printf("RESDOM");
resdom_node:
		printf("\t%d,%d,%d", ((RES_NODE *) q)->resno,
			((VAR_NODE *) q)->frmt, ctou(((VAR_NODE *) q)->frml));
		if (t == AOP)
			printf("(%d,%d)",
				((AOP_NODE *) q)->agfrmt,
				ctou(((AOP_NODE *) q)->agfrml));
		break;

	  case UOP:
		printf("UOP");
		goto bop_node;

	  case COP:
		printf("COP");
		goto bop_node;

	  case INT:
		printf("INT");
		goto bop_node;

	  case BOP:
		printf("BOP");
bop_node:
		putchar('\t');
		switch (l)
		{
		  case sizeof (char):
		  case sizeof (short):
			printf("%d", s->value[0]);
			break;

		  case sizeof (long):
			printf("%ld", ((I4TYPE *) cp)->i4type);
		}
		break;

	  case FLOAT:
		printf("FLOAT\t");
# ifndef NO_F4
		if (l == sizeof (float))
			printf("%.10f", ((F4TYPE *) cp)->f4type);
		else
# endif
			printf("%.10f", ((F8TYPE *) cp)->f8type);
		break;

	  case CHAR:
		printf("CHAR\t");
		while (l--)
		{
			putchar(*cp);
			cp++;
		}
		break;

	  case TREE:
		printf("TREE");
		break;

	  case OR:
		printf("OR");
		break;

	  case QLEND:
		printf("QLEND");
		break;

	  case BYHEAD:
		printf("BYHEAD");
		break;

	  default:
		printf("Error in writenod");
	}
	printf("/\n");
}
