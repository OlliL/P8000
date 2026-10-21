/*
 * scanner - the lexical analyser
 *
 * yylex() collects the text of a token in tokbuf (at most 0x80 characters; what
 * does not fit is counted in toolong) and returns the token number; the value
 * is left in yylval. Characters that were read too far are given back with
 * ungetc. The characters are unsigned: 0xFF is the end of the file.
 */
#include <ctype.h>
#include "cas.h"
#include "parser.h"

/* the operators: the longest match wins by their order in the table */
static const struct optab {
	char	o_str[4];
	int	o_tok;
} optab[] = {
	{ ",", 30 },
	{ "+", 4 },
	{ "-", 5 },
	{ "/", 9 },
	{ "*", 10 },
	{ "|", 18 },
	{ "@", 22 },
	{ "{", 25 },
	{ "}", 26 },
	{ "(", 27 },
	{ ")", 28 },
	{ "=", 31 },
	{ "::=", 32 },
	{ "::", 20 },
	{ ":=", 33 },
	{ ":", 19 },
	{ "^FS", 6 },
	{ "^fs", 6 },
	{ "^FD", 7 },
	{ "^fd", 7 },
	{ "^F", 8 },
	{ "^f", 8 },
	{ "^S", 23 },
	{ "^s", 23 },
	{ "^O", 24 },
	{ "^o", 24 },
	{ "^<", 11 },
	{ "^>", 12 },
	{ "^&", 13 },
	{ "^|", 14 },
	{ "^X", 15 },
	{ "^x", 15 },
	{ "#", 21 },
	{ "<<", 16 },
	{ ">>", 17 },
	{ "", 0 }
};

char tokbuf[0x100];

static void ungetch(int c);

/* put c into the token buffer */
#define ADD(c)	if (cnt > 0x80) toolong++; else { *bp++ = c; cnt++; }
/* close the token text */
#define TERM()	if (cnt > 0x80) toolong++; else { *bp = '\0'; bp++; cnt++; }
/* give the last character back and drop it from the text */
#define UNGET()	{ ungetch(c); bp--; *bp = '\0'; cnt--; c = bp[-1]; }

int
yylex(void)
{
	int tok, cnt, idx, j, toolong, first;
	char *bp;
	struct pst *sym;
	struct sym *p, *q;
	const struct optab *tp;
	unsigned char c, c1, c2;

again:
	toolong = 0;
	tok = 0x22;
	bp = tokbuf;
	cnt = 0;
	do
		c = getch();
	while (c == ' ' || c == '\t' || c == ';');
	ADD(c);
	if (c == '/') {
		c = getch();
		ADD(c);
		switch (c) {
		case '*':
			c1 = ' ';
			c2 = ' ';
			while (c1 != '*' || c2 != '/') {
				c1 = getch();
				if (c1 == '\n' || c1 == '\r')
					curline++;
				if (c1 == '*') {
					c2 = getch();
					if (c2 == '\n' || c2 == '\r')
						curline++;
				}
				if (c1 == 0xFF && feof(stdin))
					break;		/* the comment is not closed */
			}
			goto again;
		case '/':
			do
				c = getch();
			while (c != '\n' && c != '\r' && !(c == 0xFF && feof(stdin)));
			ADD(c);
			break;
		default:
			UNGET();
		}
	}
	if (isalpha(c) || c == '?' || c == '_') {
		/* a name: a mnemonic, a register, or a user symbol */
		do {
			c = getch();
			ADD(c);
		} while (isalnum(c) || c == '_' || c == '?');
		UNGET();
		TERM();
		sym = pstlook(tokbuf);
		if (sym)
			goto found;
		idx = stlook(tokbuf, 0);
		if (idx == 0) {
			yylval.p = tokbuf;
			tok = 0x56;
			goto done;
		}
		p = stabp(idx);
		if (p->s_type == 3)
			goto alias;
		if (isdef(idx) == 0)
			goto symidx;
		goto value;
	alias:
		q = itabp(p->s_8);
		sym = &pst[q->s_2];
	found:
		yylval.pp = sym;
		tok = sym->p_tok;
		goto done;
	value:
		yylval.l = symval(idx);
		tok = 0x55;
		goto done;
	symidx:
		yylval.i = idx;
		tok = 0x24;
		goto done;
	}
	if (c == '~') {
		/* a local name */
		c = getch();
		ADD(c);
		if (!(isalpha(c) || c == '?' || c == '_'))
			goto done;
		do {
			c = getch();
			ADD(c);
		} while (isalnum(c) || c == '_' || c == '?');
		UNGET();
		TERM();
		idx = stlook(tokbuf, 6);
		if (idx == 0) {
			yylval.p = tokbuf;
			tok = 0x57;
			goto done;
		}
		p = stabp(idx);
		if (p->s_type == 3)
			goto alias;
		if (isdef(idx) == 0)
			goto symidx;
		goto value;
	}
	if (c == '.') {
		/* the location counter, a number .5 or a directive */
		c = getch();
		ADD(c);
		if (isdigit(c)) {
			UNGET();
			tok = 2;
			goto fraction;
		}
		if (isalpha(c) || c == '?' || c == '_') {
			do {
				c = getch();
				ADD(c);
			} while (isalnum(c) || c == '_' || c == '?');
			UNGET();
			TERM();
			sym = pstlook(tokbuf);
			if (sym)
				goto found;
			tok = 0x23;
			goto done;
		}
		UNGET();
		tok = 3;
		goto done;
	}
	if (c == '%') {
		/* a hexadecimal number %1F or a number of base 2 or 8: %(2)101, %(8)17 */
		c = getch();
		ADD(c);
		if (isxdigit(c)) {
			do {
				c = getch();
				ADD(c);
			} while (isxdigit(c));
			UNGET();
			TERM();
			yylval.l = xtol(&tokbuf[1], 16);
			tok = 0x55;
			goto done;
		}
		if (c != '(')
			goto done;
		c = getch();
		ADD(c);
		switch (c) {
		case '2':
			c = getch();
			ADD(c);
			if (c != ')')
				goto done;
			do {
				c = getch();
				ADD(c);
			} while (c == '1' || c == '0');
			UNGET();
			TERM();
			yylval.l = xtol(&tokbuf[4], 2);
			tok = 0x55;
			goto done;
		case '8':
			c = getch();
			ADD(c);
			if (c != ')')
				goto done;
			do {
				c = getch();
				ADD(c);
			} while (c >= '0' && c <= '7');
			UNGET();
			TERM();
			yylval.l = xtol(&tokbuf[4], 8);
			tok = 0x55;
			goto done;
		default:
			goto done;
		}
	}
	if (isdigit(c)) {
		do {
			c = getch();
			ADD(c);
		} while (isdigit(c));
		if (c != '.' && c != 'e' && c != 'E') {
			UNGET();
			TERM();
			yylval.l = xtol(tokbuf, 10);
			tok = 0x55;
			goto done;
		}
		if (c == '.') {
			tok = 2;
		fraction:
			do {
				c = getch();
				ADD(c);
			} while (isdigit(c));
			UNGET();
			TERM();
		}
		if (c != 'e' && c != 'E')
			goto done;
		c = getch();
		ADD(c);
		if (c == '+' || c == '-') {
			c = getch();
			ADD(c);
		}
		if (!isdigit(c)) {
			tok = 0x22;
			goto done;
		}
		do {
			c = getch();
			ADD(c);
		} while (isdigit(c));
		UNGET();
		TERM();
		goto done;
	}
	if (c == '\n' || c == '\r') {
		tok = 0x1d;
		goto done;
	}
	if (c == 0xFF) {
		tok = -1;
		goto done;
	}
	if (c == '"' || c == '\'') {
		/* a string */
		c1 = c;
		do {
			c = getch();
			ADD(c);
		} while (c != c1 && !(c == 0xFF && feof(stdin)));
		TERM();
		yylval.p = tokbuf;
		tok = 0x58;
		goto done;
	}
	/* an operator: compare with the table, one character after the other */
	j = 0;
	first = 1;
	tp = optab;
	for (;;) {
		if (tp->o_str[j] == 0)
			goto done;
	cmp:
		if (tp->o_str[j] == c) {
			if (tp->o_str[j + 1] != 0) {
				c = getch();
				ADD(c);
				j++;
				first = 0;
				goto cmp;
			}
			tok = tp->o_tok;
			goto done;
		}
		if (first == 0) {
			UNGET();
			j--;
			first++;
		}
		tp++;
	}
done:
	if (tok == 0x22) {
		error(2, "Invalid token\n");
		do
			c = getch();
		while (c != ' ' && c != '\t' && c != ';' && !(c == 0xFF && feof(stdin)));
		ADD(c);
		UNGET();
		return (tok);
	}
	if (tok == 0x23) {
		error(2, "Unknown keyword\n");
		return (tok);
	}
	(void)toolong;
	return (tok);
}

/* the next character of the source (stdin); with the option -^ it is echoed */
int
getch(void)
{
	int c;

	c = (unsigned char)getc(stdin);
	if (echoflag)
		printf("%c", c);
	return (c);
}

/* give a character back */
static void
ungetch(int c)
{
	if (ungetc((unsigned char)c, stdin) == EOF) {
		perror("scanner");
		done(-1);
	}
}
