# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/tree.h"
# include	"../h/pipes.h"
# include	"../h/symbol.h"
# include	"parser.h"

char		Trfrmt;		/* format for type checking */
char		Trfrml;		/* format length for type checking */

/*
**  FORMAT
**	routine to compute the format of the result relation attributes
**	it is called after ATTLOOKUP so the tuple defining the current
**	attribute is already available.
**	if the element is a function of more than one attribute, the result
**	domain format is computed by the following rules:
**		- no fcns allowed on character attributes
**		- fcn of integer attribs is an integer fcn with
**		  length = MAX(length of all attributes)
**		- fcn of floating point attribs is a floating point
**		  fcn with length = MIN(length of all attribs)
**		- fcn of integer and floating attributes is a
**		  floating fcn with length = MIN(length of all floating
**		  attributes)
*/
format(result1)
QTREE			*result1;
{
	register char			rfrmt;
	register char			rfrml;
	register QTREE			*result;
	struct constop			*cpt;
	extern struct out_arg		AAout_arg;
	extern struct constop		Coptab[];

	result = result1;
	switch (result->sym.type)
	{
	  case VAR:
		rfrmt = ((VAR_NODE *) result)->frmt;
		rfrml = ((VAR_NODE *) result)->frml;
      		break;

	  case AOP:
		switch (((OP_NODE *) result)->opno)
		{
		  case opAVG:
		  case opAVGU:
			rfrmt = FLOAT;
			rfrml = sizeof (double);
			if (((AOP_NODE *) result)->agfrmt == CHAR)
				/* character domain not allowed in these aggs */
				yyerror(AVGTYPE, (char *) 0);
			break;

		  case opCNT:
		  case opCNTU:
			rfrmt = INT;
			rfrml = sizeof (long);
			break;

		  case opANY:
			rfrmt = INT;
			rfrml = sizeof (short);
			break;

		  case opSUM:
		  case opSUMU:
			rfrmt = ((AOP_NODE *) result)->agfrmt;
			rfrml = ((AOP_NODE *) result)->agfrml;
			if (rfrmt == CHAR)
				/* no char domains for these aggs */
				yyerror(SUMTYPE, (char *) 0);
			break;

		  default:
			rfrmt = ((AOP_NODE *) result)->agfrmt;
			rfrml = ((AOP_NODE *) result)->agfrml;
			break;
		}
		break;

	  case AGHEAD:
		/*
		** can get format info from the AOP node because
		** it already has format info computed
		*/
		if (result->left->sym.type == AOP)
		{
			/* no by-list */
			rfrmt = ((VAR_NODE *) result->left)->frmt;
			rfrml = ((VAR_NODE *) result->left)->frml;
		}
		else
		{
			/* skip over by-list */
			rfrmt = ((VAR_NODE *) result->left->right)->frmt;
			rfrml = ((VAR_NODE *) result->left->right)->frml;
		}
		break;

	  case RESDOM:
		format(result->right);
		return;

	  case INT:
	  case FLOAT:
	  case CHAR:
		rfrmt = result->sym.type;
		rfrml = result->sym.len;
		break;

	  case COP:
		for (cpt = Coptab; cpt->copname; cpt++)
		{
			if (((OP_NODE *) result)->opno == cpt->copnum)
			{
				rfrmt = cpt->coptype;
				rfrml = cpt->coplen;
				break;
			}
		}
		if (!cpt->copname)
			AAsyserr(18002, ((OP_NODE *) result)->opno);
		break;

	  case UOP:
		switch (((OP_NODE *) result)->opno)
		{
		  case opATAN:
		  case opCOS:
		  case opGAMMA:
		  case opLOG:
		  case opSIN:
		  case opSQRT:
		  case opEXP:
			format(result->left);
			if (Trfrmt == CHAR)
				/*
				** no character expr in FOP
				** if more ops are added, must change error message				*/
				yyerror(FOPTYPE, (char *) 0);

		  case opFLT8:
			/* float8 is type conversion and can have char values */
			rfrmt = FLOAT;
			rfrml = sizeof (double);
			break;

# ifndef NO_F4
		  case opFLT4:
			rfrmt = FLOAT;
			rfrml = sizeof (float);
			break;
# endif

		  case opINT1:
			rfrmt = INT;
			rfrml = sizeof (char);
			break;

		  case opINT2:
			rfrmt = INT;
			rfrml = sizeof (short);
			break;

		  case opINT4:
			rfrmt = INT;
			rfrml = sizeof (long);
			break;

		  case opASCII:
			format(result->left);
			rfrmt = CHAR;
			rfrml = Trfrml;
			if (Trfrmt == INT)
			{
				if (Trfrml == sizeof (short))
					rfrml = AAout_arg.i2width;
				else if (Trfrml == sizeof (long))
					rfrml = AAout_arg.i4width;
				else if (Trfrml == sizeof (char))
					rfrml = AAout_arg.i1width;
				else
					AAsyserr(18003, Trfrml);
				break;
			}
			if (Trfrmt == FLOAT)
			{
				if (Trfrml == sizeof (double))
					rfrml = AAout_arg.f8width;
# ifndef NO_F4
				else if (Trfrml == sizeof (float))
					rfrml = AAout_arg.f4width;
# endif
				else
					AAsyserr(18004, Trfrml);
				break;
			}
			if (Trfrmt == CHAR)
				break;
			AAsyserr(18005, Trfrmt);

		  case opNOT:
			if (!Qlflag)
				AAsyserr(18006);
			return;

		  case opMINUS:
		  case opPLUS:
			format(result->right);
			if (Trfrmt == CHAR)
				/* no chars for these unary ops */
				yyerror(UOPTYPE, (char *) 0);
			return;

		  case opABS:
			format(result->left);
			if (Trfrmt == CHAR)
				/* no char values in fcn */
				yyerror(FOPTYPE, (char *) 0);
			return;

		  default:
			AAsyserr(18007, ((OP_NODE *) result)->opno);
		}
		break;

	  case BOP:
		switch (((OP_NODE *) result)->opno)
		{

		  case opEQ:
		  case opNE:
		  case opLT:
		  case opLE:
		  case opGT:
		  case opGE:
			if (!Qlflag)
				AAsyserr(18008);
			format(result->right);
			rfrmt = Trfrmt;
			format(result->left);
			if ((rfrmt == CHAR) != (Trfrmt == CHAR))
				/* type conflict on relational operator */
				yyerror(RELTYPE, (char *) 0);
			return;

		  case opADD:
		  case opSUB:
		  case opMUL:
		  case opDIV:
			format(result->left);
			rfrmt = Trfrmt;
			rfrml = Trfrml;
			format(result->right);
			if (rfrmt == CHAR || Trfrmt == CHAR)
				/* no opns on characters allowed */
				yyerror(NUMTYPE, (char *) 0);
			if (rfrmt == FLOAT || Trfrmt == FLOAT)
			{
				if (rfrmt == FLOAT && Trfrmt == FLOAT)
				{
					if (Trfrml < rfrml)
						rfrml = Trfrml;
				}
				else if (Trfrmt == FLOAT)
					rfrml = Trfrml;
				rfrmt = FLOAT;
			}
			else
				if (Trfrml > rfrml)
					rfrml = Trfrml;
			break;

		  case opMOD:
			format(result->left);
			rfrmt = Trfrmt;
			rfrml = Trfrml;
			format(result->right);
			if (rfrmt != INT || Trfrmt != INT)
				/* mod operator not defined */
				yyerror(MODTYPE, (char *) 0);
			if (Trfrml > rfrml)
				rfrml = Trfrml;
			break;

		  case opPOW:
			format(result->right);
			rfrmt = Trfrmt;
			rfrml = Trfrml;
			format(result->left);
			if (rfrmt == CHAR || Trfrmt == CHAR)
			{
				/* no char values for pow */
				yyerror(NUMTYPE, (char *) 0);
			}
# ifndef NO_F4
			if ((rfrmt == FLOAT && rfrml == sizeof (float))
			|| (Trfrmt == FLOAT && Trfrml == sizeof (float)))
			{
				rfrmt = FLOAT;
				rfrml = sizeof (float);
			}
			else
# endif
			{
				rfrmt = FLOAT;
				rfrml = sizeof (double);
			}
			break;

		  case opCONCAT:
			format(result->left);
			rfrmt = Trfrmt;
			rfrml = Trfrml;
			format(result->right);
			if (rfrmt != CHAR || Trfrmt != CHAR)
				/* only character domains allowed */
				yyerror(CONCATTYPE, (char *) 0);
			rfrml += Trfrml;
			break;

		  default:
			AAsyserr(18009, ((OP_NODE *) result)->opno);
		}
	}
	Trfrmt = rfrmt;
	Trfrml = rfrml;
}
