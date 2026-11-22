%{
#include <stdint.h>
#define YYSTYPE int16_t	/* the value stack of the WEGA yacc holds 16 bit ints */
#include <string.h>
#define z8000 1			/* as in cpp.c */

struct symtab {
	char *name;
	char *value;
};
extern char *outp, *inp, *newp;
extern int flslvl;
extern char fastab[];
struct symtab *lookup(char *, int);
char *skipbl(char *);
void pperror(char *, ...);
int yylex(void);
int yyerror(char *, ...);
static int tobinary(char *, int);

/*
 * The expression arithmetic of the original is 16 bit and the Z8000 gives these results for the cases
 * that C leaves undefined: DIV leaves the register pair unchanged when the divisor is 0 or the quotient
 * does not fit (-32768 / -1), so the quotient is the dividend and the remainder its sign (0 or -1);
 * the shift count is the low byte of the right operand as a signed number: below 0 it shifts the other way,
 * 16 or more shifts everything out.
 */
static int16_t
div16(int a, int b)
{
	if (b == 0 || (a == -32768 && b == -1))
		return (int16_t)a;
	return (int16_t)(a / b);
}

static int16_t
mod16(int a, int b)
{
	if (b == 0 || (a == -32768 && b == -1))
		return a < 0 ? -1 : 0;
	return (int16_t)(a % b);
}

/* c > 0 shifts left, c < 0 right; the count is the low byte of the right operand (signed) */
static int16_t
shift16(int a, int b, int left)
{
	int c = (signed char)b;

	if (!left)
		c = (signed char)-c;		/* the count is negated in a byte */
	if (c >= 0)
		return c >= 16 ? 0 : (int16_t)((unsigned)a << c);
	c = -c;
	return c >= 16 ? (a < 0 ? -1 : 0) : (int16_t)(a >> c);
}

#define shl16(a, b)	shift16(a, b, 1)
#define shr16(a, b)	shift16(a, b, 0)
%}
%token number stop DEFINED
%token EQ NE LE GE LS RS
%token ANDAND OROR
%left ','
%right '='
%right '?' ':'
%left OROR
%left ANDAND
%left '|' '^'
%left '&'
%nonassoc EQ NE
%nonassoc '<' '>' LE GE
%left LS RS
%left '+' '-'
%left '*' '/' '%'
%right '!' '~' UMINUS
%left '(' '.'
%%
S:	e stop	{return($1);}


e:	  e '*' e
		{$$ = $1 * $3;}
	| e '/' e
		{$$ = div16($1, $3);}
	| e '%' e
		{$$ = mod16($1, $3);}
	| e '+' e
		{$$ = $1 + $3;}
	| e '-' e
		{$$ = $1 - $3;}
	| e LS e
		{$$ = shl16($1, $3);}
	| e RS e
		{$$ = shr16($1, $3);}
	| e '<' e
		{$$ = $1 < $3;}
	| e '>' e
		{$$ = $1 > $3;}
	| e LE e
		{$$ = $1 <= $3;}
	| e GE e
		{$$ = $1 >= $3;}
	| e EQ e
		{$$ = $1 == $3;}
	| e NE e
		{$$ = $1 != $3;}
	| e '&' e
		{$$ = $1 & $3;}
	| e '^' e
		{$$ = $1 ^ $3;}
	| e '|' e
		{$$ = $1 | $3;}
	| e ANDAND e
		{$$ = $1 && $3;}
	| e OROR e
		{$$ = $1 || $3;}
	| e '?' e ':' e
		{$$ = $1 ? $3 : $5;}
	| e ',' e
		{$$ = $3;}
	| term
		{$$ = $1;}
term:
	  '-' term %prec UMINUS
		{$$ = -$2;}
	| '!' term
		{$$ = !$2;}
	| '~' term
		{$$ = ~$2;}
	| '(' e ')'
		{$$ = $2;}
	| DEFINED '(' number ')'
		{$$= $3;}
	| DEFINED number
		{$$ = $2;}
	| number
		{$$= $1;}
%%
static char Sccsid[] __attribute__((unused)) = "@[$]cpy.y\t1.2  10/13/82 18:47:37 Copr.1981 87wega3.2.";
# include "yylex.c"
