/*
** ---  RC_PARSE.C ------ PARSER FOR YACC OUTPUT  -----------------------------
**
**	Version: @(#)rc_parse.c		4.0	02/05/89
**
*/

# include	"rc.h"

# define	ACTION(state)		(state & 07777)
# define	OPERATION(state)	(state >> 12)

/* the OPERATIONS */
# define	SHIFT_ON_ERROR		010400
# define	ERROR			0
# define	SKIP			1
# define	SHIFT			2
# define	REDUCE			3
# define	ACCEPT			4

/* SIZE for state and value stack */
# define	MAXDEPTH		150


extern char	*yyval;			/* defined in the table file	 */
extern char	*yylval;			/* defined in the table file	 */
extern char	**yypv;			/* defined in the table file	 */
extern short	yygo[];			/* defined in the table file	 */
extern short	yypgo[];		/* defined in the table file	 */
extern short	yyr1[];			/* defined in the table file	 */
extern short	yyr2[];			/* defined in the table file	 */
extern short	yyact[];		/* defined in the table file	 */
extern short	yypact[];		/* defined in the table file	 */



# ifdef RC_TRACE
int		rc_debug =	0;	/* TR_PARSE for debugging (rc.h) */
# endif



rc_parse()
{
	char		*values[MAXDEPTH];	/*    value stack	 */
	short		states[MAXDEPTH];	/*    state stack	 */
	register short	*yyps;		/* pointer to state stack	 */
	register short	*act;		/* pointer to performing action	 */
	register int	state;		/* current parser state		 */
	register int	next_state;
	register int	token;		/* current input token number	 */
	register int	i;
	register int	errorflag;	/* error recovery flag		 */

	state = 0;
	token = -1;
	errorflag = 0;
	yyps = &states[-1];
	yypv = &values[-1];

stack:
	*++yyps = state;	/* push state into state stack */
	*++yypv = yyval;	/* push value into value stack */

# ifdef RC_TRACE
	if (TR_PARSE)
		printf("STATE %d value %d char %d\n", state, yyval, token);
# endif

new_state:
	/* set act to point to the parsing actions for the new state */
	i = yypact[state + 1];
	act = &yyact[i];

next_action:		/* get the next action, and perform it */
	next_state = ACTION(*act);
	switch (OPERATION(*act++))	/* switch on operation */
	{
	  case SKIP:
		if (token < 0)
		{
			/* get next token */
			token = rc_lex();
# ifdef RC_TRACE
			if (TR_PARSE)
				printf( "CHARACTER %d read\n", token);
# endif
		}
		if (next_state != token)
			act++;
		goto next_action;	/* get next action */

	  case SHIFT:		/* goto next_state */
		state = next_state;
		yyval = yylval;	/* $i = value of last token */
		token = -1;
		if (errorflag)
			errorflag--;
		goto stack;	/* stack new state and value */

	  case REDUCE:		/* rule next_state */
# ifdef RC_TRACE
		if (TR_PARSE)
			printf("REDUCE %d\n", next_state);
# endif
		yyps -= yyr2[next_state];	/* pop state stack */
		yypv -= yyr2[next_state];	/* pop value stack */
		yyval = yypv[1];		/* $$ = $1	   */

		if (Compile && yyactr(next_state))
			 goto abort;

		/* consult goto table to find next state */
		i = yyr1[next_state];
		i = yypgo[i];
		act = &yygo[i];
		next_state = *yyps;
		for ( ; *act != next_state && *act >= 0; act += 2)
			continue;
		state = act[1];
		goto stack;  /* stack new state and value */

	  case ACCEPT:
		return (0);

	  case ERROR:		/* attempt to resume parsing */
		switch (errorflag)
		{
		  case 0:	/* brand new syntax error */
			rc_error(0);

		  case 1:	 /* incompletely recovered error, try again */
			errorflag = 2;

			/* find a state where "error" is a legal SHIFT */
			while (yyps >= states)
			{
				/* search yyps actions */
				i = *yyps;
				i = yypact[i + 1];
				act = &yyact[i];
				for ( ; OPERATION(*act) == SKIP; act += 2)
					if (*act == SHIFT_ON_ERROR)
						goto found;

				/* the current yyps has no shift */
				/* on "error", pop stack	 */

# ifdef RC_TRACE
				if (TR_PARSE)
				{
					printf("ERROR recovery poyyps state %d",
						yyps[0]);
					printf(", uncovers %d\n", yyps[-1]);
				}
# endif
				yyps--;
				yypv--;
			}

			/* there is no state on the stack */
			/* with an error shift ... abort  */
abort:
			return (1);

found:	/* we have a state with a shift on "error", resume parsing */
			state = act[1] & 07777;
			goto stack;

		  case 2:  /* no shift yet; clobber input char */
# ifdef RC_TRACE
			if (TR_PARSE)
				printf("err recov discards char %d\n", token);
# endif

			/* don't discard EOF; quit */
			if (!token)
				goto abort;
			token = -1;
			goto new_state;   /* try again in the same state */

		}
	}

	return (1);
}
