# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/pipes.h"
# include	"../h/tree.h"
# include	"../h/symbol.h"
# include	"parser.h"

# define	BADCOP	    -1	/* error return on lookup	*/
int		Eql;		/* indicates EQL preprocessor on */

/*
** TREE
**  	FUNCTION TO ADD NODE TO QUERY TREE
**	RETURN VALUE IS POINTER TO NODE JUST CREATED
*/
QTREE			*tree(lptr, rptr, typ, len, valu1, attnum1)
QTREE			*lptr;
QTREE			*rptr;
char			typ;
int			len;
char			*valu1;
struct atstash		*attnum1;
{
	register struct atstash		*attnum;
	register QTREE			*tptr;
	register char			*valu;
	extern char			*need();
	extern char			*AAbmove();
	extern QTREE			*norm();

#	ifdef xPTM
	if (AAtTf(76, 2))
		timtrace(5);
#	endif

	attnum = attnum1;
	valu = valu1;
	tptr = (QTREE *) need(Qbuf, len + QT_HDR_SIZ);
	tptr->left = lptr;
	tptr->right = rptr;
	tptr->sym.type = typ;
	tptr->sym.len = len;
	switch (typ)
	{
	  case VAR:
		((VAR_NODE *) tptr)->varno = (short) valu;
		((VAR_NODE *) tptr)->attno = attnum->atbid;
		((VAR_NODE *)tptr )->frmt = attnum->atbfrmt;
		((VAR_NODE *) tptr)->frml = attnum->atbfrml;
		((VAR_NODE *) tptr)->valptr = (char *) 0;
		break;

	  case ROOT:
	  case AGHEAD:
	  case BYHEAD:
	  case AND:
	  case OR:
	  case QLEND:
		break;

	  case UOP:
	  case BOP:
		((OP_NODE *) tptr)->opno = (short) valu;
		format(tptr);
		break;

	  case COP:
		if ((((OP_NODE *) tptr)->opno = getcop(valu)) == BADCOP)
			/* bad const operator */
			yyerror(BADCONSTOP, valu, (char *) 0);
		break;

	  case AOP:
		format(tptr->right);
		((AOP_NODE *) tptr)->agfrmt = Trfrmt;
		((AOP_NODE *) tptr)->agfrml = Trfrml;

	  case RESDOM:
		((RES_NODE *) tptr)->resno = (short) valu;
		format(tptr);
		((VAR_NODE *) tptr)->frmt = Trfrmt;
		((VAR_NODE *) tptr)->frml = Trfrml;
		break;

	  default:
		/* INT, FLOAT, CHAR */
		AAbmove(valu, tptr->sym.value, ctou(len));
		break;
	}
#	ifdef xPTM
	if (AAtTf(76, 2))
		timtrace(6);
#	endif
	return (tptr);
}

/*
** TREEPR
**  	TREE PRINT ROUTINE
**	CREATES STRING WHERE EACH TARGET LIST ELEMENT AND QUALIFICATION
**	CLAUSE IS IN POLISH POSTFIX FORM BUT OVERALL LIST IS IN INFIX
**	FORM
*/
treepr(p1)
QTREE			*p1;
{
	register QTREE			*p;
	register int			l;
	extern struct pipfrmt		Pipebuf;

	p = p1;
	if (p->sym.type == ROOT || p->sym.type == BYHEAD)
	{
		writesym(TREE, 0, (char *) 0);
#		ifdef	xPTR2
		AAtTfp(26, 8, "TREE node\n");
#		endif
	}
	if (p->left)
		treepr(p->left);
	if (p->right)
		treepr(p->right);

	if (p->sym.type <= CHAR)
	{
#		ifdef	xPTR2
		if (AAtTf(26, 9))
			nodewr(p);
#		endif
		wrpipe(P_NORM, &Pipebuf, W_down, &(p->sym), TYP_LEN_SIZ);
		if (l = ctou(p->sym.len))
			wrpipe(P_NORM, &Pipebuf, W_down, p->sym.value, l);
		return;
	}
	AAsyserr(18023);
}

# ifdef	xPTR1
/*
** NODEWR
**	printf a tree node for debugging purposes
*/
nodewr(p1)
QTREE			*p1;
{
	register QTREE			*p;
	char				str[MAXFIELD + 1];
	register char			*ptr;
	extern char			*AAbmove();

	p = p1;
	printf("addr=0%o, l=0%o, r=0%o, typ=%d, len=%d:\n",
		p, p->left, p->right, p->sym.type, p->sym.len);
	switch (p->sym.type)
	{
	  case VAR:
		printf("\t\tvarno=%d, attno=%d, frmt=%d, frml=%d\n", ((VAR_NODE *) p)->varno, ((VAR_NODE *) p)->attno, ((VAR_NODE *) p)->frmt, ((VAR_NODE *) p)->frml);
		break;

	  case AOP:
		printf("\t\taop=%d, frmt=%d, frml=%d\n", ((RES_NODE *) p)->resno, ((VAR_NODE *) p)->frmt, ((VAR_NODE *) p)->frml);
		break;

	  case RESDOM:
		printf("\t\trsdmno=%d, frmt=%d, frml=%d\n", ((RES_NODE *) p)->resno, ((VAR_NODE *) p)->frmt, ((VAR_NODE *) p)->frml);
		break;

	  case CHAR:
		ptr = AAbmove(p->sym.value, str, ctou(p->sym.len));
		*ptr = 0;
		printf("\t\tstring=%s\n", str);
		break;

	  case INT:
		ptr = (char *) p->sym.value;
		switch (p->sym.len)
		{
		  case sizeof (short):
			printf("\t\tvalue=%d\n", ((I2TYPE *) ptr)->i2type);
			break;

		  case sizeof (long):
			printf("\t\tvalue=%ld\n", ((I4TYPE *) ptr)->i4type);
			break;
		}
		break;

	  case FLOAT:
		ptr = (char *) p->sym.value;
# ifndef NO_F4
		switch (p->sym.len)
		{
		  case sizeof (float):
			printf("\t\tvalue=%f\n", ((F4TYPE *) ptr)->f4type);
			break;

		  case sizeof (double):
# endif
			printf("\t\tvalue=%f\n", ((F8TYPE *) ptr)->f8type);
# ifndef NO_F4
			break;
		}
# endif
		break;

	  case UOP:
	  case BOP:
	  case COP:
		printf("\t\top=%d\n", ((OP_NODE *) p)->opno);
		break;
	}
}
# endif

/*
** WINDUP
**	assign resno's to resdoms of an agg fcn
*/
windup(ptr)
QTREE			*ptr;
{
	register int			tot;
	register int			kk;
	register QTREE			*t;

	/* COUNT THE RESDOM'S OF THIS TARGET LIST */
	kk = 1;
	for (t = ptr; t; t = t->left)
		kk++;
	tot = 1;
	for (t=ptr; t;t = t->left)
		((RES_NODE *) t)->resno = kk - tot++;
}

/*
** ADDRESDOM - makes a new entry for the target list
**
**	Trname must contain the name of the resdom to
**	use for the header, create and Rsdmno for append, replace
**
**	the parameters are pointers to the subtrees to be
**	suspended from the node
*/
QTREE			*addresdom(lptr, rptr)
QTREE			*lptr, *rptr;
{
	extern QTREE			*tree();
	register QTREE			*rtval;
	register struct atstash		*aptr;
	char				buf[10];	/* buffer type and length in ascii for dbu */
	extern struct atstash		*attlookup();

	switch (Opflag)
	{
	  case mdRETR:
	  case mdRET_UNI:
	  case mdVIEW:
		Rsdmno++;
		if (Rsdmno >= MAXDOM)
			/* too many resdoms */
			yyerror(RESXTRA, (char *) 0);
		rtval = tree(lptr, rptr, RESDOM, RES_SIZ, CAST(Rsdmno));
		if (!Eql || Resrng)
		{
			/* buffer info for header or CREATE */
			setp(Trname);
			buf[0] = ctou(Trfrmt);
			AAsmove(AA_iocv(ctou(Trfrml)), &buf[1]);
			setp(buf);
		}
		break;

	  default:
		/*
		** for append and replace, the result domain
		** number is determined by the location of
		** the attribute in the result relation
		*/
		if (AAsequal(Trname, "tid"))
			/* attrib not found */
			yyerror(NOATTRIN, Trname, Resrng->relnm, (char *) 0);
#		ifdef	DISTRIB
		if (AAsequal(Trname, "sid"))
			/* attrib not found */
			yyerror(NOATTRIN, Trname, Resrng->relnm, (char *) 0);
#		endif
		aptr = attlookup(Resrng, Trname);
		Rsdmno = aptr->atbid;
		rtval = tree(lptr, rptr, RESDOM, RES_SIZ, CAST(Rsdmno));
		if (Opflag != (int) mdPROT)	/* INTEGRITY not possible here */
			attcheck(aptr);
		break;
	}
	return (rtval);
}

/*
** GETCOP
**	routine to lookup 'string' in constant operators table
**	constant table is declared in tables.y
**	structure is defined in ../parser.h
*/
getcop(string)
char	*string;
{
	register struct constop	*cpt;
	register char		*sptr;
	extern struct constop	Coptab[];

	sptr = string;
	for (cpt = Coptab; cpt->copname; cpt++)
		if (AAsequal(sptr, cpt->copname))
			return (cpt->copnum);
	return (BADCOP);
}
