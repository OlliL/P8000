# include	<stdio.h>
# include	"../conf/gen.h"
# include	"../h/ing_db.h"
# include	"../h/aux.h"
# include	"../h/catalog.h"
# include	"../h/access.h"
# include	"../h/batch.h"
# include	"../h/pipes.h"
# include	"../h/lock.h"
# include	"../h/scanner.h"
# include	"../h/version.h"
# include	"../h/unix.h"

struct tab_of_op
{
	int	no_of_op;
	char	*name_of_op;
};

struct tab_of_op	Tab_of_op[] =
{
	  1,	"ALPHA",		/* scanner.h	*/
	  2,	"AMATTR_ERR",		/* access.h	*/
	  3,	"AMCLOSE_ERR",		/* access.h	*/
	  4,	"AMFIND_ERR",		/* access.h	*/
	  5,	"AMINVL_ERR",		/* access.h	*/
	  6,	"AMNOATTS_ERR",		/* access.h	*/
	  7,	"AMNOFILE_ERR",		/* access.h	*/
	  8,	"AMOPNVIEW_ERR",	/* access.h	*/
	  9,	"AMREAD_ERR",		/* access.h	*/
	 10,	"AMREL_ERR",		/* access.h	*/
	 11,	"AMSEEK_ERR",		/* access.h	*/
	 12,	"AMWRITE_ERR",		/* access.h	*/
	 13,	"ATTID",		/* ing_db.h	*/
	 14,	"ATTNAME",		/* ing_db.h	*/
	 15,	"ATTOWNER",		/* ing_db.h	*/
	 16,	"ATTRELID",		/* ing_db.h	*/
	 17,	"A_ABT",		/* lock.h	*/
	 18,	"A_DBCONCUR",		/* access.h	*/
	 19,	"A_NEWFMT",		/* access.h	*/
	 20,	"A_QRYMOD",		/* access.h	*/
	 21,	"A_RLS1",		/* lock.h	*/
	 22,	"A_RLSA",		/* lock.h	*/
	 23,	"A_RTN",		/* lock.h	*/
	 24,	"A_SLP",		/* lock.h	*/
	 25,	"BADOP",		/* scanner.h	*/
	 26,	"BATCHSIZE",		/* batch.h	*/
	 27,	"BLKSIZ",		/* access.h	*/
	 28,	"BUF_DIRECT",		/* access.h	*/
	 29,	"BUF_DIRTY",		/* access.h	*/
	 30,	"BUF_LOCKED",		/* access.h	*/
	 31,	"CNTRL",		/* scanner.h	*/
	 32,	"CNTRLCHR",		/* scanner.h	*/
	 33,	"COMMTERM",		/* scanner.h	*/
	 34,	"COREBUFSIZE",		/* access.h	*/
	 35,	"CS_LOCK",		/* lock.h	*/
	 36,	"CVAR_F8",		/* scanner.h	*/
	 37,	"CVAR_I2",		/* scanner.h	*/
	 38,	"CVAR_I4",		/* scanner.h	*/
	 39,	"CVAR_S",		/* scanner.h	*/
	 40,	"DB_LOCK",		/* lock.h	*/
	 41,	"ERRDELIM",		/* conf/gen.h	*/
	 42,	"ERR_STAT",		/* pipes.h	*/
	 43,	"EXACTKEY",		/* ing_db.h	*/
	 44,	"EXEC_DBU",		/* aux.h	*/
	 45,	"EXEC_DECOMP",		/* aux.h	*/
	 46,	"EXEC_ERROR",		/* aux.h	*/
	 47,	"EXEC_OVQP",		/* aux.h	*/
	 48,	"EXEC_PARSER",		/* aux.h	*/
	 49,	"EXEC_QRYMOD",		/* aux.h	*/
	 50,	"FALSE",		/* ing_db.h	*/
	 51,	"FCONSTERR",		/* scanner.h	*/
	 52,	"FILEMODE",		/* aux.h	*/
	 53,	"FULLKEY",		/* ing_db.h	*/
	 54,	"GOVAL",		/* scanner.h	*/
	 55,	"HDRSIZ",		/* pipes.h	*/
	 56,	"HRANGEKEY",		/* ing_db.h	*/
	 57,	"I1MASK",		/* ing_db.h	*/
	 58,	"IDSIZE",		/* batch.h	*/
	 59,	"ING_RSIZE",		/* lock.h	*/
	 60,	"ING_SSIZE",		/* lock.h	*/
	 61,	"ING_UID",		/* conf/conf.h	*/
	 62,	"ING_WSIZE",		/* lock.h	*/
	 63,	"INTRELID",		/* catalog.h	*/
	 64,	"INTRELOWNER",		/* catalog.h	*/
	 65,	"INTTREE",		/* catalog.h	*/
	 66,	"IOWNERP",		/* catalog.h	*/
	 67,	"IRELIDI",		/* catalog.h	*/
	 68,	"IRELIDP",		/* catalog.h	*/
	 69,	"LAST_STAT",		/* pipes.h	*/
	 70,	"LOCKS",		/* conf/conf.h	*/
	 71,	"LOG2WORDSIZE",		/* aux.h	*/
	 72,	"LRANGEKEY",		/* ing_db.h	*/
	 73,	"MACHINE",		/* conf/conf.h	*/
	 74,	"MAXAGG",		/* ing_db.h	*/
	 75,	"MAXDOM",		/* ing_db.h	*/
	 76,	"MAXFIELD",		/* aux.h	*/
	 77,	"MAXFILES",		/* unix.h	*/
	 78,	"MAXKEYS",		/* ing_db.h	*/
	 79,	"MAXLINE",		/* aux.h	*/
	 80,	"MAXNAME",		/* ing_db.h	*/
	 81,	"MAXPARMS",		/* ing_db.h	*/
	 82,	"MAXSTRING",		/* scanner.h	*/
	 83,	"MAXTID",		/* access.h	*/
	 84,	"MAXTUP",		/* ing_db.h	*/
	 85,	"MAXVAR",		/* ing_db.h	*/
	 86,	"MERGESIZE",		/* access.h	*/
	 87,	"MINTUP",		/* ing_db.h	*/
	 88,	"MODTID",		/* access.h	*/
	 89,	"MOD_DATE",		/* version.h	*/
	 90,	"M_BTREE",		/* access.h	*/
	 91,	"M_DB",			/* lock.h	*/
	 92,	"M_ERROR",		/* lock.h	*/
	 93,	"M_EXCL",		/* lock.h	*/
	 94,	"M_HASH",		/* access.h	*/
	 95,	"M_HEAP",		/* access.h	*/
	 96,	"M_ISAM",		/* access.h	*/
	 97,	"M_MODE",		/* lock.h	*/
	 98,	"M_ON",			/* lock.h	*/
	 99,	"M_REL",		/* lock.h	*/
	100,	"M_SHARE",		/* lock.h	*/
	101,	"M_TA",			/* lock.h	*/
	102,	"M_TE",			/* lock.h	*/
	103,	"M_TRUNC",		/* access.h	*/
	104,	"NACCBUFS",		/* access.h	*/
	105,	"NAMELONG",		/* scanner.h	*/
	106,	"NOKEY",		/* ing_db.h	*/
	107,	"NORM_STAT",		/* pipes.h	*/
	108,	"NO_REL",		/* lock.h	*/
	109,	"NULL",			/* ing_db.h	*/
	110,	"NUMBR",		/* scanner.h	*/
	111,	"NUMBUFOFLO",		/* scanner.h	*/
	112,	"OPATR",		/* scanner.h	*/
	113,	"PBUFSIZ",		/* pipes.h	*/
	114,	"PGBLKS",		/* conf/conf.h	*/
	115,	"PGHDR",		/* access.h	*/
	116,	"PGPTRSIZ",		/* access.h	*/
	117,	"PGSIZE",		/* access.h	*/
	118,	"PROPERMID",		/* catalog.h	*/
	119,	"PRORELID",		/* catalog.h	*/
	120,	"PRORELOWN",		/* catalog.h	*/
	121,	"PROTERM",		/* catalog.h	*/
	122,	"PROTREE",		/* catalog.h	*/
	123,	"PRO_AGGR",		/* catalog.h	*/
	124,	"PRO_APP",		/* catalog.h	*/
	125,	"PRO_DEL",		/* catalog.h	*/
	126,	"PRO_REPL",		/* catalog.h	*/
	127,	"PRO_RETR",		/* catalog.h	*/
	128,	"PRO_TEST",		/* catalog.h	*/
	129,	"PUNCT",		/* scanner.h	*/
	130,	"P_END",		/* pipes.h	*/
	131,	"P_EXECID",		/* pipes.h	*/
	132,	"P_FLUSH",		/* pipes.h	*/
	133,	"P_FUNCID",		/* pipes.h	*/
	134,	"P_INT",		/* pipes.h	*/
	135,	"P_NORM",		/* pipes.h	*/
	136,	"P_PARAM",		/* pipes.h	*/
	137,	"P_PRIME",		/* pipes.h	*/
	138,	"P_SYNC",		/* pipes.h	*/
	139,	"P_WRITE",		/* pipes.h	*/
	140,	"REFOWNER",		/* catalog.h	*/
	141,	"REFRELID",		/* catalog.h	*/
	142,	"REFTYPE",		/* catalog.h	*/
	143,	"RELID",		/* ing_db.h	*/
	144,	"RELOWNER",		/* ing_db.h	*/
	145,	"REL_DATE",		/* version.h	*/
	146,	"REL_LOCK",		/* lock.h	*/
	147,	"SBUFOFLO",		/* scanner.h	*/
	148,	"SBUFSIZ",		/* scanner.h	*/
	149,	"SCANTID",		/* access.h	*/
	150,	"SECBASE",		/* catalog.h	*/
	151,	"SECINDEX",		/* catalog.h	*/
	152,	"STACKSIZ",		/* ing_db.h	*/
	153,	"STRLONG",		/* scanner.h	*/
	154,	"STRTERM",		/* scanner.h	*/
	155,	"SYMERR",		/* scanner.h	*/
	156,	"SYNC_STAT",		/* pipes.h	*/
	157,	"S_BINARY",		/* aux.h	*/
	158,	"S_CATALOG",		/* aux.h	*/
	159,	"S_CONCUR",		/* aux.h	*/
	160,	"S_DISCOPY",		/* aux.h	*/
	161,	"S_DISCRIT",		/* aux.h	*/
	162,	"S_DISTRIBUTED",	/* aux.h	*/
	163,	"S_INDEX",		/* aux.h	*/
	164,	"S_INTEG",		/* aux.h	*/
	165,	"S_NOUPDT",		/* aux.h	*/
	166,	"S_PROTALL",		/* aux.h	*/
	167,	"S_PROTRET",		/* aux.h	*/
	168,	"S_PROTUPS",		/* aux.h	*/
	169,	"S_VBASE",		/* aux.h	*/
	170,	"S_VIEW",		/* aux.h	*/
	171,	"TREEID",		/* catalog.h	*/
	172,	"TREEOWNER",		/* catalog.h	*/
	173,	"TREERELID",		/* catalog.h	*/
	174,	"TREESEQ",		/* catalog.h	*/
	175,	"TREETYPE",		/* catalog.h	*/
	176,	"TRUE",			/* ing_db.h	*/
	177,	"TUP_BUF_SIZ",		/* access.h	*/
	178,	"UF_DBLIST",		/* aux.h	*/
	179,	"UF_FLAGS",		/* aux.h	*/
	180,	"UF_GID",		/* aux.h	*/
	181,	"UF_IFILE",		/* aux.h	*/
	182,	"UF_NAME",		/* aux.h	*/
	183,	"UF_NFIELDS",		/* aux.h	*/
	184,	"UF_PTAB",		/* aux.h	*/
	185,	"UF_STAT",		/* aux.h	*/
	186,	"UF_UCODE",		/* aux.h	*/
	187,	"UF_UID",		/* aux.h	*/
	188,	"USERING_DB",		/* unix.h	*/
	189,	"USE_LOCK",		/* lock.h	*/
	190,	"U_APROCTAB",		/* aux.h	*/
	191,	"U_CREATDB",		/* aux.h	*/
	192,	"U_DRCTUPDT",		/* aux.h	*/
	193,	"U_EPROCTAB",		/* aux.h	*/
	194,	"U_QRYMODOFF",		/* aux.h	*/
	195,	"U_SUPER",		/* aux.h	*/
	196,	"U_TRACE",		/* aux.h	*/
	197,	"U_UPSYSCAT",		/* aux.h	*/
	198,	"VERSION",		/* version.h	*/
	199,	"WORDSIZE",		/* aux.h	*/
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
		printf("h/scanner.h\tALPHA\t\t%6d\n", ALPHA);
		break;

	  case   2:
		printf("h/access.h\tAMATTR_ERR\t%6d\n", AMATTR_ERR);
		break;

	  case   3:
		printf("h/access.h\tAMCLOSE_ERR\t%6d\n", AMCLOSE_ERR);
		break;

	  case   4:
		printf("h/access.h\tAMFIND_ERR\t%6d\n", AMFIND_ERR);
		break;

	  case   5:
		printf("h/access.h\tAMINVL_ERR\t%6d\n", AMINVL_ERR);
		break;

	  case   6:
		printf("h/access.h\tAMNOATTS_ERR\t%6d\n", AMNOATTS_ERR);
		break;

	  case   7:
		printf("h/access.h\tAMNOFILE_ERR\t%6d\n", AMNOFILE_ERR);
		break;

	  case   8:
		printf("h/access.h\tAMOPNVIEW_ERR\t%6d\n", AMOPNVIEW_ERR);
		break;

	  case   9:
		printf("h/access.h\tAMREAD_ERR\t%6d\n", AMREAD_ERR);
		break;

	  case  10:
		printf("h/access.h\tAMREL_ERR\t%6d\n", AMREL_ERR);
		break;

	  case  11:
		printf("h/access.h\tAMSEEK_ERR\t%6d\n", AMSEEK_ERR);
		break;

	  case  12:
		printf("h/access.h\tAMWRITE_ERR\t%6d\n", AMWRITE_ERR);
		break;

	  case  13:
		printf("h/ing_db.h\tATTID\t\t%6d\n", ATTID);
		break;

	  case  14:
		printf("h/ing_db.h\tATTNAME\t\t%6d\n", ATTNAME);
		break;

	  case  15:
		printf("h/ing_db.h\tATTOWNER\t%6d\n", ATTOWNER);
		break;

	  case  16:
		printf("h/ing_db.h\tATTRELID\t%6d\n", ATTRELID);
		break;

	  case  17:
		printf("h/lock.h\tA_ABT\t\t%6d\n", A_ABT);
		break;

	  case  18:
		printf("h/access.h\tA_DBCONCUR\t%06o\n", A_DBCONCUR);
		break;

	  case  19:
		printf("h/access.h\tA_NEWFMT\t%06o\n", A_NEWFMT);
		break;

	  case  20:
		printf("h/access.h\tA_QRYMOD\t%06o\n", A_QRYMOD);
		break;

	  case  21:
		printf("h/lock.h\tA_RLS1\t\t%6d\n", A_RLS1);
		break;

	  case  22:
		printf("h/lock.h\tA_RLSA\t\t%6d\n", A_RLSA);
		break;

	  case  23:
		printf("h/lock.h\tA_RTN\t\t%6d\n", A_RTN);
		break;

	  case  24:
		printf("h/lock.h\tA_SLP\t\t%6d\n", A_SLP);
		break;

	  case  25:
		printf("h/scanner.h\tBADOP\t\t%6d\n", BADOP);
		break;

	  case  26:
		printf("h/batch.h\tBATCHSIZE\t%6d\n", BATCHSIZE);
		break;

	  case  27:
		printf("h/access.h\tBLKSIZ\t\t%6d\n", BLKSIZ);
		break;

	  case  28:
		printf("h/access.h\tBUF_DIRECT\t%06o\n", BUF_DIRECT);
		break;

	  case  29:
		printf("h/access.h\tBUF_DIRTY\t%06o\n", BUF_DIRTY);
		break;

	  case  30:
		printf("h/access.h\tBUF_LOCKED\t%06o\n", BUF_LOCKED);
		break;

	  case  31:
		printf("h/scanner.h\tCNTRL\t\t%6d\n", CNTRL);
		break;

	  case  32:
		printf("h/scanner.h\tCNTRLCHR\t%6d\n", CNTRLCHR);
		break;

	  case  33:
		printf("h/scanner.h\tCOMMTERM\t%6d\n", COMMTERM);
		break;

	  case  34:
		printf("h/access.h\tCOREBUFSIZE\t%6u\n", COREBUFSIZE);
		break;

	  case  35:
		printf("h/lock.h\tCS_LOCK\t\t%6ld\n", CS_LOCK);
		break;

	  case  36:
		printf("h/scanner.h\tCVAR_F8\t\t%06o\n", CVAR_F8);
		break;

	  case  37:
		printf("h/scanner.h\tCVAR_I2\t\t%06o\n", CVAR_I2);
		break;

	  case  38:
		printf("h/scanner.h\tCVAR_I4\t\t%06o\n", CVAR_I4);
		break;

	  case  39:
		printf("h/scanner.h\tCVAR_S\t\t%06o\n", CVAR_S);
		break;

	  case  40:
		printf("h/lock.h\tDB_LOCK\t\t%6ld\n", DB_LOCK);
		break;

	  case  41:
		printf("conf/gen.h\tERRDELIM\t     %c\n", ERRDELIM);
		break;

	  case  42:
		printf("h/pipes.h\tERR_STAT\t%6d\n", ERR_STAT);
		break;

	  case  43:
		printf("h/ing_db.h\tEXACTKEY\t%6d\n", EXACTKEY);
		break;

	  case  44:
		printf("h/aux.h\t\tEXEC_DBU\t     %c\n", EXEC_DBU);
		break;

	  case  45:
		printf("h/aux.h\t\tEXEC_DECOMP\t     %c\n", EXEC_DECOMP);
		break;

	  case  46:
		printf("h/aux.h\t\tEXEC_ERROR\t     %c\n", EXEC_ERROR);
		break;

	  case  47:
		printf("h/aux.h\t\tEXEC_OVQP\t     %c\n", EXEC_OVQP);
		break;

	  case  48:
		printf("h/aux.h\t\tEXEC_PARSER\t     %c\n", EXEC_PARSER);
		break;

	  case  49:
		printf("h/aux.h\t\tEXEC_QRYMOD\t     %c\n", EXEC_QRYMOD);
		break;

	  case  50:
		printf("h/ing_db.h\tFALSE\t\t%6d\n", FALSE);
		break;

	  case  51:
		printf("h/scanner.h\tFCONSTERR\t%6d\n", FCONSTERR);
		break;

	  case  52:
		printf("h/aux.h\t\tFILEMODE\t%6d\n", FILEMODE);
		break;

	  case  53:
		printf("h/ing_db.h\tFULLKEY\t\t%6d\n", FULLKEY);
		break;

	  case  54:
		printf("h/scanner.h\tGOVAL\t\t%6d\n", GOVAL);
		break;

	  case  55:
		printf("h/pipes.h\tHDRSIZ\t\t%6d\n", HDRSIZ);
		break;

	  case  56:
		printf("h/ing_db.h\tHRANGEKEY\t%6d\n", HRANGEKEY);
		break;

	  case  57:
		printf("h/ing_db.h\tI1MASK\t\t%06o\n", I1MASK);
		break;

	  case  58:
		printf("h/batch.h\tIDSIZE\t\t%6d\n", IDSIZE);
		break;

	  case  59:
		printf("h/lock.h\tING_RSIZE\t%6d\n", ING_RSIZE);
		break;

	  case  60:
		printf("h/lock.h\tING_SSIZE\t%6d\n", ING_SSIZE);
		break;

	  case  61:
		printf("conf/conf.h\tING_UID\t\t%6d\n", ING_UID);
		break;

	  case  62:
		printf("h/lock.h\tING_WSIZE\t%6d\n", ING_WSIZE);
		break;

	  case  63:
		printf("h/catalog.h\tINTRELID\t%6d\n", INTRELID);
		break;

	  case  64:
		printf("h/catalog.h\tINTRELOWNER\t%6d\n", INTRELOWNER);
		break;

	  case  65:
		printf("h/catalog.h\tINTTREE\t\t%6d\n", INTTREE);
		break;

	  case  66:
		printf("h/catalog.h\tIOWNERP\t\t%6d\n", IOWNERP);
		break;

	  case  67:
		printf("h/catalog.h\tIRELIDI\t\t%6d\n", IRELIDI);
		break;

	  case  68:
		printf("h/catalog.h\tIRELIDP\t\t%6d\n", IRELIDP);
		break;

	  case  69:
		printf("h/pipes.h\tLAST_STAT\t%6d\n", LAST_STAT);
		break;

	  case  70:
		printf("conf/conf.h\tLOCKS\t\t%6d\n", LOCKS);
		break;

	  case  71:
		printf("h/aux.h\t\tLOG2WORDSIZE\t%6d\n", LOG2WORDSIZE);
		break;

	  case  72:
		printf("h/ing_db.h\tLRANGEKEY\t%6d\n", LRANGEKEY);
		break;

	  case  73:
		printf("conf/conf.h\tMACHINE\t\t%s\n", MACHINE);
		break;

	  case  74:
		printf("h/ing_db.h\tMAXAGG\t\t%6d\n", MAXAGG);
		break;

	  case  75:
		printf("h/ing_db.h\tMAXDOM\t\t%6d\n", MAXDOM);
		break;

	  case  76:
		printf("h/aux.h\t\tMAXFIELD\t%6d\n", MAXFIELD);
		break;

	  case  77:
		printf("h/unix.h\tMAXFILES\t%6d\n", MAXFILES);
		break;

	  case  78:
		printf("h/ing_db.h\tMAXKEYS\t\t%6d\n", MAXKEYS);
		break;

	  case  79:
		printf("h/aux.h\t\tMAXLINE\t\t%6d\n", MAXLINE);
		break;

	  case  80:
		printf("h/ing_db.h\tMAXNAME\t\t%6d\n", MAXNAME);
		break;

	  case  81:
		printf("h/ing_db.h\tMAXPARMS\t%6d\n", MAXPARMS);
		break;

	  case  82:
		printf("h/scanner.h\tMAXSTRING\t%6d\n", MAXSTRING);
		break;

	  case  83:
		printf("h/access.h\tMAXTID\t\t%6d\n", MAXTID);
		break;

	  case  84:
		printf("h/ing_db.h\tMAXTUP\t\t%6d\n", MAXTUP);
		break;

	  case  85:
		printf("h/ing_db.h\tMAXVAR\t\t%6d\n", MAXVAR);
		break;

	  case  86:
		printf("h/access.h\tMERGESIZE\t%6d\n", MERGESIZE);
		break;

	  case  87:
		printf("h/ing_db.h\tMINTUP\t\t%6d\n", MINTUP);
		break;

	  case  88:
		printf("h/access.h\tMODTID\t\t%6d\n", MODTID);
		break;

	  case  89:
		printf("h/version.h\tMOD_DATE\t%s\n", MOD_DATE);
		break;

	  case  90:
		printf("h/access.h\tM_BTREE\t\t%6d\n", M_BTREE);
		break;

	  case  91:
		printf("h/lock.h\tM_DB\t\t%06o\n", M_DB);
		break;

	  case  92:
		printf("h/lock.h\tM_ERROR\t\t%06o\n", M_ERROR);
		break;

	  case  93:
		printf("h/lock.h\tM_EXCL\t\t%06o\n", M_EXCL);
		break;

	  case  94:
		printf("h/access.h\tM_HASH\t\t%6d\n", M_HASH);
		break;

	  case  95:
		printf("h/access.h\tM_HEAP\t\t%6d\n", M_HEAP);
		break;

	  case  96:
		printf("h/access.h\tM_ISAM\t\t%6d\n", M_ISAM);
		break;

	  case  97:
		printf("h/lock.h\tM_MODE\t\t%06o\n", M_MODE);
		break;

	  case  98:
		printf("h/lock.h\tM_ON\t\t%06o\n", M_ON);
		break;

	  case  99:
		printf("h/lock.h\tM_REL\t\t%06o\n", M_REL);
		break;

	  case 100:
		printf("h/lock.h\tM_SHARE\t\t%06o\n", M_SHARE);
		break;

	  case 101:
		printf("h/lock.h\tM_TA\t\t%06o\n", M_TA);
		break;

	  case 102:
		printf("h/lock.h\tM_TE\t\t%06o\n", M_TE);
		break;

	  case 103:
		printf("h/access.h\tM_TRUNC\t\t%6d\n", M_TRUNC);
		break;

	  case 104:
		printf("h/access.h\tNACCBUFS\t%6d\n", NACCBUFS);
		break;

	  case 105:
		printf("h/scanner.h\tNAMELONG\t%6d\n", NAMELONG);
		break;

	  case 106:
		printf("h/ing_db.h\tNOKEY\t\t%6d\n", NOKEY);
		break;

	  case 107:
		printf("h/pipes.h\tNORM_STAT\t%6d\n", NORM_STAT);
		break;

	  case 108:
		printf("h/lock.h\tNO_REL\t\t%6ld\n", NO_REL);
		break;

	  case 109:
		printf("h/ing_db.h\tNULL\t\t%6d\n", NULL);
		break;

	  case 110:
		printf("h/scanner.h\tNUMBR\t\t%6d\n", NUMBR);
		break;

	  case 111:
		printf("h/scanner.h\tNUMBUFOFLO\t%6d\n", NUMBUFOFLO);
		break;

	  case 112:
		printf("h/scanner.h\tOPATR\t\t%6d\n", OPATR);
		break;

	  case 113:
		printf("h/pipes.h\tPBUFSIZ\t\t%6d\n", PBUFSIZ);
		break;

	  case 114:
		printf("conf/conf.h\tPGBLKS\t\t%6d\n", PGBLKS);
		break;

	  case 115:
		printf("h/access.h\tPGHDR\t\t%6d\n", PGHDR);
		break;

	  case 116:
		printf("h/access.h\tPGPTRSIZ\t%6d\n", PGPTRSIZ);
		break;

	  case 117:
		printf("h/access.h\tPGSIZE\t\t%6d\n", PGSIZE);
		break;

	  case 118:
		printf("h/catalog.h\tPROPERMID\t%6d\n", PROPERMID);
		break;

	  case 119:
		printf("h/catalog.h\tPRORELID\t%6d\n", PRORELID);
		break;

	  case 120:
		printf("h/catalog.h\tPRORELOWN\t%6d\n", PRORELOWN);
		break;

	  case 121:
		printf("h/catalog.h\tPROTERM\t\t%6d\n", PROTERM);
		break;

	  case 122:
		printf("h/catalog.h\tPROTREE\t\t%6d\n", PROTREE);
		break;

	  case 123:
		printf("h/catalog.h\tPRO_AGGR\t%06o\n", PRO_AGGR);
		break;

	  case 124:
		printf("h/catalog.h\tPRO_APP\t\t%06o\n", PRO_APP);
		break;

	  case 125:
		printf("h/catalog.h\tPRO_DEL\t\t%06o\n", PRO_DEL);
		break;

	  case 126:
		printf("h/catalog.h\tPRO_REPL\t%06o\n", PRO_REPL);
		break;

	  case 127:
		printf("h/catalog.h\tPRO_RETR\t%06o\n", PRO_RETR);
		break;

	  case 128:
		printf("h/catalog.h\tPRO_TEST\t%06o\n", PRO_TEST);
		break;

	  case 129:
		printf("h/scanner.h\tPUNCT\t\t%6d\n", PUNCT);
		break;

	  case 130:
		printf("h/pipes.h\tP_END\t\t%6d\n", P_END);
		break;

	  case 131:
		printf("h/pipes.h\tP_EXECID\t%6d\n", P_EXECID);
		break;

	  case 132:
		printf("h/pipes.h\tP_FLUSH\t\t%6d\n", P_FLUSH);
		break;

	  case 133:
		printf("h/pipes.h\tP_FUNCID\t%6d\n", P_FUNCID);
		break;

	  case 134:
		printf("h/pipes.h\tP_INT\t\t%6d\n", P_INT);
		break;

	  case 135:
		printf("h/pipes.h\tP_NORM\t\t%6d\n", P_NORM);
		break;

	  case 136:
		printf("h/pipes.h\tP_PARAM\t\t%6d\n", P_PARAM);
		break;

	  case 137:
		printf("h/pipes.h\tP_PRIME\t\t%6d\n", P_PRIME);
		break;

	  case 138:
		printf("h/pipes.h\tP_SYNC\t\t%6d\n", P_SYNC);
		break;

	  case 139:
		printf("h/pipes.h\tP_WRITE\t\t%6d\n", P_WRITE);
		break;

	  case 140:
		printf("h/catalog.h\tREFOWNER\t%6d\n", REFOWNER);
		break;

	  case 141:
		printf("h/catalog.h\tREFRELID\t%6d\n", REFRELID);
		break;

	  case 142:
		printf("h/catalog.h\tREFTYPE\t\t%6d\n", REFTYPE);
		break;

	  case 143:
		printf("h/ing_db.h\tRELID\t\t%6d\n", RELID);
		break;

	  case 144:
		printf("h/ing_db.h\tRELOWNER\t%6d\n", RELOWNER);
		break;

	  case 145:
		printf("h/version.h\tREL_DATE\t%s\n", REL_DATE);
		break;

	  case 146:
		printf("h/lock.h\tREL_LOCK\t%6ld\n", REL_LOCK);
		break;

	  case 147:
		printf("h/scanner.h\tSBUFOFLO\t%6d\n", SBUFOFLO);
		break;

	  case 148:
		printf("h/scanner.h\tSBUFSIZ\t\t%6d\n", SBUFSIZ);
		break;

	  case 149:
		printf("h/access.h\tSCANTID\t\t%6d\n", SCANTID);
		break;

	  case 150:
		printf("h/catalog.h\tSECBASE\t\t%6d\n", SECBASE);
		break;

	  case 151:
		printf("h/catalog.h\tSECINDEX\t%6d\n", SECINDEX);
		break;

	  case 152:
		printf("h/ing_db.h\tSTACKSIZ\t%6d\n", STACKSIZ);
		break;

	  case 153:
		printf("h/scanner.h\tSTRLONG\t\t%6d\n", STRLONG);
		break;

	  case 154:
		printf("h/scanner.h\tSTRTERM\t\t%6d\n", STRTERM);
		break;

	  case 155:
		printf("h/scanner.h\tSYMERR\t\t%6d\n", SYMERR);
		break;

	  case 156:
		printf("h/pipes.h\tSYNC_STAT\t%6d\n", SYNC_STAT);
		break;

	  case 157:
		printf("h/aux.h\t\tS_BINARY\t%06o\n", S_BINARY);
		break;

	  case 158:
		printf("h/aux.h\t\tS_CATALOG\t%06o\n", S_CATALOG);
		break;

	  case 159:
		printf("h/aux.h\t\tS_CONCUR\t%06o\n", S_CONCUR);
		break;

	  case 160:
		printf("h/aux.h\t\tS_DISCOPY\t%06o\n", S_DISCOPY);
		break;

	  case 161:
		printf("h/aux.h\t\tS_DISCRIT\t%06o\n", S_DISCRIT);
		break;

	  case 162:
		printf("h/aux.h\t\tS_DISTRIBUTED\t%06o\n", S_DISTRIBUTED);
		break;

	  case 163:
		printf("h/aux.h\t\tS_INDEX\t\t%06o\n", S_INDEX);
		break;

	  case 164:
		printf("h/aux.h\t\tS_INTEG\t\t%06o\n", S_INTEG);
		break;

	  case 165:
		printf("h/aux.h\t\tS_NOUPDT\t%06o\n", S_NOUPDT);
		break;

	  case 166:
		printf("h/aux.h\t\tS_PROTALL\t%06o\n", S_PROTALL);
		break;

	  case 167:
		printf("h/aux.h\t\tS_PROTRET\t%06o\n", S_PROTRET);
		break;

	  case 168:
		printf("h/aux.h\t\tS_PROTUPS\t%06o\n", S_PROTUPS);
		break;

	  case 169:
		printf("h/aux.h\t\tS_VBASE\t\t%06o\n", S_VBASE);
		break;

	  case 170:
		printf("h/aux.h\t\tS_VIEW\t\t%06o\n", S_VIEW);
		break;

	  case 171:
		printf("h/catalog.h\tTREEID\t\t%6d\n", TREEID);
		break;

	  case 172:
		printf("h/catalog.h\tTREEOWNER\t%6d\n", TREEOWNER);
		break;

	  case 173:
		printf("h/catalog.h\tTREERELID\t%6d\n", TREERELID);
		break;

	  case 174:
		printf("h/catalog.h\tTREESEQ\t\t%6d\n", TREESEQ);
		break;

	  case 175:
		printf("h/catalog.h\tTREETYPE\t%6d\n", TREETYPE);
		break;

	  case 176:
		printf("h/ing_db.h\tTRUE\t\t%6d\n", TRUE);
		break;

	  case 177:
		printf("h/access.h\tTUP_BUF_SIZ\t%6d\n", TUP_BUF_SIZ);
		break;

	  case 178:
		printf("h/aux.h\t\tUF_DBLIST\t%6d\n", UF_DBLIST);
		break;

	  case 179:
		printf("h/aux.h\t\tUF_FLAGS\t%6d\n", UF_FLAGS);
		break;

	  case 180:
		printf("h/aux.h\t\tUF_GID\t\t%6d\n", UF_GID);
		break;

	  case 181:
		printf("h/aux.h\t\tUF_IFILE\t%6d\n", UF_IFILE);
		break;

	  case 182:
		printf("h/aux.h\t\tUF_NAME\t\t%6d\n", UF_NAME);
		break;

	  case 183:
		printf("h/aux.h\t\tUF_NFIELDS\t%6d\n", UF_NFIELDS);
		break;

	  case 184:
		printf("h/aux.h\t\tUF_PTAB\t\t%6d\n", UF_PTAB);
		break;

	  case 185:
		printf("h/aux.h\t\tUF_STAT\t\t%6d\n", UF_STAT);
		break;

	  case 186:
		printf("h/aux.h\t\tUF_UCODE\t%6d\n", UF_UCODE);
		break;

	  case 187:
		printf("h/aux.h\t\tUF_UID\t\t%6d\n", UF_UID);
		break;

	  case 188:
		printf("h/unix.h\tUSERING_DB\t%s\n", USERING_DB);
		break;

	  case 189:
		printf("h/lock.h\tUSE_LOCK\t%6ld\n", USE_LOCK);
		break;

	  case 190:
		printf("h/aux.h\t\tU_APROCTAB\t%06o\n", U_APROCTAB);
		break;

	  case 191:
		printf("h/aux.h\t\tU_CREATDB\t%06o\n", U_CREATDB);
		break;

	  case 192:
		printf("h/aux.h\t\tU_DRCTUPDT\t%06o\n", U_DRCTUPDT);
		break;

	  case 193:
		printf("h/aux.h\t\tU_EPROCTAB\t%06o\n", U_EPROCTAB);
		break;

	  case 194:
		printf("h/aux.h\t\tU_QRYMODOFF\t%06o\n", U_QRYMODOFF);
		break;

	  case 195:
		printf("h/aux.h\t\tU_SUPER\t\t%06o\n", U_SUPER);
		break;

	  case 196:
		printf("h/aux.h\t\tU_TRACE\t\t%06o\n", U_TRACE);
		break;

	  case 197:
		printf("h/aux.h\t\tU_UPSYSCAT\t%06o\n", U_UPSYSCAT);
		break;

	  case 198:
		printf("h/version.h\tVERSION\t\t%s\n", VERSION);
		break;

	  case 199:
		printf("h/aux.h\t\tWORDSIZE\t%6d\n", WORDSIZE);
		break;

	  default:
		printf("illegal CONST\n");
	}
}
