
/*
**  GRAMMAR.Y
**
**	This file contains a grammar for ingres parsing.  It is setup
**	for the (7-31-78) version of yacc.
**
**	Use:
**		to use for non-distributed ingres:
**			grep -v "DDD" > grammar.y
**
**	Trace Flags:
**		Grammar.y ~~ 38, 39
*/

%{
/* SCANNER/PARSER GLOBALS & TABLES */
# include	<ingres.h>
# include	<aux.h>
# include	<tree.h>
# include	<symbol.h>
# include	<pv.h>
# include	"parser.h"
# include	<sccs.h>
# include 	<errors.h>

SCCSID(@(#)grammar.y	8.4	3/2/85)

# ifdef		xPTR1
# define	YYDEBUG
# endif

int				i;
struct atstash			*aptr;
char				permbuf[3];
/* space for two names, their null bytes and the seperator */
char				modbuf[(2 * (MAXNAME + 1)) + 1];
static char			hqmbuf[2];

extern DESC			Reldesc;
extern int			Opflag;
extern QTREE			*Lastree;
extern QTREE			*Tidnode;
extern int			Rsdmno;
extern int			Resrng;
extern int			Qrymod;
extern int			Permcomd;
extern char			*Trname;
extern int			Qlflag;
extern struct atstash 		Faketid;
extern char			*Tuple;
extern TID			tid;

# ifdef	DISTRIB
extern struct atstash		Fakesid;
# endif

extern char			*Indexname;

extern QTREE			*tree();
extern QTREE			*tlprepend();
extern QTREE			*addresdom();
extern QTREE			*xdot();
extern QTREE			*norml();
extern struct atstash		*attlookup();
extern int			rngent();
extern int			rnglook();
extern PARRNG			Parrng[];
extern STRKEEPER		*substring(),*endvals();
%}

/* NEW BEGIN-END THINGS */
%start		program

/* UNION YYSTYPE DEFINED */
%union
{
	int				type_type;	/* OPERATOR TYPES ETC. */
	QTREE				*tree_type;
	int				rng_type;
	char				char_type;
	int				int_type;
	short				*I2_type;
	long				*I4_type;
	float				*F4_type;
	double				*F8_type;
	char				*string_type;
	STRKEEPER			*substr_type;
}

/* COMMANDS */
%token		APPEND	COPY	CREATE	DELETE	DESTROY	HELP	INDEX	MODIFY
%token		PRINT	RANGE	REPLACE	RETRIEVE	SAVE		
%token		DEFINE	PERMIT	VIEW	INTEGRITY
%token		DELIM	USE	UNUSE
/*DDD*/%token	DISTRIBUTE

/* 'NOISE' WORDS */
%token		ALL	BY	FROM	IN	INTO	UNIQUE	AT
%token		IS	OF	ON	ONTO	TO	UNTIL	WHERE
/*DDD*/%token	DISTRD

/* CONSTANTS */
%token		NAME	SCONST	I2CONST	I4CONST F4CONST	F8CONST

/* PUNCTUATION */
%token		COMMA	LPAREN	PERIOD	RPAREN	COLON	BGNCMNT	ENDCMNT
%token		LBRAC	RBRAC	DOLLAR	PCT

/* UNARY ARITHMETIC OPERATORS */
%token		UAOP

/* BINARY ARITHMETIC OPERATORS */
%token		BAOP	BAOPH

/* BOUNDS OPERATORS */
%token		BDOP

/* EQUALITY OPERATORS */
%token		EOP

/* LOGICAL OPERATORS */
%token		LBOP	LUOP

/* FUNCTIONAL OPERATORS */
%token		FOP	FBOP

/* AGGREGATE OPERATORS */
%token		AGOP

/* TYPES FOR INGRES TOKENS */
%type	<type_type>	IS
%type	<string_type>	NAME	SCONST
%type	<I2_type>	I2CONST
%type	<I4_type>	I4CONST
%type	<F4_type>	F4CONST
%type	<F8_type>	F8CONST
%type	<type_type>	UAOP
%type	<type_type>	BAOP	BAOPH
%type	<type_type>	BDOP
%type	<type_type>	EOP
%type	<type_type>	LBOP	LUOP
%type	<type_type>	FOP	FBOP
%type	<type_type>	AGOP
%type   <char_type>	LPAREN	RPAREN	LBRAC	RBRAC  PCT

/* TYPES FOR INGRES NON-TERMINALS */
%type	<tree_type>	permtarg	permtlist	permtlelm
/*DDD*/%type	<tree_type>	distribute	distcrits	dcriterion
%type	<tree_type>	tlclause	tlist	tlelm
%type	<tree_type>	qualclause	qual	clause	afcn	aggrfcn
%type	<type_type>	relop
%type	<tree_type>	domseq	targdom	attrib
%type	<rng_type>	var
%type	<tree_type>	attribfcn
%type	<type_type>	uop
%type	<string_type>	alias
%type   <substr_type>   subelm	stringpart	grpelm	nameprt
%type	<char_type>	leftclose	rightclose

/* DEFINE ASCENDING PRECEDENCE FOR OPERATORS */
%left		LBOP
%left		LUOP
%left		UAOP
%left		BAOP
%left		BAOPH
%nonassoc	unaryop

%%
program:	program stmnt =
		{
#			ifdef	xPTR1
			tTfp(38, 0, "*** [program stmnt] parsed.\n");
#			endif

			if (endquelst(Opflag) < 0)
				return (-1);
		}
	|	stmnt =
		{
#			ifdef	xPTR1
			tTfp(38, 1, "*** [stmnt] parsed.\n");
#			endif

			if (endquelst(Opflag) < 0)
				return (-1);
		}
	|
		{
#			ifdef	xPTR1
			tTfp(38, 2, "*** [(NULL)] parsed.\n");
#			endif
		}
;
stmnt:		append
	|	copy
	|	create
	|	delete 	
	|	destroy
/*DDD*/	|	distribute
	|	help
	|	index	
	|	integrity
	|	modify
	|	permit
	|	print
	|	range
	|	replace	
	|	retrieve 
	|	save
	|	view
	|	use
	|	unuse
	|	delim	
	|	error
		{
#			ifdef	xPTR1
			tTfp(38, 0, "*** [error] parsed.\n");
#			endif
		}

;
range:		rngstmnt OF NAME IS NAME =
		{
			if ((i = openr(&Reldesc, OR_RELTID, $5)) < 0)
				syserr("relname: error in openr '%d'", i);
			if (i > 0)
			{
				/* invalid relation name */
				par_error(RNGEXIST, WARN, $5, 0);
				YYERROR;
			}
			else
				rngent(R_EXTERNAL, $3, &Reldesc);
		}
;
rngstmnt:	RANGE =
		{
			Opflag = mdRANGE;
		}
;
append:		apstmnt apto relation tlclause qualclause =
		{
			/* make root node */
			Lastree = tree($4, $5, ROOT, sizeof(struct rootnode), 1);
		}
;
apstmnt:	APPEND =
		{
			Opflag = mdAPP;
		}
;
apto:		INTO
	|	ONTO
	|	TO
	|	ON
	|	;
;
delete:		delstmnt delwd relation qualclause =
		{
			/* make root node for delete, with a TIDNODE at leftmost */
			Lastree = tree(tree(NULL, Tidnode, RESDOM, sizeof(struct resdomnode), NULL), $4, ROOT, sizeof(struct rootnode), 1);
		}
;
delstmnt:	DELETE =
		{
			Opflag = mdDEL;
		}
;
delwd:		IN
	|	ON
	|	FROM
	|	;
;
replace:	repstmnt repkwd relation tlclause qualclause =
		{
			/* make root node for replace */
			Lastree = tree($4, $5, ROOT, sizeof(struct rootnode), 1);
		}
;
repstmnt:	REPLACE =
		{
			Opflag = mdREPL;
		}
;
repkwd:		INTO
	|	IN
	|	ON
	|	;
;
retrieve:	retstmnt retclause tlclause qualclause =
		{
			/* make root node for retrieve */
			Lastree = tree($3, $4, ROOT, sizeof(struct rootnode), 1);
		}
;
retstmnt:	RETRIEVE =
		{
			Opflag = mdRETR;
		}
;
retclause:	retkwd relation =
		{
			/* set up pipe block and save relname for create */
#			ifdef	xPTR2
			tTfp(38, 4, "retclause: Rsdmno %d", Rsdmno);
#			endif
			Rsdmno = 0;
			setp(PV_STR, "0");	/* relstat = nil */
			setp(PV_STR, trim_relname(Parrng[Resrng].vardesc.reldum.relid));
		}
	|	=
		{
			/* no result relation, output to terminal */
			Rsdmno = 0;
			Resrng = -1;
		}
	|	UNIQUE =
		{
			Opflag = mdRET_UNI;
			Rsdmno = 0;
			Resrng = -1;
		}
;
retkwd:		INTO
	|	TO
	|	;
;

delim:		DEFINE DELIM NAME LPAREN NAME COMMA SCONST RPAREN = 
		{
			Opflag = mdSTOP;
			if ((i = openr(&Reldesc, OR_WRITE, "rdelim")) < 0)
				syserr("relname: error in openr '%d'", i);
			if (i > 0)
			{
				/* invalid relation name */
				par_error(RNGEXIST, WARN, "rdelim", 0);
				YYERROR;
			}
			else
			{
				if (( i = make_tuples(&Reldesc, $3, $5, $7)) < 0)
				{
					closer(&Reldesc);
					par_error(BADBNF, WARN, "rdelim", 0);
				}
			}
			closer(&Reldesc);
		}
;
use:		USE NAME =
		{
			Opflag = mdSTOP;
			if ((i = openr(&Reldesc, OR_WRITE, "rdelim")) < 0)
				syserr("relname: error in openr '%d'", i);
			if (i > 0)
			{
				/* invalid relation name */
				par_error(RNGEXIST, WARN, "rdelim", 0);
				YYERROR;
			}
			else
			{
				if ((i = make_list(&Reldesc, $2)) < 0)
				{
					closer(&Reldesc);
					par_error(DELEXIST, WARN, 0);
				}
			}
			closer(&Reldesc);
		}
;
unuse:		UNUSE NAME =
		{
			Opflag = mdSTOP;
			if ( i = shrink_list($2) < 0)
			{
				par_error(NOGRP,WARN,0);
			}
		}
;

view:		viewclause tlclause qualclause =
		{
			Lastree = tree($2, $3, ROOT, sizeof(struct rootnode), 1);
		}
;
viewclause:	viewstmnt relation =
		{
			Rsdmno = 0;
			setp(PV_STR, "0040");	/* relstat = S_VIEW */
			setp(PV_STR, trim_relname(Parrng[Resrng].vardesc.reldum.relid));
		}
;
viewstmnt:	DEFINE VIEW =
		{
			Opflag = mdVIEW;
			if (!Qrymod)
			{
				/* no qrymod in database */
				par_error(NOQRYMOD, WARN, 0);
			}
		}
;
permit:		permstmnt permlist permrel permtarg permwho permplace permtd qualclause =
		{
			Lastree = tree($4, $8, ROOT, sizeof(struct rootnode), 1);
		}
;
permstmnt:	DEFINE PERMIT =
		{
			Opflag = mdPROT;
			if (!Qrymod)
			{
				/* no qrymod in database */
				par_error(NOQRYMOD, WARN, 0);
			}
		}
;
permlist:	permxlist
	|	permlist COMMA permxlist
;
permxlist:	ALL =
		{
			permcom(-1);	/* means 'all' commands */
		}
	|	RETRIEVE =
		{
			permcom(mdRETR);
		}
	|	DELETE =
		{
			permcom(mdDEL);
		}
	|	APPEND =
		{
			permcom(mdAPP);
		}
	|	REPLACE =
		{
			permcom(mdREPL);
		}
;
permrel:	permword relation =
		{
			/* put command vector into list now since this always happens */
			setp(PV_INT, Permcomd);
			Permcomd = 0;		/* reset command map */
			setp(PV_STR, trim_relname(Parrng[Resrng].vardesc.reldum.relid));
			bmove(Parrng[Resrng].vardesc.reldum.relowner, permbuf, 2);
			permbuf[2] = 0;
			setp(PV_STR, permbuf);
		}
;
permword:	ON
	|	OF
	|	TO
;
permtarg:	LPAREN permtlist RPAREN =
		{
			$$ = $2;
		}
	|	=
		{
			$$ = NULL;
		}
;
permtlist:	permtlelm
	|	permtlist COMMA permtlelm =
		{
			/*
			** attach bulk of permit tl to leftmost node of new elem
			*/
			if (!Err_current)
				$$ = tlprepend($1, $3);
		}
;
permtlelm:	NAME =
		{
			/* Resrng is set by the "relation" production */
			if (!Err_current)
			{
				Trname = $1;
				aptr = attlookup(Resrng, Trname);
				$$ = tree(NULL, NULL, VAR, sizeof(struct varnode), Resrng, aptr);
				$$ = addresdom(NULL, $$);
			}
		}
;
permwho:	TO NAME =
		{
			setp(PV_STR, $2);
		}
	|	TO ALL =
		{
			setp(PV_STR, "all");
		}
;
permplace:	AT NAME =
		{
			setp(PV_STR, $2);
		}
	|	AT ALL =
		{
			setp(PV_STR, "all");
		}
	|	=
		{
			setp(PV_STR, "all");		/* default is all */
		}
;
permtd:		permtime permday
	|	permdeftime permday
	|	permtime permdefday
	|	permdeftime permdefday
;
permdeftime:	=
		{
			setp(PV_INT, 0);
			setp(PV_INT, 1440);
		}
;
permdefday:	=
		{
			setp(PV_STR, "sun");
			setp(PV_STR, "sat");
		}
;
permtime:	FROM I2CONST COLON I2CONST TO I2CONST COLON I2CONST =
		{
			setp(PV_INT, timeofday($2, $4));
			setp(PV_INT, timeofday($6, $8));
		}
;
permday:	ON NAME TO NAME =
		{
			setp(PV_STR, $2);
			setp(PV_STR, $4);
		}
;
integrity:	integstmnt integnoise relation integis qual =
		{
			Lastree = tree(NULL, norml($5), ROOT, sizeof(struct rootnode), 1);
			Qlflag--;	/* turn off here */
		}
;
integstmnt:	DEFINE INTEGRITY =
		{
			Opflag = mdINTEG;
			Qlflag++;	/* OK to turn on here because integrity doesn't have a targ list */
			if (!Qrymod)
			{
				/* no qrymod in database */
				par_error(NOQRYMOD, WARN, 0);
			}
		}
;
integnoise:	ON
	|	ONTO
	|	IN
	|	OF
	|	/* null */
;
integis:	IS
	|	/* null*/
;
/*DDD*/distribute:	diststmnt relation AT distcrits =
/*DDD*/		{
/*DDD*/			if (!Err_current)
/*DDD*/			{
/*DDD*/				$$ = tree(NULL, NULL, QLEND, 0);
/*DDD*/				Lastree = tree($4, $$, ROOT, sizeof(struct rootnode), 1);
/*DDD*/			}
/*DDD*/		}
/*DDD*/;
/*DDD*/diststmnt:	DISTRIBUTE =
/*DDD*/				Opflag = mdDISTRIB;
/*DDD*/;
/*DDD*/distcrits:	dcriterion =
/*DDD*/		{
/*DDD*/			$$ = $1;
/*DDD*/		}
/*DDD*/	|	distcrits dcriterion =
/*DDD*/		{
/*DDD*/			$$ = tlprepend($1, $2);
/*DDD*/		}
/*DDD*/;
/*DDD*/dcriterion:	NAME where qual =
/*DDD*/		{
/*DDD*/			Qlflag--;
/*DDD*/			syserr("Warning this node may be the wrong size\n");
/*DDD*/			if (!Err_current)
/*DDD*/				$$ = tree(NULL, norml($3), SITE, 2, $1);
/*DDD*/		}
/*DDD*/;
relation:	NAME =
		{
#			ifdef	xPTR2
			tTfp(38, 3, "res rel name/var: '%s'\n", $1);
#			endif
			switch (Opflag)
			{
			  case mdRETR:
			  case mdVIEW:
				/* result better not be a rel name */
				if ((i = openr(&Reldesc, OR_RELTID, $1)) < 0)
					syserr("relation: err openr '%d'", i);
				if (i == 0)
				{
					/* reln exists */
					if (bequal(Reldesc.reldum.relowner, Usercode, UCODE_SZ))
					{
						/* same owner, can't duplicate name */
						par_error(RESEXIST, WARN, $1, 0);
						YYERROR;
					}
					else if (!Err_current)
					{
						/* owned by dba -- purge range table */
						rngdel($1);
					}
				}
				if (!Err_current)
				{
					bmove(Usercode, Reldesc.reldum.relowner, UCODE_SZ);
					pmove($1, Reldesc.reldum.relid, MAXNAME, ' ');
					Resrng = rngent(R_INTERNAL, "", &Reldesc);
				}
				break;

			  case mdAPP:
				/* result is a rel name */
				if (!Err_current)
				{
					Resrng = rnglook($1, LOOKREL);
					if (Resrng < 0)
					{
						if ((i = openr(&Reldesc, OR_RELTID, $1)) < 0)
							syserr("relation: err openr '%d'", i);
						if (i)
						{
							/* invalid relation name */
							par_error(RESAPPEX, WARN, $1, 0);
							YYERROR;
						}
						Resrng = rngent(R_INTERNAL, "", &Reldesc);
					}
					else
						ctlmod_decl(Resrng);
					checkupd(Resrng);
				}
				break;

			  case mdPROT:
			  case mdINTEG:
#			  ifdef	DISTRIB
			  case mdDISTRIB:
#			  endif
				/* the result is a tuple variable */
				Resrng = rnglook($1, LOOKVAR);
				if (Resrng < 0)
				{
					/* variable not declared */
					par_error(NOVBLE, WARN, $1, 0);
					YYERROR;
				}
				else
					ctlmod_decl(Resrng);
				break;

			  case mdREPL:
			  case mdDEL:
				/* the result is a tuple variable */
				Resrng = rnglook($1, LOOKVAR);
				if (Resrng < 0)
					/* variable not declared */
				{
					par_error(NOVBLE, WARN, $1, 0);
					YYERROR;
				}
				else
					ctlmod_decl(Resrng);

				checkupd(Resrng);
				Tidnode = tree(NULL, NULL, VAR, sizeof(struct varnode), Resrng, &Faketid);
				break;
			  default:
			    ;
			}
		}
;
tlclause:	LPAREN tlist RPAREN =
		{
			$$ = $2;

			/*
			** replace must have tid node as left branch
			**	(so does delete but it doesn't have a targ list)
			*/
			if (Opflag == mdREPL && !Err_current)
			{
				$$ = tlprepend(tree(NULL, Tidnode, RESDOM, sizeof(struct resdomnode), 0), $$);
			}
		}
;
tlist:		tlelm
	|	tlist COMMA tlelm =
		{
			/*
			** attach bulk of targ list to leftmost node
			** of new element
			*/
			if (!Err_current)
				$$ = tlprepend($1, $3);
		}
;
tlelm:		NAME is afcn =
		{
			Trname = $1;
			/* make a new resdom entry for targ list */
			if (!Err_current)
				$$ = addresdom(NULL, $3);
		}
	|	attrib =
		{
		/* makes a new resdom entry for targ list */
			if (!Err_current)
				$$ = addresdom(NULL, $1);
		}
	|	var PERIOD ALL =
		{
			if (Opflag == mdREPL)
			{
				/* ALL not defined for REPLACE */
				par_error(REPALL, WARN,
				    trim_relname(Qt.qt_rangev[$1].rngvdesc->relvname), 0);
				YYERROR;
			}
			/* makes set of new resdom entries for targ list */
			else if (!Err_current)
				$$ = xdot($1);
		}
;
is:		IS
	|	BY
;
qualclause:	where qual =
		{
			$$ = norml($2);
			Qlflag--;
			if (Opflag == mdREPL)
			    qualindex();
		}
	|	=
		{
			/* null qualification */
			if (Opflag == mdREPL)
			    qualindex();
			$$ = norml(NULL);
		}
;
where:		WHERE =
		{
			Qlflag++;
		}
;
qual:		LPAREN qual RPAREN =
		{
			$$ = $2;
		}
	|	LUOP qual =
		{
			$$ = tree(NULL, $2, UOP, 2, $1);
		}
	|	qual LBOP qual =
		{
			$$ = tree($1, $3, $2, sizeof (struct rootnode) -2, 0);
		}
	|	clause
;
clause:		afcn relop afcn =
		{
			$$ = tree($1, $3, BOP, 2, $2);
		}
;
relop:		EOP
	|	IS
	|	BDOP
;
afcn:		aggrfcn
	|	attribfcn
	|	afcn BAOPH afcn =
		{
			$$ = tree($1, $3, BOP, 2, $2);
		}
	|	afcn BAOP afcn =
		{
			$$ = tree($1, $3, BOP, 2, $2);
		}
	|	afcn UAOP afcn =
		{
			$$ = tree($1, $3, BOP, 2, $2);
		}
	|	LPAREN afcn RPAREN =
		{
			$$ = $2;
		}
	|	uop afcn	%prec unaryop	=
		{
			$$ = tree(NULL, $2, UOP, 2, $1);
		}
	|	FOP LPAREN afcn RPAREN =
		{
			$$ = tree($3, NULL, UOP, 2, $1);
		}
	|	FBOP LPAREN afcn COMMA afcn RPAREN =
		{
			$$ = tree($3, $5, BOP, 2, $1);
		}
;
aggrfcn:	AGOP LPAREN afcn BY domseq qualclause RPAREN =
		{
#			ifdef	xPTR2
			tTfp(39, 0, "agg func\n");
#			endif
			windup($5);
			$$ = tree(tree($5, tree(NULL, $3, AOP, 6, $1), BYHEAD, sizeof(struct resdomnode), 0), $6, AGHEAD, sizeof(struct rootnode), 0);
			tlprepend(tree(NULL, NULL, TREE, 0), $$);
		}
	|	AGOP LPAREN afcn qualclause RPAREN =
		{
			$$ = tree(tree(NULL, $3, AOP, 6, $1), $4,  AGHEAD, sizeof(struct rootnode), 0);
		}
;
domseq:		targdom
	|	domseq COMMA targdom =
		{
			$$ = tlprepend($1, $3);
		}
;
targdom:	afcn =
		{
			$$ = tree(NULL, $1, RESDOM, sizeof(struct resdomnode), Rsdmno);
		}
;
nameprt:	NAME =
		{
			$$ = substring($1,1);
		}
	|	SCONST =
		{
			$$ = substring($1,0);
		}
;
subelm:		DOLLAR =
		{ 
			$$ = substring(NULL,0);
		}
	|	nameprt DOLLAR =
		{
			$1->flag[0] |= 2;
			$$ = $1;
		}
	|	nameprt =
		{
			$$ = $1;
		}
	|	I2CONST subelm =
		{
			setnumber($2,$1);
			$$ = $2;
		}
;
grpelm:		subelm COMMA subelm =
		{
			groupstrings($1,$3);
			$$ = $1;
		}	
;
leftclose:	PCT =
		{
			$$ = $1;
		}
	|	LPAREN =
		{
			$$ = $1;
		}
;
rightclose:	PCT =
		{
			$$ = $1;
		}
	|	RPAREN =
		{
			$$ = $1;
		}
;
stringpart:	leftclose subelm rightclose =
		{
			$$ = endvals($2,$1,$3);
		}
	|	leftclose grpelm rightclose =
		{
			$$ = endvals($2,$1,$3);
		}
;
attrib:		var PERIOD NAME =
		{
#			ifdef	xPTR2
			tTfp(39, 1, "attrib %12s.%12s found\n",
			Qt.qt_rangev[$1].rngvdesc->relvname, $3);
#			endif

			/* remember attribute name */
			Trname = $3;

			/* look up attribute */
			aptr = attlookup($1, Trname);
			$$ = tree(NULL, NULL, VAR, sizeof(struct varnode), $1, aptr);
		}
	|   	attrib stringpart =
		{
			$1->sym.value.sym_var.varstr = $2;
			$$ = $1;
		}
;
var:		NAME =
		{
			$$ = rnglook($1, LOOKVAR);
			if ($$ < 0)
			{
				/* variable not declared */
				par_error(NOVBLE, WARN, $1, 0);
				YYERROR;
			}
			else
				ctlmod_decl($$);
		}
;
attribfcn:	I2CONST =
		{
			$$ = tree(NULL, NULL, INT, 2, $1);
		}
	|	I4CONST =
		{
			$$ = tree(NULL, NULL, INT, 4, $1);
		}
	|	F4CONST =
		{
			$$ = tree(NULL, NULL, FLOAT, 4, $1);
		}
	|	F8CONST =
		{
			$$ = tree(NULL, NULL, FLOAT, 8, $1);
		}
	|	SCONST =
		{
			patmat($1);
			$$ = tree(NULL, NULL, CHAR, length($1), $1);
		}
	|	NAME =
		{
			$$ = tree(NULL, NULL, COP, 2, $1);
		}
	|	attrib
;
uop:		UAOP	%prec unaryop	=
		{
			if ($1 == opADD)
				$$ = opPLUS;
			else
				if ($1 == opSUB)
					$$ = opMINUS;
		}
;
copy:		copstmnt alias LPAREN coparam RPAREN keywd SCONST =
		{
#			ifdef	xPTR2
			tTfp(39, 3, "copy %12s,%12s\n", $2, $7);
#			endif

			setp(PV_STR, $7);
		}
;
copstmnt:	COPY =
		{
			Opflag = mdCOPY;
		}
;
coparam:	cospecs
	|	;
;
cospecs:	alias is coent
	|	cospecs COMMA alias is coent
;
coent:		alias
	|	SCONST =
		{
			setp(PV_STR, $1);
		}
;
alias:		NAME =
		{
			if (!Err_current)
			{
				setp(PV_STR, $1);
				if (Opflag == mdDESTROY || Opflag == mdCREATE
#					ifdef	DISTRIB
					|| Opflag == mdDCREATE
#					endif
								)
					rngdel($1);
			}
		}
;
specs:		alias is alias
	|	specs COMMA alias is alias
;
keywd:		INTO =
		{
			setp(PV_STR, "\0");
			setp(PV_STR, "i");
		}
	|	FROM =
		{
			setp(PV_STR, "\0");
			setp(PV_STR, "f");
		}
;
create:		crestmnt alias LPAREN specs RPAREN
;
crestmnt:	CREATE =
		{
			Opflag = mdCREATE;

			/* set up parameters for regular create */
			setp(PV_STR, "0");		/* relstat = nil */
		}
/*DDD*/	|	CREATE DISTRD =
/*DDD*/		{
/*DDD*/			Opflag = mdDCREATE;
/*DDD*/
/*DDD*/			/* setup parameters for distributed create */
/*DDD*/			setp(PV_STR, "U");
/*DDD*/			setp(PV_STR, "");
/*DDD*/			setp(PV_STR, "01000");	/* relstat = S_DISTRIBUTED */
/*DDD*/		}
;
destroy:	destmnt keys
	|	destqm destlist
	|	destmnt DELIM NAME =
		{
			Opflag = mdSTOP;
			if ((i = openr(&Reldesc, OR_WRITE, "rdelim")) < 0)
				syserr("relname: error in openr '%d'", i);
			if (i > 0)
			{
				/* invalid relation name */
				par_error(RNGEXIST, WARN, "rdelim", 0);
				YYERROR;
			}
			else
			{
				if ( i = destroy_delim(&Reldesc, $3) < 0)
				{
					closer(&Reldesc);
					par_error(DELEXIST, WARN, "rdelim",0);
				}
			}
			closer(&Reldesc);
		}
;
destmnt:	DESTROY =
		{
			Opflag = mdDESTROY;
		}
;
destqm:		destmnt INTEGRITY NAME =
		{
			Opflag = mdREMQM;
			if (!Qrymod)
				/* no qrymod in database */
				par_error(NOQRYMOD, WARN, 0);
			setp(PV_STR, "6");
			setp(PV_STR, $3);
		}
	|	destmnt PERMIT NAME =
		{
			Opflag = mdREMQM;
			if (!Qrymod)
				/* no qrymod in database */
				par_error(NOQRYMOD, WARN, 0);
			setp(PV_STR, "5");
			setp(PV_STR, $3);
		}
;
destlist:	I2CONST =
		{
			i = iocv(*($1));
			setp(PV_STR, i);
		}
	|	destlist COMMA I2CONST =
		{
			i = iocv(*($3));
			setp(PV_STR, i);
		}
	|	ALL
;
help:		helstmnt hlist
	|	helstmnt =
		{
			setp(PV_INT, RELIST);	/* all relns */
		}
	|	helqmstmnt hqmlist
	|	heldelstmnt =
		{
			setp(PV_INT, ALLDELLIST);	/* all delims */
		}
	|	heldelstmnt dlist
;
helstmnt:	HELP =
		{
			Opflag = mdHELP;
		}
;
heldelstmnt:	HELP DELIM =
		{
			Opflag = mdHELP;
		}
;
helqmstmnt:	HELP VIEW =
		{
			Opflag = mdDISPLAY;
			if (!Qrymod)
				/* no qrymod in database */
				par_error(NOQRYMOD, WARN, 0);
			smove("4", hqmbuf);
		}
	|	HELP PERMIT =
		{
			Opflag = mdDISPLAY;
			if (!Qrymod)
				/* no qrymod in database */
				par_error(NOQRYMOD, WARN, 0);
			smove("5", hqmbuf);
		}
	|	HELP INTEGRITY =
		{
			Opflag = mdDISPLAY;
			if (!Qrymod)
				/* no qrymod in database */
				par_error(NOQRYMOD, WARN, 0);
			smove("6", hqmbuf);
		}

;
hlist:		hparam
	|	hlist COMMA hparam
	|	ALL =
		{
			setp(PV_INT, ALLRELINFO);
		}
;
dlist:		dparam
	|	dlist COMMA dparam
;
dparam:	NAME =
		{
			/* relation */
			setp(PV_INT, DELLIST);
			setp(PV_STR, $1);
		}
;
hparam:		NAME =
		{
			/* relation */
			setp(PV_INT, RELINFO);
			setp(PV_STR, $1);
		}
	|	SCONST =
		{
			/* manual page */
			setp(PV_INT, MANSEC);
			setp(PV_STR, $1);
		}
;
hqmlist:	NAME =
		{
			setp(PV_STR, hqmbuf);
			setp(PV_STR, $1);
		}
	|	hqmlist COMMA NAME =
		{
			setp(PV_STR, hqmbuf);
			setp(PV_STR, $3);
		}
;
index:		instmnt LPAREN keys RPAREN =
		{
			if (Rsdmno > MAXKEYS)
				/* too many attributes in key */
				par_error(INDEXTRA, WARN, 0);
		}
;
instmnt:	indexq ON NAME IS NAME =
		{
			/* init INDEX command */
			Rsdmno = 0;
			setp(PV_STR, $3);
			setp(PV_STR, $5);
			Indexname = $5;
		}
;
indexq:		INDEX =
		{
			Opflag = mdINDEX;
		}
;
modify:		modstmnt alias TO modstorage modkeys modqual
;
modstmnt:	MODIFY =
		{
			Opflag = mdMODIFY;
			Rsdmno = 0;
		}
;
modstorage:	NAME =
		{
			setp(PV_STR, $1);
		}
modkeys:	modstkey modrptkey
	|	;
;
modstkey:	ON =
		{
			setp(PV_STR, "name");
		}
;
modrptkey:	modbasekey
	|	modrptkey COMMA modbasekey
;
modbasekey:	NAME =
		{
			setp(PV_STR, $1);
		}
	|	NAME COLON NAME =
		{
			concat($1, ztack(":", $3), modbuf);
			setp(PV_STR, modbuf);
		}
;
modqual:	modcond modfill
	|	;
;
modcond:	WHERE =
		{
			setp(PV_STR, "\0");
		}
;
modfill:	modfillnum
	|	modfill COMMA modfillnum
;
modfillnum:	NAME IS I2CONST =
		{
			setp(PV_STR, $1);
			i = iocv(*($3));
			setp(PV_STR, i);
		}
	|	NAME IS NAME =
		{
			setp(PV_STR, $1);
			setp(PV_STR, $3);
		}
;
keys:		alias =
		{
			Rsdmno++;
		}
	|	keys COMMA alias =
		{
			Rsdmno++;
		}
;
print:		prinstmnt keys
;
prinstmnt:	PRINT =
		{
			Opflag = mdPRINT;
		}
;
save:		savstmnt alias UNTIL date
;
savstmnt:	SAVE =
		{
			Opflag = mdSAVE;
		}
;
date:		month day_year day_year
;
month:		alias
	|	day_year
;
day_year:	I2CONST =
		{
			i = iocv(*($1));

#			ifdef	xPTR3
			tTfp(39, 4, "day_year: %s\n", i);
#			endif

			setp(PV_STR, i);
		}
;
%%
# include	"scanner.h"
# include	"tables.y"
# include	"yyerror.y"
