/*
** ---  RP.C ------------ R_CODE PRINTER  -------------------------------------
**
**	Version: @(#)rp.c		4.0	02/05/89
**
*/

# include	<stdio.h>
# include	"r_code.h"
# ifdef MSDOS
# include	<fcntl.h>
# endif


static short		Count;
static short		Length;
static short		Return;
static FILE		*Input;


bytes(rc, len)
register char	*rc;
register int	len;
{
	register int		c;

	Count += len;
	while (--len >= 0)
	{
		if ((c = getc(Input)) < 0)
		{
			printf("\nUNEXPECTED *** EOF ***\n");
			fflush(stdout);
			exit(1);
		}
		*rc++ = c;
	}
}


byte()
{
	char			c;

	bytes(&c, sizeof (char));
	return (ctou(c));
}


word()
{
	register  int		w;

	w = byte() << BYTE_LOG2;
	return (w | byte());
}


i2_4(msg)
register char	*msg;
{
	int		i;

	bytes(&i, sizeof (int));
	printf(" %s=%d", msg, Length = i);
}


i4()
{
	long			i;

	bytes(&i, sizeof (long));
	printf(" long=%ld", i);
}


f8(msg)
register char	*msg;
{
	double	i;

	bytes(&i, sizeof (double));
	printf(" %s=%10.3f", msg, i);
}


c0(msg)
register char	*msg;
{
	char			i[MAX_FIELD + 1];

	bytes(i, Length);
	i[Length] = '\0';
	printf(" %s=|%s|", msg, i);
}


var(msg)
register char	*msg;
{
	register int		v;
	register int		var;

	printf(" %s", msg);
	v = word();
	var = v & VAR_MASK;
	if ((v & VAR_VAR_MASK) == VAR_VAR_MASK)
		printf(".var$var");
	else if (var)
		printf(".var");
	printf("=%d", Length = v & VAR_VALUE);
	return (var);
}


main(argc, argv)
int	argc;
char	**argv;
{
# ifdef SETBUF
	char			buf[BUFSIZ];

	setbuf(stdout, buf);
# endif

	PQR_system = "r_code decoder";
	PQR();

	if (argc == 1)
		*argv = "r_code";
	else
		argv++;

# ifdef MSDOS
	Input = fopen(*argv, "rb");
# else
	Input = fopen(*argv, "r");
# endif
	if (!Input)
	{
		printf("Cannot open *** %s ***\n", *argv);
		fflush(stdout);
		exit(2);
	}

	printf("R_CODE HEADER:\n");
	printf("--------------\n");
	Length = sizeof R_res - 1;
	c0("R_reserved");
	putchar('\n');
	i2_4("R_cols");
	putchar('\n');
	i2_4("R_lines");
	putchar('\n');
	i2_4("R_all_lines");
	putchar('\n');
	i2_4("R_for_all");
	putchar('\n');
	i2_4("R_first_page");
	putchar('\n');
	i2_4("R_last_page");
	putchar('\n');
	i2_4("R_from_tuple");
	putchar('\n');
	i2_4("R_to_tuple");
	printf("\n R_big=");
	if (byte())
		printf("TRUE");
	else
		printf("FALSE");

	printf("\n\nR_CODE: file %s\n-------", *argv);
	Count = 1;
	interp();
	printf("\nUNEXPECTED R_CODE FORMAT\n");
}


interp()
{
	register int		code;
	register int		i;
	register int		rc;

	for ( ; ; )
	{
		fflush(stdout);
		printf("\n%4d\t", Count);
		printf("%4d\t", code = byte());
		switch (code)
		{
		  case bdLT:
			printf("bdLT");
			continue;

		  case bdEQ:
			printf("bdEQ");
			continue;

		  case bdLE:
			printf("bdLE");
			continue;

		  case bdGT:
			printf("bdGT");
			continue;

		  case bdNE:
			printf("bdNE");
			continue;

		  case bdGE:
			printf("bdGE");
			continue;

		  case pushFALSE:
			printf("pushFALSE");
			continue;

		  case pushTRUE:
			printf("pushTRUE");
			continue;

		  case pushNULL:
			printf("pushNULL");
			continue;

		  case push1INT:
			printf("push1INT integer=%d", ctoi(byte()));
			continue;

		  case push2INT:
			printf("push2INT");
			printf(" integer=%d", Length = word());
			continue;

		  case push4INT:
			printf("push4INT");
			i4();
			continue;

		  case pushREAL:
			printf("pushREAL");
			f8("real");
			continue;

		  case pushSTRING:
			printf("pushSTRING");
			var("length");
			c0("string");
			continue;

		  case pushCONST:
			printf("pushCONST = ");
konst:
			switch (code = byte())
			{
			  case SECOND:
				printf("SECOND");
				break;

			  case MINUTE:
				printf("MINUTE");
				break;

			  case HOUR:
				printf("HOUR");
				break;

			  case M_DAY:
				printf("M_DAY");
				break;

			  case MONTH:
				printf("MONTH");
				break;

			  case YEAR:
				printf("YEAR");
				break;

			  case W_DAY:
				printf("W_DAY");
				break;

			  case Y_DAY:
				printf("Y_DAY");
				break;

			  case IS_DST:
				printf("IS_DST");
				break;

			  default:
				printf(" unknown CONST %d", code);
			}
			continue;

		  case pushCOL:
			printf("pushCOL");
			continue;

		  case pushLINE:
			printf("pushLINE");
			continue;

		  case pushALLINES:
			printf("pushALLINES");
			continue;

		  case pushPAGE:
			printf("pushPAGE");
			continue;

		  case pushTUPLE:
			printf("pushTUPLE");
			continue;

		  case pushLAST:
			printf("pushLAST");
push_tuple:
			var("dom");
			var("from");
			var("to");
			continue;

		  case pushVALUE:
			printf("pushVALUE");
			goto push_tuple;

		  case pushNEXT:
			printf("pushNEXT");
			goto push_tuple;

		  case pushVAR:
			printf("pushVAR");
			var("#");
			continue;

		  case pushSWITCH:
			printf("pushSWITCH");
			var("#");
			continue;

		  case rcNOT:
			printf("rcNOT");
			continue;

		  case rcOR:
			printf("rcOR");
			continue;

		  case rcAND:
			printf("rcAND");
			continue;

		  case rcEND_SET:
			printf("rcEND_SET");
			continue;

		  case rcMIN:
			printf("rcMIN");
			continue;

		  case rcADD:
			printf("rcADD");
			continue;

		  case rcSUB:
			printf("rcSUB");
			continue;

		  case rcMUL:
			printf("rcMUL");
			continue;

		  case rcDIV:
			printf("rcDIV");
			continue;

		  case rcDIV_R:
			printf("rcDIV_R");
			continue;

		  case rcMOD:
			printf("rcMOD");
			continue;

		  case rcRC_END:
			printf("rcRC_END\n");
			fflush(stdout);
			exit(0);

		  case rcRETURN:
			printf("rcRETURN");
			if (Return)
			{
				--Return;
				return;
			}
			continue;

		  case rcRTTS:
			printf("rcRTTS");
			continue;

		  case rcEOTUP:
			printf("rcEOTUP");
			continue;

		  case rcALL:
			printf("rcALL");
			continue;

		  case rcONE:
			printf("rcONE");
			continue;

		  case rcDATE:
			printf("rcDATE");
			continue;

		  case rcTIME:
			printf("rcTIME");
			continue;

		  case prCOL:
			printf("prCOL");
			continue;

		  case prLINE:
			printf("prLINE");
			continue;

		  case prALLINES:
			printf("prALLINES");
			continue;

		  case prNEWPAGE:
			printf("prNEWPAGE");
			continue;

		  case prPAGE:
			printf("prPAGE");
			continue;

		  case prTUPLE:
			printf("prTUPLE");
			continue;

		  case prVAR:
			printf("prVAR");
			var("#");
			continue;

		  case prCONST:
			printf("prCONST = ");
			goto konst;

		  case prSPACE:
			printf("prSPACE");
			var("#");
			continue;

		  case prNEWLINE:
			printf("prNEWLINE");
			var("#");
			continue;

		  case prNEWCOL:
			printf("prNEWCOL");
			var("col");
			continue;

		  case prLAST:
			printf("prLAST");
print_tuple:
			var("dom");
			continue;

		  case prVALUE:
			printf("prVALUE");
			goto print_tuple;

		  case prNEXT:
			printf("prNEXT");
			goto print_tuple;

		  case fmtB_BIT:
			printf("fmtB_BIT");
format:
			var("length");
			continue;

		  case fmtW_BIT:
			printf("fmtW_BIT");
			goto format;

		  case fmtB_OCT:
			printf("fmtB_OCT");
			goto format;

		  case fmtW_OCT:
			printf("fmtW_OCT");
			goto format;

		  case fmtB_DEC:
			printf("fmtB_DEC");
			goto format;

		  case fmtW_DEC:
			printf("fmtW_DEC");
			goto format;

		  case fmtB_HEX:
			printf("fmtB_HEX");
			goto format;

		  case fmtW_HEX:
			printf("fmtW_HEX");
			goto format;

		  case prSTRING:
			printf("prSTRING");
			goto input;

		  case prINPUT:
			printf("prINPUT");
input:
			var("length");
			code = Length;
			var("batch_length");
			Length = code;
			c0("string");
			continue;

		  case fmtINT:
			printf("fmtINT");
			var("length");
			f8("scale");
			printf(" pic_length=%d", Length = word());
			c0("pic_string");
			continue;

		  case fmtREAL:
			printf("fmtREAL");
			var("length");
			f8("scale");
			var("before");
			var("point");
			var("ignore");
			var("norm");
			printf(" exp_length=%d", Length = word());
			c0("exp_string");
			continue;

		  case fmtCHAR:
			printf("fmtCHAR");
			var("length");
			var("from");
			var("to");
			var("batch_length");
			continue;

		  case fmtGRAPH:
			printf("fmtGRAPH");
			var("length");
			if (!var("from"))
				f8("value");
			if (!var("to"))
				f8("value");
			printf(" with char=%c", byte());
			continue;

		  case rcCASE:
			printf("rcCASE");
			var("#");
			i = Length;
			printf(" jumptable %d", Count + word());
			for (code = 0; code < i; code++)
			{
				printf("\n\tCASE %d CODE", code);
				Return++;
				interp();
			}
			printf("\n\tOUT CODE");
			Return++;
			interp();
			printf("\n\tJUMP_TABLE");
			rc = Count;
			for (code = 0; code < i; code++)
			{
				printf("\n%4d\t", Count);
				printf("CASE %d on %d", code, rc + word());
			}
			printf("\n%4d\t", Count);
			printf("OUT on %d", rc + word());
			continue;

		  case rcREPEAT:
			printf("rcREPEAT");
			var("#");
			printf(" jump to %d", Count + word());
			continue;

		  case onBATCH:
			printf("onBATCH");
jump_over:
			printf(" jump over to %d", Count + word());
			Return++;
			interp();
			continue;

		  case onB_LINE:
			printf("onB_LINE");
			goto jump_over;

		  case onE_LINE:
			printf("onE_LINE");
			goto jump_over;

		  case onB_PAGE:
			printf("onB_PAGE");
			goto jump_over;

		  case onE_PAGE:
			printf("onE_PAGE");
			goto jump_over;

		  case rcJUMP:
			printf("rcJUMP");
			printf(" to %d", Count + word());
			continue;

		  case rcCALL:
			printf("rcCALL");
			printf(" procedure on %d", Count + word());
			continue;

		  case setVAR:
			printf("setVAR");
expr_end:
			printf(" end of expression on %d", Count + word());
			continue;

		  case setSWITCH:
			printf("setSWITCH");
			goto expr_end;

		  case rcIF:
			printf("rcIF");
			printf(" then on %d", Count + word());
			continue;

		  case rcTHEN:
			printf("rcTHEN");
			printf(" else on %d", Count + word());
			continue;

		  default:
			printf("unknown code %d", code);
		}
	}
}
