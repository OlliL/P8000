/* IDENT: 100359 - roerr */

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
* Liste der verwendeten Fehlercodes fur Relationale Operatoren
*
*****************************************************************************/

/* Fehlercodes bei fehlerhafter Abarbeitung eines DML-Makro EDxxxx */

# define EDCELM 70	
# define EDRETR 71	
# define EDSTOR 72	
# define EDSEQR 73	
# define EDKYAD 74	
# define EDKYDF 75	
# define EDCLOS 76	
# define EDRDMD 77	
# define EDDEFN 78	
# define EDRDCD 79	
# define EDCDED 80	
# define EDRDVL 81	
# define EDFCTL 82	
# define EDOPN  83	
# define EDOPNR 84	
# define EDOPNU 85	
# define EDKYRT 86	
# define EDDELS 87	
# define EDNXTK 88	
# define EDSGTW 89	
# define EDDELI 90	
# define EDSCAN 91	
# define EDWTVL 92
# define EDWTUA 93
# define EDRDUA 94
# define EDCREA 95
# define EDRENA 96
# define EDREFM 97
# define EDCREC 98


/* Fehlercodes der Relationalen Operatoren im Parameter RC */

# define TVOPEN  1	/* TV ist bereits eroeffnet */
# define ARGERR 2        /* Argumentfehler in Standardfunktion */
# define EOPNN 3         /* Argument d. einstelligen Operators EINMINUS ist nicht numerisch */
# define EOPNL 4         /* Argument d. einstelligen Operators NICHT ist nicht logisch */
# define NOOVERWRITE 5   /* Datei nicht zum ueberschreiben freigegeben */
# define TERMBUF 6       /* Terminalpuffer ist zu klein */
# define OFDATA 7	/* Datenueberlauf bei Angleichung der Dezimalstellenzahl */
# define NOTV 8		/* TV kein Buchstabe */
# define UNTV 9		/* bei qualifizierten Namen wurde falsche TV angegeben */
# define SYNTAXLEX 10	/* Syntaxfehler durch LEX festgestellt */
# define KEYDUP 11	/* Schluesselspalte ist bereits definiert */
# define NOKOMMA 12	/* es wird eine Liste erwartet, Komma als Trennzeichen nicht gefunden */
# define NOKLAZ 13	/* schliessende Klammer wird erwartet - nicht gefunden */
# define NOMEMORY 14	/* malloc kann keinen weiteren Speicherplatz zuordnen */
# define OFPOLNOT 15	/* Bereich fuer Interndarstellung von Ausdruecken ist voll */
# define NOZLISTE 16	/* ungueltige Zuordnungsliste */
# define NOCOND 17	/* ungueltige Bedingung */
# define NOLOCATE 18	/* CONTINUE gegeben ohne vorausgehendes LOCATE */
# define NOAUSDR 19	/* ungueltiger Ausdruck */
# define NOCONV 20	/* Konvertierung nicht unterstuetzt */
# define TVNDEF 21	/* TV nicht eroeffnet */
# define NOINIT 22	/* kein INIT-Operator abgearbeitet oder falsche ZVT-Adresse angegeben */
# define FPAR 23		/* ungueltiger Parameter */
# define EFGETS 24	/* Fehler beim Lesen eines seriellen Files */
# define NOPOLNOT 25	/* Fehler bei Interndarstellung von Ausdruecken */
# define EFPUTS 26	/* Fehler beim Schreiben in ein serielles File */
# define DIVBYZERO 27	/* Gleitkomma-Division durch Null */
# define EFOPEN 28	/* Fehler beim Eroeffnen eines seriellen Files */
# define NOSORTNAME 29	/* ungueltiger Name eines Schluesselbaumes angegeben */
# define SRTDUP 30	/* Indexspalte mit gleichem Namen bereits definiert */
# define COLDUP 31	/* Spalte ist bereits definiert (CREA) */
# define NOSORTLI 32	/* falsche Sortliste bei PRNT angegeben */
# define GETINDEX 33	/* kein Ausgabe-Index gefunden */
# define NOZKAUSDR 34	/* kein Zeichenkettenausdruck  */
# define NOAFKTLI 35	/* fehlerhafte Liste von Aggregationsfunktionen */
# define NOSPALI 36	/* fehlerhafte Liste von Spaltennamen */
# define NOOUTCOL 37	/* Spaltenname im Dateibereich fuer Ausgabe nicht enthalten */
# define NOMDMKOM 38	/* Eingabedatei fuer WTMD enthaelt falsches Kommando */
# define NOMDMENT 39	/* Entity im MDM nicht gefunden */
# define NONUMCOL 40	/* keine num. Spalte -> AGGR nicht moeglich */
# define INVSPNAME 41	/* 1. Zeichen des Spaltennamen ist kein Alphazeichen sein */
# define EKYUPDT 42	/* Fehler bei Aktualisierung Schluesselbaum */
# define NOSETFKT 43	/* keine Set-Funktion */
# define NOKEYCOL 44	/* Schluesselspalte nicht definiert */
# define NOMEMNAME 45	/* Speichername nicht gefunden */
# define NOAKTKEY 46	/* kein AKTKEY fuer FIND gesetzt */
# define KABBRUCH 47	/* definierter Kommandoabbruch */

/*-------------------------------------------------------------VAX----------*/

# define SYNTAX 48       /* Syntaxfehler im Kommando */
# define NOIMPL 49	/* Funktion noch nicht implementiert */
# define EWRITE 50	/* Fehler bei WRITE in SFILE */
# define EREAD 51	/* Fehler bei READ aus SFILE */
# define ELSEEK 52	/* Fehler bei LSEEK im SFILE */
# define EOPEN 53	/* Fehler bei OPEN eines SFILE */
# define ECREAT 54	/* Fehler bei CREAT eines SFILE */

/*-------------------------------------------------------------SORTIERUNG---*/

# define SORTCCT 55	/* Can't Create Temp-File (zum Schreiben) */
# define SORTCRT 56	/* Can't Reopen Temp-File (jetzt zum Lesen) */	
# define SORTSOF 57	/* Stack-Over-Flow */
# define SORTUEOF 58	/* Undefines End Of File (Temp.sort) */
# define SORTWERR 59	/* Fehler beim Schreiben auf des temporaeren Files */
# define SORTFPOR 60	/* Field-Position Out of Range (bei Sort.-feldbeschr.)*/
# define SORTFLOR 61	/* Field-Length Out of Range */
# define SORTSYN  62	/* fehlerhafte Aufruffolge so_srta(), so_srto() */

/*---------------------------------------------------------FORMATIERTE E/A---*/

# define FMERRSC 63	/* Fehler bei Positionieren des Cursors */
# define FMERRIN 64	/* Fehler bei Einlesen einer Zeichenkette */
# define FMERROU 65	/* Fehler bei Ausgabe einer Zeichenkette */
# define FMERRCL 66	/* Fehler bei Loeschen des Bildschirms */
# define FMERRCR 67	/* Fehler bei Initialisierung des Terminals */
# define FMNOINI 68	/* Terminal nicht fuer Full-Screen initialisierbar */

/*---------------------------------------------------------NEUE UNIXFEHLER --*/

# define NOSFI 69	/* KEINE TQL-DATENBANKDATEI */


/*---------------------------------------------------------------------------*/


# define INTERN 777	/* interner Fehler */
 
/*--------------------------------------------------WEITERE TQL MELDUNGEN----*/ 

# define FRUNVIEW 101	/* Fehler bei Abarbeitung einer SET RELATION - Kette */
# define FDEFVIEW 102	/* Fehler bei Definition eines View (SET RELATION) */
# define NOGETS   103	/* Keine GET-Variablen im READ-Zugriff */
# define FALSE_PARM 104	/* Syntaxfehler - falsche Parameteranzahl */
# define MAXEXTKEY 105	/* Max. 10 externe Zugriffshilfen zugelassen */ 
# define TVEOF 106	/* Ende der Topas-Datei -> kein _retr mehr moeglich */
# define OTOPCOM 107	/* Kommando nur fuer TOPAS-Datei implementiert */
# define SCANWIK 108	/* SCAN mit aktiven Schluesselbaum aufgerufen */
# define FSPSTRU 109 	/* Spaltenstruktur ist fehlerhaft */
# define NOMARKSPALTE 110  /* keine Markierungsspalte in der Datenbank */
			  /* ==> kein DELETE bzw. RECALL - Kommando moeglich */
# define NOSTRUDBF 111	/* Datenbankfile ist kein Struktur-Datenbankfile */
# define NOSAVEFILE 112	/* keine von TQL erstellte SAVE-Datei angegeben */
			/* ==> kein RESTORE-Kommando moeglich */
# define	TVREAD 113	/* TOPAS-Datei mit Modus READ eroeffnet */
			/* -> Aenderungs-Kommandos nicht moeglich */
# define NOFLOAT 114	/* Funktion nicht fuer Parameter vom Typ Gleitkomma */
# define NODATE  115	/* das angegebene Datum ist kein gueltiges Datum */
# define OTHAKTKEY 777   /* hat Buche sich ausgedacht ???????? */
