# include	<stdio.h>
# include	"gen.h"
# include	"constants.h"
# include	"globals.h"

/*
**  COMMENT.C -- routine to scan comments inside an EQL statement
**
**	Uses the endcmnt token code to find the endcmnt
**	terminal string, then reads the input until it sees this
**	terminal (must be <= 2 characters), returning EOF_TOK and
**	giving an error diagnostic if end-of-file is encountered.
**
**	Returns:
**		CONTINUE -- valid comment
**		EOF_TOK -- EOF in comment
**
**	Side Effects:
**		deletes comments from within an EQL statement
**
*/
comment()
{
	register int		i;
	register int		l;
	register struct optab	*op;
	char			buf[3];

	/* find end of comment operator */
	for (op = Optab; op->op_term; op++)
		if (op->op_token == Tokens.sp_endcmnt)
			break;

	if (!op->op_term)
		AAsyserr(13000);
	/* scan for the end of comment */
	l = AAlength(op->op_term);
	if (l > sizeof buf - 1)
		AAsyserr(13001, l, op->op_term, sizeof buf -1);

	/* fill buffer to length of endmnt terminal */
	for (i = 0; i < l; i++)
	{
		if ((buf[i] = getch()) == EOF_TOK)
		{
nontermcom :
			/* non-terminated comment */
			yysemerr("premature EOF encountered in comment", (char *) 0);
			return (EOF_TOK);
		}
	}

	/* shift on input until endcmnt */
	while (!AAbequal(buf, op->op_term, l))
	{
		for (i = 0; i < l - 1; i++)
			buf[i] = buf[i + 1];
		if ((buf[l - 1] = getch()) == EOF_TOK)
			goto nontermcom;
	}
	return (CONTINUE);
}
