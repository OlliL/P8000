#
# define	ALL                 	257
# define	AS                  	258
# define	BIG                 	259
# define	CALL                	260
# define	CASE                	261
# define	CLEAR               	262
# define	COL                 	263
# define	COLUMN              	264
# define	COMPLEMENT          	265
# define	DEFINE              	266
# define	DO                  	267
# define	DOMAIN              	268
# define	ELSE                	269
# define	END                 	270
# define	ENDA                	271
# define	ENDC                	272
# define	ENDI                	273
# define	ENDO                	274
# define	ENDR                	275
# define	ENDW                	276
# define	EXCEPTION           	277
# define	FOR                 	278
# define	GROUPBEGIN          	279
# define	GROUPEND            	280
# define	IF                  	281
# define	IN                  	282
# define	IS                  	283
# define	LENGTH              	284
# define	LINE                	285
# define	NEW                 	286
# define	NEWLINE             	287
# define	NEWPAGE             	288
# define	ON                  	289
# define	ONE                 	290
# define	OUT                 	291
# define	PAGE                	292
# define	PROC                	293
# define	REPEAT              	294
# define	REPORT              	295
# define	RETURN              	296
# define	SEMICOL             	297
# define	SET                 	298
# define	SPACE               	299
# define	SWITCH              	300
# define	TERMINAL            	301
# define	THEN                	302
# define	TUPLE               	303
# define	NVAR                	304
# define	WHILE               	305
# define	BYTE                	306
# define	F_CHAR              	307
# define	FORMAT              	308
# define	GRAPHIC             	309
# define	INTEGER             	310
# define	REAL                	311
# define	WORD                	312
# define	BATCH               	313
# define	BEFORE              	314
# define	DATE                	315
# define	EXP                 	316
# define	FROM_TO             	317
# define	IGNORE              	318
# define	NORM                	319
# define	PIC                 	320
# define	POINT               	321
# define	SCALE               	322
# define	WITH                	323
# define	BCONST              	324
# define	ICONST              	325
# define	LCONST              	326
# define	NAME                	327
# define	RCONST              	328
# define	SCONST              	329
# define	SYS_CONST           	330
# define	LPAREN              	331
# define	RPAREN              	332
# define	BGNCMNT             	333
# define	ENDCMNT             	334
# define	AUOP                	335
# define	ABOP                	336
# define	ABIS                	337
# define	BDOP                	338
# define	EOP                 	339
# define	LUOP                	340
# define	LBOP                	341
# define	unaryop             	342
# include	"rc.h"

/* FORMAT STRUCTURE (formatted output) */
static struct format	Format;
extern char		*RC_INIT_JUMP();

# define	yyclearin		(yychar = -1)
# define	yyerrok			(yyerrflag = 0)
extern int	yychar;
extern int	yyerrflag;

char		*yyval = (char *) 0;
char		**yypv;
char		*yylval = (char *) 0;

yyactr(__np__)
register int	__np__;
{
switch(__np__){

  case 1:	{
			RC_BYTE(rcRC_END);
			rc_report(yypv[2]);
		} break;
  case 5:	{
			S_desc = yypv[2];
		} break;
  case 6:	{
			S_desc = "desc";
		} break;
  case 11:	{
			R_lines = (int) yypv[2];
		} break;
  case 12:	{
			R_all_lines = (int) yypv[3];
		} break;
  case 13:	{
			R_cols = (int) yypv[2];
		} break;
  case 14:	{
			if (!((int) yypv[1]))
				R_first_page = (int) yypv[3];
			else
				R_last_page = (int) yypv[3];
		} break;
  case 15:	{
			if (!((int) yypv[1]))
				R_from_tuple = (int) yypv[3];
			else
				R_to_tuple = (int) yypv[3];
		} break;
  case 16:	{
			R_big = TRUE;
		} break;
  case 17:	{
			RC_MACRO(yypv[2], (int) yypv[4]);
		} break;
  case 18:	{
			RC_IDOM((int) yypv[3], (int) yypv[5], (int) yypv[6]);
		} break;
  case 19:	{
			RC_NDOM(yypv[3], (int) yypv[5], (int) yypv[6]);
		} break;
  case 24:	{
			RC_BYTE(rcJUMP);
			RC_JUMP(yypv[1]);
			R_for_all = R_rc - R_code;
		} break;
  case 29:	{
			RC_BYTE(rcEOTUP);
		} break;
  case 35:	{
			RC_BYTE(rcRETURN);
			RC_UPDATE(yypv[1]);
		} break;
  case 36:	{
			if (!Proc[((int) yypv[1]) - 1])
				rc_error(3, yypv[1]);
			else
			{
				RC_BYTE(rcCALL);
				RC_JUMP(Proc[((int) yypv[1]) - 1]);
			}
		} break;
  case 37:	{
			RC_UPDATE(yypv[2]);
		} break;
  case 38:	{
			/* jump to condition */
			RC_BYTE(rcJUMP);
			RC_JUMP(yypv[1] - 1);
			/* update jump over WHILE */
			RC_UPDATE(yypv[2]);
		} break;
  case 39:	{
			RC_BYTE(rcREPEAT);
			RC_WORD((int) yypv[2]);
			RC_JUMP(yypv[1]);
		} break;
  case 40:	{
			rc_case(yypv[1]);
		} break;
  case 41:	{
			RC_BYTE(rcRTTS);
			RC_UPDATE(yypv[1]);	/* jump over exception */
		} break;
  case 42:	{
			Format.str_len = AAlength(yypv[1]);
			RC_BYTE(prINPUT);
			RC_WORD(Format.str_len);
			RC_WORD(Format.batch_len);
			RC_STR(yypv[1], Format.str_len);
		} break;
  case 43:	{
			RC_BYTE((int) yypv[1]);
		} break;
  case 44:	{
			RC_BYTE(prNEWLINE);
			RC_WORD((int) yypv[2]);
		} break;
  case 45:	{
			RC_BYTE(prNEWLINE);
			RC_WORD((int) yypv[3]);
		} break;
  case 46:	{
			goto new_line;
		} break;
  case 47:	{
new_line:
			RC_BYTE(prNEWLINE);
			RC_WORD(A_NEWLINE);
		} break;
  case 48:	{
			goto new_page;
		} break;
  case 49:	{
new_page:
			RC_BYTE(prNEWPAGE);
		} break;
  case 50:	{
			RC_BYTE(prNEWCOL);
			RC_WORD((int) yypv[2]);
		} break;
  case 51:	{
			RC_BYTE(prSPACE);
			RC_WORD((int) yypv[2]);
		} break;
  case 52:	{
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
		} break;
  case 55:	{
			yypv[1] = RC_INIT_JUMP(setSWITCH);
			RC_BYTE(pushSWITCH);
			RC_WORD((int) yypv[2]);
			RC_BYTE(pushFALSE);
			goto expr_end;
		} break;
  case 56:	{
			yypv[1] = RC_INIT_JUMP(setSWITCH);
			RC_BYTE(pushSWITCH);
			RC_WORD((int) yypv[2]);
			RC_BYTE(pushSWITCH);
			RC_WORD((int) yypv[2]);
			RC_BYTE(rcNOT);
			goto expr_end;
		} break;
  case 57:	{
			goto expr_end;
		} break;
  case 58:	{
			RC_BYTE(pushTRUE);
			goto expr_end;
		} break;
  case 59:	{
			RC_BYTE(Format.data);
			goto expr_end;
		} break;
  case 60:	{
expr_end:
			RC_BYTE(rcEND_SET);
			RC_UPDATE(yypv[1]);
		} break;
  case 61:	{
			yyval = RC_INIT_JUMP(setSWITCH);
			RC_BYTE(pushSWITCH);
			RC_WORD((int) yypv[2]);
		} break;
  case 62:	{
			yyval = RC_INIT_JUMP(setVAR);
			RC_SYS_VAR((int) yypv[2], FALSE);
			RC_SYS_VAR((int) yypv[2], FALSE);
			Format.data = (int) yypv[3];
		} break;
  case 63:	{
			yyval = RC_INIT_JUMP(setVAR);
			RC_SYS_VAR((int) yypv[2], FALSE);
		} break;
  case 64:	{
			if (Proc[((int) yypv[2]) - 1])
				rc_error(2, yypv[2]);
			else
			{
				/* initialze jump over proc */
				yyval = RC_INIT_JUMP(rcJUMP);
				/* now get the address of procedure */
				Proc[((int) yypv[2]) - 1] = R_rc;
			}
		} break;
  case 65:	{
			yyval = yypv[3];
		} break;
  case 66:	{
			yyval = yypv[2];
		} break;
  case 67:	{
			if (((int) yypv[1]) <= 0 || ((int) yypv[1]) > MAX_PROCS)
				rc_error(1, yypv[1]);
		} break;
  case 68:	{
			RC_UPDATE(yypv[1]);
		} break;
  case 69:	{
if_code:
			/* initialize jump to THEN */
			yyval = RC_INIT_JUMP(rcIF);
		} break;
  case 70:	{
			/* initialize jump to END IF */
			yyval = RC_INIT_JUMP(rcJUMP);
			/* update jump to THEN */
			RC_UPDATE(yypv[1]);
		} break;
  case 71:	{
then_code:
			/* initialize jump to ELSE */
			yyval = RC_INIT_JUMP(rcTHEN);
		} break;
  case 74:	{
			RC_UPDATE(yypv[1]);
		} break;
  case 75:	{
			goto if_code;
		} break;
  case 77:	{
			goto then_code;
		} break;
  case 78:	{
			yyval = R_rc;
		} break;
  case 79:	{
			if (Case[0])
				rc_error(19);
			else
			{
				RC_BYTE(prVAR);
				RC_WORD((int) yypv[2]);
				/* initialize case# and addr(jump_table) */
				yyval = RC_INIT_JUMP(rcCASE);
				RC_WORD(0);	/* case# */
				Case_no = 0;
				Case[0] = R_rc;
			}
		} break;
  case 84:	{
			RC_BYTE(rcRETURN);
			if (++Case_no == MAX_CASES)
				rc_error(23);
			else
				Case[Case_no] = R_rc;
		} break;
  case 85:	{
out_code:
			RC_BYTE(rcRETURN);
		} break;
  case 86:	{
			goto out_code;
		} break;
  case 87:	{
			/* initialize jump over exception */
			yyval = RC_INIT_JUMP((int) yypv[2]);
		} break;
  case 88:	{
			Format.batch_len = (int) yypv[4];
			yyval = yypv[2];
		} break;
  case 89:	{
			Format.batch_len = A_BATCH;
			yyval = yypv[2];
		} break;
  case 91:	{
			Format.fr_to[1] = yypv[2];
		} break;
  case 93:	{
			/* initialize string output */
			Format.batch_len = A_BATCH;
			Format.fr_to[0] = R_rc;
			Format.fr_to[1] = (char *) 0;	/* used as repeat info */
		} break;
  case 94:	{
			Format.str_len = AAlength(yypv[1]);
			Format.str_adr = yypv[1];
		} break;
  case 95:	{
			Format.batch_len = (int) yypv[2];
		} break;
  case 96:	{
			if (!Format.data)
				rc_error(21);
			RC_BYTE(fmtINT);
			RC_WORD(Format.length);
			RC_STR(&Format.scale, sizeof (double));

			/* picture string */
			RC_WORD(Format.str_len);
			RC_STR(Format.str_adr, Format.str_len);
		} break;
  case 97:	{
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
		} break;
  case 98:	{
			if (!Format.data)
				rc_error(21);
			RC_BYTE(fmtCHAR);
			RC_WORD(Format.length);
			RC_WORD((int) Format.fr_to[0]);
			RC_WORD((int) Format.fr_to[1]);
			RC_WORD(Format.batch_len);
		} break;
  case 99:	{
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
		} break;
  case 100:	{
			RC_BYTE((int) yypv[1]);
			RC_WORD((int) yypv[2]);
		} break;
  case 101:	{
			RC_BYTE((int) yypv[1]);
			RC_WORD(0);
		} break;
  case 102:	{
			/* initialize Format structure */
			Format.data = FALSE;
			Format.length = 0;
			Format.scale = A_SCALE;
			Format.str_len = 0;
			Format.str_adr = 0;
		} break;
  case 109:	{
			Format.length = (int) (yyval = yypv[2]);
		} break;
  case 110:	{
			Format.scale = *((double *) yypv[2]);
		} break;
  case 111:	{
			Format.scale = *((long *) yypv[2]);
		} break;
  case 112:	{
			Format.scale = ((int) yypv[2]);
		} break;
  case 114:	{
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
		} break;
  case 119:	{
			if (((int) yypv[2]) > MAX_I1)
				rc_error(22, 0, MAX_I1);
			else
				Format.before = ((int) yypv[2]);
		} break;
  case 120:	{
			if (((int) yypv[2]) > MAX_I1)
				rc_error(22, 0, MAX_I1);
			else
				Format.point = ((int) yypv[2]);
		} break;
  case 122:	{
			Format.ignore |= B_IGNORE;
		} break;
  case 123:	{
			Format.norm = ((int) yypv[2]);
		} break;
  case 125:	{
			if (((int) yypv[1]) < L_NORM || ((int) yypv[1]) > H_NORM)
				rc_error(22, L_NORM, H_NORM);
			else if (((int) yypv[1]) < 0)
			{
				Format.ignore |= B_NORM;
				yyval = (char *) (- ((int) yypv[1]));
			}
		} break;
  case 127:	{
			/* initialze Format structure */
			Format.data = FALSE;
			Format.length = 0;
			Format.fr_to[0] = (char *) A_FROM;
			Format.fr_to[1] = (char *) 0;
			Format.batch_len = A_BATCH;
		} break;
  case 134:	{
			Format.fr_to[(int) yypv[1]] = yypv[2];
		} break;
  case 135:	{
			/* initialize Format structure */
			Format.data = FALSE;
			Format.length = 0;
			Format.fr_to_v[0] = 0;
			Format.fr_to_v[1] = 0;
			Format.fr_to_8[0] = 0;
			Format.fr_to_8[1] = 0;
			Format.str_len = sizeof (char);
			Format.str_adr = "*";
		} break;
  case 139:	{
			Format.fr_to_v[(int) yypv[1]] = (int) yypv[2];
		} break;
  case 140:	{
			Format.fr_to_8[(int) yypv[1]] = (int) yypv[2];
		} break;
  case 141:	{
			Format.fr_to_8[(int) yypv[1]] = *((long *) yypv[2]);
		} break;
  case 142:	{
			Format.fr_to_8[(int) yypv[1]] = *((double *) yypv[2]);
		} break;
  case 143:	{
			if (!Format.str_len)
				Format.str_adr = " ";
		} break;
  case 145:	{
			if (Format.data++)
				rc_error(20);
		} break;
  case 146:	{
			RC_BYTE(prCONST);
			RC_WORD((int) yypv[1]);
		} break;
  case 147:	{
			RC_SYS_VAR((int) yypv[1], TRUE);
		} break;
  case 149:	{
			switch ((int) yypv[1])
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
			RC_WORD((int) yypv[2]);
		} break;
  case 151:	{
			RC_BYTE((int) yypv[1]);
		} break;
  case 152:	{
			RC_BYTE((int) yypv[2]);
		} break;
  case 153:	{
			RC_BYTE((int) yypv[2]);
		} break;
  case 154:	{
			RC_BYTE((int) yypv[1]);
		} break;
  case 155:	{
			RC_BYTE(pushSWITCH);
			RC_WORD((int) yypv[1]);
		} break;
  case 156:	{
			RC_BYTE(pushLAST);
			RC_WORD((int) yypv[2]);
			RC_WORD(0);	/* from */
			RC_WORD(0);	/*  to  */
			RC_BYTE(pushVALUE);
			RC_WORD((int) yypv[2]);
			RC_WORD(0);	/* from */
			RC_WORD(0);	/*  to  */
			RC_BYTE(bdNE);
		} break;
  case 157:	{
			RC_BYTE(pushVALUE);
			RC_WORD((int) yypv[2]);
			RC_WORD(0);	/* from */
			RC_WORD(0);	/*  to  */
			RC_BYTE(pushNEXT);
			RC_WORD((int) yypv[2]);
			RC_WORD(0);	/* from */
			RC_WORD(0);	/*  to  */
			RC_BYTE(bdNE);
		} break;
  case 159:	{
			/* skip positive sign */
			if (((int) yypv[1]) == ((int) rcMIN))
				RC_BYTE(rcMIN);
		} break;
  case 160:	{
			RC_BYTE((int) yypv[2]);
		} break;
  case 161:	{
			if (((int) yypv[2]) == ((int) rcMIN))
				RC_BYTE(rcSUB);
			else
				RC_BYTE((int) yypv[2]);
		} break;
  case 162:	{
			if (!((int) yypv[1]))
				RC_BYTE(pushNULL);
			else if (((int) yypv[1]) <= MAX_I1 && ((int) yypv[1]) >= MIN_I1)
			{
				RC_BYTE(push1INT);
				RC_BYTE((int) yypv[1]);
			}
			else
			{
				RC_BYTE(push2INT);
				RC_WORD((int) yypv[1]);
			}
		} break;
  case 163:	{
			RC_BYTE(push4INT);
			RC_STR(yypv[1], sizeof (long));
		} break;
  case 164:	{
			RC_BYTE(pushREAL);
			RC_STR(yypv[1], sizeof (double));
		} break;
  case 165:	{
			RC_BYTE(pushSTRING);
			RC_WORD(Format.str_len);
			RC_STR(yypv[1], Format.str_len);
		} break;
  case 166:	{
			RC_BYTE(pushCONST);
			RC_BYTE((int) yypv[1]);
		} break;
  case 167:	{
			RC_SYS_VAR((int) yypv[1], FALSE);
		} break;
  case 168:	{
			RC_WORD((int) Format.fr_to[0]);
			RC_WORD((int) Format.fr_to[1]);
		} break;
  case 176:	{
			RC_BYTE((int) yypv[1]);
			RC_WORD((int) yypv[2]);
			/* initialize Format structure */
			Format.fr_to[0] = (char *) 0;
			Format.fr_to[1] = (char *) 0;
		} break;
  case 178:	{
			yyval = (char *) SYS_PAGE;
		} break;
  case 179:	{
			yyval = (char *) SYS_LINE;
		} break;
  case 180:	{
			yyval = (char *) SYS_ALL_LINE;
		} break;
  case 181:	{
			yyval = (char *) SYS_COL;
		} break;
  case 182:	{
			yyval = (char *) SYS_TUPL;
		} break;
  case 183:	{
			if (((int) yypv[3]) < 0 || ((int) yypv[3]) >= MAX_VAR)
				rc_error(6, yypv[3]);
			else
				yyval = (char *) (((int) yypv[3]) | VAR_VAR_MASK);
		} break;
  case 184:	{
			if (((int) yypv[2]) < 0 || ((int) yypv[2]) >= MAX_VAR)
				rc_error(6, yypv[2]);
			else
				yyval = (char *) (((int) yypv[2])| VAR_MASK);
		} break;
  case 185:	{
			if (((int) yypv[2]) >= MAX_BVAR)
				rc_error(7, yypv[2]);
			else
				yyval = yypv[2];
		} break;
  case 186:	{
			if (((int) yypv[1]) < 0)
			{
				rc_error(8, yypv[1]);
				yyval = 0;
			}
		} break;
  case 189:	{
			yyval = yypv[1] + 1;
		} break;
  case 207:	{
			RC_BYTE(rcONE);
		} break;
  case 208:	{
			RC_BYTE(rcALL);
			yyval = R_rc;
		} break;
}
return (0);
}
int		yyerrval = 256;



# include	"rc_tab.y"

short	yyact[] =
{
	0,	4352,	8195,	4362,	8196,	4391,	8197,	0
,	4096,	16384,	0,	4423,	8198,	0,	12290,	4391
,	8199,	0,	12292,	4364,	8201,	12294,	12291,	4419
,	8203,	12296,	4423,	8204,	0,	4354,	8205,	0
,	4352,	8214,	4353,	8209,	4355,	8212,	4360,	8210
,	4362,	8213,	4381,	8208,	4413,	8211,	0,	12293
,	4352,	8235,	4356,	8242,	4357,	8246,	4358,	8236
,	4359,	8231,	4361,	8237,	4377,	8256,	4382,	8229
,	4383,	8228,	4384,	8230,	4385,	8247,	4389,	8241
,	4390,	8245,	4394,	8255,	4395,	8232,	4397,	8248
,	4401,	8257,	4403,	8261,	4404,	8263,	4405,	8262
,	4406,	8259,	4407,	8260,	4411,	8227,	4425,	8264
,	12319,	4352,	8214,	4353,	8209,	4355,	8212,	4360
,	8210,	4362,	8213,	4381,	8208,	4413,	8211,	12295
,	12297,	4421,	8266,	0,	4381,	8267,	0,	4421
,	8268,	0,	4388,	8269,	4399,	8270,	0,	12304
,	4364,	8272,	4423,	8271,	0,	12308,	4374,	8276
,	0,	4352,	8235,	4356,	8242,	4357,	8246,	4358
,	8236,	4359,	8231,	4361,	8237,	4377,	8256,	4382
,	8229,	4383,	8228,	4384,	8230,	4385,	8247,	4389
,	8241,	4390,	8245,	4394,	8255,	4395,	8232,	4397
,	8248,	4401,	8257,	4403,	8261,	4404,	8263,	4405
,	8262,	4406,	8259,	4407,	8260,	4411,	8227,	4425
,	8264,	12318,	12321,	12322,	12324,	4398,	8281,	0
,	4363,	8284,	0,	4400,	8288,	4421,	8286,	0
,	4363,	8291,	0,	12330,	12331,	4400,	8288,	4421
,	8286,	12334,	4381,	8294,	4388,	8295,	0,	12336
,	12340,	12341,	12342,	4396,	8299,	0,	4379,	8301
,	12346,	4353,	8316,	4360,	8317,	4364,	8320,	4381
,	8315,	4388,	8314,	4399,	8318,	4400,	8288,	4421
,	8305,	4422,	8306,	4424,	8307,	4425,	8264,	4426
,	8309,	4427,	8303,	4431,	8312,	0,	4421,	8323
,	0,	4389,	8324,	4421,	8323,	0,	4353,	8316
,	4360,	8317,	4364,	8320,	4375,	8332,	4376,	8333
,	4381,	8315,	4388,	8314,	4396,	8299,	4399,	8318
,	4400,	8288,	4420,	8330,	4421,	8305,	4422,	8306
,	4424,	8307,	4425,	8264,	4426,	8309,	4427,	8327
,	4431,	8312,	4436,	8328,	0,	12366,	4400,	8288
,	0,	4373,	8336,	0,	4425,	8337,	0,	4400
,	8288,	4409,	8340,	4421,	8286,	12380,	4353,	8316
,	4360,	8317,	4364,	8354,	4380,	8347,	4381,	8315
,	4388,	8314,	4399,	8318,	4400,	8288,	4416,	8349
,	4418,	8348,	4426,	8351,	0,	4353,	8316,	4360
,	8317,	4364,	8354,	4380,	8347,	4381,	8315,	4388
,	8314,	4399,	8318,	4400,	8288,	4410,	8359,	4412
,	8361,	4414,	8362,	4415,	8363,	4417,	8360,	4418
,	8348,	4426,	8351,	0,	4353,	8316,	4360,	8317
,	4364,	8354,	4380,	8347,	4381,	8315,	4388,	8314
,	4399,	8318,	4400,	8288,	4409,	8340,	4413,	8371
,	4426,	8351,	0,	4353,	8316,	4360,	8317,	4364
,	8354,	4380,	8347,	4381,	8315,	4388,	8314,	4399
,	8318,	4400,	8288,	4413,	8375,	4419,	8376,	4426
,	8351,	0,	4353,	8316,	4360,	8317,	4364,	8354
,	4380,	8347,	4381,	8315,	4388,	8314,	4399,	8318
,	4400,	8288,	4426,	8351,	0,	4353,	8316,	4360
,	8317,	4381,	8315,	4388,	8314,	4396,	8299,	4399
,	8318,	4400,	8288,	0,	12357,	12363,	12381,	12390
,	12402,	12415,	12423,	4402,	8382,	4408,	8383,	12478
,	12382,	12298,	12299,	4421,	8384,	0,	12301,	4421
,	8385,	0,	4421,	8386,	0,	4379,	8387,	0
,	4421,	8388,	4423,	8389,	0,	4374,	8395,	0
,	4353,	8397,	0,	12320,	4392,	8398,	0,	4365
,	8400,	12361,	12359,	4366,	8404,	4372,	8403,	0
,	12365,	12474,	12475,	4400,	8407,	4421,	8408,	0
,	4363,	8291,	4387,	8411,	12374,	12370,	4366,	8415
,	4370,	8414,	0,	12332,	4400,	8288,	4421,	8286
,	12335,	12337,	12338,	12339,	12343,	12344,	4431,	8420
,	4432,	8419,	12347,	12450,	12451,	12452,	12453,	12454
,	12455,	12456,	12460,	12465,	12466,	12467,	4381,	8423
,	0,	12469,	12470,	4413,	8371,	12463,	4431,	8420
,	4432,	8419,	12348,	12352,	12355,	12354,	4437,	8427
,	12356,	4353,	8316,	4360,	8317,	4364,	8320,	4375
,	8332,	4376,	8333,	4381,	8315,	4388,	8314,	4396
,	8299,	4399,	8318,	4400,	8288,	4420,	8330,	4421
,	8305,	4422,	8306,	4424,	8307,	4425,	8264,	4426
,	8309,	4427,	8327,	4431,	8312,	4436,	8328,	0
,	4379,	8433,	4431,	8420,	4432,	8419,	4434,	8434
,	4435,	8432,	0,	12442,	12443,	4437,	8427,	12362
,	4378,	8438,	12369,	4363,	8440,	12480,	4409,	8441
,	12377,	12378,	12379,	4353,	8316,	4360,	8317,	4364
,	8354,	4380,	8347,	4381,	8315,	4388,	8314,	4399
,	8318,	4400,	8288,	4416,	8349,	4418,	8348,	4426
,	8351,	12384,	12391,	12393,	12394,	12395,	12396,	4421
,	8447,	4422,	8446,	4424,	8445,	0,	4425,	8264
,	0,	12433,	12434,	12435,	12436,	4353,	8316,	4360
,	8317,	4364,	8354,	4380,	8347,	4381,	8315,	4388
,	8314,	4399,	8318,	4400,	8288,	4410,	8359,	4412
,	8361,	4414,	8362,	4415,	8363,	4417,	8360,	4418
,	8348,	4426,	8351,	12385,	12403,	12405,	12406,	12410
,	4400,	8288,	4421,	8455,	0,	12412,	4353,	8316
,	4360,	8317,	4364,	8354,	4380,	8347,	4381,	8315
,	4388,	8314,	4399,	8318,	4400,	8288,	4409,	8340
,	4413,	8371,	4426,	8351,	12386,	12416,	12418,	12419
,	12420,	12421,	4353,	8316,	4360,	8317,	4364,	8354
,	4380,	8347,	4381,	8315,	4388,	8314,	4399,	8318
,	4400,	8288,	4413,	8375,	4419,	8376,	4426,	8351
,	12387,	12424,	12426,	4400,	8288,	4421,	8461,	4422
,	8462,	4424,	8463,	0,	12432,	4353,	8316,	4360
,	8317,	4364,	8354,	4381,	8315,	4388,	8314,	4399
,	8318,	4400,	8288,	4426,	8351,	0,	12389,	12349
,	4379,	8467,	4433,	8466,	0,	12476,	12477,	12300
,	12302,	12303,	4421,	8468,	0,	4379,	8469,	0
,	4379,	8470,	0,	4393,	8471,	0,	4366,	8475
,	4367,	8474,	4374,	8395,	0,	12314,	4386,	8479
,	12498,	12313,	12323,	4366,	8483,	4369,	8482,	0
,	12358,	12326,	12493,	4401,	8485,	0,	12364,	4366
,	8488,	4371,	8487,	0,	4421,	8489,	0,	12472
,	4366,	8492,	4368,	8491,	0,	12371,	12372,	12329
,	12489,	12490,	12333,	12473,	4437,	8427,	12345,	4428
,	8496,	4431,	8420,	4432,	8419,	0,	12447,	12468
,	4413,	8371,	12462,	12464,	12353,	4428,	8499,	4437
,	8427,	0,	4379,	8433,	4428,	8496,	4431,	8420
,	4432,	8419,	4434,	8434,	4435,	8432,	0,	12439
,	12457,	12458,	12459,	12444,	12445,	12367,	12368,	12375
,	12479,	12383,	12392,	12397,	12398,	12399,	12400,	12401
,	12437,	12404,	12407,	12408,	12409,	12411,	12413,	12414
,	12417,	12422,	12425,	12427,	12428,	12429,	12430,	12431
,	12388,	12350,	12351,	12305,	4403,	8505,	4406,	8503
,	4407,	8504,	0,	12289,	12312,	12315,	12483,	4353
,	8507,	0,	12316,	4366,	8510,	4370,	8509,	0
,	12497,	12496,	12325,	12487,	4377,	8512,	0,	12360
,	12494,	12327,	12491,	4390,	8513,	0,	12471,	12328
,	12485,	4357,	8514,	0,	12373,	12448,	4432,	8419
,	12449,	12446,	12461,	12440,	12438,	4431,	8420,	4432
,	8419,	12441,	12376,	4421,	8515,	0,	12309,	12310
,	12311,	4421,	8516,	0,	12484,	12317,	12481,	12495
,	12488,	12492,	12486,	12306,	12307,	12482,	-1
};

short	yypact[] =
{
	0,	1,	8,	11,	14,	15,	18,	19
,	22,	23,	26,	29,	32,	47,	48,	97
,	112,	113,	116,	119,	122,	127,	128,	133
,	134,	137,	186,	187,	48,	188,	189,	192
,	195,	200,	48,	203,	204,	205,	210,	215
,	195,	195,	216,	217,	218,	219,	219,	222
,	225,	225,	254,	257,	262,	262,	301,	302
,	305,	308,	311,	318,	341,	372,	395,	418
,	437,	452,	453,	454,	455,	456,	457,	458
,	459,	464,	465,	466,	467,	470,	471,	474
,	477,	480,	48,	485,	48,	488,	491,	492
,	495,	48,	498,	499,	48,	504,	48,	505
,	506,	507,	512,	517,	48,	518,	523,	524
,	529,	530,	531,	532,	195,	533,	262,	534
,	225,	225,	539,	540,	541,	542,	543,	544
,	545,	546,	547,	548,	549,	550,	553,	554
,	555,	195,	558,	563,	564,	254,	565,	566
,	569,	262,	608,	619,	620,	195,	195,	621
,	624,	627,	630,	633,	634,	195,	635,	658
,	659,	660,	661,	662,	195,	663,	670,	673
,	674,	675,	676,	195,	677,	708,	709,	710
,	195,	195,	670,	711,	712,	717,	718,	741
,	742,	743,	744,	745,	195,	746,	769,	770
,	771,	670,	780,	781,	798,	799,	800,	805
,	806,	807,	808,	809,	810,	813,	816,	819
,	822,	829,	48,	48,	830,	833,	627,	834
,	835,	48,	840,	841,	842,	843,	846,	847
,	852,	855,	856,	861,	48,	862,	863,	864
,	865,	866,	867,	868,	225,	225,	871,	878
,	879,	880,	883,	884,	262,	885,	890,	903
,	225,	904,	905,	906,	907,	908,	909,	910
,	911,	912,	195,	913,	914,	915,	916,	917
,	918,	919,	920,	921,	922,	923,	924,	925
,	926,	927,	928,	929,	930,	931,	932,	933
,	934,	935,	936,	937,	938,	939,	940,	940
,	947,	948,	949,	950,	951,	954,	955,	627
,	960,	961,	962,	963,	964,	967,	968,	969
,	970,	971,	974,	975,	976,	977,	980,	981
,	982,	985,	986,	987,	988,	989,	994,	995
,	998,	999,	1000,	1001,	1004,	1005,	1006,	830
,	1007,	1008,	1009,	1010,	1011,	1012,	1013,	-1
};

short	yyr1[] =
{
	0,	1,	1,	2,	2,	3,	3,	4
,	4,	7,	7,	8,	8,	8,	8,	8
,	8,	8,	8,	8,	8,	9,	9,	9
,	6,	10,	11,	11,	14,	15,	5,	5
,	18,	18,	19,	19,	19,	19,	19,	19
,	19,	19,	19,	19,	19,	19,	19,	19
,	19,	19,	19,	19,	19,	19,	19,	20
,	20,	20,	20,	20,	20,	43,	45,	47
,	21,	22,	22,	49,	23,	50,	24,	51
,	25,	25,	27,	52,	28,	53,	30,	33
,	55,	55,	34,	34,	56,	35,	35,	37
,	39,	39,	40,	40,	40,	58,	60,	59
,	41,	41,	41,	41,	41,	41,	61,	62
,	62,	72,	72,	72,	72,	70,	73,	73
,	73,	74,	63,	64,	64,	76,	76,	76
,	76,	76,	76,	76,	76,	77,	77,	65
,	66,	66,	78,	78,	78,	78,	79,	67
,	68,	68,	80,	80,	80,	80,	80,	80
,	80,	75,	71,	71,	71,	81,	44,	44
,	44,	44,	44,	44,	44,	44,	46,	46
,	46,	46,	46,	46,	46,	46,	46,	46
,	46,	82,	82,	82,	83,	84,	84,	84
,	85,	48,	48,	48,	48,	48,	48,	54
,	54,	42,	31,	31,	69,	69,	69,	57
,	57,	17,	17,	12,	12,	36,	36,	26
,	26,	38,	38,	32,	32,	29,	29,	16
,	13,	86,	86,	-1
};

short	yyr2[] =
{
	0,	9,	1,	2,	1,	2,	0,	2
,	0,	1,	2,	2,	3,	2,	3,	3
,	1,	4,	6,	6,	1,	1,	1,	1
,	3,	2,	1,	2,	2,	3,	1,	0
,	2,	1,	1,	3,	1,	4,	3,	4
,	4,	3,	1,	1,	2,	3,	1,	2
,	1,	2,	2,	2,	1,	1,	1,	2
,	2,	3,	1,	2,	2,	2,	3,	3
,	2,	3,	2,	1,	2,	1,	2,	1
,	2,	0,	2,	1,	2,	1,	1,	3
,	1,	0,	1,	2,	2,	2,	0,	3
,	4,	2,	2,	2,	1,	1,	1,	2
,	2,	2,	2,	2,	3,	2,	1,	1
,	2,	1,	1,	1,	1,	2,	2,	2
,	2,	2,	1,	1,	2,	1,	1,	2
,	2,	2,	1,	2,	1,	1,	1,	1
,	1,	2,	1,	1,	1,	1,	2,	1
,	1,	2,	1,	2,	2,	2,	2,	2
,	1,	1,	1,	1,	1,	2,	3,	2
,	3,	3,	1,	1,	2,	2,	3,	2
,	3,	3,	1,	1,	1,	1,	1,	1
,	1,	1,	1,	1,	1,	3,	2,	1
,	2,	1,	1,	1,	2,	1,	1,	3
,	2,	2,	1,	1,	2,	2,	1,	1
,	0,	1,	2,	1,	2,	1,	2,	1
,	2,	1,	1,	1,	2,	1,	2,	3
,	3,	1,	0,	-1
};

short	yygo[] =
{
	0,	-1,	1,	-1,	2,	-1,	8,	-1
,	10,	27,	86,	33,	100,	81,	198,	83
,	204,	88,	209,	91,	213,	93,	214,	99
,	220,	201,	284,	202,	285,	208,	292,	219
,	301,	-1,	23,	-1,	81,	-1,	14,	14
,	73,	-1,	15,	278,	314,	-1,	310,	-1
,	82,	-1,	199,	-1,	280,	-1,	83,	199
,	281,	-1,	200,	-1,	201,	-1,	202,	-1
,	316,	-1,	24,	24,	85,	-1,	25,	-1
,	26,	-1,	27,	-1,	28,	-1,	29,	-1
,	87,	-1,	207,	-1,	289,	-1,	30,	-1
,	90,	-1,	210,	-1,	31,	36,	101,	39
,	104,	40,	105,	57,	147,	102,	224,	107
,	225,	128,	233,	140,	243,	141,	244,	148
,	250,	155,	252,	162,	257,	167,	259,	168
,	260,	179,	266,	249,	309,	-1,	93,	-1
,	294,	-1,	32,	-1,	97,	-1,	217,	-1
,	298,	-1,	33,	-1,	221,	-1,	34,	-1
,	41,	-1,	42,	44,	106,	45,	108,	63
,	188,	-1,	139,	-1,	46,	52,	142,	109
,	226,	135,	236,	136,	238,	235,	306,	-1
,	134,	-1,	47,	47,	110,	48,	129,	111
,	229,	112,	230,	135,	237,	227,	302,	228
,	303,	239,	308,	-1,	137,	-1,	48,	58
,	160,	59,	160,	60,	160,	61,	160,	62
,	160,	63,	189,	149,	160,	163,	160,	173
,	160,	180,	160,	186,	160,	-1,	118,	50
,	133,	132,	234,	-1,	130,	-1,	51,	-1
,	88,	-1,	52,	-1,	91,	31,	95,	36
,	95,	39,	95,	40,	95,	54,	143,	57
,	95,	102,	95,	107,	95,	128,	95,	140
,	95,	141,	95,	148,	95,	155,	95,	162
,	95,	167,	95,	168,	95,	171,	264,	179
,	95,	183,	268,	249,	95,	-1,	121,	-1
,	245,	97,	218,	-1,	98,	205,	288,	286
,	319,	-1,	247,	-1,	57,	57,	146,	-1
,	177,	47,	116,	48,	116,	51,	116,	52
,	116,	109,	116,	111,	116,	112,	116,	135
,	116,	136,	116,	157,	256,	169,	261,	184
,	272,	227,	116,	228,	116,	235,	116,	239
,	116,	-1,	66,	-1,	58,	-1,	149,	-1
,	59,	-1,	163,	-1,	60,	-1,	173,	-1
,	61,	-1,	180,	-1,	62,	59,	165,	60
,	175,	61,	182,	62,	186,	163,	165,	173
,	175,	180,	182,	-1,	151,	62,	187,	186
,	273,	-1,	158,	149,	251,	-1,	150,	59
,	166,	163,	166,	-1,	152,	-1,	153,	59
,	172,	60,	178,	61,	185,	163,	172,	173
,	178,	180,	185,	-1,	154,	163,	258,	-1
,	164,	-1,	262,	173,	265,	-1,	174,	127
,	232,	232,	305,	-1,	176,	180,	267,	-1
,	181,	-1,	161,	-1,	239,	-1,	112,	-1
,	119,	-1,	127,	318,	325,	-1,	286,	-1
};

short	yypgo[] =
{
	0,	1,	3,	5,	7,	9,	35,	37
,	39,	43,	47,	49,	51,	53,	55,	59
,	61,	63,	65,	67,	71,	73,	75,	77
,	79,	81,	83,	85,	87,	89,	91,	93
,	127,	129,	131,	133,	135,	137,	139,	141
,	143,	145,	147,	155,	157,	169,	171,	189
,	191,	215,	221,	223,	225,	227,	229,	271
,	273,	277,	283,	285,	289,	323,	325,	327
,	329,	331,	333,	335,	337,	339,	341,	357
,	363,	367,	373,	375,	389,	393,	395,	399
,	405,	409,	411,	413,	415,	417,	419,	-1
};
