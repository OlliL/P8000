# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/symbol.h"
# include	"../h/tree.h"
# include	"../h/pipes.h"
# include	"ovqp.h"

prsym(ss)
SYMBOL		*ss;
{
	char			*p;
	register int		i;
	register int		j;
	register SYMBOL		*s;
	char			temp[sizeof (double)];

	s = ss;
	i = s->type;
	j = ctou(s->len);
	p = (char *) s->value;
	if (i == S_VAR)
	{
		i = VAR;	/* execute var portion */
		printf("s_");	/* first part of "s_var" message */
	}
	if (i == VAR)
	{
		/* beware : do S_VAR's have attno's? */
		printf("var:att#=%d:", ((I2TYPE *) p)->i2type);
		i = ((SVAR_NODE *) s)->vtype;
		j = ctou(((SVAR_NODE *) s)->vlen);
		p = (char *) (((SVAR_NODE *) s)->vpoint);
		if (i != CHAR)
		{
			/* move INT & FLOAT to word boundary */
			if (i == INT && j == sizeof (char))
				((I2TYPE *) temp)->i2type = ctoi(*p);
			else
				AAbmove(p, temp, j);
			p = temp;
		}
	}

	printf("%d:%d:value='", i, j);
	switch (i)
	{
	  case AND:
	  case AOP:
	  case BOP:
	  case OR:
	  case RESDOM:
	  case UOP:
		printf("%d (operator)", ((I2TYPE *) p)->i2type);
		break;

	  case INT:
		if (j == sizeof (long))
			printf("%ld", ((I4TYPE *) p)->i4type);
		else
			printf("%d", ((I2TYPE *) p)->i2type);
		break;

	  case FLOAT:
# ifndef NO_F4
		if (j == sizeof (float))
			printf("%10.3f", ((F4TYPE *) p)->f4type);
		else
# endif
			printf("%10.3f", ((F8TYPE *) p)->f8type);
		break;

	  case RESULTID:
	  case SOURCEID:
	  case CHAR:
		prstr(p, j);
		break;

	  case AGHEAD:
	  case BYHEAD:
	  case QLEND:
	  case ROOT:
	  case TREE:
		printf(" (delim)");
		break;

	  case CHANGESTRAT:
	  case REOPENRES:
	  case EXIT:
	  case QMODE:
	  case RETVAL:
	  case USERQRY:
		if (j)
			printf("%d", ((I2TYPE *) p)->i2type);
		printf(" (status)");
		break;

	  default:
		printf("\n\tError in PRSYM: bad type= %d\n",i);
	}
	printf("'\n");
}


prstack(ss)
SYMBOL		*ss;
{
	register SYMBOL		*s;
	register char		*p;

	s = ss;

	if (s->type == CHAR)
	{
		printf("%d:%d:value='", s->type, s->len);
		p = (char *) s->value;
		prstr(((STRINGP *) p)->stringp, ctou(s->len));
		printf("'\n");
	}
	else
		prsym(s);
}


prstr(pt, len)
char	*pt;
int	len;
{
	register char	*p;
	register int	l;

	l = len + 1;
	p = pt;
	while (--l)
	{
		putchar(*p);
		p++;
	}
}
