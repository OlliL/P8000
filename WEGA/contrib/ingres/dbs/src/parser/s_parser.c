# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/scanner.h"

# define	MAXDEPTH	150

/*
**  Parser for YACC Output
**  This is the same as the yacc parser found in the BS system
**  library "/lib/liby.a".  There have been two kinds of
**  modifications. 1) The coding style has been altered to conform
**  to the DBS standard.  2) The changes marked by comments.
*/

extern char	*yyval;		/* defined in the table file */
extern char	*yylval;		/* defined in the table file */
extern char	**yypv;		/* defined in the table file */


/* -------- the next line is an DBS customization -------- */
int	yypflag =	1;	/* zero for no actions performed */
int	yydebug =	0;	/* 1 for debugging */
char	*yyv[MAXDEPTH];		/* where the values are stored */
int	yystate =	0;	/* current parser state */
int	yychar =	-1;	/* current input token number */
int	yynerrs =	0;	/* number of errors */
int	yyerrflag =	0;	/* error recovery flag */


yyparse()
{
	extern short	yygo[];
	extern short	yypgo[];
	extern short	yyr1[];
	extern short	yyr2[];
	extern short	yyact[];
	extern short	yypact[];
	/* DBS customization to make 'ps', 'p', and 'n' register variables */
	short		s[MAXDEPTH];
	register short	*ps;
	register short	*p;
	register int	n;
	register short	ac;
	register int	i;

	yystate = 0;
	yychar = -1;
	yynerrs = 0;
	yyerrflag = 0;
	ps = &s[0] - 1;
	yypv = &yyv[0] - 1;

stack:		/* put a state and value onto the stack */
	if (yydebug)
		printf("state %d value %d char %d\n", yystate, yyval, yychar);
	*++ps = yystate;
	*++yypv = yyval;

newstate:	/* set ap to point to the parsing actions for the new state */

	i = yypact[yystate + 1];
	p = &yyact[i];

actn:		/* get the next action, and perform it */
	n = (ac = *p++) & 07777;	/* n is the "address" of the action */

	switch (ac >> 12)	/* switch on operation */
	{

	  case 1:	/* skip on test */
		if (yychar < 0)
		{
#			ifdef xSTM
			if (AAtTf(76, 2))
				timtrace(13, 0);
#			endif
			yychar = yylex();
#			ifdef xSTM
			if (AAtTf(76, 2))
				timtrace(14, 0);
#			endif
			if (yydebug)
				printf( "character %d read\n", yychar );
		}
		/* ---------- the next 2 lines are an DBS customization ---------- */
		if (yychar < 0)
			return (1);
		if (n != yychar)
			p++;
		goto actn;	/* get next action */

	  case 2:		/* shift */
		yystate = n;
		yyval = yylval;
		yychar = -1;
		if (yyerrflag)
			yyerrflag--;
		goto stack;	/* stack new state */

	  case 3:		/* reduce */
		if (yydebug)
			printf("reduce %d\n", n);
		ps -= yyr2[n];
		yypv -= yyr2[n];
		yyval = yypv[1];
		/* --------  the next 2 lines are an DBS customization -------- */
		if (yypflag && yyactr(n))
			 goto abort;
		/* consult goto table to find next state */
		i = yyr1[n];
		i = yypgo[i];
		p = &yygo[i];
		for ( ; *p != *ps && *p >= 0; p += 2)
			continue;
		yystate = p[1];
		goto stack;  /* stack new state and value */

	  case 4:		/* accept */
		return (0);

	  case 0:		/* error ... attempt to resume parsing */
		switch (yyerrflag)
		{

		  case 0:		/* brand new error */
		/* ----------the next 2 lines are an DBS customization ---------- */
			/* syntax error */
			yyerror(SYMERR, (char *) 0);
			yynerrs++;

		  case 1:
		  case 2: /* incompletely recovered error ... try again */
			yyerrflag = 3;

			/* find a state where "error" is a legal shift action */
			while (ps >= s)
			{
				/* search ps actions */
				i = *ps;
				i = yypact[i + 1];
				p = &yyact[i];
				for ( ; (*p >> 12) == 1; p += 2)
				if (*p == 4352)
					goto found;

				/* the current ps has no shift onn "error", pop stack */

				if (yydebug)
					printf("err recov pops state %d, uncovers %d\n", ps[0], ps[-1]);
				ps--;
				yypv--;
			}

			/* there is no state on the stack with an error shift ... abort */

		abort:
			return (1);

		found:   /* we have a state with a shift on "error", resume parsing */
			yystate = p[1] & 07777;
			goto stack;

		  case 3:  /* no shift yet; clobber input char */
			if (yydebug)
				printf("err recov discards char %d\n", yychar);

			/* don't discard EOF; quit */
			if (yychar == 0)
				goto abort;
			yychar = -1;
			goto newstate;   /* try again in the same state */

		}
	}
}
