/* IDENT: 100359 - RODEF.H */

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
# ifndef	G_NILF
# define        G_NILF  (FILE *)(-1)      /* ausgelassener FILE-Pointer */
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
*  Definitionen zur Arbeit mit Relationalen Operatoren
*
****************************************************************************/


# define RA_MAXINT 07777777777	/* groesste positive Zahl */

/* Definition der Funktionswerte der SET - Funktion */

# define RF_ALTE 0503	/* QP_ALT */
# define RF_BELL 0504	/* QP_BEL */
# define RF_CARR 0511	/* QP_CAR */
# define RF_COLO 0613	/* QP_COR */
# define RF_CONF 0514	/* QP_CON */
# define RF_CONS 0515	/* QP_COS */
# define RF_DEBU 0516	/* QP_DEB */
# define RF_DECI 0616	/* QP_DEC */
# define RF_DEFA 0517	/* QP_DEF */
# define RF_DELE 0617	/* QP_DEL */
# define RF_DELI 0620	/* QP_DLR */
# define RF_DEVI 0621	/* QP_DEV */
# define RF_ECHO 0522	/* QP_ECH */
# define RF_ESCA 0524	/* QP_ESC */
# define RF_EXAC 0525	/* QP_EXA */
# define RF_FILT 0624	/* QP_FLT */
# define RF_FIXE 0625	/* QP_FIX */
# define RF_FORM 0527	/* QP_FOM */
# define RF_HEAD 0533	/* QP_HEA */
# define RF_HELP 0631	/* QP_HEL */
# define RF_INDE 0534	/* QP_IND */
# define RF_INTE 0535	/* QP_INE */
# define RF_KYUP 0605	/* QP_KYU */
# define RF_MARG 0540	/* QP_MAR */
# define RF_MDMP 0606	/* QP_MDP */
# define RF_MENU 0636	/* QP_MEN */
# define RF_NBUF 0542	/* QP_NBU */
# define RF_PATH 0640	/* QP_PAT */
# define RF_PRIN 0550	/* QP_PRN */
# define RF_PROC 0610	/* QP_PRO */
# define RF_RELA 0645	/* QP_REL */
# define RF_SAFE 0647	/* QP_SAF */
# define RF_SCOR 0777	/* -----------------> BEI WILLI NICHT ENTHALTEN !!!! */
# define RF_SEGR 0563	/* QP_SEG */
# define RF_STEP 0564	/* QP_STE */
# define RF_TALK 0567	/* QP_TAL */
# define RF_TOPU 0570	/* QP_TUT */
# define RF_UNIQ 0654	/* QP_UNI */
# define RF_VOLU 0573	/* QP_VOL */

# define ANY '\177'
# define NONAME '\176'   /* keine Namenspruefung bei LEX */
# define NCON '\175'     /* keine Namenspruefung bei dresp */	

/* Zeichenkettenzeichen */

# define ZKCH1 '\''
# define ZKCH2 '"'

# define PECH '\0'	/* Endezeichen fuer Parameterzeile */

/* Definitionen fuer Bearbeitung von Ausdruecken */

# define RA_MAXTERMINALSYMBOL  0777

# define RA_MASKE 07000
# define RA_EINZELZEICHEN 06100
# define RA_KLAA 06101
# define RA_KLAZ 06102
# define RA_INDIREKT 06103
# define RA_KENDE 06106
# define RA_KOMMA 06107

# define RA_NAME 01000
# define RA_DATEIDES 01001
# define RA_MEMNAME 01002
# define RA_SONSTNAME 01003

# define RA_KONSTANTE 02000
# define RA_ZKONST 02010
# define RA_NKONST 02020
# define RA_LKONST 02030
# define RA_TRUE 02031
# define RA_FALSE 02032

# define RA_FUNKTION 03000
# define RA_FKTEIN 03100
			/* Oktalkodierung: 3 x y y   [ 3 - Funktion ]         */
			/*                   ^   Anzahl der Argumente         */
			/*		       ^ zweistellige laufende Nummer */
# define RA_FBOF 03001	    /* ==> Funktionen ohne Argumente ohne Datumsbezug */
# define RA_FEOF 03002		
# define RA_FRECNO 03003
# define RA_FCOL 03006
# define RA_FROW 03007
# define RA_FPCOL 03010
# define RA_FPROW 03011
# define RA_FRC 03012		/* RECCOUNT */
# define RA_FRS 03013		/* RECSIZE  */
# define RA_FDELE 03014		/* DELETED */
# define RA_FDBF 03015
# define RA_FPSPACE 03016	/* Plattenplatz */
# define RA_FFOUND 03017
# define RA_FISCOLOR 03020
# define RA_FFKMAX 03021
# define RA_FOS 03022
# define RA_FVERSION 03023

# define RA_FDATE 03070		/* Datumsbezogene Funktion (ab oktal 60) */
# define RA_FTIME 03071

# define RA_FASC 03101		/* ==> Funktionen mit 1 Argument */
# define RA_FCHR 03102
# define RA_FFILE 03103
# define RA_FLEN 03104
# define RA_FTRIM 03105
# define RA_FRTRIM 03105
# define RA_FVAL 03106
# define RA_FUPPER 03107
# define RA_FLOWER 03110
# define RA_FISAL 03111
# define RA_FISLOW 03112
# define RA_FISUPP 03113
# define RA_FSPA 03115
# define RA_FTYPE 03116
# define RA_FFIELD 03117
# define RA_FLTRIM 03120
# define RA_FNDX 03121
# define RA_FFKLABEL 03122

# define RA_FABS 03150		/* mathematische Funktionen mit 1 Argument */
# define RA_FINT 03151		/* laufende Nummer ab oktal 40 */
# define RA_FEXP 03152
# define RA_FLOG 03153
# define RA_FSQRT 03154

# define RA_FDOW  03160		/* Datumsfunktionen mit 1 Argument */
# define RA_FCDOW 03161		/* laufende Nummer ab oktal 60 */
# define RA_FMONTH 03162
# define RA_FCMONTH 03163
# define RA_FDAY 03164
# define RA_FYEAR 03165
# define RA_FCTOD 03166
# define RA_FDTOC 03167
				/* ==> Funktionen mit 2 Argumenten */
# define RA_FZKENT 03201		/* AT */
# define RA_FLEFT 03202
# define RA_FRIGHT 03203
# define RA_FREPLI 03204		/* REPLICATE */
# define RA_FTRANS 03205		/* TRANSFORM */

# define RA_FMIN 03241		/* math. Funktion */
# define RA_FMAX 03242		/* math. Funktion */
# define RA_FMOD 03243		/* math. Funktion */
# define RA_FROUND 03244		/* math. Funktion (45 auch vergeben (FCELM) */

# define RA_FSTR 03301		/* ==> Funktionen mit 3 Argumenten */
# define RA_FTEILZK 03302	/* SUBSTR */
# define RA_FIIF 03303		/* IIF */

# define RA_FCELM 03445		/* ==> Funktionen mit 4 Argumenten */
# define RA_FSTUFF 03402

# define RA_OPERATOR  04000
# define RA_PRIORITAET 070

# define RA_NICHT 04061
# define RA_EINMINUS 04062  /*Einseitiges Minus */
# define RA_MUL 04051
# define RA_DIV 04052
# define RA_PLUS 04041
# define RA_MINUS 04042
# define RA_MAX 04053
# define RA_MIN 04054

# define RA_GLEICH 04031
# define RA_UNGLEICH 04032
# define RA_GROESSER 04033
# define RA_KLEINER 04034
# define RA_GRGLEICH 04035
# define RA_KLGLEICH 04036
# define RA_UND 04021
# define RA_ODER 04011

/* Definitionen fuer Pol. Notation */

# define RA_RS 07001
# define RA_SW 07002
# define RA_WS 07003
# define RA_RUNSTOP 07004
# define RA_CC 07005
# define RA_RM 07006
# define RA_WM 07007
# define RA_FS 07010
# define RA_TS 07011
# define RA_LC 07012
 
/* Definitionen fuer Spalte $$$$MARK */
 
# define RB_MARKDEL 01	/* Kennzeiche fuer Satz geloescht */
