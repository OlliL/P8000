/*
** ---  RG_INT.C -------- R-CODE-INTERPRETER  ---------------------------------
**
**		R_CODE (soft) Processor
**
**	Version: @(#)rg_int.c		4.0	02/05/89
**
*/

# include	<stdio.h>
# include	"rg_int.h"

SYMBOL		Stack[MAX_STACK + 1];
SYMBOL		*Print = &Stack[MAX_STACK];

# ifdef	RG_TRACE
static int	I_level;
# endif


rg_interp()
{
	register char	*rc;		/* R-CODE pointer    */
	register int	c;		/* the actual R-CODE */
	register int	i;
	register char	*push_rc;	/* pushed R_CODE ptr */
	short		no;
	char		*inp;
	extern char	*fmt_graph();
	extern char	*rg_case();
	extern char	*AApad();
# ifdef RG_TRACE
	extern char		*Code[];
# endif

	RUN = 0;
	rc = R_rc;

# ifdef RG_TRACE
	if (TR_INTERP)
	{
		I_level++;
		printf("*>*\n");
	}
# endif

	while (c = RC_BYTE)
	{
# ifdef RG_TRACE
		if (TR_INTERP)
			printf("*%d*\t%4d %s",
				I_level, rc - R_code - 1, Code[c]);
# endif
		i = 0;

		/* get the first argument of the TWO BYTES CODES */
		if (RANGE(L_2_BYTES, c, H_2_BYTES))
		{
			/* it may be a var, get the value        */
			RC_WORD;
			R_val = var_to_int();
# ifdef RG_TRACE
			if (TR_INTERP)
				printf(" %d", R_val);
# endif
		}

		/* get the first (or the second for rcCASE and   */
		/* rcREPEAT) argument of the THREE BYTES CODES,  */
		/* it's a relative R-CODE address                */
		if (RANGE(L_3_BYTES, c, H_3_BYTES))
		{
			/* first read the offset, then update !! */
			RC_WORD;
			push_rc = R_rc = rc + R_word;
# ifdef RG_TRACE
			if (TR_INTERP)
				printf(" -->%d", push_rc - R_code);
# endif
		}
# ifdef RG_TRACE
		if (TR_INTERP)
			putchar('\n');
# endif

		/* most CODES have their arguments,              */
		/* START to interpret R-CODE                     */
		switch (c)
		{
		/* ONE BYTE CODE */
		  case rcRC_END:
			new_page();
			RUN++;
			break;

		  case rcRETURN:
# ifdef RG_TRACE
			if (TR_INTERP)
			{
				--I_level;
				printf("*<*\n");
			}
# endif
			R_rc = rc;
			return;

		  case rcRTTS:
		  case rcEOTUP:
		  case rcALL:
			RUN++;
			break;

		  case rcONE:
			continue;

		  case rcDATE:
		  case rcTIME:
			rg_prtime(c == (int) rcTIME);
			break;

		  case prCOL:
			i = SYS_col - Col_begin + 1;
			goto push;

		  case prLINE:
			i = SYS_line - Line_begin + 1;
			if (!i)
				i = 1;
			goto push;

		  case prALLINES:
			i = SYS_line - Line_begin + 1;
			if (!i)
				i = 1;
			i = R_lines - i + 1;
			goto push;

		  case prNEWPAGE:
			if (RUNline || RUNpage)
				RUN++;
			else
				new_page();
			break;

		  case prPAGE:
			i = SYS_page;
			goto push;

		  case prTUPLE:
			i = SYS_tuple;
push:
			push_int(Print, i);
			continue;


		/* THE ANACHRONISM CODE */
		  case prVAR:
			RC_WORD;
			i = ck_var();
			push_dbl(Print, Var[i].f8type);
			continue;


		/* TWO Bytes CODES */
		  case prCONST:
			i = rg_getime(R_val);
			goto push;

		  case prSPACE:
			SYS_col = AApad(SYS_col, ck_val1(A_SPACE));
			break;

		  case prNEWLINE:
			if (RUNline)
				RUN++;
			else
			{
				if ((i = R_val) < 1 || i > H_NEWLINE)
					i = A_NEWLINE;
				for ( ; i; --i)
					new_line();
			}
			break;

		  case prNEWCOL:
			set_col(ck_val1(A_COL));
			break;

		  case prLAST:
			rg_pr_dom(Last);
			continue;

		  case prVALUE:
			rg_pr_dom(Value);
			continue;

		  case prNEXT:
			rg_pr_dom(Next);
			continue;

		  case fmtB_BIT:
			fmt_byte(BIT);
			break;

		  case fmtW_BIT:
			fmt_word(BIT);
			break;

		  case fmtB_OCT:
			fmt_byte(OCT);
			break;

		  case fmtW_OCT:
			fmt_word(OCT);
			break;

		  case fmtB_DEC:
			fmt_byte(DEC);
			break;

		  case fmtW_DEC:
			fmt_word(DEC);
			break;

		  case fmtB_HEX:
			fmt_byte(HEX);
			break;

		  case fmtW_HEX:
			fmt_word(HEX);
			break;


		/* THE ANACHRONISM CODE */
		  case prSTRING:
			i = R_val;			/* !!		*/
			RC_WORD;			/* batch length	*/
			rg_batch(i, var_to_int(), i, rc);
			rc += i;			/* update	*/
			break;

		  case prINPUT:
			RC_WORD;			/* batch length	*/
			i = rg_input(rc, &inp);		/* prompt	*/
			rg_batch(i, var_to_int(), i, inp);
			rc += R_val;			/* update	*/
			break;

		  case fmtINT:
			fmt_int(rc);
adjust:
			rc += sizeof (double);	/* skip scale */
			RC_WORD;		/* pic length */
			rc += R_word;		/* skip picture string */
			break;

		  case fmtREAL:
			fmt_real(rc);
			rc += 8;	/* skip before, point, ignore, norm */
			goto adjust;

		  case fmtCHAR:		/* R_val == output length */
			RC_WORD;	/*       == from          */
			i = var_to_int();
			RC_WORD;	/*       == to            */
			no = var_to_int();
			RC_WORD;	/*       == batch length  */
			fmt_char(i, no, var_to_int());
			break;

		  case fmtGRAPH:
			rc = fmt_graph(rc);
			break;


		/* THE ANACHRONISM CODE */
		  case rcCASE:
			/* get address of case == R_rc */
			/* and return address  == rc   */
			rc = rg_case();
			rg_interp();
			continue;

		  case rcREPEAT:
			if (rg_repeat(rc))
				goto jump;
			continue;


		/* THREE BYTES CODES */
		  case onBATCH:
			ONbatch = rc;
			/* jump over exception */
			goto jump;

		  case onB_LINE:
			ONb_line = rc;
			/* jump over exception */
			goto jump;

		  case onE_LINE:
			ONe_line = rc;
			/* jump over exception */
			goto jump;

		  case onB_PAGE:
			ONb_page = rc;
			/* jump over exception */
			goto jump;

		  case onE_PAGE:
			ONe_page = rc;
			/* jump over exception */
			/* fall through        */

		  case rcJUMP:
jump:
			rc = push_rc;
			continue;

		  case rcCALL:
			rg_interp();
			continue;

		  case setVAR:
		  case setSWITCH:
		  case rcIF:
			rg_expr(rc, c);
			goto jump;	/* goto THEN            */

		  case rcTHEN:
			if (!R_expr)
				goto jump;	/* goto ELSE    */
			continue;

		  default:
			rg_error("ILLEGAL R-CODE `%u'", c);
			rg_exit(EXIT_R_CODE);
		}

		check_line();
		if (RUN)
		{
# ifdef RG_TRACE
			if (TR_INTERP)
			{
				--I_level;
				printf("*<*\n");
			}
# endif
			R_rc = rc;
			return;
		}
	}
}
