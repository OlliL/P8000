 /*
** ---  RG_EXPR.C ------- EXPRESSION INTERPRETER  -----------------------------
**
**		R_EXPR_CODE (soft) Co-processor
**
**	Version: @(#)rg_expr.c		4.0	02/05/89
**
*/

# include	"rg_int.h"


# define	SYM_NULL		((SYMBOL *) 0)
# define	EXPR			(tos = rg_expr(rc, rcEXPR))
# define	EXPR_END		return (tos)
# define	EXPR_EXIT		return (SYM_NULL)

# define	DOUBLE			sizeof (double)
# define	ROUND			0.5

# define	FRMT			sy_frmt
# define	FRML			sy_frml


static int	Frmt = FLOAT;		/* expected */
static int	Frml = DOUBLE;		/*  result  */


SYMBOL	*rg_expr(rc, code)
register char	*rc;
int	 code;	/* the reason why to interpret expression, */
		/* (rcIF, setVAR, setSWITCH or rcEXPR)     */
{
	register SYMBOL		*tos;
	register SYMBOL		*s;
	register int		c;	/* the atual R-EXPR-CODE   */
	register int		i;
	register char		*p;
	register union anytype	*any;
	register union anytype	*a;
	BOOL			set_flag;     /* var assign flag   */
	extern char		*push_rc();
	extern char		*AAbmove();
	extern double		floor();
	extern char		*print_type();
# ifdef RG_TRACE
	extern char		*Code[];
# endif

	R_expr = set_flag = FALSE;	/* assume interpreter dies */
	tos = &Stack[MAX_STACK];

	switch (code)
	{
	  case setSWITCH:
		set_flag = TRUE;
		/* fall	through	*/

	  case rcIF:
		Frmt = BIN;
		Frml = sizeof (BOOL);
		break;

	  case setVAR:
		set_flag = TRUE;
		Frmt = FLOAT;
		Frml = DOUBLE;
		/* fall	through	*/

	  case rcEXPR:
		break;

	  default:
		rg_error("INVALID start of expression (`%u')", code);
		goto abo_expr;
	}

# ifdef RG_TRACE
	if (TR_EXPR)
		printf("+>+\tset_flag=%d Frmt=%d Frml=%d\n",
			set_flag, Frmt, Frml);
# endif

	while (c = RC_BYTE)
	{
# ifdef RG_TRACE
		if (TR_EXPR)
			printf("+ +\t%4d %s\n", rc - R_code - 1, Code[c]);
# endif

		if (tos	== Stack && RANGE(L_PUSH, c, H_PUSH))
		{
			rg_error("EXRESSION STACK OVERFLOW");
abo_expr:
			rg_exit(EXIT_R_CODE);
		}

		a = any = &tos->sy_val;
		switch (c)
		{
		  case bdLT:
		  case bdEQ:
		  case bdLE:
		  case bdGT:
		  case bdNE:
		  case bdGE:
			s = tos++;
			i = rg_compare(c, tos++, s);
# ifdef RG_TRACE
			if (TR_INTERP)
				printf("COMPARE\twant 0%o ret 0%o\n", c, i);
# endif
			if (i < 0)
				EXPR_EXIT;
			push_bin(--tos, bitset(i, c));
			continue;

		  case pushFALSE:
		  case pushTRUE:
			push_bin(--tos, c == (int) pushTRUE);
			continue;

		  case pushNULL:
			push_dbl(--tos, (double) 0);
			continue;

		  case push1INT:
			if ((i = RC_BYTE) > MAX_I1)
				i -= BYTE_MASK + 1;
			push_dbl(--tos, (double) i);
			continue;

		  case push2INT:
			RC_WORD;
			push_dbl(--tos, (double) R_word);
			continue;

		  case push4INT:
			push_dbl(--tos, (double) 0);	/* empty initialization */
			any = &tos->sy_val;
			AAbmove(rc, any->c0type, sizeof (long));
			rc += sizeof (long);
			any->f8type = any->i4type;
			continue;

		  case pushREAL:
			push_dbl(--tos, (double) 0);	/* empty initialization */
			any = &tos->sy_val;
			AAbmove(rc, any->c0type, tos->FRML);
			rc += tos->FRML;
			continue;

		  case pushSTRING:
			RC_WORD;
			rc = push_rc(--tos, rc, R_word);
			continue;

		  case pushCONST:
			i = rg_getime(RC_BYTE);
			push_dbl(--tos, (double) i);
			continue;

		  case pushCOL:
			if (set_flag)
			{
				if (EXPR)
				{
					any = &tos->sy_val;
					set_col(i = any->f8type);
				}
				EXPR_END;
			}
			i = SYS_col - Col_begin + 1;
			push_dbl(--tos, (double) i);
			continue;

		  case pushLINE:
			if (set_flag)
			{
				if (EXPR)
				{
					any = &tos->sy_val;
					set_line(i = any->f8type);
				}
				EXPR_END;
			}
			i = SYS_line - Line_begin + 1;
			push_dbl(--tos, (double) i);
			continue;

		  case pushALLINES:
			if (set_flag)
				rg_error("ALL LINES is read only");
			else
			{
				i = SYS_line - Line_begin + 1;
				if (!i)
					i = 1;
				i = R_lines - i + 1;
				push_dbl(--tos, (double) i);
			}
			continue;

		  case pushPAGE:
			if (set_flag)
			{
				if (EXPR)
				{
					any = &tos->sy_val;
					if ((i = any->f8type) > 0)
						SYS_page = i;
				}
				EXPR_END;
			}
			push_dbl(--tos, (double) SYS_page);
			continue;

		  case pushTUPLE:
			if (set_flag)
			{
				if (EXPR)
				{
					any = &tos->sy_val;
					if ((i = any->f8type) >= 0)
						SYS_tuple = i;
				}
				EXPR_END;
			}
			push_dbl(--tos, (double) SYS_tuple);
			continue;

		  case pushLAST:
			p = Last;
			goto push_dom;

		  case pushVALUE:
			p = Value;
			goto push_dom;

		  case pushNEXT:
			p = Next;
push_dom:
			RC_WORD;		/* dom  */
			R_val = var_to_int();
			RC_WORD;		/* from */
			i = var_to_int();
			RC_WORD;		/* to   */
			rg_push_dom(--tos, p, i, var_to_int());
			continue;

		  case pushVAR:
			RC_WORD;
			i = ck_var();
			if (set_flag)
			{
				if (EXPR)
				{
					any = &tos->sy_val;
					Var[i].f8type = any->f8type;
				}
				EXPR_END;
			}
			push_dbl(--tos, Var[i].f8type);
			continue;

		  case pushSWITCH:
			RC_WORD;
			i = ck_bvar();
			if (set_flag)
			{
				if (EXPR)
				{
					any = &tos->sy_val;
					Bvar[i] = any->booltype;
				}
				EXPR_END;
			}
			push_bin(--tos, Bvar[i]);
			continue;

		  case rcNOT:	/* double check ? */
			s = tos;
			if ( ck_2_frm (s, tos, BIN, sizeof (BOOL), "NOT"))
				EXPR_EXIT;
			a->booltype = !any->booltype;
			continue;

		  case rcOR:
			s = tos++;
			if (ck_2_frm(s, tos, BIN, sizeof (BOOL), "OR"))
				EXPR_EXIT;
			a = &tos->sy_val;
			a->booltype = a->booltype || any->booltype;
			continue;

		  case rcAND:
			s = tos++;
			if (ck_2_frm(s, tos, BIN, sizeof (BOOL), "AND"))
				EXPR_EXIT;
			a = &tos->sy_val;
			a->booltype = a->booltype && any->booltype;
			continue;

		  case rcEND_SET:
		  case rcTHEN:
			if (tos	!= &Stack[MAX_STACK - 1])
			{
				rg_error("LEAVING %d SYMBOLS in EXPR-STACK",
					&Stack[MAX_STACK - 1] - tos);
				goto expr_error;
			}
			if (Frmt == FLOAT && Frml == DOUBLE)
				convert(tos);
			if (ck_frm(tos, Frmt, Frml))
			{
				rg_error("WRONG EXPR-TYPE %s%d, expected %s%d",
					print_type(tos->FRMT), tos->FRML,
					print_type(Frmt), Frml);
expr_error:
				rg_error("Expression not assigned");
				EXPR_EXIT;
			}
			if (c == (int) rcTHEN)
				R_expr = a->booltype;

# ifdef RG_TRACE
			if (TR_EXPR)
			{
				printf("+<+\t");
				if (Frmt == BIN)
					printf("bool(%d)\n", a->booltype);
				else
					printf("%10.3f\n", a->f8type);
			}
# endif

			EXPR_END;

		  case rcMIN:	/* double check	? */
			s = tos;
			if (ck_2_frm(s, tos, FLOAT, DOUBLE, "-"))
				EXPR_EXIT;
			a->f8type = - any->f8type;
			continue;

		  case rcADD:
			s = tos++;
			if (ck_2_frm(s, tos, FLOAT, DOUBLE, "+"))
				EXPR_EXIT;
			a = &tos->sy_val;
			a->f8type += any->f8type;
			continue;

		  case rcSUB:
			s = tos++;
			if (ck_2_frm(s, tos, FLOAT, DOUBLE, "-"))
				EXPR_EXIT;
			a = &tos->sy_val;
			a->f8type -= any->f8type;
			continue;

		  case rcMUL:
			s = tos++;
			if (ck_2_frm(s, tos, FLOAT, DOUBLE, "*"))
				EXPR_EXIT;
			a = &tos->sy_val;
			a->f8type *= any->f8type;
			continue;

		  case rcDIV:
			s = tos++;
			if (ck_2_frm(s, tos, FLOAT, DOUBLE, ":"))
				EXPR_EXIT;
			a = &tos->sy_val;
			if (any->f8type == 0.0)
			{
				rg_error("ILLEGAL division: %10.3f : 0", a->f8type);
				EXPR_EXIT;
			}
			a->f8type /= any->f8type;
			continue;

		  case rcDIV_R:
			s = tos++;
			if (ck_2_frm(s, tos, FLOAT, DOUBLE, "/"))
				EXPR_EXIT;
			a = &tos->sy_val;
			if (any->f8type == 0.0)
			{
				rg_error("ILLEGAL division: %10.3f / 0", a->f8type);
				EXPR_EXIT;
			}
			a->f8type = floor(a->f8type / any->f8type);
			continue;

		  case rcMOD:
			s = tos++;
			if (ck_2_frm(s, tos, FLOAT, DOUBLE, "%"))
				EXPR_EXIT;
			a = &tos->sy_val;
			a->f8type = (long) a->f8type % (long) any->f8type;
			continue;

		  default:
			rg_error("ILLEGAL EXPRESSION R-CODE `%u'", c);
			rg_exit(EXIT_R_CODE);
			/*NOTREACHED*/
		}	/* switch */

	}	/* while */

	EXPR_EXIT;
}
