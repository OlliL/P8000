# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/catalog.h"
# ifdef BC
# define	AND		 0
# define	AOP		 4	/* AGGREGATE OPERATORS		*/
# define	BOP		 3	/* BINARY OPERATORS		*/
# define	COP		 5	/* CONSTANT OPERATORS		*/
# define	OR		 1
# define	RESDOM		11	/* RESULT DOMAIN NUMBER		*/
# define	UOP		 2	/* UNARY OPERATORS		*/
# define	mdDEL		 4
# define	mdREPL		 3
# define	opABS		10
# define	opADD		 0
# define	opANY		 8
# define	opASCII		 7
# define	opATAN		 3
# define	opAVG		 4
# define	opAVGU		 5
# define	opCNT		 0
# define	opCNTU		 1
# define	opCONCAT	12
# define	opCOS		 4
# define	opDBA		 0
# define	opDIV		 3
# define	opEQ		 5
# define	opEXP		11
# define	opFLT4		15
# define	opFLT8		16
# define	opGAMMA		 5
# define	opGE		10
# define	opGT		 9
# define	opINT1		12
# define	opINT2		13
# define	opINT4		14
# define	opLE		 8
# define	opLOG		 6
# define	opLT		 7
# define	opMAX		 7
# define	opMIN		 6
# define	opMINUS		 1
# define	opMOD		11
# define	opMUL		 2
# define	opNE		 6
# define	opNOT		 2
# define	opPLUS		 0
# define	opPOW		 4
# define	opSIN		 8
# define	opSQRT		 9
# define	opSUB		 1
# define	opSUM		 2
# define	opSUMU		 3
# define	opUSERCODE	 1
# else
# include	"../h/symbol.h"
# endif
# include	"../h/tree.h"
# include	"../h/access.h"
# include	"../h/pipes.h"
# include	"ovqp.h"

struct stacksym	Stack[STACKSIZ];	/* stack for OVQP interpreter */

extern double	pow();
extern double	sqrt();
extern double	log();
extern double	exp();
extern double	sin();
extern double	cos();
extern double	atan();
extern double	gamma();

/*
** INTERPRET
**
**	 Processes the retrieved tuple from the Source relation
**	 according to the symbols in the list.  Recognition
**	 of characteristic delimiters and separators initiates
**	 action appropriate to a target list or qualification list
**	 as the case may be.
**
**	 Between delimiters, the symbol list is expected to be in
**	 Polish postfix form.  A qualification list is further
**	 expected to be in conjunctive normal form, with Boolean
**	 operators infixed.
*/
SYMBOL	 *interpret(list)
SYMBOL	**list;	/* ptr to list of sym pointers */
{
	register struct stacksym	*tos;
	register short			*val1;	/* ptr to value of operand 1 */
	register short			*val2;	/* ptr to value of operand 2 */
	register SYMBOL			*op1;	/* operand popped off stack  */
	register SYMBOL			*op2;	/* operand popped off stack  */
	register int			opval;
	int				optype;
	int				l1;
	char				*s1;
	int				byflag;
	long				hitid;
	extern char			*AAusercode;
	extern				ov_err();
	int				cb_mark;
	extern char			*Ovqpbuf;

#	ifdef xOTR1
	AAtTfp(23, 0, "INTERP: list=0%o\n", list);
#	endif
#	ifdef xOTM
	if (AAtTf(76, 2))
		timtrace(11, 0);
#	endif

	byflag = (list == Bylist);	/* set byflag if aggregate function */
	tos = Stack - 1;
	/* reset the concat and ascii operator buffer */
	seterr(Ovqpbuf, CBUFULL, ov_err);
	cb_mark = markbuf(Ovqpbuf);

loop:
#	ifdef xOTR1
	if (AAtTf(23, 1) && tos >= Stack)
	{
		printf("\ttops of stack=");
		prstack(tos);	/* print top of stack element */
	}
#	endif
	/* check for stack overflow */
	l1 = getsymbol(++tos, &list);

	if (l1)
	{
#		ifdef xOTM
		if (AAtTf(76, 2))
			timtrace(12, 0);
#		endif

		freebuf(Ovqpbuf, cb_mark);
		return ((SYMBOL *) tos);
	}
	optype = ((SYMBOL *) tos)->type;
	opval = *((SYMBOL *) tos)->value;
	tos--;	/* assume that stack will be poped */

	switch (optype)
	{
	  case INT:
	  case FLOAT:
	  case CHAR:
		++tos;		/* just leave symbol on stack */
		goto loop;

	  case COP:
		++tos;		/* new symbol goes on stack */
		((SYMBOL *) tos)->type = CHAR;
		val1 = ((SYMBOL *) tos)->value;
		switch (opval)
		{
		  case opDBA:
			((STRINGP *) val1)->stringp = AAdmin.adhdr.adowner;
			((SYMBOL *) tos)->len = sizeof (short);
			goto loop;

		  case opUSERCODE:
			((STRINGP *) val1)->stringp = AAusercode;
			((SYMBOL *) tos)->len = sizeof (short);
			goto loop;
		}

	  case AND:		/* if top value false return immediately */
		if (!*((SYMBOL *) tos)->value)
		{
#			ifdef xOTM
			if (AAtTf(76, 2))
				timtrace(12, 0);
#			endif
			freebuf(Ovqpbuf, cb_mark);
			return ((SYMBOL *) tos);
		}
		else
			--tos;
		freebuf(Ovqpbuf, cb_mark);
		goto loop;

	  case OR:		/* if top value is true then skip to
				** end of disjunction. */
		if (*((SYMBOL *) tos)->value)
		{
			tos++;
			do
			{
				getsymbol(tos, &list);
			} while (((SYMBOL *) tos)->type != AND);
			optype = AND;
			--tos;
		}
		--tos;
		goto loop;

	  case RESDOM:
		freebuf(Ovqpbuf, cb_mark); /* init the concat and ascii buffer */
		val1 = ((SYMBOL *) tos)->value;
		if (Result)
		{
			if (opval)	/* if gt zero then opval represents a real domain */
			{
				if (byflag)
					opval++;	/* skip over count field for ag functs */
				rcvt(tos, Result->relfrmt[opval], Result->relfrml[opval]);
				tout(tos, Outtup+Result->reloff[opval], Result->relfrml[opval]);
			}
			else	/* opval refers to the tid and this is an update */
			{
				Uptid = ((I4TYPE *) val1)->i4type;	/* copy tid */
				if (Ov_qmode == ((int) mdREPL) || (Diffrel && Ov_qmode == ((int) mdDEL) && ctoi(Result->reldum.relindxd) == SECBASE ))
				{
					/* Origtup must be left with the orig
					** unaltered tuple, and Outtup must
					** be initialized with the orig tuple.
					**
					** Outtup only matters with REPL.
					** Scan() sets up Origtup so when
					** Source == Result, origtup is already
					** correct.
					*/

					if (Diffrel)
					{
						if (l1 = AAam_get(Result, &Uptid, &hitid, Origtup, CURTUP))
							AAsyserr(17002, Uptid, l1);
						AAbmove(Origtup, Outtup, Result->reldum.relwid);
					}
					else
					{
						AAbmove(Intup, Outtup, Result->reldum.relwid);
					}
				}
			}
		}
		else
		{
			if (Eql)
				eqlatt(tos);	/* send attribute to EQL */
			else
			{
				s1 = (char *) ((SYMBOL *) tos)->value;
				if (((SYMBOL *) tos)->type == CHAR)
					s1 = ((STRINGP *) s1)->stringp;
				else if (((SYMBOL *) tos)->type == INT && ((SYMBOL *) tos)->len == sizeof (char))
					((I1TYPE *) s1)->i1type = ((I2TYPE *) s1)->i2type;
# ifndef NO_F4
				else if (((SYMBOL *) tos)->type == FLOAT && ((SYMBOL *) (tos + 1))->len == sizeof (float))
				{
					((F4TYPE *) s1)->f4type = ((F8TYPE *) s1)->f8type;
					((SYMBOL *) tos)->len = sizeof (float);
				}
# endif
				/* print attribute */
				AApratt(((SYMBOL *) tos)->type, ctou(((SYMBOL *) tos)->len), s1);
			}
		}
		--tos;
		goto loop;


	  case BOP:
		op2 = (SYMBOL *) tos--;
		op1 = (SYMBOL *) tos;
		typecheck(op1, op2, opval);
		val1 = op1->value;
		val2 = op2->value;

		switch (((SYMBOL *) tos)->type)
		{
		  case INT:
			switch (((SYMBOL *) tos)->len)
			{
			  case sizeof (char):
			  case sizeof (short):
				switch (opval)
				{
				  case opADD:
					*val1 += *val2;
					goto loop;
				  case opSUB:
					*val1 -= *val2;
					goto loop;
				  case opMUL:
					*val1 *= *val2;
					goto loop;
				  case opDIV:
					*val1 /= *val2;
					goto loop;
				  case opMOD:
					*val1 %= *val2;
					goto loop;


				  case opPOW:
					itof8(op1);
					itof8(op2);
					((F8TYPE *) val1)->f8type = pow(((F8TYPE *) val1)->f8type, ((F8TYPE *) val2)->f8type);
					goto loop;


				  /* relational operator */
				  default:
					l1 = *val1 - *val2;
					*val1 = relop_interp(opval, l1);
					goto loop;
				}

			  case sizeof (long):
				switch (opval)
				{
				  case opADD:
					((I4TYPE *) val1)->i4type += ((I4TYPE *) val2)->i4type;
					goto loop;

				  case opSUB:
					((I4TYPE *) val1)->i4type -= ((I4TYPE *) val2)->i4type;
					goto loop;

				  case opMUL:
					((I4TYPE *) val1)->i4type *= ((I4TYPE *) val2)->i4type;
					goto loop;

				  case opDIV:
					((I4TYPE *) val1)->i4type /= ((I4TYPE *) val2)->i4type;
					goto loop;

				  case opMOD:
					((I4TYPE *) val1)->i4type %= ((I4TYPE *) val2)->i4type;
					goto loop;

				  case opPOW:
					itof8(op1);
					itof8(op2);
					((F8TYPE *) val1)->f8type = pow(((F8TYPE *) val1)->f8type, ((F8TYPE *) val2)->f8type);
					goto loop;

				  /* relational operator */
				  default:
					((SYMBOL *) tos)->len = sizeof (short);
					if (((I4TYPE *) val1)->i4type > ((I4TYPE *) val2)->i4type)
						l1 = 1;
					else
						if (((I4TYPE *) val1)->i4type == ((I4TYPE *) val2)->i4type)
							l1 = 0;
						else
							l1 = -1;

					*val1 = relop_interp(opval, l1);
					goto loop;

			    }
		}

		  case FLOAT:
			switch (opval)
			{
			  case opADD:
				((F8TYPE *) val1)->f8type += ((F8TYPE *) val2)->f8type;
				goto loop;

			  case opSUB:
				((F8TYPE *) val1)->f8type -= ((F8TYPE *) val2)->f8type;
				goto loop;

			  case opMUL:
				((F8TYPE *) val1)->f8type *= ((F8TYPE *) val2)->f8type;
				goto loop;

			  case opDIV:
				((F8TYPE *) val1)->f8type /= ((F8TYPE *) val2)->f8type;
				goto loop;

			  case opPOW:
				((F8TYPE *) val1)->f8type = pow(((F8TYPE *) val1)->f8type, ((F8TYPE *) val2)->f8type);
				goto loop;

			  default:
				((SYMBOL *) tos)->type = INT;
				((SYMBOL *) tos)->len = sizeof (short);
				if (((F8TYPE *) val1)->f8type > ((F8TYPE *) val2)->f8type)
					l1 = 1;
				else
					if (((F8TYPE *) val1)->f8type == ((F8TYPE *) val2)->f8type)
						l1 = 0;
					else
						l1 = -1;
				*val1 = relop_interp(opval, l1);
				goto loop;
			}

		  case CHAR:
			if (opval == (int) opCONCAT)
			{
				concatsym(op1, op2);	/* concatenate the two symbols */
				goto loop;
			}
			l1 = lexcomp(((STRINGP *) val1)->stringp, ctou(op1->len), ((STRINGP *) val2)->stringp, ctou(op2->len));
			((SYMBOL *) tos)->type = INT;
			((SYMBOL *) tos)->len = sizeof (short);
			*val1 = relop_interp(opval, l1);
			goto loop;
		}	/* end of BOP */

	  case UOP:
		val1 = ((SYMBOL *) tos)->value;
		switch (opval)
		{
		  case opMINUS:
		  case opABS:
			if (((SYMBOL *) tos)->type == CHAR)
				ov_err(BADUOPC);
			l1 = opval == (int) opMINUS;
			switch (((SYMBOL *) tos)->type)
			{
			  case INT:
				switch (((SYMBOL *) tos)->len)
				{
				  case sizeof (char):
				  case sizeof (short):
					if (l1 || (*val1 < 0))
						*val1 = -*val1;
			   		goto loop;

				  case sizeof (long):
					if (l1 || (((I4TYPE *) val1)->i4type < 0))
						((I4TYPE *) val1)->i4type = -((I4TYPE *) val1)->i4type;
					goto loop;
				}

			  case FLOAT:
				if (l1 || (((F8TYPE *) val1)->f8type < 0.0))
					((F8TYPE *) val1)->f8type = -(((F8TYPE *) val1)->f8type);
				goto loop;
			}

		  case opNOT:
			*val1 = !*val1;
		  case opPLUS:
			if (((SYMBOL *) tos)->type == CHAR)
				ov_err(BADUOPC);
			goto loop;

		  case opASCII:
			ascii(tos);
			goto loop;

		  case opINT1:
			typecoerce(tos, INT, 1);
			goto loop;

		  case opINT2:
			typecoerce(tos, INT, sizeof (short));
			goto loop;

		  case opINT4:
			typecoerce(tos, INT, sizeof (long));
			goto loop;

# ifndef NO_F4
		  case opFLT4:
			typecoerce(tos, FLOAT, sizeof (float));
			goto loop;
# endif

		  case opFLT8:
			typecoerce(tos, FLOAT, sizeof (double));
			goto loop;

		  default:
			if (((SYMBOL *) tos)->type == CHAR)
				ov_err(BADUOPC);
			if (((SYMBOL *) tos)->type == INT)
				itof8(tos);
			switch (opval)
			{
			  case opATAN:
				((F8TYPE *) val1)->f8type = atan(((F8TYPE *) val1)->f8type);
				goto loop;

			  case opGAMMA:
				((F8TYPE *) val1)->f8type = gamma(((F8TYPE *) val1)->f8type);
				goto loop;

			  case opLOG:
				((F8TYPE *) val1)->f8type = log(((F8TYPE *) val1)->f8type);
				goto loop;

			  case opSIN:
				((F8TYPE *) val1)->f8type = sin(((F8TYPE *) val1)->f8type);
				goto loop;

			  case opCOS:
				((F8TYPE *) val1)->f8type = cos(((F8TYPE *) val1)->f8type);
				goto loop;

			  case opSQRT:
				((F8TYPE *) val1)->f8type = sqrt(((F8TYPE *) val1)->f8type);
				goto loop;

			  case opEXP:
				((F8TYPE *) val1)->f8type = exp(((F8TYPE *) val1)->f8type);
				goto loop;

			  default:
				AAsyserr(17003, opval);
			}
		}


	  case AOP:
		aop_interp(opval, tos);
		goto loop;

	}
}



/*
**	relop_interp interprets the relational operators
**	(ie. EQ, NE etc.) and returns true or false
**	by evaluating i.
**
**	i should be greater than, equal or less than zero.
*/
relop_interp(opval, i)
register int	opval;
register int	i;
{
	switch (opval)
	{
	  case opEQ:
		return (i == 0);

	  case opNE:
		return (i != 0);

	  case opLT:
		return (i < 0);

	  case opLE:
		return (i <= 0);

	  case opGT:
		return (i > 0);

	  case opGE:
		return (i >= 0);

	  default:
		AAsyserr(17004, opval);
	}
}


/*
**	Aggregate values are stored in Outtup. Tend points
**	to the spot for the next aggregate. Aop_interp()
**	computes the value for the aggregate and leaves
**	the result in the position pointed to by Tend.
*/
aop_interp(opval, tos)
int				opval;
register struct stacksym	*tos;
{
	register char			*number;
	register int			i;
	register int			l1;
	register char			*val;
	char				val_buf[sizeof (double)];

	number = val_buf;
	/* note: this assumes that there are always  	*/
	/* sizeof (double) bytes which can be moved.	*/
	/* if it moves beyond Tend, it's ok		*/
	AAbmove(Tend, number, sizeof (double));
	val = (char *) ((SYMBOL *) tos)->value;
	switch (opval)
	{
	  case opSUMU:
	  case opSUM:
		if (*Counter <= 1)
			goto puta;
		switch (((SYMBOL *) tos)->type)
		{
		  case INT:
			switch (((SYMBOL *) tos)->len)
			{
			  case sizeof (char):
				((I2TYPE *) val)->i2type += ctoi(((I1TYPE *) number)->i1type);
				goto puta;

			  case sizeof (short):
				((I2TYPE *) val)->i2type += ((I2TYPE *) number)->i2type;
				goto puta;

			  case sizeof (long):
				((I4TYPE *) val)->i4type += ((I4TYPE *) number)->i4type;
				goto puta;
			}

		  case FLOAT:
# ifndef NO_F4
			if (((SYMBOL *) tos)->len == sizeof (float))
				((F8TYPE *) number)->f8type = ((F4TYPE *) number)->f4type;
# endif
			((F8TYPE *) val)->f8type += ((F8TYPE *) number)->f8type;
			goto puta;

		  default:
			ov_err(BADSUMC);	/* cant sum char fields */
		}

	  case opCNTU:
	  case opCNT:
		((SYMBOL *) tos)->type = CNTTYPE;
		((SYMBOL *) tos)->len = CNTLEN;
		((I4TYPE *) val)->i4type = *Counter;
		goto puta;

	  case opANY:
		((SYMBOL *) tos)->type = ANYTYPE;
		((SYMBOL *) tos)->len = ANYLEN;
		if (*Counter)
		{
			((I2TYPE *) val)->i2type = 1;
			if (!Bylist && (Agcount == 1))
				Targvc = 0;	/* if simple agg. stop scan */
		}
		else
			((I2TYPE *) val)->i2type = 0;
		goto puta;

	  case opMIN:
	  case opMAX:
		if (*Counter <= 1)
			goto puta;
		switch (((SYMBOL *) tos)->type)
		{
		  case INT:
			switch (((SYMBOL *) tos)->len)
			{
			  case sizeof (char):
				i = (ctoi(((I1TYPE *) val)->i1type) < ctoi(((I1TYPE *) number)->i1type));
				break;

			  case sizeof (short):
				i = (((I2TYPE *) val)->i2type < ((I2TYPE *) number)->i2type);
				break;

			  case sizeof (long):
				i = (((I4TYPE *) val)->i4type < ((I4TYPE *) number)->i4type);
				break;
			}
			break;

		  case FLOAT:
# ifndef NO_F4
			if (((SYMBOL *) tos)->len == sizeof (float))
				((F8TYPE *) number)->f8type = ((F4TYPE *) number)->f4type;
# endif
			i  = (((F8TYPE *) val)->f8type < ((F8TYPE *) number)->f8type);
			break;

		  case CHAR:
			l1 = ctou(((SYMBOL *) tos)->len);
			i = (lexcomp(((STRINGP *) val)->stringp, l1, Tend, l1) < 0);
			break;

		  default:
			AAsyserr(17005, ((SYMBOL *) tos)->type);
		}

		/* check result of comparison */
		if (opval == (int) opMAX)
			i = !i;	/* complement test for opMAX */
		if (i)
			goto puta;	/* condition true. new value */

		/* condition false. Keep old value */
		goto done;


	  case opAVGU:
	  case opAVG:
		if (((SYMBOL *) tos)->type == INT)
			itof8(tos);
		else
			if (((SYMBOL *) tos)->type == CHAR)
				ov_err(BADAVG);
		if (*Counter > 1)
		{
			((F8TYPE *) val)->f8type = ((F8TYPE *) number)->f8type + (((F8TYPE *) val)->f8type - ((F8TYPE *) number)->f8type)/ *Counter;
		}
		((SYMBOL *) tos)->len = sizeof (double);
		goto puta;

	  default:
		AAsyserr(17006, ((SYMBOL *) tos)->type);
	}

puta:
	tout(tos, Tend, ((SYMBOL *) tos)->len);
done:
	Tend += ctou(((SYMBOL *) tos)->len);
}
