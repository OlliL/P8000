


				  - 1 -




		   IIIINNNNGGGGRRRREEEESSSS ---- KKKKOOOOMMMMPPPPAAAATTTTIIIIBBBBLLLLEEEE DDDDAAAATTTTEEEENNNNBBBBAAAANNNNKKKKSSSSYYYYSSSSTTTTEEEEMMMMEEEE

			      B.-M. Paschke
			       J. Wenzlaff




       1.  EEEEiiiinnnnlllleeeeiiiittttuuuunnnngggg

       Datenbanken sind	Systeme	zur Beschreibung,  Speicherung	und
       Wiedergewinnung	 von  umfangreichen  Datenmengen,  die	von
       mehreren	Anwendungsprogrammen genutzt werden.  Sie  befreien
       den Nutzer vom Umgang mit der physischen	Struktur der Daten.
       Bei mehrnutzerfaehigen Systemen regeln  sie  den	 parallelen
       Zugriff	auf den	gleichen Datenbestand und gewaerleisten	die
       Wiederherstellbarkeit  der  Datenintegritaet   bei   System-
       fehlern.
       Eine Aenderung  der  Datenbankstruktur  erfordert  nicht	 in
       jedem  Fall eine	Aenderung der Anwenderprogramme.
       Ist die Datenbanksprache	in eine	hoehere	 Programmiersprache
       eingebettet,  wird  dem Programmierer die Moeglichkeit gege-
       ben,  die  Dateiarbeit  rechnerunabhaengig   zu	 gestalten.
       Dadurch	wird  die  Portabilitaet  von Programmen wesentlich
       verbessert.  Entwicklungs-  und	Zielrechner  muessen  nicht
       identisch sein.

	    INGRES ist international eines der bedeutendsten Daten-
       banksysteme  (DBS).   Es	 wird von sieben der zehn groessten
       Industrieunternehmen der	 USA  benutzt  und  ist	 mit  einem
       Marktanteil von 36 Prozent /1/ Marktfuehrer bei relationalen
       Datenbanksystemen  fuer	VAX-Rechner.  In der BRD wurde 1987
       eine INGRES-Benutzergruppe /2/ mit mehr als  50	Grossanwen-
       dern gegruendet.
       Es kann eingeschaetzt werden, dass es sich bei INGRES um	das
       fortschrittlichste  relationale	DBS handelt. 50	Prozent	der
       Gewinne	von  Relational	 Technology  (RTI)  werden  in	die
       weitere Forschung investiert.






       __________

       /1/ UNIX	MAGAZIN	5/88 S.	57-58

       /2/ unix/mail 4/87 S. 12












				  - 2 -



       2.  VVVVeeeerrrrffffuuuueeeeggggbbbbaaaarrrrkkkkeeeeiiiitttt vvvvoooonnnn IIIINNNNGGGGRRRREEEESSSS----kkkkoooommmmppppaaaattttiiiibbbblllleeeennnn DDDDBBBBSSSS iiiinnnn	ddddeeeerrrr DDDDDDDDRRRR

       In der DDR steht	z. Z. fuer alle	Rechner	 mit  einer  Verar-
       beitungsbreite >= 16 Bit	ein INGRES-kompatibles DBS zur Ver-
       fuegung.
       Es handelt sich dabei in	allen Faellen um  Systeme,  die	 im
       wesentlichen  von  J.  Wenzlaff aus dem Zentralinstitut fuer
       Kybernetik und Informationsprozesse (ZKI)  erstellt  wurden.
       Ihm steht der Quelltext des Datenbanksystems INGRES zur Ver-
       fuegung,	der von	ihm auch staendig  gewartet  und  erweitert
       wird.   Sie  repraesentieren, im	Vergleich zum INGRES, einen
       Stand von 1984. Alle Systeme sind mehrplatzfaehig.

	Rechner		      Betriebssystem  Name des DBS
	---------------------------------------------------------
	BC 5120.16	      MUTOS 8000      ING_DB

	A 7150,	EC 1834,      DCP	      INDES
	IBM XT / AT	      MS-DOS	      INDES
			      VENIX, XENIX    ING_DB

	P 8000		      WEGA	      ING_DB

	K 1630,	SM 4, I	100   MUTOS	      ING_DB

	SM 1420, SM 52/11     UNIX	      ING_DB

	HP 9000		      HP-UX	      ING_DB

	AT 386		      XENIX 386	      ING_DB

	K 1840,		      MUTOS 1800      DABA 32
	VAX-Rechner	      ULTRIX	      ING_DB
			      UNIX (BSD	4.x)  ING_DB
			      SVP bzw. VMS    Entwicklung geplant

	ESER-Rechner	      VMX	      ING_DB
			      PSU	      z.Z. in Entwicklung

       Der Vertrieb erfolgt z. Z. durch	verschiedene Einrichtungen.
       ING_DB  wird vom	ZKI, INDES von der NVA und DABA	32 vom Kom-
       binat ROBOTRON /3/ angeboten.  Es ist vorgesehen, dass  alle
       Systeme	in Zukunft vom Kombinat	ROBOTRON vertrieben werden.
       Entsprechende Verhandlungen stehen kurz vor dem Abschluss.



       __________

       /3/ Neue	Technik	im Buero 3/88 S. 84-86












				  - 3 -



       3.  MMMMooooeeeegggglllliiiicccchhhhkkkkeeeeiiiitttteeeennnn zzzzuuuurrrr DDDDaaaatttteeeennnnuuuueeeebbbbeeeerrrrnnnnaaaahhhhmmmmeeee aaaauuuussss aaaannnnddddeeeerrrreeeennnn SSSSyyyysssstttteeeemmmmeeeennnn

       Zur Uebernahme von Daten	 aus  anderen  Systemen	 steht	ein
       leistungsfaehiges  Kopierkommando zur Verfuegung. Es ermoeg-
       licht nicht nur die Uebernahme von Daten	aus  ASCII-Dateien,
       sondern	auch das Ueberspielen von Integer- bzw.	Gleitkomma-
       zahlen.	Zur Uebernahme von dBASE-Dateien wird in Kuerze	ein
       spezielles Kopierprogramm zur Verfuegung	stehen.
       Da die  Moeglichkeit  besteht,  Datenbankanweisungen  in	 C-
       Programme  zu  integrieren (siehe Abschnitt Programmierung),
       koennen Daten problemlos	in jedes beliebige  Format  konver-
       tiert werden.


       4.  PPPPrrrrooooggggrrrraaaammmmmmmmiiiieeeerrrruuuunnnngggg uuuunnnndddd AAAArrrrbbbbeeeeiiiitttt mmmmiiiitttt ddddeeeerrrr DDDDaaaatttteeeennnnbbbbaaaannnnkkkk

       Zur  Nutzung  der  Datenbank  stehen  verschiedene   Nutzer-
       schnittstellen zur Verfuegung.

	    QUEL   - QUEry Language
	    EQUEL  - Embedded QUEL interface to	C
	    AMX	   - Access Method - an	eXtended interface
	    RC,	RG - ReportCompiler, ReportGenerator

       Die Sprache QUEL	wird nicht nur fuer Anfragen an	die  Daten-
       bank  benutzt,  sie ermoeglicht auch jegliche Manipulationen
       in der Datenbank.  Die Query-Kommandos  werden  ueber  einen
       interaktiven  Terminal-Monitor  eingegeben,  dann  an  einen
       Makro-Prozessor weitergeleitet und  gelangen  von  dort	zum
       Datenbanksystem.
       Es besteht die Moeglichkeit, den	Monitorpuffer  durch  einen
       beliebigen  im  Betriebssystem  verfuegbaren  Texteditor	 zu
       bearbeiten. Weiterhin koennen Kommandofolgen in einer  Datei
       abgelegt	und jederzeit in den Monitorpuffer geladen werden.
       Da mehrere Kommandos zu einer Query zusammengestellt  werden
       koennen,	 Eingaben  und Verzweigungen moeglich sind, besteht
       mit QUEL	bereits	die Moeglichkeit der Programmierung.

	    Da aber viele Anwendungen den Einsatz komplexerer  Pro-
       gramme  erfordern,  wird	 mit EQUEL die Moeglichkeit bereit-
       gestellt,  QUEL-Kommandos  in  C-Programme  zu  integrieren.
       Dabei  stehen  alle  QUEL-Kommandos zur Verfuegung. Variable
       Angaben	in  den	 EQUEL-Kommandos  koennen   aus	  C-String-
       Variablen  gelesen  werden.  Das	 Ergebnis  eines  retrieve-
       Kommandos wird in C-Variablen abgelegt.	Damit besteht keine
       Einschraenkung  bei der Nutzung der Datenbank auf eine reine
       Datenbanksprache.

	    Fuer  sich	staendig  wiederholende	 Aufgaben  mit	der
       gleichen	 Datenbank  steht  der AMX-Compiler zur	Verfuegung.
       Mit ihm koennen sehr schnelle und kurze	Programme  erstellt











				  - 4 -



       werden,	da  die	 Zugriffsmethoden  des DBS fest	in die Pro-
       gramme integriert werden. Die  Auswertung  des  Aufbaus	der
       einzelnen Relationen (Tabellen) erfolgt bereits waehrend	der
       Uebersetzung, so	dass zur Laufzeit nicht	mehr das DBS benoe-
       tigt  wird. Dies	hat zur	Folge, das AMX-Programme nach einer
       Veraenderung der	Struktur benutzter  Relationen	neu  ueber-
       setzt werden muessen.

	    Der	 Reportgenerator   stellt   ein	  leistungsfaehiges
       Hilfsmittel  zur	 Aufbereitung von Listen zur Verfuegung. Er
       benutzt eine eigene "Programmiersprache", die von einem Pro-
       grammierer  sicherlich  schnell	zu  erlernen, aber fuer	den
       EDV-Laien nicht handhabbar ist.	Dafuer bietet er alles,	was
       man sich	von einem Reportgenerator wuenscht.

	    Viele relationale  Datenbanksysteme	 stellen  ein  SQL-
       Interface  zur  Verfuegung. Damit wird der Eindruck erweckt,
       dass  SQL  bereits  eine	 Standardsprache  fuer	relationale
       Datenbanken darstellt.
       Bei den hier genannten Systemen steht  diese  Sprache  nicht
       zur  Verfuegung.	Die Sprache QUEL ist in	seinen wesentlichen
       Eigenschaften mit SQL vergleichbar und bietet alle Moeglich-
       keiten einer modernen Datenbanksprache.
       Bender und Bien weisen in ihrem Artikel "Kriterien fuer	die
       Auswahl von Datenbanksystemen in	der Sozialwissenschaft"	/4/
       darauf hin, dass	die einzelnen SQL-Pakete nur bedingt kompa-
       tibel sind.  Da somit  SQL-Programme  nur  bedingt  portabel
       sind,  besteht  bis zur Erarbeitung eines verbindlichen SQL-
       Standards  keine	 Notwendigkeit	zur  Bereitstellung  dieser
       Sprache.


       5.  SSSSppppeeeeiiiicccchhhheeeerrrrssssttttrrrruuuukkkkttttuuuurrrreeeennnn ffffuuuueeeerrrr RRRReeeellllaaaattttiiiioooonnnneeeennnn

       Die Effizienz eines  DBS	 haengt	 im  wesentlichen  von	den
       benutzten  Speicherstrukturen  ab.  Bei	den  hier genannten
       Systemen	stehen folgende	Speicherstrukturen zur Auswahl:

	    heap - unstrukturiert und ohne Schluessel
	    isam - index sequentielle Struktur
	    hash - hash	Struktur

       Es besteht fuer alle drei Speicherstrukturen  die  Moeglich-
       keit,  Zeichenketten  in	 verdichteter Form abzulegen, d. h.


       __________

       /4/ ZA-Informationen 23 (November 1988) S. 113-118
	   Universitaet	zu Koeln












				  - 5 -



       nachlaufende Leerzeichen	werden nicht abgespeichert.

	    Zu jeder Relation koennen  eine  beliebige	Anzahl	von
       Sekundaerindizes	bestehen. Diese	koennen	wieder in einer	der
       drei Speicherstrukturen organisiert sein.

	    Vom	Datenbanksystem	wird fuer jede Anfrage der optimale
       Zugriffspfad ausgewaehlt.


       6.  GGGGrrrreeeennnnzzzzwwwweeeerrrrtttteeee uuuunnnndddd FFFFoooorrrrmmmmaaaatttteeee ddddeeeerrrr DDDDoooommmmaaaaeeeennnneeeennnn

       Jede Datenbank kann aus einer beliebigen	 Anzahl	 von  Rela-
       tionen  bestehen.  Die Zahl der gleichzeitig benutzten Rela-
       tionen  wird  nur   durch   das	 jeweilige   Betriebssystem
       beschraenkt. Ebenso ist die Groesse einer Relation durch	die
       maximale	 Groesse  eines	 Dateisystems  auf  dem	  benutzten
       Rechner	eingeschraenkt.	  Die Anzahl der Domaenen (Spalten)
       fuer eine Relation ist fuer 16-Bit-Rechner auf 49 bzw.  fuer
       32-Bit-Rechner  auf  128	 begrenzt.  Die	maximale Laenge	der
       Tupel (Zeilen) betraegt 1010 Bytes.  Fuer die  Abspeicherung
       der Domaenen koennen folgenden Formate gewaehlt werden:

	    c1 - c255  Zeichenketten von 1-255 Bytes Laenge
	    i1	       1-Byte Integer
	    i2	       2-Byte Integer
	    i4	       4-Byte Integer
	    f4	       4-Byte floating
	    f8	       8-Byte floating

       Numerische Domaenen  werden  mit	 Null  initialisiert.	Die
       Behandlung  von	nicht  vorhandenen Werten bleibt dem Nutzer
       ueberlassen.

	    Die	genannten Grenzwerte  werden  oftmals  als  zu	eng
       bewertet.  Da die maximale Laenge einer Zeichenkette auf	255
       Bytes beschraenkt ist, erscheint	 es  empfehlenswert,  Texte
       zeilenweise    abzuspeichern    bzw.   eine   Relation	mit
       Textbausteinen (z. B. Saetzen)  anzulegen  und  ueber  einen
       Schluessel mit anderen Relationen zu verbinden.
       Sollte jedoch jemand mehr als 49	bzw. 128 Domaenen in  einer
       Relation	benoetigen, ist	er in den meisten Faellen gut bera-
       ten, wenn er sein Datenbankmodell  noch	einmal	ueberdenkt.
       Gegebenenfalls	koennen	  mehrere  Relationen  ueber  einen
       Schluessel miteinander verbunden	werden.

















				  - 6 -



       7.  ZZZZuuuuggggrrrriiiiffffffffsssssssscccchhhhuuuuttttzzzz uuuunnnndddd DDDDaaaatttteeeennnnssssiiiicccchhhheeeerrrrhhhheeeeiiiitttt

       Voraussetzung fuer die Benutzung	 des  Datenbanksystems	ist
       ein  Eintrag  im	 Nutzerverzeichnis.  Dieser  legt  auch	die
       Rechte des Nutzers innerhalb des	Datenbanksystems fest.
       Fuer Relationen kann  der  Datenbankadministrator  Zugriffs-
       rechte vergeben.	Diese koennen sich auf bestimmte Kommandos,
       Zeitraeume und Terminals	beziehen.  Weiterhin ist  es  moeg-
       lich,  die Rechte der einzelnen Nutzer auf einzelne Domaenen
       und datenabhaengig sogar	auf einzelne Tupel einzuschraenken.

	    Zur	Sicherung der Integritaet der  Datenbank  bei  Sys-
       temabstuerzen  wird  garantiert,	 dass  jede Veraenderung am
       Datenbestand zuerst in eine Protokolldatei geschrieben wird,
       welches	erst  nach  erfolgreicher Aenderung geloescht wird.
       Fuer den	Fall eines Systemabsturzes waehrend einer  Aktuali-
       sierung steht ein "restore"-Programm zur	Verfuegung, welches
       die Integritaet der  Datenbank  gegebenenfalls  wieder  her-
       stellt.

	    Da die genannten Systeme mehrplatzfaehig sind,  besteht
       die  Notwendigkeit  einer  "Gleichzeitigkeitskontrolle".	 Es
       wird gesichert, dass bei	Aenderungen in einer  Relation	die
       betreffende  Seite bzw. die gesamte Relation fuer diese Zeit
       gesperrt	wird.


       8.  SSSSppppeeeeiiiicccchhhheeeerrrrbbbbeeeeddddaaaarrrrffff

       Die  Abspeicherung  der	Daten  auf  externen  Datentraegern
       erfolgt	nach einem Seitenkonzept. Jede Seite enthaelt Tupel
       einer Relation und zwar so, dass	jedes Tupel ganz auf  einer
       Seite untergebracht ist.	 Der Fuellfaktor einer Seite ist im
       wesentliche von der gewaehlten  Speicherstruktur	 abhaengig.
       Er  kann	 durch den Nutzer festgelegt werden.  Damit hat	der
       Nutzer die Moeglichkeit,	fuer  Relationen,  in  denen  keine
       bzw.    nur  wenige  Aenderungen	 zu  erwarten  sind,  einen
       hoeheren	Fuellfaktor anzugeben und damit	den  Speicherbedarf
       zu beeinflussen.	 Als vorteilhaft erweist sich die Tatsache,
       dass die	Daten entsprechend der gewaehlten  Speicherstruktur
       nach  ihrem Hauptschluessel sortiert in den Dateien abgelegt
       werden und somit	fuer den  jeweiligen  Hautschluessel  keine
       sekundaere  Indexdatei  notwendig ist.  Weiterhin ist darauf
       hinzuweisen, dass numerische Daten nicht	 als  Zeichenketten
       in  den Dateien gespeichert werden (wie es z. B.	in dem weit
       verbreiteten dBASE erfolgt).

	    Ein	weiterer Gesichtspunkt bei der Bewertung eines	DBS
       ist der Hauptspeicherbedarf bei Mehrplatzsystemen, sowie	der
       benoetigte externe Speicherplatz	fuer die Programme des DBS.
       Bei  den	hier genannten Systemen	wird ein externer Speicher-











				  - 7 -



       platz von 1,2 MByte (MS-DOS) bis	etwa 2 MByte (ESER)  benoe-
       tigt.  Es  besteht  also	 auch auf kleinen Systemen noch	die
       Moeglichkeit, Daten abzuspeichern.

	    Der	Hauptspeicherbedarf des	DBS ist	 als  ausgesprochen
       gering	einzuschaetzen.	   Auf	 einem	P  8000	 mit  einem
       Hauptspeicher von 1 MByte wurde von 3 Terminals gleichzeitig
       ING_DB  gestartet. Es konnte dabei keine	zusaetzliche Behin-
       derung der einzelnen Prozesse durch ein	notwendiges  "swap-
       pen"  festgestellt werden. Natuerlich war aber eine Verlang-
       samung der einzelnen Prozesse durch die	notwendige  Resour-
       centeilung  zu  bemerken. Ein sinnvolles	Arbeiten war jedoch
       noch moeglich.


       9.  LLLLiiiitttteeeerrrraaaattttuuuurrrr

       Zum Datenbanksystem ING_DB steht	 eine  umfangreiche,  stel-
       lenweise	 nicht	gerade	leicht verstaendliche Dokumentation
       zur  Verfuegung.	 Sie  zeichnet	sich  dadurch	aus,   dass
       insbesondere die	Speicherstrukturen und Zugriffsmethoden	des
       DBS  sehr  gut  dargestellt  werden.  Die  Entscheidung	des
       Nutzers fuer eine der moeglichen	Benutzerschnittstellen wird
       durch ein Kapitel ueber die Einbettung von Datenbanksprachen
       in   hoehere   Programmiersprachen   erleichtert.  Hilfreich
       erscheint auch  die  Tatsache,  dass  die  Arbeitsweise	des
       Makroprozessors	ausfuerlich  erlaeutert	 wird.	Dadurch	ist
       diese Dokumentation sehr	gut geeignet, die Arbeitsweise	des
       DBS  verstehen  zu  lernen und beim Entwurf des Datenbankmo-
       dells zu	beruecksichtigen.

	    In der Zeitschrift "Neue Technik im	Buero",	Heft 3/1988
       ist  ein	 Ueberblicksartikel von	Gruenhagen (DABA 32 - rela-
       tionales	Datenbanksystem	unter MUTOS 1800)  erschienen,	der
       in knapper Form auf die Moeglichkeiten von DABA 32 eingeht.

	    Zum	aktuellen  Entwicklungsstand  von  INGRES  ist	vom
       Vogel-Verlag  fuer  voraussichtlich  April  1989	 ein  Chip-
       Sonderheft geplant.

	    Das	Buch von Date "A Guide to  INGRES"  (Addison  Wesly
       1987) kann als Standardwerk bezeichnet werden.


       10.  ZZZZuuuussssaaaammmmmmmmeeeennnnffffaaaassssssssuuuunnnngggg

       In der DDR  steht  fuer	alle  16-  und	32-Bit-Rechner	ein
       INGRES-kompatibles  DBS	zur  Verfuegung. Alle diese Systeme
       sind in der Nutzerschnittstelle	und  im	 Leistungsvermoegen
       vollstaendig   identisch.    Der	  Uebergang  zwischen  ver-
       schiedenen Rechnern wird	dadurch	betraechtlich  vereinfacht.











				  - 8 -



       Da  im  ZKI  die	 Quellen  fuer das DBS vorhanden sind, kann
       davon ausgegangen werden, dass dieses DBS  auch	fuer  jeden
       exotischen  (ROBOTRON-)	Rechner	bereitgestellt werden kann.
       Damit besteht nicht die Abhaengigkeit vom Import	 westlicher
       Systeme.
       Die Systeme werden vom ZKI staendig aktualisiert	und  gewar-
       tet.
       Die    genannten	   DBS	  sind	  sehr	   leistungsstarke,
       mehrplatzfaehige	 Systeme,  die sich vor	allem fuer den pro-
       fessionellen Einsatz eignen. Fuer die Benutzung	durch  EDV-
       Laien   koennte	 man   sich  eine  komfortablere  Benutzer-
       schnittstelle  wuenschen.  Mit  EQUEL-  bzw.  AMX-Programmen
       koennen jedoch sehr leistungsfaehige und	benutzerfreundliche
       Bedienoberflaechen fuer den jeweiligen Einsatzfall  geschaf-
       fen werden.



	    Anschrift der Autoren:

	    B.-M. Paschke

	    Institut fuer Krankenhaushygiene
	    des	Bereiches Medizin (Charite)
	    der	Humboldt-Universitaet zu Berlin

	    Otto-Grotewohl-Str.1
	    Postfach 140
	    Berlin
	    1 0	4 0		     Tel. 2202411  App.	319



	    J. Wenzlaff

	    Zentralinstitut fuer Kybernetik
	    und	Informationsprozesse der
	    Akademie der Wissenschaften

	    Kurstrasse 30 - 34
	    Berlin
	    1 0	8 0		      Tel. 2072	322
























				 CONTENTS


	1.  Einleitung..........................................  1

	2.  Verfuegbarkeit von INGRES-kompatiblen DBS in der
	    DDR.................................................  2

	3.  Moeglichkeiten zur Datenuebernahme aus anderen
	    Systemen............................................  3

	4.  Programmierung und Arbeit mit der Datenbank.........  3

	5.  Speicherstrukturen fuer Relationen..................  4

	6.  Grenzwerte und Formate der Domaenen.................  5

	7.  Zugriffsschutz und Datensicherheit..................  6

	8.  Speicherbedarf......................................  6

	9.  Literatur...........................................  7

       10.  Zusammenfassung.....................................  7



































