/* IDENT: 100359 - qdef */
/* Design: van Treek */

# ifndef		TQLSYS
# include	"TQLSYS.H"		/* Systemdefinitionen */
# endif

/*
** Definition der Symbole fuer ausgelassene Funktionsargumente
*/

# ifndef	G_NILA
# define	G_NILA	(char *)(-1)	/* ausgelassener Adress-Parameter */
# endif

# ifndef	G_NILI
# define	G_NILI	(-1)		/* ausgelassener INT-Parameter */
# endif

# ifndef	G_NILL
# define	G_NILL	(-1l)		/* ausgelassener LONG-Parameter */
# endif

# ifndef	G_NILC
# define	G_NILC	(char)0xFF	/* ausgelassener CHAR-Parameter */
# endif
# ifndef        G_NILF
# define        G_NILF  (FILE *)(-1)    /* ausgelassener FILE-Pointer   */
# endif
/*
** Definition des Macros DEBUG zum Ausschalten des Funktionsrufes
*/

# ifndef	gdebug

# define	gdebug(x,y)		/* DEBUG ausschalten */     \
{     \
char	*parm1, *parm2;     \
	parm1 = (char *)x;	/* Zuweisung wird durch Optimierung entfernt*/ \
	parm2 = (char *)y;	/* Zuweisung wird durch Optimierung entfernt*/ \
}

# endif

/****************************************************************************
* 
* Symbole der Kommandoschluesselwoerter, die in der ersten Position
* der Kommandozeile auftreten koennen.
*
****************************************************************************/

# define	QT_ACC	0
# define	QT_APP	QT_ACC + 1
# define	QT_AVE	QT_APP + 1
# define	QT_BRO	QT_AVE + 1
# define	QT_CAN	QT_BRO + 1
# define	QT_CAS	QT_CAN + 1
# define	QT_CHA	QT_CAS + 1
# define	QT_CLE	QT_CHA + 1
# define	QT_CLO	QT_CLE + 1
# define	QT_COT	QT_CLO + 1
# define	QT_COP	10
# define	QT_COU	QT_COP + 1
# define	QT_CRE	QT_COU + 1
# define	QT_DBG	QT_CRE + 1
# define	QT_DEL	QT_DBG + 1
# define	QT_DIR	QT_DEL + 1
# define	QT_DIS	QT_DIR + 1
# define	QT_DO	QT_DIS + 1
# define	QT_EDI	QT_DO  + 1
# define QT_EJE	QT_EDI + 1
# define	QT_ELS	20
# define	QT_ENC	QT_ELS + 1
# define	QT_ENI	QT_ENC + 1
# define	QT_END	QT_ENI + 1
# define	QT_ENT	QT_END + 1
# define	QT_ERA	QT_ENT + 1
# define	QT_EXI	QT_ERA + 1
# define	QT_FIN	QT_EXI + 1
# define	QT_GO	QT_FIN + 1
# define	QT_GRA	QT_GO  + 1		/* GRAPHIC  */
# define	QT_IF	QT_GRA + 1
# define	QT_IND	31
# define	QT_INP	QT_IND + 1
# define	QT_INS	QT_INP + 1
# define	QT_JOI	QT_INS + 1
# define	QT_LAB	QT_JOI + 1
# define	QT_LIS	QT_LAB + 1
# define	QT_LOC	QT_LIS + 1
# define	QT_LOO	QT_LOC + 1
# define	QT_MOD	QT_LOO + 1
# define	QT_NOT	QT_MOD + 1
# define	QT_OTH	41
# define	QT_PAC	QT_OTH + 1
# define	QT_PAR	QT_PAC + 1
# define	QT_PRI	QT_PAR + 1
# define	QT_PUB	QT_PRI + 1
# define	QT_QUI	QT_PUB + 1
# define	QT_REA	QT_QUI + 1
# define	QT_REC	QT_REA + 1
# define	QT_REI	QT_REC + 1
# define	QT_REL	QT_REI + 1
# define	QT_REN	51
# define	QT_RPL	QT_REN + 1
# define	QT_REP	QT_RPL + 1
# define	QT_RST	QT_REP + 1
# define	QT_RTU	QT_RST + 1
# define	QT_RUN	QT_RTU + 1
# define	QT_SAV	QT_RUN + 1
# define	QT_SCA	QT_SAV + 1
# define	QT_SEE	QT_SCA + 1			/* SEEK */
# define	QT_SEL	QT_SEE + 1
# define	QT_SET	QT_SEL + 1
# define	QT_SKI	62
# define	QT_SOR	QT_SKI + 1
# define	QT_STO	QT_SOR + 1
# define	QT_SUM	QT_STO + 1
# define	QT_TEX	QT_SUM + 1
# define	QT_TOT	QT_TEX + 1
# define	QT_TYP	QT_TOT + 1
# define	QT_UPD	QT_TYP + 1
# define	QT_USE	QT_UPD + 1
# define	QT_WAI	QT_USE + 1
# define	QT_ZAP	72
# define	QT_FRA	QT_ZAP + 1
# define	QT_POS	QT_FRA + 1
# define	QT_PRO	QT_POS + 1
# define	QT_HEL	QT_POS + 1
# define QT_ASS	QT_HEL + 1
# define	QT_EOF	QT_ASS + 1
# define	QT_PLO	QT_EOF + 1

/****************************************************************************
*
* Symbole der Parameter-Schluesselworte  
*
****************************************************************************/


# define	QP_ADD	0501
# define	QP_ALL	0502
# define	QP_ALT	0503
# define	QP_BEL	0504
# define	QP_BEF	0505
# define	QP_BLA	0506
# define	QP_BOT	0507
# define	QP_CAL	0510
# define	QP_CAR	0511
# define	QP_CAS	0512
# define	QP_COL	0513
# define	QP_CON	0514
# define	QP_COS	0515
# define	QP_DEB	0516
# define	QP_DEF	0517
# define	QP_DES	0520
# define	QP_DLM	0521
# define	QP_ECH	0522
# define	QP_ERA	0523
# define	QP_ESC	0524
# define	QP_EXA	0525
# define	QP_FIE	0526
# define	QP_FOM	0527
# define	QP_FOR	0530
# define	QP_FRO	0531
# define	QP_GET	0532
# define	QP_HEA	0533
# define	QP_IND	0534
# define	QP_INE	0535
# define	QP_LIK	0536
# define	QP_LIN	0537
# define	QP_MAR	0540
# define	QP_MEM	0541
# define	QP_NBU	0542
# define	QP_NEX	0543
# define	QP_NOT	0544
# define	QP_OFF	0545
# define	QP_ON	0546
# define	QP_PRI	0547
# define	QP_PRN	0550
# define	QP_RAW	0551
# define	QP_REA	0552
# define	QP_REO	0553
# define	QP_RNA	0554
# define	QP_REP	0555
# define	QP_RPO	0556
# define	QP_RSE	0557
# define	QP_SCR	0560
# define	QP_SDF	0561
# define	QP_SEO	0562
# define	QP_SEG	0563
# define	QP_STE	0564
# define	QP_STU	0565
# define	QP_SUM	0566
# define	QP_TAL	0567
# define	QP_TUT	0570
# define	QP_TOP	0571
# define	QP_TO	0572
# define	QP_VOL	0573
# define	QP_WHI	0574
# define	QP_WIT	0575
# define	QP_MDM	0576
# define	QP_EXT	0577
# define	QP_KEY	0600
# define	QP_CMN	0601
# define	QP_MST	0602
# define	QP_MPO	0603
# define	QP_DTE	0604
# define	QP_KYU	0605
# define	QP_MDP	0606
# define	QP_MME	0607
# define	QP_PRO	0610
# define	QP_ALI	0611
# define	QP_CLE	0612
# define	QP_COR	0613
# define	QP_COM	0614
# define	QP_DTB	0615
# define	QP_DEC	0616
# define	QP_DEL	0617
# define	QP_DLR	0620
# define	QP_DEV	0621
# define	QP_EXC	0622
# define	QP_FIL	0623
# define	QP_FLT	0624
# define	QP_FIX	0625
# define	QP_FRM	0626
# define	QP_FUN	0627
# define	QP_GTS	0630
# define	QP_HEL	0631
# define	QP_INT	0632
# define	QP_LAB	0633
# define	QP_MAS	0634
# define	QP_MDS	0635
# define	QP_MEN	0636
# define	QP_NOE	0637
# define	QP_PAT	0640
# define	QP_PIC	0641
# define	QP_PLA	0642
# define	QP_RAD	0643
# define	QP_RAG	0644
# define	QP_REL	0645
# define	QP_RPL	0646
# define	QP_SAF	0647
# define	QP_SAM	0650
# define	QP_SVE	0651
# define	QP_SAY	0652
# define	QP_STA	0653
# define	QP_UNI	0654
# define	QP_GRA	0655

/********************************************************
*
* FEHLER-DEFINITIONEN
*
********************************************************/

# define	QA_NOKDO	1
# define	QA_SYNTAX	2
# define	QA_NOTFOUND	3
# define	QA_EXPRESSION	4
# define	QA_KDOFILE	5
# define	QA_OVERWHILE	6
# define	QA_OVERIF	7
# define	QA_OVERDOCASE	8
# define	QA_NOSPACE	9
# define	QA_NOTVARIALBLE	10
# define	QA_NOTPRG	11
# define	QA_NOTREC	12
# define	QA_NOTALIAS	13
# define	QA_NOTIMP	14
# define	QA_PRGERROR	15
# define	QA_NOTHELP	16
# define	QA_BEREICH	17
# define	QA_NOTOPEN	18
# define QA_INTERUPT	19

/* 
** Sonstige Definitionen
*/

# define QS_NONAME '\176'  /* keine Namenspruefung bei LEX */


/* Zeichenkettenzeichen */

# define QZ_ZKCH1 '\''
# define QZ_ZKCH2 '"'
# define QZ_ZKCH3 '['


# define QA_KLAA 06101
# define QA_KLAZ 06102
# define QA_KENDE 06106
# define QA_KOMMA 0200

# define QA_SONSTNAME 01003

# define QA_KONSTANTE 02000
# define QA_ZKONST 02010
# define QA_NKONST 02020
# define QA_LKONST 02030

# define QA_TRUE 02031
# define QA_FALSE 02032

# define QA_FUNKTION 03000
# define QA_OPERATOR 04000
# define QA_PRIORITAET 070

# define QA_NICHT 04061
# define QA_EINMINUS 04062  /* einseitiges Minus */

# define QA_MUL 04051
# define QA_DIV 04052
# define QA_PLUS 04041
# define QA_MINUS 04042

# define QA_GLEICH 04031
# define QA_UNGLEICH 04032
# define QA_GROESSER 04033
# define QA_KLEINER 04034
# define QA_GRGLEICH 04035
# define QA_KLGLEICH 04036

# define QA_UND 04021
# define QA_ODER 04011

