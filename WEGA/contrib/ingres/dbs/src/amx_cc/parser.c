# include	"amx.h"

/* SYMBOL TABLE ENTRY */
struct s_entry		*Symtab;

/* DECLARATIONS */
int		Cvarindex;
struct cvartbl	Cvartbl[MAXCVAR + 1];
int		Newline;	/* flag to indicate line end	*/
int		Line;		/* source line counter		*/
int		Type_spec = opINT;	/* type of C-variable	*/
int		Lcase;		/* UPPER->lower conversion flag */

/* PRINTING TABLES */
char		*TYPE_SPEC[] =
{
	"char",			/* opCHAR   */
	"short",		/* opINT    */
	"long",			/* opLONG   */
	"float",		/* opFLT    */
	"double",		/* opDOUBLE */
	"char"			/* opSTRING */
};

char		*CONVERT[] =
{
	"ctoi(*(char*)",	/* opCHAR   */
	"*((short*)",		/* opINT    */
	"*((long*)",		/* opLONG   */
	"*((float*)",		/* opFLT  */
	"*((double*)",		/* opDOUBLE */
	"((char*)"		/* opSTRING */
};

char		*SC_SPEC[] =
{
	"auto",			/* opAUTO   */
	"static",		/* opSTATIC */
	"extern"		/* opEXTERN */
};

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

/* defined in the table file  */
extern char	*yyval;
extern char	*yylval;
extern char	**yypv;
extern short	yygo[];
extern short	yypgo[];
extern short	yyr1[];
extern short	yyr2[];
extern short	yyact[];
extern short	yypact[];

parse()
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
new_state:
	/* set act to point to the parsing actions for the new state */
	i = yypact[state + 1];
	act = &yyact[i];

next_action:		/* get the next action, and perform it */
	next_state = ACTION(*act);
	switch (OPERATION(*act++))	/* switch on operation */
	{
	  case SKIP:
		if (token < 0)	/* get next token */
			token = lex();
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
		yyps -= yyr2[next_state];	/* pop state stack */
		yypv -= yyr2[next_state];	/* pop value stack */
		yyval = yypv[1];		/* $$ = $1	   */

		if (yyactr(next_state))
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
			amxerror(88);

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
				yyps--;
				yypv--;
			}

			/* there is no state on the stack */
			/* with an error shift ... abort  */
abort:
			return (1);

found:	/* we have a state with a shift on "error", resume parsing */
			state =  ACTION(act[1]);
			goto stack;

		  case 2:	/* no shift yet; clobber input char */
				/* don't discard EOF; quit          */
			if (!token)
				goto abort;
			token = -1;
			goto new_state;   /* try again in the same state */

		}
	}

	/*NOTREACHED*/
}
