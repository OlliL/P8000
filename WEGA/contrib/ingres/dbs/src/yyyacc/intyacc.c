# include	<stdio.h>
# include	"TABLES"

/*
**	INTYACC  --  interactive parser for yacc tables
**
**
**	insert YACC tables
**
**	short	yygo[]		= { ... };
**	short	yypgo[]		= { ... };
**	short	yyr1[]		= { ... };
**	short	yyr2[]		= { ... };
**	short	yyact[]		= { ... };
**	short	yypact[]	= { ... };
**	char	*yysterm[]	= { ... };
**
**	as file TABLES !!!
*/

# define	ACTION(state)		((state) & 07777)
# define	OPERATION(state)	((state) >> 12)

/* the OPERATIONS */
# define	ERROR			0
# define	SKIP			1
# define	SHIFT			2
# define	REDUCE			3
# define	ACCEPT			4

/* SIZE for state and value stack */
# define	MAXDEPTH		150


main()
{
	register int	state;		/* current parser state		*/
	register int	token;		/* current input token number	*/
	short		states[MAXDEPTH];	/*    state stack	*/
	register short	*ps;		/* pointer to state stack	*/
	int		values[10 * MAXDEPTH];	/*    token stack	*/
	register int	*pv;		/* pointer to token stack	*/
	register short	*act;		/* pointer to performing action	*/
	register int	next_state;
	register int	i;
	int		r_ahead;	/* read ahead flag		*/
	int		new_alter;	/* flag for a new alternative	*/
	int		alter[50];	/*            alternative stack	*/
	int		a;		/* pointer to alternative stack	*/
	char		cbuf[6];	/*            character buffer	*/
	char		*c;		/* pointer to character buffer	*/

newstart:
	values[0] = -2;	/* no TOKEN given ! ! ! */

restart:
	printf("\n\n");
	state = 0;
	token = -1;
	ps = &states[-1];
	pv = &values[-1];

stack:
	*++ps = state;	/* push state into state stack */

new_state:
	/* set act to point to the parsing actions for the new state */
	i = yypact[state + 1];
	act = &yyact[i];

next_action:		/* get the next action, and perform it */
	next_state = ACTION(*act);
	switch (OPERATION(*act++))	/* switch on operation */
	{
	  case SKIP:
		if (r_ahead = token == -1)
		{
			/* get next token */
			token = *++pv;
			a = 0;
			new_alter = 1;
		}
		if (token == -2)
		{
			if (new_alter)
				printf("\n(%2d)", a);
			else
				new_alter = 1;
			if (next_state)
				printf(" %s", yysterm[next_state - 256]);
			else
				printf(" EOF");
			alter[a++] = next_state;
		}
		if (next_state != token)
		{
			act++;
			if (token != -2)
			{
				token = -1;
				--pv;
			}
		}
		else if (r_ahead)
		{
			if (next_state)
				printf(" %s", yysterm[next_state - 256]);
			else
				printf(" EOF");
		}
		else
		{
			if (next_state)
				printf(" %s", yysterm[next_state - 256]);
			else
				printf(" EOF");
		}
		goto next_action;	/* get next action */

	  case SHIFT:		/* goto next_state */
		state = next_state;
		token = -1;
		goto stack;	/* stack new state and value */

	  case REDUCE:		/* rule next_state */
		if (token == -2 && new_alter)
		{
			printf("\n(%2d)", a);
			new_alter = 0;
		}
		printf(" R%d", next_state);
		ps -= yyr2[next_state];		/* pop state stack */

		/* consult goto table to find next state */
		i = yyr1[next_state];
		i = yypgo[i];
		act = &yygo[i];
		next_state = *ps;
		for ( ; *act != next_state && *act >= 0; act += 2)
			continue;
		state = act[1];
		goto stack;  /* stack new state and value */

	  case ACCEPT:
		printf("\n\nE N D[J   O F   G R A M M A R\n");
		return (0);

	  case ERROR:
		if (token != -2)
		{
			printf("\n\nU N E X P E C T E D   E R R O R\n");
			return (2);
		}

		i = a - 1;
		if (a == 1)
			goto suggest;

		/* read alternative */
		printf("\n(??) ");
		fflush(stdout);
		c = cbuf;
		while (*c = getchar())
		{
			switch (*c)
			{
			  case '\n':
				*c = 0;
				goto newtoken;

			  case '\b':
				*--pv = -2;
				printf("q");
				break;
			  case '\033':
				goto restart;

			  case '\025':
				goto newstart;

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
				c++;
				break;
			}
		}
newtoken:
		i = atoi(cbuf);
suggest:
		if (i >= a)
		{
			printf("\n\nG R A M M A R   I N T E R R U P T E D\n");
			return (1);
		}
		*pv++ = alter[i];
		*pv = -2;
		goto restart;
	}
}
