/* IDENT: 100359 - rostrd */

# include	"TQLSYS.H"              /* Systemdefinitionen */

# include <stdio.h>

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
# define	G_NILF	(FILE *)(-1)	/* ausgelassener FILE-Pointer  */
# endif

/****************************************************************************
*
* Definition der Makros zur Arbeit mit Relationalen Operatoren
*
****************************************************************************/

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
* Definition der Strukturen zur Arbeit mit Relationalen Operatoren
*
*****************************************************************************/

# ifndef NULL
# define NULL 0 		/* Horn 24.1.89 wegen # undef NULL in stdio.h */
# endif


# define O_RDONLY 000		/* Definitionen aus <file.h> */
# define O_RDWR 002

# ifndef QC_MAXPGR
# ifdef  SSVP
# define QC_MAXPGR 32
# else
# define QC_MAXPGR 5
# endif
# endif

# define RO_MAXFILSPEC 100
# define RO_MAXVALSTACK 2048
# define RO_MAXPOLNOT 3072
# define RO_MAXMEMORY 100
# define RO_MAXZEILE 256
# define RO_MAXRECORD 512
# define RO_MINSPLISTE 1		/* Anzahl der Spalten, wo Optimierung ansetzt */

/*
** Definition der Datentypen fuer TYP-Feld von RO_WERT und RO_DED
*/

#define MAXTYPE 5		/* Max. Typeanzahl */

#define C	0		/* CHAR */
#define D	1		/* DATE */
#define N	2		/* NUMERIC CHAR */
#define L	3		/* LOGIC */
#define F	4		/* FIXED BIN */
#define E	5		/* FLOAT */


/* Datenelementbeschreiber fuer RO */

struct ro_ded
{

	int  byte;	/* Spaltenbreite in Bytes */
	char type;	/* Typkennzeichen */
	char ro_ded_frei[sizeof (int) - 1]; /* Einhaltung Wortausrichtung */
	int  prec;	/* Anzahl der Dezimalstellen bei num. Spaltentypen */
	int  pos;	/* Position der Spalte im seriellen File */
};

# ifndef	_cded

# define	_cded(pded, ty, by, pre) \
{ \
struct	ro_ded	*ded; \
	ded = pded; \
	ded->type = (char)ty; \
	ded->byte = (int)by; \
	ded->prec = (int)pre; \
}

# endif

/* Struktur zur Arbeit mit Werten */

#define RO_HWERT (sizeof(int) * 3)  /* Laenge des Kopfes der Wertstruktur */

struct ro_wert{
	int  len;	/* Laenge des Puffers in Byte */
	char typ;	/* Bearbeitungstyp der Spalten im Puffer */
	char wfrei[sizeof (int) - 1];
	int  wprec;	/* Anzahl der Dezimalstellen */
	union{
		char c[4];
		long n;
		float g;
		char l; 	/* fuer Typ L (logisch) = 'T' oder = 'F' */
		short s;
	}
	puffer; 	/* Zugriff auf n-Wert:	i=wert.puffer.n[3] */
};


/* Spaltenbeschreibung fuer RO */

struct ro_spalte{
	char t; 		/* Tupelvariable */
	char name[9];		/* Spaltenname mit Blanks gefuellt */
	struct ro_ded ded;	/* Beschreibung der Spalte */
	struct ro_ded charded;	/* Beschreibung der Spalte im Zeichenkettenformat */
	struct ro_wert *lowerb,*upperb;  /* Grenzen fuer Spaltenwerte */
	/* wenn Schluesselbaum: */
	char *lowkey;		/* logisch kleinster Schluessel */
	char *highkey;		/* logisch groesster Schluessel */
	short *sortdescr;	/* Sortierfeldbeschreibungen fuer */
				/* max. 19 Spalten + Index (auch Sortierfeld) */
};

/* Informationen fuer Realisierung VIEW */

struct ro_view{
	char viewspalten[RO_MAXZEILE]; /* Spaltenliste fuer Verkettung ueber Schluessel */
	short vofflenliste[100]; /* -"-  bei SFILE mit Schluessel */
	int viewpol[100];  /* Pol. Not. fuer num. Ausdruck bei Verkettung ueber Index */
	char nexttv;  /* Tupelvariable fuer naechsten Dateibereich */
	char viewzeichen[RO_MAXZEILE]; /* View in Zeichendarstellung ( Kommando nach TO) */
};

struct ro_filter{
	char fcond[RO_MAXZEILE];  /* Filter in Zeichendarstellung */
	int filterpol[100];  /* Filter in Pol. Notation */
};

/* Fuer jede eroeffnete Datei wird ein Dateibereich angelegt.
Dieser kann auch vom Nutzer gelesen werden. */

struct ro_dateibereich{
	int lendateibereich;
	long anzzeilen; /* Anzahl der zugewiesenen Zeilen einer TD */
	long index;	/* zuletzt benutzter Zeilenindex */
	long indexalt;	/* vorher benutzter Index */
	char modus;	/* Modus der Eroeffnung R, U, L */
	char reln[5];	/* log. Relationsname */
		/*  SFI - einfaches seq. File mit Metadaten */
	char filespec[RO_MAXFILSPEC];	/* phys. Name der TD */
	int anzspalten; 		/* Anzahl der Spalten der TD */
	struct ro_spalte *spalten;	/* -> Liste aller Spalten der TD */
	struct rb_keytree *aktschluessel;/* -> Schluesselbaum, ueber den der
					   augenblickliche Zugriff erfolgt */
	char *wertaktschluessel;	/* zuletzt benutzter Schluesselwert der
					   aktiven Schluesselspalte */
	int lenfilepuffer;
	char *filepuffer;
	FILE *fp;		/* Filepointer fuer OPEX-Verarbeitung */
	int fd; 		/* Filedescriptor fuer SFILE */
	int anfdaten;		/* Byteoffset zum ersten Datensatz */
	struct ro_spalte *markspalte; /* Datensaetze zum Loeschen markiert */
	struct ro_filter *filter;  /* zugeordnter Filter */
	struct ro_view *view;	/* Informationen ueber Dateiverkettung */
	int found_flag; 	/* =1: FIND/LOCATE ergolgreich, =0: sonst */
				/* fuer Funktion FOUND() */
	int  *lastlocatepol;	/* Interndarst. der Bedingung aus LOCATE */
				/* merken. Wird bei CONTINUE gebraucht */
	int eof_flag;		/* =1: EOF erreicht, =0: sonst */
	char *indexliste;	/* Indexliste fuer 'kyupdate' */
	char *cur_indliste;	/* Arbeitspointer fuer Indexliste */
	struct rb_keytree *pkeytree;	/* 1. eroeffneter Schluesselbaum */
	char dfrei[8];		/* reserviert fuer die Zukunft */
};

/* Struktur mit allen Informationen fuer Schluesselbaumpflege */

struct rb_kyupdt{
	int maxukeylen; 	/* laengster Schluesselbegriff -> malloc */
	FILE *fpdel;		/* Hilfdatei fuer DEL-Schluessel */
	struct ro_spalte *upkey[20];	/* -> Schluesselspalten, die akt. werden muessen */
	int *changeind; 	/* -> Indezes der veraenderten Zeilen */
	char kfrei[40]; 	/* Reserve */

	/* Puffer Hilfsdatei */
	char keyname[4];	/* Name Schluesselbaum */
	long delindex;		/* Index fuer DEL-Schluessel */
	char keypuffer[2];	/* Schl. entsp. malloc(sizeof(struct rb_kyupdt)+maxukeylen) */
};

/* Struktur fuer Metadatenbereich eines SFILE */

struct ro_fcontrol{
	int fsatzlaenge;	/* Laenge eines Datensatzes in Byte */
	int fanzspalten;	/* Anzahl der Spalten */
	int fanfdaten;		/* Offset zum ersten Datensatz */
	int fanfwertetabellen;	/* Offset zur ersten Wertetabelle */
	int flenwertetabellen;	/* Laenge aller Wertetabellen */
	int fanzzeilen; 	/* Anzahl der Zeilen im SFILE */
	char keypfile[10][RO_MAXFILSPEC]; /* Filespezifik. fuer 10 Schluesselbaeume */
	struct ro_spalte keyspalte[10]; /* Spaltenbeschreiber fuer 10 SS */
	char ffrei[20]; 	/* Reserve */
};


/* SET Funktionen fuer Relationale Operatoren */

struct ro_set{


	int skyupdt;	 /* Schluesselbaumpflege ein/aus */

	/* SET-Funktionen entsprechend dBASE III */

	int alternate;
	FILE *fpalternate;  /* Filepointer fuer Alternatedatei */
	int bell;
	int carry;
	char color[30];     /* Farbtafel in Zeichenkettenform */
	int confirm;
	int console;
	int debug;
	int decimals;
	char sdefault[RO_MAXFILSPEC];
	int deleted;	/* 0 = Zeile nicht geloescht */
	int sdelimiter;  /* 1.Byte = linkes Begrenzungszeichen
			   2.Byte = rechtes Begrenzungszeichen
			   0 = Standard  */
	int device;
	int echo;
	int escape;
	int exact;
	/* aktueller FILTER wird im zugeordneten Dateibereich abgelegt */
	int fixed;
	FILE *format;  /* Formatdatei fuer privaten Bildaufbau */
	/* function	--> wird sofort abgearbeitet */
	int  heading;
	int help;
	/* index	--> wird sofort abgearbeitet */
	int intensity;
	int margin;  /* Anzahl der eingerueckten Zeichen */
	int menus;
	/* path 	--> wird sofort abgearbeitet */
	int print;
	FILE *fpprint;	/* Drucker wird auf Datei PRINT.TXT abgebildet */
	char *procedure;  /* Pointer auf Speicherbereich mit Kommandos */
	/* relation	--> wird sofort abgearbeitet */
	int safety;
	int graphic;
	int step;
	int talk;
	int unique;
	int century;
	int set_frei[20];   /* for future use */
};


/* Zentrale Verarbeitungstabelle */

struct ro_zvt{
	int rostat;	/* Statuscode intern */
	int roconv;	/* Statuscode fuer Konvertierungsfehler */
	char zident[30];  /* IDENT-Information und Schutzidentifikator */
	char user[30];	/* Nutzeridentifikation */
	struct ro_dateibereich *tt['Z'-'A'+1]; /* Adressen eroeffnete
	Dateibereiche -- portabel angegeben ESER -> Luecken bleiben frei */
	int  *pstack;	/* Stack fuer Auswertung von Ausdruecken */
	int  *ptop;	/* Stackpointer fuer STACK */
	int  *ppolnot;	/* Bereich fuer Ausdruck in Polnischer Notation */
	int  *ppoltop;	/* Pointer fuer POLNOT */
	/* globale Paramerter fuer Arbeit mit LEX */
	char lext;	/* Vorgabe der Tupelvariable fuer Spaltenname */
	/* oder Uebergabe selektierter Dateibereich bei r_set */
	struct ro_spalte *lexspalte;  /* -> Spalte, die zuletzt von Lex erkannt wurde */
	int lexlen;	/* Laenge des zuletzt erkannten Symbols */
	struct ro_set set;
	char *error;	/* Bereich fuer Fehlerinformationen */
	int lenerror;	/* Laenge des Bereiches */
	int eofflag;	/* Dateiende fuer Funktion EOF() merken */
	int stopflag;	/* bei >0 Kommandoabbruch */

	/* Realisierung der formatierten E/A */
	int getanz;	/* Anzahl der zu realisierenden 'GETs' */
	struct rd_stfeaget *firstget;	/* ->: ersten 'GET'-Befehl */
	struct rd_stfeaget *lastget;	/* ->: letzten 'GET'-Befehl */
	int fearow;	/* aktuelle Display-Zeilennummer */
	int feacol;	/* aktuelle Display-Spaltennummer */
	int term_init_flag;   /* =1: Display initialisiert (sonst 0) */
	int tql_menue;	/* =1: Standardmenue aktiv */
	int printf_anz; /* Anzahl der abgesetzten printf-Aufrufe */
	int flagwhile;	/* Flag fuer While */
	int fea_intern; /* =1: Aufruf der fm-Moduln intern, 0: extern */
	int zrecconv;	/* =1: '$RECCONV' mittels _defd definiert, 0: sonst */
	char ztbyte;	/* Voruebersetzungsbyte bei Kommandos aus Programm */
			/* -1	  => Kommando konnte nicht uebersetzt werden */
			/* QT_XXX => Num.Code des Schluesselwortes */
	char *zskipfalse; /* Adr. des Kommandos, welches bei .FALSE. abzu-
			     arbeiten ist , zeigt auf den Vorsatz des Kdo. */
	char *zskiptrue; /* Adr. des Kommandos, welches bei .TRUE. abzu-
			     arbeiten ist , zeigt auf den Vorsatz des Kdo. */
	char *zpolsave; /* Adr. des POLNOT fuer das aktuelle Kdo. */
	int simarkupdt; /* Anfangslaenge der Bitleiste in MARKUPDT fuer SFI */
	int readmessage;/* =1: Fehlermeldung auf Bildschirm, =0: sonst */
	char *memlevel[QC_MAXPGR]; /* Adr. der Hash-Tabellen fuer die Speicher-
				      variablen pro Programmebene */
	char zidende[16]; /* Endemarkierung ***Ende ZVT*** fuer Test */
	char zfrei[84]; /* Reserve fuer weitere Eintragungen */
};


/* GET-Struktur zur Realisierung der 'GET'-Funktion */

struct rd_stfeaget {
	char gettyp;			/* = 'M': Speichervariable (Memory) */
					/* = 'S': Spaltenname */
	char *getvar;		/* Pointer auf Wertstruktur der SP-Variablen */
				/* bzw. Pointer auf Spaltenstruktur */
	int getbuflen;			/* Laenge des  Eingabepuffers */
	int getoriglen; 		/* Laenge einer Spalte als String */
	int getrow;			/* Zeilennummer */
	int getcol;			/* Spaltennummer */
	struct rd_stfeaget *flastget;	/* vorhergehendes 'GET' */
	struct rd_stfeaget *fnextget;	/* nachfolgendes 'GET' */
	short getrange; 		/* =1: Range angegeben, =0: sonst */
	struct ro_wert *getlower;	/* untere Grenze (Range) */
	struct ro_wert *getupper;	/* obere Grenze (Range) */
	char getpic[81];		/* Picture (max. 80 Zeichen und '\0') */
	char get_char[2];		/* Charakter-Darstellung von Zahlen */
					/* bzw. Puffer fuer Spaltenwerte */
					/* bzw. Puffer f. Picture-Aufbereitung*/
					/* nach hinten laenger */
};

/* Strukturen zur Arbeit mit Memory */

# define RM_MEMLEN sizeof(struct rm_memtab)

struct rm_memtab
{
	char mname[10]; /* Speichername */
	short mlen;	/* Laenge des groessten zugewiesenen Speicherwertes */
	int moff;	/* Offset d. Speicherwertes relativ zum Speicheranfang*/
	int mprot;	/* = 0, falls Speichervariable nicht geschuetzt
			   = Programmstufe, in der Speichervariable geschuetzt*/
};

struct rm_tabhead
{
	char *zmemory;	/* -> Speicher zur Abspeicherung der Speicherwerte */
	int maxmemory;	/* maximale Laenge des vorhandenen Speichers (Bytes) */
	int lenmemory;	/* Laenge des belegten Speicherplatzes (Bytes) */
	int manz;	/* Anzahl der in der Hash-Tabelle abgelegten Namen */
	short maxanz;	/* maximale Anzahl der ablegbaren Speichernamen */
};

/* Struktur fuer Metadaten eines Schluesselbaumes */

struct rb_keymeta {
	char km_spalten[88];	/* einzelne Spaltennamen (+ 'END.') */
	int km_spanz;		/* Anzahl der einfliessenden Spalten */
	int km_lenkey;		/* Laenge eines Schluesselseintrages (mit Index) */
	int km_anzkey;		/* Anzahl der Schluesseleintraege */
};

#define KEYMETA  sizeof(struct rb_keymeta)

/* Struktur fuer einen Schluesselbaum */

struct rb_keytree {
	int kfd;		/* Filedescriptor */
	char kf_name[256];	/* vollstaendiger Filename */
	char kspliste[88];	/* Spaltenliste fuer Schluesselbaum */
	int kspanz;		/* Anzahl der Spalten im Schluesselbaum */
	int klenkey;		/* Laenge eines Schluessels (mit Index) */
	int kanzkey;		/* Anzahl der Schluesseleintraege */
	char *klowkey;		/* logisch kleinster Schluessel */
	char *khighkey; 	/* logisch groesster Schluessel */
	short *ksortdescr;	/* Sortierfeldbeschreibung */
	int khsflag;		/* =0: Filearbeit, =1: HS-Arbeit */
	char *khsbuffer;	/* Puffer fuer HS-Arbeit */
	int klastoffset;	/* letzter Offset im Schluesselbaum */
	struct rb_keytree *knexttree;	/* Verkettung */
	char kfrei[16]; 	/* reserviert fuer die Zukunft */
};

