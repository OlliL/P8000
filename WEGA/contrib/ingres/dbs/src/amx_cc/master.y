/* KEY WORDS */
%term	APPEND	CLOSE	COMMIT	COUNT	DELETE	DBS	EXIT
%term	GET	GOTO	INSERT	LENGTH	OPEN	OVER	PERMIT
%term	PRINT	REPLACE	SCAN	USE	USER

/* LOCKING */								/*PL*/
%term	ABORT	IF	DEADLK	LEVEL	MAXLK	READLK	RL_PG		/*PL*/
%term	TIMEOUT	WITH							/*PL*/

/* SEPERATE COMPILING */						/*SC*/
%term	EXPORT	IMPORT							/*SC*/

/* C VAR */
%term	ALLOC	TYPE

/* PUNCTUATION */
%term	BGNCMNT	COMMA	DOM	ENDCMNT	LBRACE	LBRKT	LPAREN
%term	MINUS	PLUS	RBRACE	RBRKT	RPAREN	SEMICOL	STAR

/* BOUNDS OPERATORS */
%term	BDOP	IS

/* CONSTANTS */
%term	ICONST	LCONST	NAME	RCONST	SCONST

%{
# include	"amx.h"
%}

%%

%{
	extern char		*name_lookup();
	extern char		*scan();
	extern int		write();
%}

program:	dbs stmt_list
	=	{
			amx();
		}
	|	expimp							/*SC*/
	=	{							/*SC*/
			amx();						/*SC*/
		}							/*SC*/
;
dbs:		DBS uflag bflag name
	=	{
			if ($1 != (char *) M_SHARE)
			{
				clrbit(DB_SHARE, Xlocks);
				No_locks++;
			}
			tmp($4, $2);
		}
	|	import expimp						/*SC*/
;
expimp:		export dbs db_var_list LPAREN rel_list RPAREN		/*SC*/
;									/*SC*/
export:		EXPORT							/*SC*/
	=	{							/*SC*/
			if (!ExpImp)					/*SC*/
				ExpImp = 1;				/*SC*/
		}							/*SC*/
import:		IMPORT SCONST						/*SC*/
	=	{							/*SC*/
			import($2);					/*SC*/
		}							/*SC*/
;									/*SC*/
rel_list:	rel_list COMMA rel					/*SC*/
	|	rel							/*SC*/
;									/*SC*/
db_var_list:	EXPORT var_list						/*SC*/
	|	/* empty */						/*SC*/
;									/*SC*/
var_list:	var_list db_var						/*SC*/
	|	db_var							/*SC*/
;									/*SC*/
db_var:		db_specifier decl_list SEMICOL				/*SC*/
	=	{							/*SC*/
			fprintf(FILE_tmp, ";\n");			/*SC*/
			Type_spec = opINT;				/*SC*/
		}							/*SC*/
;									/*SC*/
db_specifier:	TYPE							/*SC*/
	=	{							/*SC*/
			if (ExpImp < 0)					/*SC*/
				fprintf(FILE_tmp, "extern ");		/*SC*/
			fprintf(FILE_tmp, "%s\t", TYPE_SPEC[(int) $1]);	/*SC*/
			Type_spec = (int) $1;				/*SC*/
		}							/*SC*/
;									/*SC*/
uflag:		MINUS NAME
	=	{
			if (*((char *) $2) != 'u')
			{
				amxerror(106, $2);
				$$ = (char *) 0;
			}
			else if (AAlength($2) > MAXNAME + 1)
			{
				amxerror(108, $2);
				$$ = (char *) 0;
			}
			else
				$$ = $2;
		}
	|	/* empty */
	=	{
			$$ = (char *) 0;
		}
;
bflag:		PLUS ICONST
	=	{
			Xbufs = (int) $2;
		}
	|	/* empty */
;
name:		NAME
	=	{
			$$ = name_lookup($1);
		}
;
stmt_list:	stmt_list stmt
	|	stmt
;
stmt:		declaration
	|	OPEN rel
	=	{
			w_open();
		}
	|	CLOSE rel
	=	{
			w_close();
		}
	|	USE LPAREN use_list RPAREN
	=	{
			w_use(M_TE);
		}
	|	COMMIT
	=	{
			w_commit();
		}
	|	append LPAREN dom_assign RPAREN
	=	{
			end_append();
		}
	|	get var_assign LBRACE scan_list RBRACE
	=	{
			end_scan($1, 0);
		}
	|	retrieve var_assign LBRACE scan_list RBRACE
	=	{
			end_scan($1, 1);
		}
	|	GOTO
	=	{
			scan_goto((int) $1);
		}
	|	DELETE
	=	{
			delete();
		}
	|	replace LPAREN dom_assign RPAREN
	=	{
			end_replace();
		}
	|	print LPAREN dom_list RPAREN
	=	{
			fprintf(FILE_tmp, msg(209));
		}
	|	COUNT rel c_var
	=	{
			if (Destination.type == opSTRING)
				amxerror(110, Destination.c_var);
			else
				fprintf(FILE_tmp, msg(62),
					Destination.c_var, Relation->rel_uniq);
		}
	|	LENGTH rel c_var
	=	{
			if (Destination.type == opSTRING)
				amxerror(197, Destination.c_var);
			else
				fprintf(FILE_tmp, msg(157),
					Destination.c_var, Relation->rel_uniq);
		}
	|	PERMIT USER
	=	{
			fprintf(FILE_tmp, msg(60 + (int) $2));
		}
	|	SEMICOL		/* no operation */
	|	ABORT							/*PL*/
	=	{							/*PL*/
			w_abort();					/*PL*/
		}							/*PL*/
	|	deadlock LBRACE stmt_list RBRACE			/*PL*/
	=	{							/*PL*/
			fprintf(FILE_tmp, msg(209));			/*PL*/
		}							/*PL*/
	|	EXIT
	=	{
			if (Scan_rel)
				amxerror(212);
			else
				fprintf(FILE_tmp, msg(211));
		}
	|	error
;
rel:		name
	=	{
			rel_lookup($1, 0);
		}
;
use_list:	use_list COMMA use_rel
	=	{
			w_use(Lock_mode);
		}
	|	use_rel
	=	{
			w_use(-Lock_mode);
		}
;
use_rel:	use_mode rel
			     use_with					/*PL*/
;
use_mode:	DBS
	=	{
			if (!$1)
				Lock_mode = M_EXCL;
			else
				Lock_mode = (int) $1;
		}
	|	PLUS
	=	{
			Lock_mode = M_EXCL;
		}
	|	/* empty */
	=	{
			Lock_mode = M_SHARE;
		}
;
use_with:	WITH level readlock maxlocks timeout			/*PL*/
	=	{							/*PL*/
			if (!Dead_lock)					/*PL*/
				goto std_lock;				/*PL*/
		}							/*PL*/
	|	/* empty */						/*PL*/
	=	{							/*PL*/
std_lock:								/*PL*/
			Lock_level = M_REL;				/*PL*/
			Read_lock = 0;					/*PL*/
			Max_locks = -1;					/*PL*/
			Time_out = -1;					/*PL*/
		}							/*PL*/
;									/*PL*/
level:		LEVEL IS RL_PG						/*PL*/
	=	{							/*PL*/
			Lock_level = (int) $3;				/*PL*/
		}							/*PL*/
	|	/* empty */						/*PL*/
	=	{							/*PL*/
			Lock_level = M_REL;				/*PL*/
		}							/*PL*/
;									/*PL*/
readlock:	READLK IS DBS						/*PL*/
	=	{							/*PL*/
			Read_lock = (int) $3;				/*PL*/
		}							/*PL*/
	|	/* empty */						/*PL*/
	=	{							/*PL*/
			Read_lock = 0;					/*PL*/
		}							/*PL*/
;									/*PL*/
maxlocks:	MAXLK IS ICONST						/*PL*/
	=	{							/*PL*/
			Max_locks = (int) $3;				/*PL*/
		}							/*PL*/
	|	/* empty */						/*PL*/
	=	{							/*PL*/
			Max_locks = -1;					/*PL*/
		}							/*PL*/
;									/*PL*/
timeout:	TIMEOUT IS ICONST					/*PL*/
	=	{							/*PL*/
			Time_out = (int) $3;				/*PL*/
		}							/*PL*/
	|	/* empty */						/*PL*/
	=	{							/*PL*/
			Time_out = -1;					/*PL*/
		}							/*PL*/
;									/*PL*/
append:		APPEND rel
	=	{
			bgn_append(FALSE);
		}
	|	APPEND
	=	{
			bgn_append(TRUE);
		}
;
dom_assign:	dom_assign COMMA assign
	|	assign
;
assign:		dom IS value
	=	{
			convert();
		}
;
dom:		DOM name
	=	{
			$$ = (char *) att_lookup($2, FALSE);
		}
	|	DOM ICONST
	=	{
			$$ = (char *) dom_lookup($2, FALSE);
		}
;
value:		scan_dom
	|	scan_var
	|	const
	=	{
			Source.off = C_CONST;
			Source.c_var = $1;
		}
;
scan_dom:	DOM name
	=	{
			$$ = (char *) att_lookup($2, TRUE);
		}
	|	DOM ICONST
	=	{
			$$ = (char *) dom_lookup($2, TRUE);
		}
;
get:		GET rel c_var
	=	{
			if (Destination.type != opLONG)
				amxerror(215, Destination.c_var);
			else
			{
				bgn_scan(0);
				$$ = scan((AMX_REL *) 0, 0);
			}
		}
;
retrieve:	over scan qual
	=	{
			$$ = scan($1, 1);
		}
;
over:		OVER name
	=	{
			rel_lookup($2, AMX_INTERNAL);
			$$ = (char *) Relation;
		}
	|	/* empty */
	=	{
			$$ = (char *) 0;
		}
;
scan:		SCAN rel
	=	{
			bgn_scan(1);
		}
;
qual:		qual COMMA expr
	|	expr
	|	/* empty */
;
expr:		dom relop value
	=	{
			qual((int) $1, (int) $2);
		}
;
relop:		BDOP
	|	IS
;
var_assign:	LPAREN c_var_assign RPAREN
	|	/* empty */
;
c_var_assign:	c_var_assign COMMA c_assign
	|	c_assign
;
c_assign:	c_var IS scan_dom
	=	{
			convert();
		}
	|	c_var
	=	{
			if (Destination.type != opLONG)
				amxerror(215, Destination.c_var);
			else
				fprintf(FILE_tmp, msg(217), Destination.c_var);
		}
;
scan_list:	stmt_list
	|	/* empty */
;
replace:	REPLACE
	=	{
			bgn_replace();
		}
;
deadlock:	IF LPAREN DEADLK RPAREN					/*PL*/
	=	{							/*PL*/
			fprintf(FILE_tmp, msg(83));			/*PL*/
			fprintf(FILE_tmp, msg(208));			/*PL*/
		}							/*PL*/
;									/*PL*/
print:		PRINT
	=	{
			fprintf(FILE_tmp, msg(208));
		}
;
dom_list:	dom_list COMMA scan_dom
	=	{
			w_print((int) $3);
		}
	|	scan_dom
	=	{
			w_print((int) $1);
		}
	|	/* empty */
	=	{
			w_print(0);
		}
;
const:		ICONST
	=	{
			Source.type = opINT;
			Source.len = sizeof (short);
		}
	|	LCONST
	=	{
			Source.type = opLONG;
			Source.len = sizeof (long);
		}
	|	RCONST
	=	{
			Source.type = opDOUBLE;
			Source.len = sizeof (double);
		}
	|	SCONST
	=	{
			Source.type = opSTRING;
			Source.len = AAlength($1);
		}
;
/* C VARIABLE DECLARATION */
declaration:	decl_specifier decl_list SEMICOL
	=	{
			fprintf(FILE_tmp, ";\n");
			Type_spec = opINT;
		}
;
decl_specifier:	type_specifier
	|	sc_specifier
	|	type_specifier sc_specifier
	|	sc_specifier type_specifier
;
type_specifier:	TYPE
	=	{
			fprintf(FILE_tmp, "%s\t", TYPE_SPEC[(int) $1]);
			Type_spec = (int) $1;
		}
;
sc_specifier:	ALLOC
	=	{
			fprintf(FILE_tmp, "%s\t", SC_SPEC[(int) $1]);
		}
;
decl_list:	declarator
	|	declarator comma decl_list
;
comma:		COMMA
	=	{
			fprintf(FILE_tmp, ", ");
		}
;
declarator:	NAME
	=	{
			if (Type_spec == opSTRING)
				amxerror(112, $1);
			else
			{
				ctbl($1, Type_spec);
				fprintf(FILE_tmp, "%s", $1);
			}
		}
	|	STAR NAME
	=	{
			if (Type_spec != opSTRING)
				amxerror(114, TYPE_SPEC[Type_spec], $2);
			else
			{
				ctbl($2, opSTRING);
				fprintf(FILE_tmp, "*%s", $2);
			}
		}
	|	lbrkt blob RBRKT
	=	{
			ctbl($1, opSTRING);
			putc(']', FILE_tmp);
		}
;
lbrkt:		NAME LBRKT
	=	{
			if (Type_spec != opSTRING)
				amxerror(116, TYPE_SPEC[Type_spec], $1);
			else
				fprintf(FILE_tmp, "%s[", $1);
		}
;
blob:		NAME
	=	{
			fprintf(FILE_tmp, "%s", $1);
		}
	|	ICONST
	=	{
			fprintf(FILE_tmp, "%d", (int) $1);
		}
;
c_var:		NAME
	=	{
			decl($1, FALSE);
		}
;
scan_var:	NAME
	=	{
			decl($1, TRUE);
		}
;


%%


# include	"tables.y"
