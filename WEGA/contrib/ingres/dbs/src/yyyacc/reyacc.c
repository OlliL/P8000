# include	<stdio.h>
# include	"TABLES"

/*
**	REYACC  --  recompile yacc tables
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

int	entact = sizeof yyact / sizeof yyact[0];

# define	ACTION(state)		((state) & 07777)
# define	OPERATION(state)	((state) >> 12)
# define	END_OF_GRAMMAR		-1
# define	ERROR			0
# define	SKIP			1
# define	SHIFT			2
# define	REDUCE			3
# define	ACCEPT			4

main()
{
	register int		i;
	register int		sy;
	register int		*go;
	register int		new_rule;
	register int		sv_i;
	char			buf[BUFSIZ];

	setbuf(stdout, buf);
	new_rule = 1;
	printf("*** STAR OF GRAMMAR\tSHIFT   TO  L  1   PUSH S  0\n");
	for (i = 0; i < entact; i++)
	{
		if (new_rule)
			printf("\nL%3d\t", i);

		new_rule = 1;

		sy = ACTION(yyact[i]);
		switch (OPERATION(yyact[i]))
		{
		  case END_OF_GRAMMAR:
			printf("*  *  *  E N D   O F   G R A M M A R  *  *  *\n\n");
			return;

		  case ERROR:
			printf("%12s\t(%d)", "ERROR", sy);
			break;

		  case SKIP:
			if (sy)
				printf("%12s\t", yysterm[sy - 256]);
			else
				printf("%12s\t", "EOF");
			new_rule = 0;
			continue;

		  case SHIFT:
			printf("SHIFT   TO  L%3d   PUSH S%3d", yypact[sy + 1], sy);
			break;

		case REDUCE:
			printf("R%3d  POP %2d\t", sy, yyr2[sy]);
			go = &yygo[yypgo[yyr1[sy]]];
state:
			sy = go[1];
			printf("REDUCE  TO  L%3d   PUSH S%3d", yypact[sy + 1], sy);
			if (*go >= 0)
			{
				printf("   IF S %3d", *go);
				go += 2;
				printf("\n\t\t\t");
				goto state;
			}
			break;

		  case ACCEPT:
			printf("-->\t--> -->\t   E N D   O F   A N A L Y Z E");
			break;

		  default:
			printf("*  *  *  U N K N O W N   A C T I O N  *  *  *");
		}
	}
	fflush(stdout);
}
