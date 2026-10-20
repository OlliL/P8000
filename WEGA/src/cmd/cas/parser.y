%{
static char sccsid[] = "@[$]/as/as/parser.y\t1.2 05/27/83 15:52:35 - 87wega3.2";

/*
 * parser - the syntax analyser of cas (yacc grammar)
 *
 * One statement per line.  The actions write the statement to the intermediate file
 * (ifile.c) and keep the location counter (locntr.c); the expression nodes are
 * evaluated by the functions of expr.c.  Syntax errors are reported with yyerror()
 * and the rest of the line is skipped (synrecover).
 */
#include "cas.h"

NODE *plist = 0;		/* the labels of the statement not yet defined */
int segmode = 0;		/* .seg / .nonseg */
unsigned fpflag = 0;		/* flags of a floating point instruction */
unsigned fpmode = 0;		/* its rounding and infinity mode */
int nround = 0;			/* the modes seen so far */
int ninf = 0;
int nameflag = 0;		/* a name was read: it is put to the symbol file */
char errmsg[80] = {0};

NODE *listp;			/* walks over a list of nodes */
int symmode;			/* how a name is looked up: 6 local, 0 global */

extern NODE *mknode1(), *mknode2(), *mknode6(), *mknode7(), *mkstring();
extern NODE *fn4DF6(), *fn52C6(), *fn538A(), *fn543C(), *fn5CCC(), *fn59FC();
extern NODE *fn57C0(), *fn5708(), *fn5650(), *fn5598(), *fn54E0(), *fn5876();
extern NODE *fn5928(), *fn59DC(), *fn5CAC();
extern char fn4ECC();
extern int stenter();
%}

%union {
	long	l;
	int	i;
	char	*p;
	struct pst *pp;
	NODE	*np;
}

%token DOT 3
%token PLUS 4
%token MINUS 5
%token DIV 9
%token MUL 10
%token SHL 11
%token SHR 12
%token AND 13
%token OR 14
%token XOR 15
%token LSEG 16
%token RSEG 17
%token BAR 18
%token COLON 19
%token DCOLON 20
%token HASH 21
%token AT 22
%token SSEG 23
%token OSEG 24
%token LBRACE 25
%token RBRACE 26
%token LPAR 27
%token RPAR 28
%token NL 29
%token COMMA 30
%token EQ 31
%token GASSIGN 32
%token LASSIGN 33
%token <i> SYMBOL 36
%token <pp> COMM 37
%token <pp> EXTERN 38
%token <pp> EVEN 39
%token <pp> ODD 40
%token <pp> BYTE 41
%token <pp> WORD 42
%token <pp> LONG 43
%token <pp> BLKB 44
%token <pp> BLKW 45
%token <pp> BLKL 46
%token <pp> STABL 47
%token <pp> STABN 48
%token <pp> STABP 49
%token <pp> STABR 50
%token <pp> REG 51
%token <pp> REGPAIR 52
%token <pp> BREG 53
%token <pp> COND 54
%token <pp> NVI 55
%token <pp> OPCODE 56
%token <pp> FLAGS 58
%token <pp> CTLREG 59
%token <pp> REGQUAD 60
%token <pp> FPREG 61
%token <pp> TOK62 62
%token <pp> TOK63 63
%token <pp> TOK64 64
%token <pp> TOK65 65
%token <pp> TOK66 66
%token <pp> TOK67 67
%token <pp> TOK68 68
%token <pp> TOK69 69
%token <pp> TOK70 70
%token <pp> TOK71 71
%token <pp> TOK72 72
%token <pp> FPFLAG 73
%token <pp> FPMODE 74
%token <pp> CODE 75
%token <pp> DATA 76
%token <pp> BSS 77
%token <pp> SEG 78
%token <pp> NONSEG 79
%token <pp> JPR 80
%token <pp> CALLR 81
%token <pp> PSEC 82
%token <pp> ADDR 83
%token <pp> LINE 84
%token <l> NUMBER 85
%token <p> NEWNAME 86
%token <p> NEWLOCAL 87
%token <p> STRING 88

%type <np> label name belem blist welem wlist lelem llist symref operand operands immop addrop indop idxop baseop bidxop regop expr sexpr bexpr externs commnames fpflags fpspec fpmodes term primary unary
%type <pp> adrdir
%type <np> aelem alist
%type <pp> opcode regtok flags flag modes mode keyword

%%

program	: program line
	| line	;

line	: lineinit stmt NL
		{
		chknodes(); curline++;
		}
	| lineinit stmt error
		{
		yyerror("Expecting carriage return or linefeed"); chknodes(); yychar = -1; curline++;
		}
	| NL
		{
		curline++;
		}
	| error
		{
		yyerror("Expecting beginning of line");
		}	;

lineinit	: 
		{
		iputline();
		}	;

stmt	: LBRACE
		{
		deflabels(); blkopen();
		}
	| RBRACE
		{
		deflabels(); blkclose();
		}
	| assign
	| datadir
	| instr
	| simpledir
	| label stmt
	| label
		{
		deflabels();
		}
	| error
		{
		yyerror("Expecting beginning of program");
		}	;

simpledir	: CODE
		{
		deflabels(); fn4BB0(0); iputb(12);
		}
	| DATA
		{
		deflabels(); fn4BB0(1); iputb(13);
		}
	| BSS
		{
		deflabels(); fn4BB0(2); iputb(14);
		}
	| EVEN
		{
		fn4CDA(); deflabels();
		}
	| ODD
		{
		fn4DC4();
		}
	| SEG
		{
		segmode++;
		}
	| NONSEG
		{
		segmode = 0;
		}
	| LINE NUMBER STRING
	| LINE NUMBER
	| LINE error
		{
		yyerror("Expecting line number in .line statement");
		}
	| PSEC
		{
		fn4CB4(0); iputb(0x29); iputb(0);
		}
	| PSEC name
		{
		fn4BBC($2);
		}	;

instr	: opcode operands
		{
		fn4CDA(); deflabels(); putins($1, $2);
		}
	| opcode
		{
		fn4CDA(); deflabels(); putins($1, mknode2(0L, 0L, 0L));
		}
	| opcode fpspec
		{
		fn4CDA(); deflabels(); putins($1, $2);
		}
	| JPR operands
		{
		fn4CDA(); deflabels(); fn84FC($1->p_tok, $2);
		}
	| JPR error
		{
		yyerror("Expecting operand(s) after jpr pseudo-instruction");
		}
	| CALLR operands
		{
		fn4CDA(); deflabels(); fn84FC($1->p_tok, $2);
		}
	| CALLR error
		{
		yyerror("Expecting operand(s) after callr pseudo-instruction");
		}	;

opcode	: OPCODE
	| TOK62
	| TOK63
	| TOK64
	| TOK65
	| TOK66
	| TOK67
	| TOK68	;

operands	: operand COMMA operands
		{
		$1->n_next = $3;
		}
	| operand COMMA error
		{
		yyerror("Expecting operand after comma in list of operands");
		}
	| operand
		{
		$1->n_next = 0L;
		}	;

operand	: idxop
	| baseop
	| bidxop
	| immop
	| addrop
	| indop
	| regop	;

regop	: regtok
		{
		$$ = mknode2($1->p_tok, mknode1(1, (long)$1->p_n), 0L);
		}	;

regtok	: COND
	| CTLREG
	| FLAGS
	| NVI
	| REG
	| REGPAIR
	| BREG
	| REGQUAD
	| TOK70
	| FPREG
	| TOK71
	| TOK72
	| TOK69	;

fpspec	: fpflags
	| fpmodes	;

fpmodes	: modes
		{
		$$ = mknode2($1->p_tok, mknode1(1, (long)fpmode), 0L);
		fpmode = 0;
		nround = 0;
		ninf = 0;
		}	;

modes	: mode COMMA modes
		{
		switch ($1->p_n) {
		case 0x60: case 0x62: case 0x64: case 0x66:
			if (nround != 0)
				error(2, "Too many rounding modes specified\n");
			nround++;
			fpmode |= $1->p_n;
			break;
		case 0x10: case 0x11:
			if (nround != 0)
				error(2, "Too many infinity modes specified\n");
			ninf++;
			fpmode |= $1->p_n;
			break;
		}
		}
	| mode COMMA error
		{
		yyerror("Expecting floating point mode after comma");
		}
	| mode
		{
		switch ($1->p_n) {
		case 0x60: case 0x62: case 0x64: case 0x66:
			if (nround != 0)
				error(2, "Too many rounding modes specified\n");
			nround++;
			fpmode |= $1->p_n;
			break;
		case 0x10: case 0x11:
			if (nround != 0)
				error(2, "Too many infinity modes specified\n");
			ninf++;
			fpmode |= $1->p_n;
			break;
		}
		}	;

mode	: FPMODE	;

fpflags	: flags
		{
		$$ = mknode2($1->p_tok, mknode1(1, (long)fpflag), 0L);
		fpflag = 0;
		}	;

flags	: flag COMMA flags
		{
		fpflag |= $1->p_n;
		}
	| flag COMMA error
		{
		yyerror("Expecting floating point flag after comma");
		}
	| flag
		{
		fpflag |= $1->p_n;
		}	;

flag	: FPFLAG	;

idxop	: sexpr LPAR REG RPAR
		{
		$$ = mknode2(0x61, $1, mknode1(1, (long)$3->p_n));
		}
	| sexpr LPAR error
		{
		yyerror("Expecting register number after left parenthesis in indexed operand");
		}
	| sexpr LPAR REG error
		{
		yyerror("Expecting right parenthesis in indexed operand");
		}	;

baseop	: REG LPAR HASH sexpr RPAR
		{
		$$ = mknode2(0x63, mknode1(1, (long)$1->p_n), $4);
		}
	| REG LPAR error
		{
		yyerror("Expecting # or register after left parenthesis");
		}
	| REG LPAR HASH error
		{
		yyerror("Expecting address expression after # in base addressed operand");
		}
	| REG LPAR HASH sexpr error
		{
		yyerror("Expecting right parenthesis in base addressed operand");
		}
	| REGPAIR LPAR HASH sexpr RPAR
		{
		$$ = mknode2(0x63, mknode1(1, (long)$1->p_n), $4);
		}
	| REGPAIR LPAR error
		{
		yyerror("Expecting # or register after left parenthesis");
		}
	| REGPAIR LPAR HASH error
		{
		yyerror("Expecting segmented address expression in base addressed operand");
		}
	| REGPAIR LPAR HASH sexpr error
		{
		yyerror("Expecting right parenthesis in segmented base addressed operand");
		}	;

bidxop	: REG LPAR REG RPAR
		{
		$$ = mknode2(0x64, mknode1(1, (long)$1->p_n), mknode1(1, (long)$3->p_n));
		}
	| REG LPAR REG error
		{
		yyerror("Expecting right parenthesis in base indexed operand");
		}
	| REGPAIR LPAR REG RPAR
		{
		$$ = mknode2(0x64, mknode1(1, (long)$1->p_n), mknode1(1, (long)$3->p_n));
		}
	| REGPAIR LPAR REG error
		{
		yyerror("Expecting right parenthesis in segmented base indexed operand");
		}	;

immop	: HASH sexpr
		{
		$$ = mknode2(0x62, $2, 0L);
		}
	| HASH error
		{
		yyerror("Expecting address expression after # (immediate) operator");
		}	;

addrop	: sexpr
		{
		$$ = mknode2(0x56, $1, 0L);
		}	;

indop	: AT REG
		{
		if ($2->p_n == 0)
			error(2, "Cannot perform an indirection on r0\n");
		else
			$$ = mknode2(0x60, mknode1(1, (long)$2->p_n), 0L);
		}
	| AT REGPAIR
		{
		$$ = mknode2(0x60, mknode1(1, (long)$2->p_n), 0L);
		}
	| AT error
		{
		yyerror("Expecting register or register pair after @ (indirect) operator");
		}	;

datadir	: datalist
	| externdir
	| commdir	;

externdir	: EXTERN externs
		{
		deflabels();
		}
	| EXTERN error
		{
		yyerror("Expecting identifier after .extern");
		}	;

externs	: name COMMA externs
		{
		$1->n_u.cc.c1 = 0;
		defsym($1, mknode1(3, 0L));
		}
	| name COMMA error
		{
		yyerror("Expecting identifier after comma in .extern statement");
		}
	| name
		{
		$1->n_u.cc.c1 = 0;
		defsym($1, mknode1(3, 0L));
		}	;

commdir	: COMM commargs
		{
		deflabels();
		}
	| COMM error
		{
		yyerror("Expecting numeric expression after .comm");
		}	;

commargs	: expr COMMA commnames
		{
		defcomm($1, $3);
		}
	| expr error
		{
		yyerror("Expecting comma after numeric expression in .comm statement");
		}
	| expr COMMA error
		{
		yyerror("Expecting identifier in .comm statement");
		}	;

commnames	: name COMMA commnames
		{
		$1->n_u.cc.c1 = 0;
		$1->n_next = $3;
		}
	| name COMMA error
		{
		yyerror("Expecting identifier after comma in .comm statement");
		}
	| name
		{
		$1->n_u.cc.c1 = 0;
		$1->n_next = 0L;
		}	;

datalist	: bytedir blist
		{
		for (listp = $2; listp != 0; listp = listp->n_next)
			iputnode(listp);
		}
	| worddir wlist
		{
		for (listp = $2; listp != 0; listp = listp->n_next)
			iputnode(listp);
		}
	| longdir llist
		{
		for (listp = $2; listp != 0; listp = listp->n_next)
			iputnode(listp);
		}
	| adrdir alist
		{
		for (listp = $2; listp != 0; listp = listp->n_next)
			iputnode(listp);
		}
	| bytedir
		{
		locadv(1);
		iputloc();
		}
	| worddir
		{
		locadv(2);
		iputloc();
		}
	| longdir
		{
		locadv(4);
		iputloc();
		}
	| adrdir
		{
		if (segmode != 0)
			locadv(4);
		else
			locadv(2);
		iputloc();
		}
	| BLKB expr
		{
		deflabels();
		if (fn4ECC($2) == 3)
			locadv((int)$2->n_u.l);
		else
			error(2, "Block specifier must be constant\n");
		nfreed++;
		iputloc();
		}
	| BLKB error
		{
		yyerror("Expecting numeric expression in .blkb statement");
		}
	| BLKW expr
		{
		fn4CDA();
		deflabels();
		if (fn4ECC($2) == 3)
			locadv((int)$2->n_u.l << 1);
		else
			error(2, "Block specifier must be constant\n");
		nfreed++;
		iputloc();
		}
	| BLKW error
		{
		yyerror("Expecting numeric expression in .blkw statement");
		}
	| BLKL expr
		{
		fn4CDA();
		deflabels();
		if (fn4ECC($2) == 3)
			locadv((int)$2->n_u.l << 2);
		else
			error(2, "Block specifier must be constant\n");
		nfreed++;
		iputloc();
		}
	| BLKL error
		{
		yyerror("Expecting numeric expression in .blkl statement");
		}
	| STABN expr COMMA expr COMMA expr COMMA STRING
		{
		putstabn($2, $4, $6, $8);
		}
	| STABN error
		{
		yyerror("Expecting first argument in .stabn statement");
		}
	| STABN expr error
		{
		yyerror("Expecting comma after first argument in .stabn statement");
		}
	| STABN expr COMMA error
		{
		yyerror("Expecting second argument in .stabn statement");
		}
	| STABN expr COMMA expr error
		{
		yyerror("Expecting comma after second argument in .stabn statement");
		}
	| STABN expr COMMA expr COMMA error
		{
		yyerror("Expecting third argument in .stabn statement");
		}
	| STABN expr COMMA expr COMMA expr error
		{
		yyerror("Expecting comma after third argument in .stabn statement");
		}
	| STABN expr COMMA expr COMMA expr COMMA error
		{
		yyerror("Expecting fourth argument in .stabn statement. Must be a string");
		}
	| STABR expr COMMA expr COMMA STRING
		{
		putstabr($2, $4, $6);
		}
	| STABR error
		{
		yyerror("Expecting first argument in .stabr statement");
		}
	| STABR expr error
		{
		yyerror("Expecting comma after first argument in .stabr statement");
		}
	| STABR expr COMMA error
		{
		yyerror("Expecting second argument in .stabr statement");
		}
	| STABR expr COMMA expr error
		{
		yyerror("Expecting comma after second argument in .stabr statement");
		}
	| STABR expr COMMA expr COMMA error
		{
		yyerror("Expecting third argument in .stabr statement. Must be a string");
		}
	| STABP expr COMMA expr COMMA expr COMMA expr COMMA STRING
		{
		putstabp($2, $4, $6, $8, $10);
		}
	| STABP error
		{
		yyerror("Expecting first argument in .stabp statement");
		}
	| STABP expr error
		{
		yyerror("Expecting comma after first argument in .stabp statement");
		}
	| STABP expr COMMA error
		{
		yyerror("Expecting second argument in .stabp statement");
		}
	| STABP expr COMMA expr error
		{
		yyerror("Expecting comma after second argument in .stabp statement");
		}
	| STABP expr COMMA expr COMMA error
		{
		yyerror("Expecting third argument in .stabp statement");
		}
	| STABP expr COMMA expr COMMA expr error
		{
		yyerror("Expecting comma after third argument in .stabp statement");
		}
	| STABP expr COMMA expr COMMA expr COMMA error
		{
		yyerror("Expecting fourth argument in .stabp statement");
		}
	| STABP expr COMMA expr COMMA expr COMMA expr error
		{
		yyerror("Expecting comma after fourth argument in .stabp statement");
		}
	| STABP expr COMMA expr COMMA expr COMMA expr COMMA error
		{
		yyerror("Expecting fifth argument in .stabp statement. Must be a string");
		}
	| STABL NUMBER
		{
		iputb(0x2c);
		iputw((int)$2);
		}
	| STABL error
		{
		yyerror("Expecting line number in .stabl statement");
		}	;

bytedir	: BYTE
		{
		deflabels();
		}	;

blist	: belem COMMA blist
		{
		switch ($1->n_type) {
		case 6:
			locadv($1->n_u.wl.w);
			$1->n_next = $3;
			break;
		case 1: case 9:
			$1->n_next = $3;
			locadv(1);
			break;
		case 0x16:
			((NODE *)$1->n_u.l)->n_next = $3;
			$$ = $1->n_next;
			nfreed++;
			break;
		}
		}
	| belem COMMA error
		{
		yyerror("Expecting address expression after comma in .byte statement");
		}
	| belem
		{
		switch ($1->n_type) {
		case 6:
			locadv($1->n_u.wl.w);
			$1->n_next = 0L;
			break;
		case 1: case 9:
			$1->n_next = 0L;
			locadv(1);
			break;
		case 0x16:
			((NODE *)$1->n_u.l)->n_next = 0L;
			$$ = $1->n_next;
			nfreed++;
			break;
		}
		}	;

belem	: expr LPAR sexpr RPAR
		{
		if ($3->n_type == 3)
			$3->n_type = 1;
		else if ($3->n_type == 7)
			$3->n_type = 9;
		else if ($3->n_type == 0x19)
			$3->n_type = 23;
		$$ = mknode6((int)$1->n_u.l, $3);
		nfreed++;
		}
	| expr LPAR error
		{
		yyerror("Expecting address expression in repeat count in .byte statement");
		}
	| expr LPAR sexpr error
		{
		yyerror("Expecting right parenthesis after repeat count in .byte statement");
		}
	| sexpr
		{
		if ($1->n_type == 3)
			$1->n_type = 1;
		else if ($1->n_type == 7)
			$1->n_type = 9;
		else if ($1->n_type == 0x19)
			$1->n_type = 23;
		}
	| STRING
		{
		$$ = mkstring($1);
		}	;

worddir	: WORD
		{
		fn4CDA(); deflabels();
		}	;

wlist	: welem COMMA wlist
		{
		if ($1->n_type == 6)
			locadv($1->n_u.wl.w << 1);
		else
			locadv(2);
		$1->n_next = $3;
		}
	| welem COMMA error
		{
		yyerror("Expecting address expression after comma in .word statement");
		}
	| welem
		{
		if ($1->n_type == 6)
			locadv($1->n_u.wl.w << 1);
		else
			locadv(2);
		$1->n_next = 0L;
		}	;

welem	: expr LPAR sexpr RPAR
		{
		if ($3->n_type == 3)
			$3->n_type = 2;
		else if ($3->n_type == 7)
			$3->n_type = 8;
		else if ($3->n_type == 0x19)
			$3->n_type = 24;
		$$ = mknode6((int)$1->n_u.l, $3);
		nfreed++;
		}
	| expr LPAR error
		{
		yyerror("Expecting address expression in repeat count in .word statement");
		}
	| expr LPAR sexpr error
		{
		yyerror("Expecting right parenthesis after repeat count in .word statement");
		}
	| sexpr
		{
		if ($1->n_type == 3)
			$1->n_type = 2;
		else if ($1->n_type == 7)
			$1->n_type = 8;
		else if ($1->n_type == 0x19)
			$1->n_type = 24;
		}	;

adrdir	: ADDR
		{
		fn4CDA(); deflabels();
		}	;

alist	: alist COMMA aelem
		{
		if ($1->n_type == 6) {
			if (segmode != 0)
				locadv($1->n_u.wl.w << 2);
			else
				locadv($1->n_u.wl.w << 1);
		} else {
			if (segmode != 0)
				locadv(4);
			else
				locadv(2);
		}
		$1->n_next = $3;
		}
	| alist COMMA error
		{
		yyerror("Expecting address expression after comma in .addr statement");
		}
	| aelem
		{
		if ($1->n_type == 6) {
			if (segmode != 0)
				locadv($1->n_u.wl.w << 2);
			else
				locadv($1->n_u.wl.w << 1);
		} else {
			if (segmode != 0)
				locadv(4);
			else
				locadv(2);
		}
		$1->n_next = 0L;
		}	;

aelem	: expr LPAR sexpr RPAR
		{
		if (segmode == 0) {
			if ($3->n_type == 3)
				$3->n_type = 2;
			else if ($3->n_type == 7)
				$3->n_type = 8;
			else if ($3->n_type == 0x19)
				$3->n_type = 24;
		}
		$$ = mknode6((int)$1->n_u.l, $3);
		nfreed++;
		}
	| expr LPAR error
		{
		yyerror("Expecting address expression in repeat count in .addr statement");
		}
	| expr LPAR sexpr error
		{
		yyerror("Expecting right parenthesis after repeat count in .addr statement");
		}
	| sexpr
		{
		if (segmode == 0) {
			if ($1->n_type == 3)
				$1->n_type = 2;
			else if ($1->n_type == 7)
				$1->n_type = 8;
			else if ($1->n_type == 0x19)
				$1->n_type = 24;
		}
		}	;

longdir	: LONG
		{
		fn4CDA(); deflabels();
		}	;

llist	: lelem COMMA llist
		{
		if ($1->n_type == 6)
			locadv($1->n_u.wl.w << 2);
		else
			locadv(4);
		$1->n_next = $3;
		}
	| lelem COMMA error
		{
		yyerror("Expecting address expression after comma in .long statement");
		}
	| lelem
		{
		if ($1->n_type == 6)
			locadv($1->n_u.wl.w << 2);
		else
			locadv(4);
		$1->n_next = 0L;
		}	;

lelem	: expr LPAR sexpr RPAR
		{
		$$ = mknode6((int)$1->n_u.l, $3);
		nfreed++;
		}
	| expr LPAR error
		{
		yyerror("Expecting address expression in repeat count in .long statement");
		}
	| expr LPAR sexpr error
		{
		yyerror("Expecting right parenthesis after repeat count in .long statement");
		}
	| sexpr	;

assign	: name GASSIGN sexpr
		{
		deflabels();
		$1->n_u.cc.c1 = 2;
		defsym($1, $3);
		}
	| name GASSIGN error
		{
		yyerror("Expecting address expression in global assignment statement");
		}
	| name LASSIGN sexpr
		{
		deflabels();
		$1->n_u.cc.c1 = 4;
		defsym($1, $3);
		}
	| name LASSIGN error
		{
		yyerror("Expecting address expression or reserved word in local assignment statement");
		}
	| name LASSIGN keyword
		{
		deflabels();
		$1->n_u.cc.c1 = 4;
		defalias($1, $3);
		}
	| name error
		{
		yyerror("Expecting := , ::= , :: or : after identifier");
		}
	| DOT EQ expr
		{
		deflabels();
		fn4E6A($3);
		}
	| DOT error
		{
		yyerror("Expecting = after . for assignment of pc.");
		}
	| DOT EQ error
		{
		yyerror("Expecting numeric expression after .=");
		}	;

keyword	: COMM
	| EXTERN
	| EVEN
	| ODD
	| BYTE
	| WORD
	| LONG
	| BLKB
	| BLKW
	| BLKL
	| REG
	| REGPAIR
	| BREG
	| COND
	| NVI
	| OPCODE
	| FLAGS
	| CTLREG
	| REGQUAD
	| FPREG
	| TOK62
	| TOK63
	| TOK64
	| TOK65
	| TOK66
	| TOK67
	| TOK68
	| TOK69
	| TOK70
	| TOK71
	| TOK72
	| FPFLAG
	| FPMODE
	| CODE
	| DATA
	| BSS
	| SEG
	| NONSEG	;

sexpr	: BAR expr BAR
		{
		$$ = fn52C6($2);
		shortaddr++;
		}
	| BAR error
		{
		yyerror("Expecting numeric expression after short segmented address operator");
		}
	| BAR expr error
		{
		yyerror("Expecting | at end of expression");
		}
	| SSEG expr
		{
		$$ = fn538A($2);
		shortaddr++;
		}
	| SSEG error
		{
		yyerror("Expecting numeric expression after ^S operator");
		}
	| OSEG expr
		{
		$$ = fn543C($2);
		shortaddr++;
		}
	| OSEG error
		{
		yyerror("Expecting numeric expression after ^O operator");
		}
	| expr	;

expr	: expr PLUS bexpr
		{
		$$ = fn5CCC($1, $3);
		}
	| expr PLUS error
		{
		yyerror("Expecting expression to add");
		}
	| expr MINUS bexpr
		{
		$$ = fn59FC($1, $3);
		}
	| expr MINUS error
		{
		yyerror("Expecting expression to subtract");
		}
	| bexpr	;

bexpr	: bexpr SHL term
		{
		$$ = fn57C0($1, $3);
		}
	| bexpr SHL error
		{
		yyerror("Expecting numeric expression after ^< (shift left)");
		}
	| bexpr SHR term
		{
		$$ = fn5708($1, $3);
		}
	| bexpr SHR error
		{
		yyerror("Expecting numeric expression after ^> (shift right)");
		}
	| bexpr AND term
		{
		$$ = fn5650($1, $3);
		}
	| bexpr AND error
		{
		yyerror("Expecting numeric expression after ^& (and)");
		}
	| bexpr OR term
		{
		$$ = fn5598($1, $3);
		}
	| bexpr OR error
		{
		yyerror("Expecting numeric expression after ^| (or)");
		}
	| bexpr XOR term
		{
		$$ = fn54E0($1, $3);
		}
	| bexpr XOR error
		{
		yyerror("Expecting numeric expression after ^X (exclusive-or)");
		}
	| term	;

term	: term MUL unary
		{
		$$ = fn5876($1, $3);
		}
	| term MUL error
		{
		yyerror("Expecting multiplier expression");
		}
	| term DIV unary
		{
		$$ = fn5928($1, $3);
		}
	| term DIV error
		{
		yyerror("Expecting divisor expression");
		}
	| unary	;

unary	: MINUS primary
		{
		$$ = fn59DC($2);
		}
	| MINUS error
		{
		yyerror("Expecting numeric expression after unary minus");
		}
	| PLUS primary
		{
		$$ = fn5CAC($2);
		}
	| PLUS error
		{
		yyerror("Expecting numeric expression after unary plus");
		}
	| primary	;

primary	: NUMBER
		{
		$$ = mknode1(3, $1);
		}
	| LSEG NUMBER RSEG NUMBER
		{
		$$ = mknode1(3, ($2 << 24) | $4);
		}
	| LSEG error
		{
		yyerror("Expecting segment number after <<");
		}
	| LSEG NUMBER error
		{
		yyerror("Expecting >> after segment number");
		}
	| LSEG NUMBER RSEG error
		{
		yyerror("Expecting segment offset number");
		}
	| DOT
		{
		$$ = fn4DF6();
		}
	| LPAR expr RPAR
		{
		$$ = $2;
		}
	| LPAR error
		{
		yyerror("Expecting numeric expression after left parenthesis");
		}
	| LPAR expr error
		{
		yyerror("Expecting right parenthesis in expression");
		}
	| symref	;

symref	: name
		{
		if (nameflag != 0) {
			sputb(0x15);
			sputw($1->n_u.pw.w);
			nameflag = 0;
		}
		}	;

label	: name DCOLON
		{
		$1->n_u.cc.c1 = 3;
		$1->n_next = plist;
		plist = $1;
		}
	| name COLON
		{
		$1->n_u.cc.c1 = 5;
		$1->n_next = plist;
		plist = $1;
		}	;

name	: NEWLOCAL
		{
		nameflag = 0;
		symmode = 6;
		$$ = mknode7(stenter($1, (unsigned char)symmode));
		nameflag++;
		}
	| NEWNAME
		{
		nameflag = 0;
		symmode = 0;
		$$ = mknode7(stenter($1, (unsigned char)symmode));
		nameflag++;
		}
	| SYMBOL
		{
		nameflag = 0;
		$$ = mknode7($1);
		}
	;

%%

yyerror(s)
char *s;
{
	if (strcmp(s, "yacc stack overflow") == 0) {
		sprintf(errmsg, "%s\n", s);
		error(4, errmsg);
	} else if (strcmp(s, "syntax error") == 0) {
		if (synerr == 0) {
			fprintf(stderr, "Syntax error(s) found\n");
			fprintf(stderr, "Syntax checking will continue\n");
			fprintf(stderr, "No object file will be created\n");
			synerr++;
		}
	} else {
		sprintf(errmsg, "%s\n", s);
		error(3, errmsg);
		synrecover();
	}
}

/* skip the rest of the line */
synrecover()
{
	while (yychar != 29)
		yychar = yylex();
	yyerrflag = 0;
	nfreed = nalloc;
}
