/*
** ---  RC_ERROR.C ------ YYERROR  --------------------------------------------
**
**	Version: @(#)rc_error.c		4.0	02/05/89
**
*/

# include	"rc.h"


static char	*ERROR[] =
{
/* 000 */	"SYNTAX ERROR",
/* 001 */	"Illegal PROC `%d'",
/* 002 */	"PROC `%d' declared twice",
/* 003 */	"PROC `%d' not declared",
/* 004 */	"Cannot create R_CODE file `%s'",
/* 005 */	"Cannot write to R_CODE file `%s'",
/* 006 */	"Illegal VAR `%d'",
/* 007 */	"Illegal SWITCH `%d'",
/* 008 */	"VALUE `%d' < 0 not expected",
/* 009 */	"Symbol Table OVERFLOW",
/* 010 */	"Non terminated COMMENT",
/* 011 */	"Non terminated STRING",
/* 012 */	"STRING too long (>255)",
/* 013 */	"Unknown OPERATOR `%s'",
/* 014 */	"NUMBER too long (> 255)",
/* 015 */	"FLOATING conversion error `%s'",
/* 016 */	"INTEGER conversion error `%s'",
/* 017 */	"NAME `%s' to long (>12)",
/* 018 */	"BACKUP peek buffer overflow (>2)",
/* 019 */	"CASE in CASE not allowed",
/* 020 */	"Cannot print two objects",
/* 021 */	"No object to print",
/* 022 */	"NUMBER out of range (%d..%d)",
/* 023 */	"Too many CASES",
/* 024 */	"Too many MACROS",
/* 025 */	"Illegal DOMAIN %d, expected %d",
/* 026 */	"Too many DOMAINS",
/* 027 */	"ILLEGAL field length %d",
};


# ifdef P8000
rc_error(a0, a1, a2)
register int	a0;
register char	*a1;
register char	*a2;
{
# else
rc_error(av)
int	av;
{
	register int	*a;

	a = &av;
# endif

	if (Sflag)
		printf("\n****: ");
	else
		printf("\n%4d: ", Line);
# ifdef P8000
	printf(ERROR[a0], a1, a2);
# else
	printf(ERROR[a[0]], a[1], a[2]);
# endif
	if (Sflag)
		printf("\n%4d: . . .  ", Line);
	S_error++;
}
