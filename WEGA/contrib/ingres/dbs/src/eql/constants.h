/*  CONSTANTS.H -- manifest constants, operand and operator codes	    */
/*									    */
/*	Defines:							    */
/*		op_codes for operands					    */
/*		Opflag's domain set					    */
/*		Type_spec's domain set					    */

/* 	Manifest constants used throughout EQL				    */

# define	CONTINUE	1	/* "loop" flag for yylex	    */
# ifdef	MAXNAME
# undef	MAXNAME
# endif
# define	MAXNAME		13	/* maximum length for EQL 	    */
					/* identifiers (or keywords) 	    */
# define	MAXSTRING	255	/* maximum length for EQL strings   */
# define	FILLCNT		110	/* length to fill lines		    */
					/* when in Fillmode		    */


/* debugging info conditional compilation flag */
# define	xDEBUG			/* on for "-c" and "-v" flags	    */


/*	Character types [cmap.c] 					    */

# define	EOF_TOK		0	/* end of parse input too	    */
# define	ALPHA		1	/* alphabetic or '_'		    */
# define	NUMBR		2	/* numeric			    */
# define	OPATR		3	/* other non control characters	    */
# define	PUNCT		4	/* white space			    */
# define	CNTRL		5	/* control-characters		    */


/* Modes used in parser actions to distinguish contexts in which	    */
/* the same syntax applies. Opflag is set to these modes.		    */
/*									    */
/* (There are some modes that are never referenced, but are useful	    */
/* for extension).							    */

	/* QL statements */

# define	mdAPPEND	1
# define	mdCOPY		2
# define	mdCREATE	3
# define	mdDEFINE	4
# define	mdDELETE	5
# define	mdDESTROY	6
# define	mdHELP		7
# define	mdINDEX		8
# define	mdINTEGRITY	9
# define	mdMODIFY	10
# define	mdPRINT		11
# define	mdRANGE		12
# define	mdREPLACE	13
# define	mdRETRIEVE	14
# define	mdSAVE		15
# define	mdVIEW		16
# define	mdPROT		17

	/* statements particular to EQL */

# define	mdDECL		16	/* C - declaration		    */
# define	mdCTLELM	17	/* left hand side of target list    */
					/* element in "retrieve" to C-vars  */
# define	mdEXIT		18	/* ## exit			    */
# define	mdDBS		19	/* ## dbs			    */
# define	mdTUPRET	20	/* "retrieve" w/o an "into"	    */
# define	mdFILENAME	21	/* used in "copy" statement	    */







/* define	typTYPE		xx */		/*   c types "Type_spec"    */
# define			opINT		1
# ifndef NO_F4
# define			opFLT		2
# endif
# define			opSTRING	3
# define			opDOUBLE	4
# define			opCHAR		5
# define			opLONG		6
# define			opIDSTRING	7
# define			opSTRUCT	8


/* define	typALLOC	xx */		/*  c var allocation types  */
# define			opAUTO		0
# define			opSTATIC	1
# define			opEXTERN	2
# define			opREGISTER	3
