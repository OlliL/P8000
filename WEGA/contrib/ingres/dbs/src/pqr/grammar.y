/*
** ---  GRAMMAR.Y ------- REPORT RULE SYSTEM  ---------------------------------
**
**
**	REPORT specification
**
**	adapted from	Herrmann, Knut
**			Technische Universitaet Dresden
**			Sektion Informationsverarbeitung
**			Diplomarbeit vom 25.Jan. 1986
**
**	and		DABA/REP for DABA1600
**
**
**	Version: @(#)grammar.y		4.0	02/05/89
**
*/

/* COMMANDS */
%term		ALL		AS		BIG		CALL
%term		CASE		CLEAR		COL		COLUMN
%term		COMPLEMENT	DEFINE		DO		DOMAIN
%term		ELSE		END		ENDA		ENDC
%term		ENDI		ENDO		ENDR		ENDW
%term		EXCEPTION	FOR		GROUPBEGIN	GROUPEND
%term		IF		IN		IS		LENGTH
%term		LINE		NEW		NEWLINE		NEWPAGE
%term		ON		ONE		OUT		PAGE
%term		PROC		REPEAT		REPORT		RETURN
%term		SEMICOL		SET		SPACE		SWITCH
%term		TERMINAL	THEN		TUPLE		NVAR
%term		WHILE

/* CONVERSION WORDS */
%term		BYTE		F_CHAR		FORMAT		GRAPHIC
%term		INTEGER		REAL		WORD

/* FORMAT WORDS */
%term		BATCH		BEFORE		DATE		EXP
%term		FROM_TO		IGNORE		NORM		PIC
%term		POINT		SCALE		WITH

/* CONSTANTS */
%term		BCONST		/* boolean const	*/
%term		ICONST		/* integer const	*/
%term		LCONST		/* long integer const	*/
%term		NAME		/* name const		*/
%term		RCONST		/* real const		*/
%term		SCONST		/* string const		*/
%term		SYS_CONST

/* PUNCTUATION */
%term		LPAREN		RPAREN		BGNCMNT		ENDCMNT

/* OPERATORS */
%term		AUOP	/* ARITHMETIC UNARY OPERATORS	*/
%term		ABOP	/* ARITHMETIC BINARY OPERATORS	*/
%term		ABIS	/* BINARY ASSIGNMENT OPERATORS	*/
%term		BDOP	/* BOUNDS OPERATORS		*/
%term		EOP	/* EQUALITY OPERATORS		*/
%term		LUOP	/* LOGICAL BINARY OPERATORS	*/
%term		LBOP	/* LOGICAL BINARY OPERATORS	*/

/* DEFINE ASCENDING PRECEDENCE FOR OPERATORS */
%left		LBOP
%left		LUOP
%left		AUOP
%left		ABOP
%binary		unaryop


%{
# include	"rc.h"

/* FORMAT STRUCTURE (formatted output) */
static struct format	Format;
extern char		*RC_INIT_JUMP();

%}


%%


program	:	rep_kwd NAME desc fmt_def AS stmt_s report stmt_s SEMICOL
	=	{
			RC_BYTE(rcRC_END);
			rc_report($2);
		}
	|	error
;
rep_kwd	:	DEFINE REPORT
	|	REPORT
;
desc	:	DOMAIN NAME
	=	{
			S_desc = $2;
		}
	|	/* empty */
	=	{
			S_desc = "desc";
		}
;
fmt_def	:	WITH fmt_s
	|	/* empty */
;
fmt_s	:	fmt
	|	fmt_s fmt
;
fmt	:	LINE ICONST
	=	{
			R_lines = (int) $2;
		}
	|	ALL LINE ICONST
	=	{
			R_all_lines = (int) $3;
		}
	|	COLUMN ICONST
	=	{
			R_cols = (int) $2;
		}
	|	FROM_TO PAGE ICONST
	=	{
			if (!((int) $1))
				R_first_page = (int) $3;
			else
				R_last_page = (int) $3;
		}
	|	FROM_TO TUPLE ICONST
	=	{
			if (!((int) $1))
				R_from_tuple = (int) $3;
			else
				R_to_tuple = (int) $3;
		}
	|	BIG
	=	{
			R_big = TRUE;
		}
	|	DEFINE NAME IS ICONST
	=	{
			RC_MACRO($2, (int) $4);
		}
	|	DEFINE DOMAIN ICONST IS dom_fmt ICONST
	=	{
			RC_IDOM((int) $3, (int) $5, (int) $6);
		}
	|	DEFINE DOMAIN NAME IS dom_fmt ICONST
	=	{
			RC_NDOM($3, (int) $5, (int) $6);
		}
	|	error
;
dom_fmt	:	INTEGER
	|	REAL
	|	F_CHAR
;
report	:	for_all print enda_kwd
	=	{
			RC_BYTE(rcJUMP);
			RC_JUMP($1);
			R_for_all = R_rc - R_code;
		}
;
for_all	:	fora_kwd stmt_s
;
print	:	tuple_s
	|	print tuple_s
;
tuple_s	:	tuple stmt_s
;
tuple	:	for1_kwd stmt_s end1_kwd
	=	{
			RC_BYTE(rcEOTUP);
		}
;


/* statements */

stmt_s	:	s_stmt
	|	/* empty */
;
s_stmt	:	s_stmt stmt
	|	stmt
;
		/* ASSIGNMENT */
stmt	:	assign

		/* PROCEDURE declaration */
	|	proc_hd stmt_s RETURN
	=	{
			RC_BYTE(rcRETURN);
			RC_UPDATE($1);
		}

		/* procedure CALL statement */
	|	call
	=	{
			if (!Proc[((int) $1) - 1])
				rc_error(3, $1);
			else
			{
				RC_BYTE(rcCALL);
				RC_JUMP(Proc[((int) $1) - 1]);
			}
		}

		/* IF statement */
	|	if_cond if_then if_else endi_kwd
	=	{
			RC_UPDATE($2);
		}

		/* WHILE statement */
	|	wh_cond wh_do endw_kwd
	=	{
			/* jump to condition */
			RC_BYTE(rcJUMP);
			RC_JUMP($1 - 1);
			/* update jump over WHILE */
			RC_UPDATE($2);
		}

		/* REPEAT statement */
	|	repeat value stmt_s endr_kwd
	=	{
			RC_BYTE(rcREPEAT);
			RC_WORD((int) $2);
			RC_JUMP($1);
		}

		/* CASE statement */
	|	case_hd case_s out endc_kwd
	=	{
			rc_case($1);
		}

		/* ON statement */
	|	on_hd stmt_s endo_kwd
	=	{
			RC_BYTE(rcRTTS);
			RC_UPDATE($1);	/* jump over exception */
		}

		/* TERMINAL statement */
	|	console
	=	{
			Format.str_len = AAlength($1);
			RC_BYTE(prINPUT);
			RC_WORD(Format.str_len);
			RC_WORD(Format.batch_len);
			RC_STR($1, Format.str_len);
		}

		/* print DATE or TIME */
	|	DATE
	=	{
			RC_BYTE((int) $1);
		}

		/* print NEWLINE */
	|	NEWLINE value
	=	{
			RC_BYTE(prNEWLINE);
			RC_WORD((int) $2);
		}
	|	NEW LINE value
	=	{
			RC_BYTE(prNEWLINE);
			RC_WORD((int) $3);
		}
	|	NEWLINE
	=	{
			goto new_line;
		}
	|	NEW LINE
	=	{
new_line:
			RC_BYTE(prNEWLINE);
			RC_WORD(A_NEWLINE);
		}

		/* print NEWPAGE */
	|	NEWPAGE
	=	{
			goto new_page;
		}
	|	NEW PAGE
	=	{
new_page:
			RC_BYTE(prNEWPAGE);
		}

		/* set COLUMN pointer */
	|	COL value
	=	{
			RC_BYTE(prNEWCOL);
			RC_WORD((int) $2);
		}

		/* print SPACES */
	|	SPACE value
	=	{
			RC_BYTE(prSPACE);
			RC_WORD((int) $2);
		}

		/* print constant STRING */
	|	string
	=	{
			RC_BYTE(prSTRING);
			RC_WORD(Format.str_len);
			RC_WORD(Format.batch_len);
			RC_STR(Format.str_adr, Format.str_len);
			if (Format.fr_to[1])
			{
				RC_BYTE(rcREPEAT);
				RC_WORD((int) Format.fr_to[1]);
				RC_JUMP(Format.fr_to[0]);
			}
		}

		/* formatted OUTPUT statement */
	|	format

		/* ERROR */
	|	error
;
		/* ASSIGMENT */
assign	:	CLEAR b_var
	=	{
			$1 = RC_INIT_JUMP(setSWITCH);
			RC_BYTE(pushSWITCH);
			RC_WORD((int) $2);
			RC_BYTE(pushFALSE);
			goto expr_end;
		}
	|	COMPLEMENT b_var
	=	{
			$1 = RC_INIT_JUMP(setSWITCH);
			RC_BYTE(pushSWITCH);
			RC_WORD((int) $2);
			RC_BYTE(pushSWITCH);
			RC_WORD((int) $2);
			RC_BYTE(rcNOT);
			goto expr_end;
		}
	|	b_assign IS b_expr
	=	{
			goto expr_end;
		}
	|	b_assign
	=	{
			RC_BYTE(pushTRUE);
			goto expr_end;
		}
	|	c_assign expr
	=	{
			RC_BYTE(Format.data);
			goto expr_end;
		}
	|	n_assign expr
	=	{
expr_end:
			RC_BYTE(rcEND_SET);
			RC_UPDATE($1);
		}
;
b_assign:	SET b_var
	=	{
			$$ = RC_INIT_JUMP(setSWITCH);
			RC_BYTE(pushSWITCH);
			RC_WORD((int) $2);
		}
;
c_assign:	SET sys_var ABIS
	=	{
			$$ = RC_INIT_JUMP(setVAR);
			RC_SYS_VAR((int) $2, FALSE);
			RC_SYS_VAR((int) $2, FALSE);
			Format.data = (int) $3;
		}
;
n_assign:	SET sys_var IS
	=	{
			$$ = RC_INIT_JUMP(setVAR);
			RC_SYS_VAR((int) $2, FALSE);
		}
;
		/* PROCEDURE declaration */
proc_hd	:	PROC proc_nam
	=	{
			if (Proc[((int) $2) - 1])
				rc_error(2, $2);
			else
			{
				/* initialze jump over proc */
				$$ = RC_INIT_JUMP(rcJUMP);
				/* now get the address of procedure */
				Proc[((int) $2) - 1] = R_rc;
			}
		}
;
		/* procedure CALL statement */
call	:	CALL PROC proc_nam
	=	{
			$$ = $3;
		}
	|	CALL proc_nam
	=	{
			$$ = $2;
		}
;
proc_nam:	ICONST
	=	{
			if (((int) $1) <= 0 || ((int) $1) > MAX_PROCS)
				rc_error(1, $1);
		}
;
		/* IF statement */
if_cond	:	if_kwd b_expr
	=	{
			RC_UPDATE($1);
		}
;
if_kwd	:	IF
	=	{
if_code:
			/* initialize jump to THEN */
			$$ = RC_INIT_JUMP(rcIF);
		}
;
if_then	:	th_kwd stmt_s
	=	{
			/* initialize jump to END IF */
			$$ = RC_INIT_JUMP(rcJUMP);
			/* update jump to THEN */
			RC_UPDATE($1);
		}
;
th_kwd	:	THEN
	=	{
then_code:
			/* initialize jump to ELSE */
			$$ = RC_INIT_JUMP(rcTHEN);
		}
;
if_else	:	ELSE stmt_s
	|	/* empty */
;
		/* WHILE statement */
wh_cond	:	wh_kwd b_expr
	=	{
			RC_UPDATE($1);
		}
;
wh_kwd	:	WHILE
	=	{
			goto if_code;
		}
;
wh_do	:	do stmt_s
;
do	:	DO
	=	{
			goto then_code;
		}
;
		/* REPEAT statement */
repeat	:	REPEAT
	=	{
			$$ = R_rc;
		}
;
		/* CASE statement */
case_hd	:	CASE var in_kwd
	=	{
			if (Case[0])
				rc_error(19);
			else
			{
				RC_BYTE(prVAR);
				RC_WORD((int) $2);
				/* initialize case# and addr(jump_table) */
				$$ = RC_INIT_JUMP(rcCASE);
				RC_WORD(0);	/* case# */
				Case_no = 0;
				Case[0] = R_rc;
			}
		}
;
in_kwd	:	IN
	|	/* empty */
;
case_s	:	one_case
	|	case_s one_case
;
one_case:	DO stmt_s
	=	{
			RC_BYTE(rcRETURN);
			if (++Case_no == MAX_CASES)
				rc_error(23);
			else
				Case[Case_no] = R_rc;
		}
;
out	:	OUT stmt_s
	=	{
out_code:
			RC_BYTE(rcRETURN);
		}
	|	/* empty */
	=	{
			goto out_code;
		}
;
		/* ON statement */
on_hd	:	ON EXCEPTION do_kwd
	=	{
			/* initialize jump over exception */
			$$ = RC_INIT_JUMP((int) $2);
		}
;
		/* TERMINAL statement */
console	:	TERMINAL SCONST BATCH value
	=	{
			Format.batch_len = (int) $4;
			$$ = $2;
		}
	|	TERMINAL SCONST
	=	{
			Format.batch_len = A_BATCH;
			$$ = $2;
		}
;
		/* print constant STRING */
string	:	str_kwd batch
	|	str_kwd	value
	=	{
			Format.fr_to[1] = $2;
		}
	|	str_kwd
;
str_kwd	:	sconst
	=	{
			/* initialize string output */
			Format.batch_len = A_BATCH;
			Format.fr_to[0] = R_rc;
			Format.fr_to[1] = (char *) 0;	/* used as repeat info */
		}
;
sconst	:	SCONST
	=	{
			Format.str_len = AAlength($1);
			Format.str_adr = $1;
		}
;
batch	:	BATCH value
	=	{
			Format.batch_len = (int) $2;
		}
;
		/* formatted OUTPUT statement	*/
format	:	int_stm int_fmt
	=	{
			if (!Format.data)
				rc_error(21);
			RC_BYTE(fmtINT);
			RC_WORD(Format.length);
			RC_STR(&Format.scale, sizeof (double));

			/* picture string */
			RC_WORD(Format.str_len);
			RC_STR(Format.str_adr, Format.str_len);
		}
	|	dbl_stm dbl_fmt
	=	{
			if (!Format.data)
				rc_error(21);
			RC_BYTE(fmtREAL);
			RC_WORD(Format.length);
			RC_STR(&Format.scale, sizeof (double));
			RC_WORD(Format.before);
			RC_WORD(Format.point);
			RC_WORD(Format.ignore);
			RC_WORD(Format.norm);

			/* exponent string */
			RC_WORD(Format.str_len);
			RC_STR(Format.str_adr, Format.str_len);
		}
	|	chr_stm chr_fmt
	=	{
			if (!Format.data)
				rc_error(21);
			RC_BYTE(fmtCHAR);
			RC_WORD(Format.length);
			RC_WORD((int) Format.fr_to[0]);
			RC_WORD((int) Format.fr_to[1]);
			RC_WORD(Format.batch_len);
		}
	|	gra_stm gra_fmt
	=	{
			if (!Format.data)
				rc_error(21);
			RC_BYTE(fmtGRAPH);
			RC_WORD(Format.length);

			/* from */
			RC_WORD((int) Format.fr_to_v[0]);
			if (!Format.fr_to_v[0])
				RC_STR(&Format.fr_to_8[0], sizeof (double));

			/* to */
			RC_WORD(Format.fr_to_v[1]);
			if (!Format.fr_to_v[1])
				RC_STR(&Format.fr_to_8[1], sizeof (double));

			/* with char */
			RC_STR(Format.str_adr, sizeof (char));
		}
		/* BIT/OCT/DEC/HEX */
	|	byte_kwd length data
	=	{
			RC_BYTE((int) $1);
			RC_WORD((int) $2);
		}
	|	byte_kwd data
	=	{
			RC_BYTE((int) $1);
			RC_WORD(0);
		}
;
/* INTEGER format  */
int_stm	:	INTEGER
	=	{
			/* initialize Format structure */
			Format.data = FALSE;
			Format.length = 0;
			Format.scale = A_SCALE;
			Format.str_len = 0;
			Format.str_adr = 0;
		}
;
int_fmt	:	i_fmt
	|	int_fmt i_fmt
;
i_fmt	:	length
	|	scale
	|	pic
	|	object
;
length	:	LENGTH value
	=	{
			Format.length = (int) ($$ = $2);
		}
;
scale	:	SCALE RCONST
	=	{
			Format.scale = *((double *) $2);
		}
	|	SCALE LCONST
	=	{
			Format.scale = *((long *) $2);
		}
	|	SCALE ICONST
	=	{
			Format.scale = ((int) $2);
		}
;
pic	:	PIC sconst
;
/* REAL    format  */
dbl_stm	:	REAL
	=	{
			/* initialize Format structure */
			Format.data = FALSE;
			Format.length = 0;
			Format.scale = A_SCALE;
			Format.before = A_BEFORE;
			Format.point = A_POINT;
			Format.str_len = 0;
			Format.str_adr = 0;
			Format.ignore = FALSE;
			Format.norm = A_NORM;
		}
;
dbl_fmt	:	d_fmt
	|	dbl_fmt d_fmt
;
d_fmt	:	length
	|	scale
	|	BEFORE value
	=	{
			if (((int) $2) > MAX_I1)
				rc_error(22, 0, MAX_I1);
			else
				Format.before = ((int) $2);
		}
	|	POINT value
	=	{
			if (((int) $2) > MAX_I1)
				rc_error(22, 0, MAX_I1);
			else
				Format.point = ((int) $2);
		}
	|	EXP sconst
	|	IGNORE
	=	{
			Format.ignore |= B_IGNORE;
		}
	|	NORM norm
	=	{
			Format.norm = ((int) $2);
		}
	|	object
;
norm	:	ICONST
	=	{
			if (((int) $1) < L_NORM || ((int) $1) > H_NORM)
				rc_error(22, L_NORM, H_NORM);
			else if (((int) $1) < 0)
			{
				Format.ignore |= B_NORM;
				$$ = (char *) (- ((int) $1));
			}
		}
	|	var
;
/* CHAR    format  */
chr_stm	:	F_CHAR
	=	{
			/* initialze Format structure */
			Format.data = FALSE;
			Format.length = 0;
			Format.fr_to[0] = (char *) A_FROM;
			Format.fr_to[1] = (char *) 0;
			Format.batch_len = A_BATCH;
		}
;
chr_fmt	:	c_fmt
	|	chr_fmt c_fmt
;
c_fmt	:	length
	|	fr_to
	|	batch
	|	object
;
fr_to	:	FROM_TO value
	=	{
			Format.fr_to[(int) $1] = $2;
		}
;
/* GRAPHIC format  */
gra_stm	:	GRAPHIC
	=	{
			/* initialize Format structure */
			Format.data = FALSE;
			Format.length = 0;
			Format.fr_to_v[0] = 0;
			Format.fr_to_v[1] = 0;
			Format.fr_to_8[0] = 0;
			Format.fr_to_8[1] = 0;
			Format.str_len = sizeof (char);
			Format.str_adr = "*";
		}
;
gra_fmt	:	g_fmt
	|	gra_fmt g_fmt
;
g_fmt	:	length
	|	FROM_TO var
	=	{
			Format.fr_to_v[(int) $1] = (int) $2;
		}
	|	FROM_TO ICONST
	=	{
			Format.fr_to_8[(int) $1] = (int) $2;
		}
	|	FROM_TO LCONST
	=	{
			Format.fr_to_8[(int) $1] = *((long *) $2);
		}
	|	FROM_TO RCONST
	=	{
			Format.fr_to_8[(int) $1] = *((double *) $2);
		}
	|	WITH sconst
	=	{
			if (!Format.str_len)
				Format.str_adr = " ";
		}
	|	object
;
object	:	data
	=	{
			if (Format.data++)
				rc_error(20);
		}
;
data	:	SYS_CONST
	=	{
			RC_BYTE(prCONST);
			RC_WORD((int) $1);
		}
	|	sys_var
	=	{
			RC_SYS_VAR((int) $1, TRUE);
		}
	|	tup_kwd
;
tup_kwd	:	DOMAIN value
	=	{
			switch ((int) $1)
			{
			  case pushLAST:
				RC_BYTE(prLAST);
				break;

			  case pushVALUE:
				RC_BYTE(prVALUE);
				break;

			  case pushNEXT:
				RC_BYTE(prNEXT);
			}
			RC_WORD((int) $2);
		}
;


/* expressions */

b_expr	:	LPAREN b_expr RPAREN
	|	LUOP b_expr
	=	{
			RC_BYTE((int) $1);
		}
	|	b_expr LBOP b_expr
	=	{
			RC_BYTE((int) $2);
		}
	|	expr relop expr
	=	{
			RC_BYTE((int) $2);
		}
	|	BCONST
	=	{
			RC_BYTE((int) $1);
		}
	|	b_var
	=	{
			RC_BYTE(pushSWITCH);
			RC_WORD((int) $1);
		}
	|	GROUPBEGIN value
	=	{
			RC_BYTE(pushLAST);
			RC_WORD((int) $2);
			RC_WORD(0);	/* from */
			RC_WORD(0);	/*  to  */
			RC_BYTE(pushVALUE);
			RC_WORD((int) $2);
			RC_WORD(0);	/* from */
			RC_WORD(0);	/*  to  */
			RC_BYTE(bdNE);
		}
	|	GROUPEND value
	=	{
			RC_BYTE(pushVALUE);
			RC_WORD((int) $2);
			RC_WORD(0);	/* from */
			RC_WORD(0);	/*  to  */
			RC_BYTE(pushNEXT);
			RC_WORD((int) $2);
			RC_WORD(0);	/* from */
			RC_WORD(0);	/*  to  */
			RC_BYTE(bdNE);
		}
;
expr	:	LPAREN expr RPAREN
	|	auop expr		%prec unaryop
	=	{
			/* skip positive sign */
			if (((int) $1) == ((int) rcMIN))
				RC_BYTE(rcMIN);
		}
	|	expr ABOP expr
	=	{
			RC_BYTE((int) $2);
		}
	|	expr AUOP expr		/* binary '+' and '-' */
	=	{
			if (((int) $2) == ((int) rcMIN))
				RC_BYTE(rcSUB);
			else
				RC_BYTE((int) $2);
		}
	|	ICONST
	=	{
			if (!((int) $1))
				RC_BYTE(pushNULL);
			else if (((int) $1) <= MAX_I1 && ((int) $1) >= MIN_I1)
			{
				RC_BYTE(push1INT);
				RC_BYTE((int) $1);
			}
			else
			{
				RC_BYTE(push2INT);
				RC_WORD((int) $1);
			}
		}
	|	LCONST
	=	{
			RC_BYTE(push4INT);
			RC_STR($1, sizeof (long));
		}
	|	RCONST
	=	{
			RC_BYTE(pushREAL);
			RC_STR($1, sizeof (double));
		}
	|	sconst
	=	{
			RC_BYTE(pushSTRING);
			RC_WORD(Format.str_len);
			RC_STR($1, Format.str_len);
		}
	|	SYS_CONST
	=	{
			RC_BYTE(pushCONST);
			RC_BYTE((int) $1);
		}
	|	sys_var
	=	{
			RC_SYS_VAR((int) $1, FALSE);
		}
	|	domain
	=	{
			RC_WORD((int) Format.fr_to[0]);
			RC_WORD((int) Format.fr_to[1]);
		}
;
relop	:	EOP
	|	IS
	|	BDOP
;
auop	:	AUOP			%prec unaryop
;
domain	:	dom_kwd fr_to fr_to
	|	dom_kwd fr_to
	|	dom_kwd
;
dom_kwd	:	DOMAIN value
	=	{
			RC_BYTE((int) $1);
			RC_WORD((int) $2);
			/* initialize Format structure */
			Format.fr_to[0] = (char *) 0;
			Format.fr_to[1] = (char *) 0;
		}
;


/* (SYSTEM) values and variables */

sys_var	:	var
	|	PAGE
	=	{
			$$ = (char *) SYS_PAGE;
		}
	|	LINE
	=	{
			$$ = (char *) SYS_LINE;
		}
	|	ALL LINE
	=	{
			$$ = (char *) SYS_ALL_LINE;
		}
	|	COLUMN
	=	{
			$$ = (char *) SYS_COL;
		}
	|	TUPLE
	=	{
			$$ = (char *) SYS_TUPL;
		}
;
var	:	NVAR NVAR ICONST
	=	{
			if (((int) $3) < 0 || ((int) $3) >= MAX_VAR)
				rc_error(6, $3);
			else
				$$ = (char *) (((int) $3) | VAR_VAR_MASK);
		}
	|	NVAR ICONST
	=	{
			if (((int) $2) < 0 || ((int) $2) >= MAX_VAR)
				rc_error(6, $2);
			else
				$$ = (char *) (((int) $2)| VAR_MASK);
		}
;
b_var	:	SWITCH value
	=	{
			if (((int) $2) >= MAX_BVAR)
				rc_error(7, $2);
			else
				$$ = $2;
		}
;
value	:	ICONST
	=	{
			if (((int) $1) < 0)
			{
				rc_error(8, $1);
				$$ = 0;
			}
		}
	|	var
;


/* KEY WORDS */

byte_kwd:	FORMAT BYTE
	|	FORMAT WORD
	=	{
			$$ = $1 + 1;
		}
	|	FORMAT
;
do_kwd	:	DO
	|	/* empty */
;
end1_kwd:	ENDO
	|	END one_kwd
;
enda_kwd:	ENDA
	|	END ALL
;
endc_kwd:	ENDC
	|	END CASE
;
endi_kwd:	ENDI
	|	END IF
;
endo_kwd:	ENDO
	|	END
;
endr_kwd:	ENDR
	|	END REPEAT
;
endw_kwd:	ENDW
	|	END WHILE
;
for1_kwd:	FOR one_kwd do_kwd
	=	{
			RC_BYTE(rcONE);
		}
;
fora_kwd:	FOR ALL do_kwd
	=	{
			RC_BYTE(rcALL);
			$$ = R_rc;
		}
;
one_kwd	:	ONE
	|	/* empty */
;


%%


# include	"rc_tab.y"
