# include	"gen.h"
# include	"../h/dbs.h"

# define	STACKSZ		50
# define	RIGHTP		21
# define	END		22
# define	SEPERATOR	0
# define	OR		1
# define	AND		2
# define	EQUALS		3
# define	NEQ		4
# define	LESS		5
# define	LEQ		6
# define	GREATER		7
# define	GEQ		8
# define	ADD		9
# define	SUBTRACT	10
# define	MULTIPLY	11
# define	DIVIDE		12
# define	MOD		13

static int	ExprPrec[] =	/* Precedence table */
{
	0,	/* filler */
	1,	/* 1 -- OR */
	2,	/* 2 -- AND */
	3,	/* 3 -- EQUALS */
	3,	/* 4 -- NEQ */
	4,	/* 5 -- LESS */
	4,	/* 6 -- LEQ */
	4,	/* 7 -- GREATER */
	4,	/* 8 -- GEQ */
	5,	/* 9 -- ADD */
	5,	/* 10 -- SUBTRACT */
	6,	/* 11 -- MULTIPLY */
	6,	/* 12 -- DIVIDE */
	6	/* 13 -- MOD */
};

static int	ExprNstack[STACKSZ];
static int	*ExprNptr;
static int	ExprOstack[STACKSZ];
static int	*ExprOptr;
static int	ExprError;
static int	ExprPeek;
static char	(*ExprGet)();
static char	**ExprParm;

/*
**  Stack operations.
**
**
** POPOP
**	returns the top of the operator stack
**	and decrements this stack.
*/
popop()
{
	if (ExprOptr <= ExprOstack)
		AAsyserr(16010);
	return (*--ExprOptr);
}

/*
** PUSHOP
**	increments the stack pointer
**	and pushes op on the stack.
*/
# define	pushop(op)	*ExprOptr++ = op

/*
** POPNUM
**	returns the top of the number stack
**	and decrements the stack pointer.
*/
popnum()
{
	if (ExprNptr <= ExprNstack)
		AAsyserr(16011);
	return (*--ExprNptr);
}

/*
** PUSHNUM
**	increments the stack pointer
**	and pushes num onto the stack
*/
# define	pushnum(num)	*ExprNptr++ = num


/*
**  EXPR -- evaluate expression
**
**	This module evaluates an expression in somewhat standard
**	infix notation.  Several restrictions apply.  There are
**	no variables, since this can be simulated with the macro
**	processor.  No numeric overflow is checked.  There may be
**	no spaces, tabs, or newlines in the expression.
**
**	The text of the expression is read from ExprGet,
**	so that must be initialized before calling this routine.
**
**	Operators accepted are + - * / < > >= <= = != % ( )
**	& |.
**	Operands may be signed integers.
**	Standard precedence applies.
**
**	An expression can be viewed as a sequence of operands,
**	and operators.  If the terminator is considered to be
**	an operator, then the sequence must be composed
**	of n matched pairs of operators and operands.  NOT and
**	Negation are considered to be part of the operand and
**	are treated as such.  Thus to evaluate an expression,
**	n pairs are read until the terminator is found as the
**	last operator.
**
**	Returns:
**		value of the expression.  Undetermined value
**		on error.
**
**	Defines:
**		expr -- the main entry point.
**		valof -- the guts of the expression processor.
**		exprfind -- finds and chews on operands.
**		opfind -- finds and chews on operations.
**		exp_op -- performs operations.
**		numberget -- converts numbers.
**		exprgch -- to get characters.
**		pushop, popop, pushnum, popnum -- stack operations.
**		ExprNstack, ExprNptr, ExprOstack, ExprOptr --
**			stacks + associated data.
**		ExprError -- an error flag.
**		ExprPrec -- the precedence tables.
**		ExprPeek -- the lookbehind character for exprgch.
*/
expr(rawget, rawparm)
char		(*rawget)();
register char	**rawparm;
{
	ExprNptr = ExprNstack;
	ExprOptr = ExprOstack;
	ExprError = FALSE;
	ExprPeek = -1;
	ExprGet = rawget;
	ExprParm = rawparm;
	return (valof(END));
}


/*
**  VALOF -- compute value of expression
**
**	This is the real expression processor.  It handles sequencing
**	and precedence.
**
**	Parameters:
**		terminator -- the symbol which should terminate
**			the expression.
*/
valof(terminator)
register int	terminator;
{
	register int	number;
	register int	operator;

	pushop(SEPERATOR);		/* initialize the stack */

	for ( ; ; )
	{
		number = exprfind();
		if (ExprError)
			return (0);
		operator = opAAam_find();
		if (ExprError)
			return (0);

		if (operator == RIGHTP || operator == END)
			break;

		/* Do all previous operations with a higher precedence */
		while (ExprPrec[operator] <= ExprPrec[ExprOptr[-1]])
			number = exp_op(popop(), popnum(), number);
		pushop(operator);
		pushnum(number);
	}
	if (operator != terminator)		/* ExprError in operators */
		if (operator == RIGHTP)
			printf("%sZusaetzliche Klammer uebergangen\n", ERROR);
		else
		{
			ExprError = TRUE;
			printf("%sKlammerfehler\n", ERROR);
			printf("\t\tAusdruck = 0\n");
			return (0);
		}
	/* Empty stack for this call of valof */
	while ((operator = popop()) != SEPERATOR)
		number = exp_op(operator, popnum(), number);

	return (number);
}


/*
**  EXPRFIND -- find and chomp operand
**
**	This routine reads the next operand.  It generally just
**	reads numbers, except it also knows about unary operators
**	! and - (where it calls itself recursively), and paren-
**	theses (where it calls valof recursively).
**
**	Returns:
**		value of operand.
*/
exprfind()
{
	register int	result;
	register int	c;

	switch (c = exprgch())
	{
	  case '0':
	  case '1':
	  case '2':
	  case '3':
	  case '4':
	  case '5':
	  case '6':
	  case '7':
	  case '8':
	  case '9':
		return (numberget(c));

	  case '!':
		result = exprfind();
		return (ExprError? 0: (result <= 0));

	  case '-':
		result = exprfind();
		return (ExprError? 0: -result);

	  case '(':
		return (valof(RIGHTP));

	  case ' ':
	  case '\n':
	  case '\t':
	  case '\0':
		printf("%sVorzeitiges Ausdrucksende\n", ERROR);
		ExprError = TRUE;
		return (0);

	  default:
		printf("%s`%c' anstatt eines Ausdrucks\n", ERROR, c);
		printf("\t\tAusdruck = 0\n");
		ExprError = TRUE;
		return (0);
	}
}


/*
**  OPFIND -- find and translate operator
**
**	This reads the next operator from the input stream and
**	returns the internal code for it.
**
**	Returns:
**		The code for the next operator.
**		Zero on error.
*/
opAAam_find()
{
	register int	c;

	switch (c = exprgch())
	{
	  case '/':
		return (DIVIDE);

	  case '=':
		return (EQUALS);

	  case  '&':
		return (AND);

	  case '|':
		return (OR);

	  case '+':
		return (ADD);

	  case '-':
		return (SUBTRACT);

	  case '*':
		return (MULTIPLY);

	  case '<':
		if ((c = exprgch()) == '=')
			return (LEQ);
		ExprPeek = c;
		return (LESS);

	  case '>':
		if ((c = exprgch()) == '=')
			return (GEQ);
		ExprPeek = c;
		return (GREATER);

	  case '%':
		return (MOD);

	  case '!':
		if ((c = exprgch()) == '=')
			return (NEQ);
		goto error;

	  case ')':
		return (RIGHTP);

	  case ' ':
	  case '\t':
	  case '\n':
	  case '\0':
		return (END);

	  default:
error:
		printf("%s`%c' anstatt eines Operators\n", ERROR, c);
		ExprError = TRUE;
		return (0);
	}
}


/*
**  EXP_OP -- perform operation
**
**	Performs an operation between two values.
**
**	Parameters:
**		op -- the operation to perform.
**		lv -- the left operand.
**		rv -- the right operand.
*/
exp_op(op, lv, rv)
register int	op;
register int	lv;
register int	rv;
{
	switch (op)
	{
	  case OR:
		return ((lv > 0) || (rv > 0));

	  case AND:
		return ((lv > 0) && (rv > 0));

	  case EQUALS:
		return (lv == rv);

	  case NEQ:
		return (lv != rv);

	  case LESS:
		return (lv < rv);

	  case LEQ:
		return (lv <= rv);

	  case GREATER:
		return (lv > rv);

	  case GEQ:
		return (lv >= rv);

	  case ADD:
		return (lv + rv);

	  case SUBTRACT:
		return (lv - rv);

	  case MULTIPLY:
		return (lv * rv);

	  case DIVIDE:
		if (rv)
			return (lv / rv);
		printf("%sDivision durch Null: Quotient = 0\n", ERROR);
		return (0);

	  case MOD:
		return (lv % rv);

	  default:
		AAsyserr(16009, op);
	}
}


/*
**  NUMBERGET -- read and convert a number
**
**	Reads and converts a signed integer.
*/
numberget(c)
register int	c;
{
	register int	result;

	result = 0;
	do
	{
		result = result * 10 + c - '0';
		c = exprgch();
	} while (c >= '0' && c <= '9');
	ExprPeek = c;
	return (result);
}


/*
**  EXPRGCH -- expression character get
**
**	Gets the next character from the expression input.  Takes
**	a character out of ExprPeek first.  Also maps spaces, tabs,
**	and newlines into zero bytes.
**
**	Returns:
**		Next character.
**
**	Side Effects:
**		Clears ExprPeek if set.
*/
exprgch()
{
	register int	c;

	if ((c = ExprPeek) < 0)
		c = (*ExprGet)(ExprParm);
	ExprPeek = -1;
	if (c == ' ' || c == '\n' || c == '\t')
		c = 0;
	return (c);
}
