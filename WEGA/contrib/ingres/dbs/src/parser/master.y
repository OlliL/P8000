/*
** This file contains the specification of QL recognized by the parser.
*/

/* COMMANDS */
%term		APPEND	COPY	CREATE	DELETE	DESTROY	HELP	INDEX	MODIFY
%term		PRINT	RANGE	REPLACE	RETRIEVE	SAVE
%term		DEFINE	PERMIT	VIEW	INTEGRITY
/*DDD*/%term	DISTRIBUTE

/* 'NOISE' WORDS */
%term		ALL	BY	FROM	IN	INTO	UNIQUE	AT
%term		IS	OF	ON	TO	UNTIL	WHERE
/*DDD*/%term	DISTRD

/* CONSTANTS */
%term		NAME	SCONST	I2CONST	I4CONST F4CONST	F8CONST

/* PUNCTUATION */
%term		COMMA	LPAREN	PERIOD	RPAREN	COLON	BGNCMNT	ENDCMNT

/* UNARY ARITHMETIC OPERATORS */
%term		UAOP

/* BINARY ARITHMETIC OPERATORS */
%term		BAOP	BAOPH

/* BOUNDS OPERATORS */
%term		BDOP

/* EQUALITY OPERATORS */
%term		EOP

/* LOGICAL OPERATORS */
%term		LBOP	LUOP

/* FUNCTIONAL OPERATORS */
%term		FOP	FBOP

/* AGGREGATE OPERATORS */
%term		AGOP


/* DEFINE ASCENDING PRECEDENCE FOR OPERATORS */
%left		LBOP
%left		LUOP
%left		UAOP
%left		BAOP
%left		BAOPH
%binary		unaryop

%{
/* SCANNER/PARSER GLOBALS & TABLES */
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/tree.h"
# include	"../h/symbol.h"
# include	"parser.h"

struct rngtab		*Resrng;	/* ptr to result reln entry */
QTREE			*Lastree;	/* pointer to root node of tree */
DESC			Reldesc;	/* descriptor for range table lookup */
short			Rsdmno;		/* result domain number */
int			Opflag;		/* operator flag contains query mode */
char			*Indexname;	/* ptr to name of index */
char			*Trname;	/* pointer to attribute name */
int			Agflag;		/* how many aggs in this qry */
int			Patflag;	/* signals a pattern match reduction */
int			Qlflag;		/* set when processing a qual */
int			yyline;		/* line counter */
short			Permcomd;

static QTREE		*Tidnode;	/* pointer to tid node of targ list
					   for REPLACE, DELETE */
%}
%%
%{
register int			i;
register struct atstash		*aptr;
register char			*p;
char				permbuf[3];
extern char			*AA_iocv();
extern char			*AAztack();
extern QTREE			*tree();
extern char			*tlprepend();
extern char			*addresdom();
extern char			*xdot();
extern char			*norml();
extern struct atstash		*attlookup();
extern struct rngtab		*rngent();
extern struct rngtab		*rnglook();
/* space for two names, their null bytes and the seperator */
char				modbuf[(2 * (MAXNAME + 1)) + 1];
static char			hqmbuf[2];
%}
program:	xprogram
	|	/* empty */
;
xprogram:	xprogram stmnt =
		{
			endqlst(Opflag);
		}
	|	stmnt =
		{
			endqlst(Opflag);
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
;
range:		rngvar
	|	rngprint =
		{
			rngprint();
		}
;
rngprint:	rngstmnt
;
rngvar:		rngstmnt rngof NAME IS NAME =
		{
			if ((i = AArelopen(&Reldesc, -1, $5)) < 0)
				AAsyserr(18012, i);
			if (i > 0)
				/* invalid relation name */
				yyerror(RNGEXIST, $5, (char *) 0);
			rngent(R_EXTERNAL, $3, $5, Reldesc.reldum.relowner,
				Reldesc.reldum.relatts, Reldesc.reldum.relstat);
		}
;
rngstmnt:	RANGE =
		{
			Opflag = (int) mdRANGE;
		}
;
rngof:		OF
	|	/* empty */
;
append:		apstmnt apto relation tlclause qualclause =
		{
			/* make root node */
			Lastree = tree($4, $5, ROOT, 0);
		}
;
apstmnt:	APPEND =
		{
			Opflag = (int) mdAPP;
		}
;
apto:		TO
	|	/* empty */
;
delete:		delstmnt relation qualclause =
		{
			/* make root node for delete, with a TIDNODE at leftmost */
			Lastree = tree(tree((QTREE *) 0, Tidnode, RESDOM, RES_SIZ, (char *) 0), $3, ROOT, 0);
		}
;
delstmnt:	DELETE =
		{
			Opflag = (int) mdDEL;
		}
;
replace:	repstmnt relation tlclause qualclause =
		{
			/* make root node for replace */
			Lastree = tree($3, $4, ROOT, 0);
		}
;
repstmnt:	REPLACE =
		{
			Opflag = (int) mdREPL;
		}
;
retrieve:	retstmnt retclause tlclause qualclause =
		{
			/* make root node for retrieve */
			Lastree = tree($3, $4, ROOT, 0);
		}
;
retstmnt:	RETRIEVE =
		{
			Opflag = (int) mdRETR;
		}
;
retclause:	retkwd relation =
		{
			/* set up pipe block and save relname for create */
			Rsdmno = 0;
			setp("0");	/* relstat = nil */
			setp(Resrng->relnm);
		}
	|	/* empty */ =
		{
			/* no result relation, output to terminal */
			Rsdmno = 0;
			Resrng = 0;
		}
	|	UNIQUE =
		{
			Opflag = (int) mdRET_UNI;
			Rsdmno = 0;
			Resrng = 0;
		}
;
retkwd:		INTO
	|	/* empty */
;
view:		viewclause tlclause qualclause =
		{
			Lastree = tree($2, $3, ROOT, 0);
		}
;
viewclause:	viewstmnt relation =
		{
			Rsdmno = 0;
			setp("0040");	/* relstat = S_VIEW */
			setp(Resrng->relnm);
		}
;
viewstmnt:	DEFINE VIEW =
		{
			Opflag = (int) mdVIEW;
			if (!Qrymod)
				/* no qrymod in database */
				yyerror(NOQRYMOD, (char *) 0);
		}
;
permit:		permstmnt permlist permrel permtarg permwho permplace permtd qualclause =
		{
			Lastree = tree($4, $8, ROOT, 0);
		}
;
permstmnt:	DEFINE PERMIT =
		{
			Opflag = (int) mdPROT;
			if (!Qrymod)
				/* no qrymod in database */
				yyerror(NOQRYMOD, (char *) 0);
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
			permcom((int) mdRETR);
		}
	|	DELETE =
		{
			permcom((int) mdDEL);
		}
	|	APPEND =
		{
			permcom((int) mdAPP);
		}
	|	REPLACE =
		{
			permcom((int) mdREPL);
		}
;
permrel:	permword relation =
		{
			/* put command vector into list now since this always happens */
			setp(AA_iocv(Permcomd));
			Permcomd = 0;		/* reset command map */
			setp(Resrng->relnm);
			AAbmove(Resrng->relnowner, permbuf, 2);
			permbuf[2] = 0;
			setp(permbuf);
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
	|	/* empty */ =
		{
			$$ = (char *) 0;
		}
;
permtlist:	permtlelm
	|	permtlist COMMA permtlelm =
		{
			/* attach bulk of permit tl to leftmost node of new elem */
			$$ = tlprepend($1, $3);
		}
;
permtlelm:	NAME =
		{
			/* Resrng is set by the AA_REL production */
			Trname = $1;
			aptr = attlookup(Resrng, Trname);
			$$ = (char *) tree((QTREE *) 0, (QTREE *) 0, VAR, VAR_SIZ, CAST(Resrng->rentno), aptr);
			$$ = (char *) addresdom((QTREE *) 0, $$);
		}
;
permwho:	TO NAME =
		{
			setp($2);
		}
	|	TO ALL =
		{
			setp("all");
		}
;
permplace:	AT NAME =
		{
			setp($2);
		}
	|	AT ALL =
		{
			setp("all");
		}
	|	/* empty */ =
		{
			setp("all");		/* default is all */
		}
;
permtd:		permtime permday
	|	permdeftime permday
	|	permtime permdefday
	|	permdeftime permdefday
;
permdeftime:	/* empty */ =
		{
			setp("0");
			setp("1440");
		}
;
permdefday:	/* empty */ =
		{
			setp("sun");
			setp("sat");
		}
;
permtime:	FROM I2CONST COLON I2CONST TO I2CONST COLON I2CONST =
		{
			setp(AA_iocv(timeofday($2, $4)));
			setp(AA_iocv(timeofday($6, $8)));
		}
;
permday:	ON NAME TO NAME =
		{
			setp($2);
			setp($4);
		}
;
integrity:	integstmnt integnoise relation integis qual =
		{
			Lastree = tree((QTREE *) 0, norml($5), ROOT, 0);
			Qlflag--;	/* turn off here */
		}
;
integstmnt:	DEFINE INTEGRITY =
		{
			Opflag = (int) mdINTEG;
			Qlflag++;	/* OK to turn on here because integrity doesn't have a targ list */
			if (!Qrymod)
				/* no qrymod in database */
				yyerror(NOQRYMOD, (char *) 0);
		}
;
integnoise:	ON
	|	/* empty */
;
integis:	IS
	|	/* empty */
;
/*DDD*/distribute:	diststmnt relation AT distcrits =
/*DDD*/		{
/*DDD*/			$$ = (char *) tree((QTREE *) 0, (QTREE *) 0, QLEND, 0);
/*DDD*/			Lastree = tree($4, $$, ROOT, 0);
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
/*DDD*/			$$ = (char *) tree((QTREE *) 0, norml($3), SITE, OP_SIZ, $1);
/*DDD*/		}
/*DDD*/;
relation:	NAME =
		{
# ifdef xPTR2
			AAtTfp(32, 0, "res rel name/var: '%s'\n", $1);
# endif
			switch (Opflag)
			{
			  case mdRETR:
			  case mdVIEW:
				/* result better not be a rel name */
				if ((i = AArelopen(&Reldesc, -1, $1)) < 0)
					AAsyserr(18013, i);
				if (i == 0)
				{
					/* reln exists */
					if (AAbequal(Reldesc.reldum.relowner, AAusercode, 2))
					{
						/* same owner, can't duplicate name */
						yyerror(RESEXIST, $1, (char *) 0);
					}
					else
					{
						/* owned by dba -- purge range table */
						rngdel($1);
					}
				}
				Resrng = rngent(R_INTERNAL, "", $1, AAusercode, 0, 0);
				break;

			  case mdAPP:
				/* result is a rel name */
				Resrng = rnglook($1, LOOKREL);
				if (Resrng == 0)
				{
					if ((i = AArelopen(&Reldesc, -1, $1)) < 0)
						AAsyserr(18013, i);
					if (i)
						/* invalid relation name */
						yyerror(RESAPPEX, $1, (char *) 0);
					Resrng = rngent(R_INTERNAL, "", $1,
							Reldesc.reldum.relowner,
							Reldesc.reldum.relatts,
							Reldesc.reldum.relstat);
				}
				checkupd(Resrng);
				break;

			  case mdPROT:
			  case mdINTEG:
# ifdef DISTRIB
			  case mdDISTRIB:
# endif
				/* the result is a tuple variable */
				Resrng = rnglook($1, LOOKVAR);
				if (Resrng == 0)
					/* variable not declared */
					yyerror(NOVBLE, $1, (char *) 0);
				break;

			  case mdREPL:
			  case mdDEL:
				/* the result is a tuple variable */
				Resrng = rnglook($1, LOOKVAR);
				if (Resrng == 0)
					/* variable not declared */
					yyerror(NOVBLE, $1, (char *) 0);
				checkupd(Resrng);
				Tidnode = tree((QTREE *) 0, (QTREE *) 0, VAR, VAR_SIZ, CAST(Resrng->rentno), &Faketid);
				break;
			}
		}
;
tlclause:	LPAREN tlist RPAREN =
		{
			if (Patflag)
				/* no patt match in targ list */
				yyerror(NOPATMAT, (char *) 0);
			$$ = $2;

			/*
			** replace must have tid node as left branch
			**	(so does delete but it doesn't have a targ list)
			*/
			if (Opflag == (int) mdREPL)
			{
				$$ = tlprepend(tree((QTREE *) 0, Tidnode, RESDOM, RES_SIZ, (char *) 0), $$);
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
			$$ = tlprepend($1, $3);
		}
;
tlelm:		NAME is afcn =
		{
			Trname = $1;
			/* make a new resdom entry for targ list */
			$$ = (char *) addresdom((QTREE *) 0, $3);
		}
	|	attrib =
		{
			/* makes a new resdom entry for targ list */
			$$ = (char *) addresdom((QTREE *) 0, $1);
		}
	|	var PERIOD ALL =
		{
			if (Opflag == (int) mdREPL)
				/* ALL not defined for REPLACE */
				yyerror(REPALL, ((struct rngtab *) $1)->varname, (char *) 0);
			/* makes set of new resdom entries for targ list */
			$$ = xdot($1);
		}
;
is:		IS
	|	BY
;
qualclause:	where qual =
		{
# ifdef xPTM
			if (AAtTf(76, 2))
				timtrace(25, 0);
# endif
			$$ = norml($2);
			Qlflag--;
# ifdef xPTM
			if (AAtTf(76, 2))
				timtrace(26, 0);
# endif
		}
	|	/* empty */ =
		{
			/* null qualification */
			$$ = norml((QTREE *) 0);
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
			$$ = (char *) tree((QTREE *) 0, $2, UOP, OP_SIZ, $1);
		}
	|	qual LBOP qual =
		{
			$$ = (char *) tree($1, $3, $2, 0, (char *) 0);
		}
	|	clause
;
clause:		afcn relop afcn =
		{
			$$ = (char *) tree($1, $3, BOP, OP_SIZ, $2);
		}
;
relop:		EOP
	|	IS
	|	BDOP
;
afcn:		aggrfcn =
		{
			Agflag++;
		}
	|	attribfcn
	|	afcn BAOPH afcn =
		{
			$$ = (char *) tree($1, $3, BOP, OP_SIZ, $2);
		}
	|	afcn BAOP afcn =
		{
			$$ = (char *) tree($1, $3, BOP, OP_SIZ, $2);
		}
	|	afcn UAOP afcn =
		{
			$$ = (char *) tree($1, $3, BOP, OP_SIZ, $2);
		}
	|	LPAREN afcn RPAREN =
		{
			$$ = $2;
		}
	|	uop afcn	%prec unaryop	=
		{
			$$ = (char *) tree((QTREE *) 0, $2, UOP, OP_SIZ, $1);
		}
	|	FOP LPAREN afcn RPAREN =
		{
			$$ = (char *) tree($3, (QTREE *) 0, UOP, OP_SIZ, $1);
		}
	|	FBOP LPAREN afcn COMMA afcn RPAREN =
		{
			$$ = (char *) tree($3, $5, BOP, OP_SIZ, $1);
		}
;
aggrfcn:	AGOP LPAREN afcn BY domseq qualclause RPAREN =
		{
# ifdef xPTR2
			AAtTfp(35, 0, "agg func\n");
# endif
			windup($5);
			$$ = (char *) tree(tree($5, tree((QTREE *) 0, $3, AOP, AOP_SIZ, $1), BYHEAD, 0, (char *) 0), $6, AGHEAD, 0, (char *) 0);
		}
	|	AGOP LPAREN afcn qualclause RPAREN =
		{
			$$ = (char *) tree(tree((QTREE *) 0, $3, AOP, AOP_SIZ, $1), $4,  AGHEAD, 0, (char *) 0);
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
			$$ = (char *) tree((QTREE *) 0, $1, RESDOM, RES_SIZ, CAST(Rsdmno));
		}
;
attrib:		var PERIOD NAME =
		{
# ifdef xPTR2
			AAtTfp(37, 0, "attrib %s.%s found\n", ((struct rngtab *) $1)->varname, $3);
# endif

			/* remember attribute name */
			Trname = $3;

			/* look up attribute */
			aptr = attlookup($1, Trname);
			$$ = (char *) tree((QTREE *) 0, (QTREE *) 0, VAR, VAR_SIZ, CAST(((struct rngtab *) $1)->rentno), aptr);
		}
;
var:		NAME =
		{
			$$ = (char *) rnglook($1, LOOKVAR);
			if ($$ == 0)
				/* variable not declared */
				yyerror(NOVBLE, $1, (char *) 0);
		}
;
attribfcn:	I2CONST =
		{
			$$ = (char *) tree((QTREE *) 0, (QTREE *) 0, INT, sizeof (short), $1);
		}
	|	I4CONST =
		{
			$$ = (char *) tree((QTREE *) 0, (QTREE *) 0, INT, sizeof (long), $1);
		}
	|	F4CONST =
		{
# ifdef NO_F4
			goto f8_tree;
# else
			$$ = (char *) tree((QTREE *) 0, (QTREE *) 0, FLOAT, sizeof (float), $1);
# endif
		}
	|	F8CONST =
		{
f8_tree:
			$$ = (char *) tree((QTREE *) 0, (QTREE *) 0, FLOAT, sizeof (double), $1);
		}
	|	SCONST =
		{
			if (patmat($1) && !Qlflag)
				Patflag = 1;
			$$ = (char *) tree((QTREE *) 0, (QTREE *) 0, CHAR, AAlength($1), $1);
		}
	|	NAME =
		{
			$$ = (char *) tree((QTREE *) 0, (QTREE *) 0, COP, OP_SIZ, $1);
		}
	|	attrib
;
uop:		UAOP	%prec unaryop	=
		{
			if ($1 == (char *) ((int) opADD))
				$$ = (char *) ((int) opPLUS);
			else
				if ($1 == (char *) ((int) opSUB))
					$$ = (char *) ((int) opMINUS);
		}
;
copy:		copstmnt alias LPAREN coparam RPAREN keywd SCONST =
		{
# ifdef xPTR2
			AAtTfp(41, 0, "copy %s,%s\n", $2, $7);
# endif
			setp($7);
		}
;
copstmnt:	COPY =
		{
			Opflag = (int) mdCOPY;
		}
;
coparam:	cospecs
	|	/* empty */
;
cospecs:	alias is coent
	|	cospecs COMMA alias is coent
;
coent:		alias
	|	SCONST =
		{
			setp($1);
		}
;
alias:		NAME =
		{
			setp($1);
			if (Opflag == (int) mdDESTROY
# ifdef DISTRIB
			   || Opflag == mdDCREATE
# endif
			   || Opflag == (int) mdCREATE)
				rngdel($1);
		}
;
specs:		alias is alias
	|	specs COMMA alias is alias
;
keywd:		INTO =
		{
			setp("\0");
			setp("i");
		}
	|	FROM =
		{
			setp("\0");
			setp("f");
		}
;
create:		crestmnt alias LPAREN specs RPAREN
;
crestmnt:	CREATE =
		{
			Opflag = (int) mdCREATE;

			/* set up parameters for regular create */
			setp("0");		/* relstat = nil */
		}
/*DDD*/	|	CREATE DISTRD =
/*DDD*/		{
/*DDD*/			Opflag = mdDCREATE;
/*DDD*/
/*DDD*/			/* setup parameters for distributed create */
/*DDD*/			setp("U");
/*DDD*/			setp("");
/*DDD*/			setp("01000");	/* relstat = S_DISTRIBUTED */
/*DDD*/		}
;
destroy:	destmnt keys
	|	destqm destlist
;
destmnt:	DESTROY =
		{
			Opflag = (int) mdDESTROY;
		}
;
destqm:		destmnt INTEGRITY NAME =
		{
			Opflag = (int) mdREMQM;
			if (!Qrymod)
				/* no qrymod in database */
				yyerror(NOQRYMOD, (char *) 0);
			setp("6");
			setp($3);
		}
	|	destmnt PERMIT NAME =
		{
			Opflag = (int) mdREMQM;
			if (!Qrymod)
				/* no qrymod in database */
				yyerror(NOQRYMOD, (char *) 0);
			setp("5");
			setp($3);
		}
;
destlist:	I2CONST =
		{
			setp(AA_iocv(((I2TYPE *) $1)->i2type));
		}
	|	destlist COMMA I2CONST =
		{
			setp(AA_iocv(((I2TYPE *) $3)->i2type));
		}
	|	ALL
;
help:		helstmnt hlist
	|	helstmnt =
		{
			setp("2");	/* all relns */
		}
	|	helqmstmnt hqmlist
;
helstmnt:	HELP =
		{
			Opflag = (int) mdHELP;
		}
;
helqmstmnt:	HELP VIEW =
		{
			Opflag = (int) mdDISPLAY;
			if (!Qrymod)
				/* no qrymod in database */
				yyerror(NOQRYMOD, (char *) 0);
			AAsmove("4", hqmbuf);
		}
	|	HELP PERMIT =
		{
			Opflag = (int) mdDISPLAY;
			if (!Qrymod)
				/* no qrymod in database */
				yyerror(NOQRYMOD, (char *) 0);
			AAsmove("5", hqmbuf);
		}
	|	HELP INTEGRITY =
		{
			Opflag = (int) mdDISPLAY;
			if (!Qrymod) /* no qrymod in database */
				yyerror(NOQRYMOD, (char *) 0);
			AAsmove("6", hqmbuf);
		}
;
hlist:		hparam
	|	hlist COMMA hparam
	|	ALL =
		{
			setp("3");
		}
;
hparam:		NAME =
		{
			/* relation */
			setp("0");
			setp($1);
		}
	|	SCONST =
		{
			/* manual page */
			setp("1");
			setp($1);
		}
;
hqmlist:	NAME =
		{
			setp(hqmbuf);
			setp($1);
		}
	|	hqmlist COMMA NAME =
		{
			setp(hqmbuf);
			setp($3);
		}
;
index:		instmnt LPAREN keys RPAREN =
		{
			if (Rsdmno > MAXKEYS)
				/* too many attributes in key */
				yyerror(INDEXTRA, (char *) 0);
		}
;
instmnt:	INDEX ON NAME IS NAME =
		{
			/* init INDEX command */
			Opflag = (int) mdINDEX;
			Rsdmno = 0;
			setp($3);
			setp($5);
			Indexname = $5;
		}
;
modify:		modstmnt alias modstorage modkeys modqual
;
modstmnt:	MODIFY =
		{
			Opflag = (int) mdMODIFY;
			Rsdmno = 0;
		}
;
modstorage:	TO NAME =
		{
			setp($2);
		}
	|	/* empty */ =
		{
			setp("same");
		}
;
modkeys:	modstkey modrptkey
	|	/* empty */
;
modstkey:	ON =
		{
			setp("name");
		}
;
modrptkey:	modbasekey
	|	modrptkey COMMA modbasekey
;
modbasekey:	NAME =
		{
			setp($1);
		}
	|	NAME COLON NAME =
		{
			AAconcat($1, AAztack(":", $3), modbuf);
			setp(modbuf);
		}
;
modqual:	modcond modfill
	|	/* empty */
;
modcond:	WHERE =
		{
			setp("\0");
		}
;
modfill:	modfillnum
	|	modfill COMMA modfillnum
;
modfillnum:	NAME IS I2CONST =
		{
			setp($1);
			setp(AA_iocv(((I2TYPE *) $3)->i2type));
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
			Opflag = (int) mdPRINT;
		}
;
save:		savstmnt alias UNTIL date
;
savstmnt:	SAVE =
		{
			Opflag = (int) mdSAVE;
		}
;
date:		month day_year day_year
;
month:		alias
	|	day_year
;
day_year:	I2CONST =
		{
			p = AA_iocv(((I2TYPE *) $1)->i2type);
# ifdef xPTR3
			AAtTfp(57, 1, "day_year: %s\n", p);
# endif
			setp(p);
		}
;
%%
# include	"../h/scanner.h"
# include	"../h/bs.h"
# include	"tables.y"
# include	"yyerror.y"
