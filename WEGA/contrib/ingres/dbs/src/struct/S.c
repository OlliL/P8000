# include	<stdio.h>
# include	"../conf/gen.h"
# include	"../h/ing_db.h"
# include	"../h/symbol.h"
# include	"../h/tree.h"

struct tab_of_op
{
	int	no_of_op;
	char	*name_of_op;
};

struct tab_of_op	Tab_of_op[] =
{
	  1,	"ACK",			/* symbol.h	*/
	  2,	"AGHEAD",		/* symbol.h	*/
	  3,	"AND",			/* symbol.h	*/
	  4,	"AND_SIZ",		/* tree.h	*/
	  5,	"AOP",			/* symbol.h	*/
	  6,	"AOP_SIZ",		/* tree.h	*/
	  7,	"BADCOP",		/* symbol.h	*/
	  8,	"BINARY",		/* symbol.h	*/
	  9,	"BOP",			/* symbol.h	*/
	 10,	"BYHEAD",		/* symbol.h	*/
	 11,	"CHANGESTRAT",		/* symbol.h	*/
	 12,	"CHAR",			/* symbol.h	*/
	 13,	"CID",			/* symbol.h	*/
	 14,	"COP",			/* symbol.h	*/
	 15,	"EMPTY",		/* symbol.h	*/
	 16,	"EOTUP",		/* symbol.h	*/
	 17,	"EXIT",			/* symbol.h	*/
	 18,	"FLOAT",		/* symbol.h	*/
	 19,	"INT",			/* symbol.h	*/
	 20,	"NOACK",		/* symbol.h	*/
	 21,	"NONEMPTY",		/* symbol.h	*/
	 22,	"NOUPDATE",		/* symbol.h	*/
	 23,	"OP_SIZ",		/* tree.h	*/
	 24,	"OR",			/* symbol.h	*/
	 25,	"PAT_ANY",		/* symbol.h	*/
	 26,	"PAT_LBRAC",		/* symbol.h	*/
	 27,	"PAT_ONE",		/* symbol.h	*/
	 28,	"PAT_RBRAC",		/* symbol.h	*/
	 29,	"QLEND",		/* symbol.h	*/
	 30,	"QMODE",		/* symbol.h	*/
	 31,	"QT_HDR_SIZ",		/* tree.h	*/
	 32,	"QT_PTR_SIZ",		/* tree.h	*/
	 33,	"REOPENRES",		/* symbol.h	*/
	 34,	"RESDOM",		/* symbol.h	*/
	 35,	"RESULTID",		/* symbol.h	*/
	 36,	"RESULTVAR",		/* symbol.h	*/
	 37,	"RES_SIZ",		/* tree.h	*/
	 38,	"RETVAL",		/* symbol.h	*/
	 39,	"ROOT",			/* symbol.h	*/
	 40,	"ROOT_SIZ",		/* tree.h	*/
	 41,	"RUBACK",		/* symbol.h	*/
	 42,	"SITE",			/* symbol.h	*/
	 43,	"SOURCEID",		/* symbol.h	*/
	 44,	"SRC_SIZ",		/* tree.h	*/
	 45,	"SVAR_SIZ",		/* tree.h	*/
	 46,	"SYM_HDR_SIZ",		/* tree.h	*/
	 47,	"S_VAR",		/* symbol.h	*/
	 48,	"TREE",			/* symbol.h	*/
	 49,	"TYP_LEN_SIZ",		/* tree.h	*/
	 50,	"UOP",			/* symbol.h	*/
	 51,	"UPDATE",		/* symbol.h	*/
	 52,	"USERQRY",		/* symbol.h	*/
	 53,	"VAR",			/* symbol.h	*/
	 54,	"VAR_HDR_SIZ",		/* tree.h	*/
	 55,	"VAR_SIZ",		/* tree.h	*/
	 56,	"mdAPP",		/* symbol.h	*/
	 57,	"mdCOPY",		/* symbol.h	*/
	 58,	"mdCREATE",		/* symbol.h	*/
	 59,	"mdDCREATE",		/* symbol.h	*/
	 60,	"mdDEFINE",		/* symbol.h	*/
	 61,	"mdDEL",		/* symbol.h	*/
	 62,	"mdDESTROY",		/* symbol.h	*/
	 63,	"mdDISPLAY",		/* symbol.h	*/
	 64,	"mdDISTRIB",		/* symbol.h	*/
	 65,	"mdHELP",		/* symbol.h	*/
	 66,	"mdINDEX",		/* symbol.h	*/
	 67,	"mdINTEG",		/* symbol.h	*/
	 68,	"mdMODIFY",		/* symbol.h	*/
	 69,	"mdMOVEREL",		/* symbol.h	*/
	 70,	"mdNETQRY",		/* symbol.h	*/
	 71,	"mdPRINT",		/* symbol.h	*/
	 72,	"mdPROT",		/* symbol.h	*/
	 73,	"mdRANGE",		/* symbol.h	*/
	 74,	"mdREMQM",		/* symbol.h	*/
	 75,	"mdREPL",		/* symbol.h	*/
	 76,	"mdRESETREL",		/* symbol.h	*/
	 77,	"mdRETR",		/* symbol.h	*/
	 78,	"mdRETTERM",		/* symbol.h	*/
	 79,	"mdRET_UNI",		/* symbol.h	*/
	 80,	"mdSAVE",		/* symbol.h	*/
	 81,	"mdUPDATE",		/* symbol.h	*/
	 82,	"mdVIEW",		/* symbol.h	*/
	 83,	"mdWAITQRY",		/* symbol.h	*/
	 84,	"opABS",		/* symbol.h	*/
	 85,	"opADD",		/* symbol.h	*/
	 86,	"opANY",		/* symbol.h	*/
	 87,	"opASCII",		/* symbol.h	*/
	 88,	"opATAN",		/* symbol.h	*/
	 89,	"opAVG",		/* symbol.h	*/
	 90,	"opAVGU",		/* symbol.h	*/
	 91,	"opCONCAT",		/* symbol.h	*/
	 92,	"opCOS",		/* symbol.h	*/
	 93,	"opCOUNT",		/* symbol.h	*/
	 94,	"opCOUNTU",		/* symbol.h	*/
	 95,	"opDATE",		/* symbol.h	*/
	 96,	"opDBA",		/* symbol.h	*/
	 97,	"opDIV",		/* symbol.h	*/
	 98,	"opEQ",			/* symbol.h	*/
	 99,	"opEXP",		/* symbol.h	*/
	100,	"opFLOAT4",		/* symbol.h	*/
	101,	"opFLOAT8",		/* symbol.h	*/
	102,	"opGAMMA",		/* symbol.h	*/
	103,	"opGE",			/* symbol.h	*/
	104,	"opGT",			/* symbol.h	*/
	105,	"opINT1",		/* symbol.h	*/
	106,	"opINT2",		/* symbol.h	*/
	107,	"opINT4",		/* symbol.h	*/
	108,	"opLE",			/* symbol.h	*/
	109,	"opLOG",		/* symbol.h	*/
	110,	"opLT",			/* symbol.h	*/
	111,	"opMAX",		/* symbol.h	*/
	112,	"opMIN",		/* symbol.h	*/
	113,	"opMINUS",		/* symbol.h	*/
	114,	"opMOD",		/* symbol.h	*/
	115,	"opMUL",		/* symbol.h	*/
	116,	"opNE",			/* symbol.h	*/
	117,	"opNOT",		/* symbol.h	*/
	118,	"opPLUS",		/* symbol.h	*/
	119,	"opPOW",		/* symbol.h	*/
	120,	"opSIN",		/* symbol.h	*/
	121,	"opSQRT",		/* symbol.h	*/
	122,	"opSUB",		/* symbol.h	*/
	123,	"opSUM",		/* symbol.h	*/
	124,	"opSUMU",		/* symbol.h	*/
	125,	"opTIME",		/* symbol.h	*/
	126,	"opUSERCODE",		/* symbol.h	*/
};
int	No_of_ops =	sizeof Tab_of_op / sizeof Tab_of_op[0];

main(argc, argv)
int	argc;
char	*argv[];
{
	register int		i;
	char			buf[BUFSIZ];

	setbuf(stdout, buf);
	if (argc == 1)
		for (i = 1; i <= No_of_ops; i++)
			const(i);
	else
		for (i = 1; i < argc; i++)
			const(name(argv[i]));
}


name(chr)
char	*chr;
{
	register int		hi;
	register int		lo;
	register int		curr;

	/* is it a keyword ? */
	lo = 0;
	hi = No_of_ops - 1;
	while (lo <= hi)
	{
		curr = (lo + hi) / 2;
		switch (AAscompare(Tab_of_op[curr].name_of_op, 50, chr, 50))
		{
		  case 1:
			hi = curr - 1;
			continue;

		  case -1:
			lo = curr + 1;
			continue;

		  case 0:
			return (Tab_of_op[curr].no_of_op);
		}
	}
	return (No_of_ops + 1);
}


AAscompare(ap, al, bp, bl)
register char	*ap;
register int	al;
register char	*bp;
register int	bl;
{
	register int	a;
	register int	b;

	while (1)
	{
		/* supress blanks in both strings */
		a = al? *ap: 0;
		b = bl? *bp: 0;

		/* do inequality tests */
		if (a < b)
			return (-1);
		if (a > b)
			return (1);
		if (!a)
			return (0);

		/* go on to the next character */
		ap++;
		al--;
		bp++;
		bl--;
	}
}


const(c)
register int	c;
{
	switch (c)
	{
	  case   1:
		printf("h/symbol.h\tACK\t\t%6d\n", ACK);
		break;

	  case   2:
		printf("h/symbol.h\tAGHEAD\t\t%6d\n", AGHEAD);
		break;

	  case   3:
		printf("h/symbol.h\tAND\t\t%6d\n", AND);
		break;

	  case   4:
		printf("h/tree.h\tAND_SIZ\t\t%6d\n", AND_SIZ);
		break;

	  case   5:
		printf("h/symbol.h\tAOP\t\t%6d\n", AOP);
		break;

	  case   6:
		printf("h/tree.h\tAOP_SIZ\t\t%6d\n", AOP_SIZ);
		break;

	  case   7:
		printf("h/symbol.h\tBADCOP\t\t%6d\n", BADCOP);
		break;

	  case   8:
		printf("h/symbol.h\tBINARY\t\t%6d\n", BINARY);
		break;

	  case   9:
		printf("h/symbol.h\tBOP\t\t%6d\n", BOP);
		break;

	  case  10:
		printf("h/symbol.h\tBYHEAD\t\t%6d\n", BYHEAD);
		break;

	  case  11:
		printf("h/symbol.h\tCHANGESTRAT\t%6d\n", CHANGESTRAT);
		break;

	  case  12:
		printf("h/symbol.h\tCHAR\t\t%6d\n", CHAR);
		break;

	  case  13:
		printf("h/symbol.h\tCID\t\t%6d\n", CID);
		break;

	  case  14:
		printf("h/symbol.h\tCOP\t\t%6d\n", COP);
		break;

	  case  15:
		printf("h/symbol.h\tEMPTY\t\t%6d\n", EMPTY);
		break;

	  case  16:
		printf("h/symbol.h\tEOTUP\t\t%6d\n", EOTUP);
		break;

	  case  17:
		printf("h/symbol.h\tEXIT\t\t%6d\n", EXIT);
		break;

	  case  18:
		printf("h/symbol.h\tFLOAT\t\t%6d\n", FLOAT);
		break;

	  case  19:
		printf("h/symbol.h\tINT\t\t%6d\n", INT);
		break;

	  case  20:
		printf("h/symbol.h\tNOACK\t\t%6d\n", NOACK);
		break;

	  case  21:
		printf("h/symbol.h\tNONEMPTY\t%6d\n", NONEMPTY);
		break;

	  case  22:
		printf("h/symbol.h\tNOUPDATE\t%6d\n", NOUPDATE);
		break;

	  case  23:
		printf("h/tree.h\tOP_SIZ\t\t%6d\n", OP_SIZ);
		break;

	  case  24:
		printf("h/symbol.h\tOR\t\t%6d\n", OR);
		break;

	  case  25:
		printf("h/symbol.h\tPAT_ANY\t\t%6d\n", PAT_ANY);
		break;

	  case  26:
		printf("h/symbol.h\tPAT_LBRAC\t%6d\n", PAT_LBRAC);
		break;

	  case  27:
		printf("h/symbol.h\tPAT_ONE\t\t%6d\n", PAT_ONE);
		break;

	  case  28:
		printf("h/symbol.h\tPAT_RBRAC\t%6d\n", PAT_RBRAC);
		break;

	  case  29:
		printf("h/symbol.h\tQLEND\t\t%6d\n", QLEND);
		break;

	  case  30:
		printf("h/symbol.h\tQMODE\t\t%6d\n", QMODE);
		break;

	  case  31:
		printf("h/tree.h\tQT_HDR_SIZ\t%6d\n", QT_HDR_SIZ);
		break;

	  case  32:
		printf("h/tree.h\tQT_PTR_SIZ\t%6d\n", QT_PTR_SIZ);
		break;

	  case  33:
		printf("h/symbol.h\tREOPENRES\t%6d\n", REOPENRES);
		break;

	  case  34:
		printf("h/symbol.h\tRESDOM\t\t%6d\n", RESDOM);
		break;

	  case  35:
		printf("h/symbol.h\tRESULTID\t%6d\n", RESULTID);
		break;

	  case  36:
		printf("h/symbol.h\tRESULTVAR\t%6d\n", RESULTVAR);
		break;

	  case  37:
		printf("h/tree.h\tRES_SIZ\t\t%6d\n", RES_SIZ);
		break;

	  case  38:
		printf("h/symbol.h\tRETVAL\t\t%6d\n", RETVAL);
		break;

	  case  39:
		printf("h/symbol.h\tROOT\t\t%6d\n", ROOT);
		break;

	  case  40:
		printf("h/tree.h\tROOT_SIZ\t%6d\n", ROOT_SIZ);
		break;

	  case  41:
		printf("h/symbol.h\tRUBACK\t\t%6d\n", RUBACK);
		break;

	  case  42:
		printf("h/symbol.h\tSITE\t\t%6d\n", SITE);
		break;

	  case  43:
		printf("h/symbol.h\tSOURCEID\t%6d\n", SOURCEID);
		break;

	  case  44:
		printf("h/tree.h\tSRC_SIZ\t\t%6d\n", SRC_SIZ);
		break;

	  case  45:
		printf("h/tree.h\tSVAR_SIZ\t%6d\n", SVAR_SIZ);
		break;

	  case  46:
		printf("h/tree.h\tSYM_HDR_SIZ\t%6d\n", SYM_HDR_SIZ);
		break;

	  case  47:
		printf("h/symbol.h\tS_VAR\t\t%6d\n", S_VAR);
		break;

	  case  48:
		printf("h/symbol.h\tTREE\t\t%6d\n", TREE);
		break;

	  case  49:
		printf("h/tree.h\tTYP_LEN_SIZ\t%6d\n", TYP_LEN_SIZ);
		break;

	  case  50:
		printf("h/symbol.h\tUOP\t\t%6d\n", UOP);
		break;

	  case  51:
		printf("h/symbol.h\tUPDATE\t\t%6d\n", UPDATE);
		break;

	  case  52:
		printf("h/symbol.h\tUSERQRY\t\t%6d\n", USERQRY);
		break;

	  case  53:
		printf("h/symbol.h\tVAR\t\t%6d\n", VAR);
		break;

	  case  54:
		printf("h/tree.h\tVAR_HDR_SIZ\t%6d\n", VAR_HDR_SIZ);
		break;

	  case  55:
		printf("h/tree.h\tVAR_SIZ\t\t%6d\n", VAR_SIZ);
		break;

	  case  56:
		printf("h/symbol.h\tmdAPP\t\t%6d\n", mdAPP);
		break;

	  case  57:
		printf("h/symbol.h\tmdCOPY\t\t%6d\n", mdCOPY);
		break;

	  case  58:
		printf("h/symbol.h\tmdCREATE\t%6d\n", mdCREATE);
		break;

	  case  59:
		printf("h/symbol.h\tmdDCREATE\t%6d\n", mdDCREATE);
		break;

	  case  60:
		printf("h/symbol.h\tmdDEFINE\t%6d\n", mdDEFINE);
		break;

	  case  61:
		printf("h/symbol.h\tmdDEL\t\t%6d\n", mdDEL);
		break;

	  case  62:
		printf("h/symbol.h\tmdDESTROY\t%6d\n", mdDESTROY);
		break;

	  case  63:
		printf("h/symbol.h\tmdDISPLAY\t%6d\n", mdDISPLAY);
		break;

	  case  64:
		printf("h/symbol.h\tmdDISTRIB\t%6d\n", mdDISTRIB);
		break;

	  case  65:
		printf("h/symbol.h\tmdHELP\t\t%6d\n", mdHELP);
		break;

	  case  66:
		printf("h/symbol.h\tmdINDEX\t\t%6d\n", mdINDEX);
		break;

	  case  67:
		printf("h/symbol.h\tmdINTEG\t\t%6d\n", mdINTEG);
		break;

	  case  68:
		printf("h/symbol.h\tmdMODIFY\t%6d\n", mdMODIFY);
		break;

	  case  69:
		printf("h/symbol.h\tmdMOVEREL\t%6d\n", mdMOVEREL);
		break;

	  case  70:
		printf("h/symbol.h\tmdNETQRY\t%6d\n", mdNETQRY);
		break;

	  case  71:
		printf("h/symbol.h\tmdPRINT\t\t%6d\n", mdPRINT);
		break;

	  case  72:
		printf("h/symbol.h\tmdPROT\t\t%6d\n", mdPROT);
		break;

	  case  73:
		printf("h/symbol.h\tmdRANGE\t\t%6d\n", mdRANGE);
		break;

	  case  74:
		printf("h/symbol.h\tmdREMQM\t\t%6d\n", mdREMQM);
		break;

	  case  75:
		printf("h/symbol.h\tmdREPL\t\t%6d\n", mdREPL);
		break;

	  case  76:
		printf("h/symbol.h\tmdRESETREL\t%6d\n", mdRESETREL);
		break;

	  case  77:
		printf("h/symbol.h\tmdRETR\t\t%6d\n", mdRETR);
		break;

	  case  78:
		printf("h/symbol.h\tmdRETTERM\t%6d\n", mdRETTERM);
		break;

	  case  79:
		printf("h/symbol.h\tmdRET_UNI\t%6d\n", mdRET_UNI);
		break;

	  case  80:
		printf("h/symbol.h\tmdSAVE\t\t%6d\n", mdSAVE);
		break;

	  case  81:
		printf("h/symbol.h\tmdUPDATE\t%6d\n", mdUPDATE);
		break;

	  case  82:
		printf("h/symbol.h\tmdVIEW\t\t%6d\n", mdVIEW);
		break;

	  case  83:
		printf("h/symbol.h\tmdWAITQRY\t%6d\n", mdWAITQRY);
		break;

	  case  84:
		printf("h/symbol.h\topABS\t\t%6d\n", opABS);
		break;

	  case  85:
		printf("h/symbol.h\topADD\t\t%6d\n", opADD);
		break;

	  case  86:
		printf("h/symbol.h\topANY\t\t%6d\n", opANY);
		break;

	  case  87:
		printf("h/symbol.h\topASCII\t\t%6d\n", opASCII);
		break;

	  case  88:
		printf("h/symbol.h\topATAN\t\t%6d\n", opATAN);
		break;

	  case  89:
		printf("h/symbol.h\topAVG\t\t%6d\n", opAVG);
		break;

	  case  90:
		printf("h/symbol.h\topAVGU\t\t%6d\n", opAVGU);
		break;

	  case  91:
		printf("h/symbol.h\topCONCAT\t%6d\n", opCONCAT);
		break;

	  case  92:
		printf("h/symbol.h\topCOS\t\t%6d\n", opCOS);
		break;

	  case  93:
		printf("h/symbol.h\topCOUNT\t\t%6d\n", opCOUNT);
		break;

	  case  94:
		printf("h/symbol.h\topCOUNTU\t%6d\n", opCOUNTU);
		break;

	  case  95:
		printf("h/symbol.h\topDATE\t\t%6d\n", opDATE);
		break;

	  case  96:
		printf("h/symbol.h\topDBA\t\t%6d\n", opDBA);
		break;

	  case  97:
		printf("h/symbol.h\topDIV\t\t%6d\n", opDIV);
		break;

	  case  98:
		printf("h/symbol.h\topEQ\t\t%6d\n", opEQ);
		break;

	  case  99:
		printf("h/symbol.h\topEXP\t\t%6d\n", opEXP);
		break;

	  case 100:
		printf("h/symbol.h\topFLOAT4\t%6d\n", opFLOAT4);
		break;

	  case 101:
		printf("h/symbol.h\topFLOAT8\t%6d\n", opFLOAT8);
		break;

	  case 102:
		printf("h/symbol.h\topGAMMA\t\t%6d\n", opGAMMA);
		break;

	  case 103:
		printf("h/symbol.h\topGE\t\t%6d\n", opGE);
		break;

	  case 104:
		printf("h/symbol.h\topGT\t\t%6d\n", opGT);
		break;

	  case 105:
		printf("h/symbol.h\topINT1\t\t%6d\n", opINT1);
		break;

	  case 106:
		printf("h/symbol.h\topINT2\t\t%6d\n", opINT2);
		break;

	  case 107:
		printf("h/symbol.h\topINT4\t\t%6d\n", opINT4);
		break;

	  case 108:
		printf("h/symbol.h\topLE\t\t%6d\n", opLE);
		break;

	  case 109:
		printf("h/symbol.h\topLOG\t\t%6d\n", opLOG);
		break;

	  case 110:
		printf("h/symbol.h\topLT\t\t%6d\n", opLT);
		break;

	  case 111:
		printf("h/symbol.h\topMAX\t\t%6d\n", opMAX);
		break;

	  case 112:
		printf("h/symbol.h\topMIN\t\t%6d\n", opMIN);
		break;

	  case 113:
		printf("h/symbol.h\topMINUS\t\t%6d\n", opMINUS);
		break;

	  case 114:
		printf("h/symbol.h\topMOD\t\t%6d\n", opMOD);
		break;

	  case 115:
		printf("h/symbol.h\topMUL\t\t%6d\n", opMUL);
		break;

	  case 116:
		printf("h/symbol.h\topNE\t\t%6d\n", opNE);
		break;

	  case 117:
		printf("h/symbol.h\topNOT\t\t%6d\n", opNOT);
		break;

	  case 118:
		printf("h/symbol.h\topPLUS\t\t%6d\n", opPLUS);
		break;

	  case 119:
		printf("h/symbol.h\topPOW\t\t%6d\n", opPOW);
		break;

	  case 120:
		printf("h/symbol.h\topSIN\t\t%6d\n", opSIN);
		break;

	  case 121:
		printf("h/symbol.h\topSQRT\t\t%6d\n", opSQRT);
		break;

	  case 122:
		printf("h/symbol.h\topSUB\t\t%6d\n", opSUB);
		break;

	  case 123:
		printf("h/symbol.h\topSUM\t\t%6d\n", opSUM);
		break;

	  case 124:
		printf("h/symbol.h\topSUMU\t\t%6d\n", opSUMU);
		break;

	  case 125:
		printf("h/symbol.h\topTIME\t\t%6d\n", opTIME);
		break;

	  case 126:
		printf("h/symbol.h\topUSERCODE\t%6d\n", opUSERCODE);
		break;

	  default:
		printf("illegal SYMBOL\n");
	}
}
