	/* see ovqp/interp.c		*/
				/* see ovqp/interp.c		*/
# define	AND		0	/* see ovqp/interp.c		*/
# define	OR		1	/* see ovqp/interp.c		*/
# define	UOP		2	/* UNARY OPERATORS		*/
					/* see ovqp/interp.c		*/
enum		UOP_VAL
{
/* 0*/	opPLUS,				/* see ovqp/interp.c		*/
/* 1*/	opMINUS,			/* see ovqp/interp.c		*/
/* 2*/	opNOT,				/* see ovqp/interp.c		*/
/* 3*/	opATAN,				/* see ovqp/interp.c		*/
/* 4*/	opCOS,				/* see ovqp/interp.c		*/
/* 5*/	opGAMMA,			/* see ovqp/interp.c		*/
/* 6*/	opLOG,				/* see ovqp/interp.c		*/
/* 7*/	opASCII,			/* see ovqp/interp.c		*/
/* 8*/	opSIN,				/* see ovqp/interp.c		*/
/* 9*/	opSQRT,				/* see ovqp/interp.c		*/
/*10*/	opABS,				/* see ovqp/interp.c		*/
/*11*/	opEXP,				/* see ovqp/interp.c		*/
/*12*/	opINT1,				/* see ovqp/interp.c		*/
/*13*/	opINT2,				/* see ovqp/interp.c		*/
/*14*/	opINT4,				/* see ovqp/interp.c		*/
/*15*/	opFLT4,				/* see ovqp/interp.c		*/
/*16*/	opFLT8				/* see ovqp/interp.c		*/
};
# define	BOP		3	/* BINARY OPERATORS		*/
					/* see decomp/reformat.c	*/
					/* and ovqp/interp.c		*/
enum		BOP_VAL
{
/* 0*/	opADD,				/* see ovqp/interp.c		*/
/* 1*/	opSUB,				/* see ovqp/interp.c		*/
/* 2*/	opMUL,				/* see ovqp/interp.c		*/
/* 3*/	opDIV,				/* see ovqp/interp.c		*/
/* 4*/	opPOW,				/* see ovqp/interp.c		*/
/* 5*/	opEQ,				/* see decomp/reformat.c	*/
					/* and ovqp/interp.c		*/
/* 6*/	opNE,				/* see ovqp/interp.c		*/
/* 7*/	opLT,				/* see ovqp/interp.c		*/
/* 8*/	opLE,				/* see ovqp/interp.c		*/
/* 9*/	opGT,				/* see ovqp/interp.c		*/
/*10*/	opGE,				/* see ovqp/interp.c		*/
/*11*/	opMOD,				/* see ovqp/interp.c		*/
/*12*/	opCONCAT			/* see ovqp/interp.c		*/
};
# define	AOP		4	/* AGGREGATE OPERATORS		*/
					/* see ovqp/interp.c		*/
enum		AOP_VAL
{
/* 0*/	opCNT,				/* see ovqp/interp.c		*/
/* 1*/	opCNTU,				/* see ovqp/interp.c		*/
/* 2*/	opSUM,				/* see ovqp/interp.c		*/
/* 3*/	opSUMU,				/* see ovqp/interp.c		*/
/* 4*/	opAVG,				/* see ovqp/interp.c		*/
/* 5*/	opAVGU,				/* see ovqp/interp.c		*/
/* 6*/	opMIN,				/* see ovqp/interp.c		*/
/* 7*/	opMAX,				/* see ovqp/interp.c		*/
/* 8*/	opANY				/* see ovqp/interp.c		*/
};
# define	COP		5	/* CONSTANT OPERATORS		*/
					/* see ovqp/interp.c		*/
enum		COP_VAL
{
/* 0*/	opDBA,				/* see ovqp/interp.c		*/
/* 1*/	opUSERCODE,			/* see ovqp/interp.c		*/
/* 2*/	opDATE,
/* 3*/	opTIME
};
# define	RESDOM		11	/* RESULT DOMAIN NUMBER		*/
					/* see ovqp/interp.c		*/
					/* and qrymod/d_prot.c		*/
# define	VAR		12	/* VARIABLE			*/
					/* see decomp/reformat.c	*/
					/* and qrymod/d_prot.c		*/
# define	S_VAR		13	/* variable for which		*/
					/* a substitution has been done	*/
			/* Only occurs in OVQP-DECOMP merged process.	*/

# define	QMODE		16	/* QUERY MODE			*/
enum		QMODE_VAL
{
/* 0*/	mdRETTERM,	/* RETRIEVE to terminal, used by OVQP-DECOMP	*/
/* 1*/	mdRETR,		/* RETRIEVE, into relation perhaps		*/
/* 2*/	mdAPP,		/* APPEND to relation				*/
/* 3*/	mdREPL,		/* REPLACE in relation				*/
			/* see ovqp/interp.c				*/
/* 4*/	mdDEL,		/* DELETE from relation				*/
			/* see ovqp/interp.c				*/
/* 5*/	mdCOPY,		/* COPY INTO/FROM relation			*/
/* 6*/	mdCREATE,	/* CREATE new relation				*/
			/* see decomp/reformat.c			*/
/* 7*/	mdDESTROY,	/* DESTROY old relation				*/
/* 8*/	mdHELP,		/* print info on relation			*/
/* 9*/	mdINDEX,	/* create secondary INDEX			*/
/*10*/	mdMODIFY,	/* change access structure of relation		*/
			/* see decomp/reformat.c			*/
/*11*/	mdPRINT,	/* PRINT relation on terminal			*/
/*12*/	mdRANGE,	/* declare RANGE variable			*/
/*13*/	mdSAVE,		/* SAVE relation date				*/
/*14*/	mdDEFINE,	/* "here is a tree", to QRYMOD			*/
			/* see qrymod/d_prot.c				*/
/*15*/	mdRET_UNI,	/* RETRIEVE UNIQUE				*/
/*16*/	mdVIEW,		/* extra info for VIEW definition, to QRYMOD	*/
			/* see support/restore.c			*/
/*17*/	mdUPDATE,	/* update processor				*/
/*18*/	mdRESETREL,	/* modify to truncated				*/
/*19*/	mdDISPLAY,	/* dump QRYMOD declaration			*/
/*20*/	mdNETQRY,	/* reserved for distributed DBS			*/
/*21*/	mdMOVEREL,	/* reserved for distributed DBS			*/
/*22*/	mdPROT,		/* extra info for PERMIT definition, to QRYMOD	*/
			/* see qrymod/d_prot.c				*/
			/* and support/restore.c			*/
/*23*/	mdINTEG,	/* extra info for INTEGRITY def, to QRYMOD	*/
			/* see support/restore.c			*/
/*24*/	mdDCREATE,	/* reserved for distributed DBS			*/
/*25*/	mdWAITQRY,	/* reserved for distributed DBS			*/
/*26*/	mdREMQM,	/* remove PERMIT INTEGRITY definition		*/
/*27*/	mdDISTRIB	/* reserved for distributed DBS			*/
};
# define	ROOT		20	/* ROOT of QUERY TREE		*/
# define	QLEND		21	/* NULL branch at end of QUAL	*/
					/* see decomp/reformat.c	*/
					/* and qrymod/d_prot.c		*/
# define	BYHEAD		22	/* BY LIST HEAD			*/
# define	AGHEAD		23	/* AGGREGATE HEAD		*/
# define	TREE		25	/* SIGNALS BEGINNING of TREE	*/
					/* see decomp/reformat.c	*/
					/* and qrymod/d_prot.c		*/

# define	SITE		26	/* reserved for distributed DBS	*/

/*define	CID		29	** FOR INTEGRITY CONSTRAINTS	*/

/*define	INT		30	** INTEGER CONSTANT		*/
/*define	FLOAT		31	** FLOATING POINT CONSTANT	*/
/*define	CHAR		32	** CHARACTER CONSTANT		*/
# define	BINARY		33	/* BINARY CONST used in AAprtup	*/

# define	RESULTVAR	40	/* RESULT RELATION VAR NO.	*/
# define	SOURCEID	41	/* SOURCE RELATION NAME(S)	*/
# define	RESULTID	42	/* RESULT RELATION NAME		*/

# define	CHANGESTRAT	50	/* DEVISE OVQP NEW STRATEGY	*/
# define	USERQRY		51	/* tells OVQP that the result	*/
					/* relation is a user relation	*/
# define	EXIT		52	/* tells OVQP that qry is done	*/
# define		ACK	     1	/* OVQP should write an		*/
					/* acknowledging EOP		*/
# define		NOACK	     2	/* OVQP should not acknowledge	*/
# define		RUBACK	     3	/* internal mode for DECOMP	*/

# define	REOPENRES	54	/* OVQP should reopen resrel	*/
# define	RETVAL		55	/* RETURN FROM OVQP		*/
# define		UPDATE	    -3	/* tells DECOMP to call update	*/
# define		NOUPDATE    -2	/* dito, not to call update	*/
# define		EMPTY	    -1
# define		NONEMPTY     0

# define	EOTUP		56	/* end of tuple, OVQP to EQL	*/

/*	The following are pattern matching symbols used by OVQP		*/
# define	PAT_ANY		1	/* matches 0 or more char	*/
# define	PAT_ONE		2	/* matches any one char		*/
# define	PAT_LBRAC	3	/* left bracket for chargroups	*/
# define	PAT_RBRAC	4	/* right bracket for chargroups	*/
