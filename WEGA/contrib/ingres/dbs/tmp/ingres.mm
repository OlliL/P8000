.SA 1
.nr Hy 1
.HC ^
.nr Pt 2
.nr Oc 1
.DS C

.B "INGRES - KOMPATIBLE DATENBANKSYSTEME"

B.-M. Paschke
J. Wenzlaff


.DE
.H 1 Einleitung
Datenbanken sind Systeme zur Beschreibung, Speicherung und Wiedergewinnung
von umfangreichen Datenmengen, die von mehreren Anwendungsprogrammen
genutzt werden. Sie befreien den Nutzer vom Umgang mit der physischen
Struktur der Daten.
Bei mehrnutzerfaehigen Systemen regeln sie den parallelen Zugriff auf
den gleichen Datenbestand und gewaerleisten die Wiederherstellbarkeit
der Datenintegritaet bei Systemfehlern.
.br
Eine Aenderung der Datenbankstruktur erfordert nicht in jedem  Fall eine
Aenderung der Anwenderprogramme.
.br
Ist die Datenbanksprache
in eine hoehere Programmiersprache eingebettet, wird dem Programmierer
die Moeglichkeit gegeben, die Dateiarbeit rechnerunabhaengig zu
gestalten. Dadurch wird die Portabilitaet von Programmen wesentlich
verbessert. Entwicklungs- und Zielrechner muessen nicht identisch sein.
.P 1
INGRES ist international eines der bedeutendsten
Datenbanksysteme (DBS).
Es wird von sieben der zehn groessten Industrieunternehmen der USA
benutzt und ist mit einem Markt^anteil von 36 Prozent /1/
.FD 10 1
.FS /1/
UNIX MAGAZIN 5/88 S. 57-58
.FE
Marktfuehrer bei
relationalen Datenbanksystemen fuer VAX-Rechner. In der BRD wurde 1987
eine INGRES-Benutzergruppe /2/
.FD 10 1
.FS /2/
unix/mail 4/87 S. 12
.FE
mit mehr als 50 Grossanwendern gegruendet.
.br
Es kann eingeschaetzt werden, dass es sich bei INGRES um das
fortschrittlichste relationale DBS handelt. 50 Prozent der Gewinne von
Relational Technology (RTI) werden in die weitere Forschung investiert.
.H 1 "Verfuegbarkeit von INGRES-kompatiblen DBS in der DDR"
In der DDR steht z. Z. fuer alle Rechner mit einer Verarbeitungsbreite
>= 16 Bit ein INGRES-kompatibles DBS zur Verfuegung.
.br
Es handelt sich dabei in allen Faellen um Systeme, die im wesentlichen von
J. Wenzlaff aus dem Zentralinstitut fuer Kybernetik
und Informationsprozesse (ZKI) erstellt wurden.
Ihm steht der Quelltext des Datenbanksystems INGRES
zur Verfuegung, der von ihm auch staendig gewartet und erweitert wird.
Sie repraesentieren,
im Vergleich zum INGRES, einen Stand von 1984. Alle Systeme sind
mehrplatzfaehig.
.DS 3
Rechner               Betriebssystem  Name des DBS
---------------------------------------------------------
BC 5120.16            MUTOS 8000      ING_DB

A 7150, EC 1834,      DCP             INDES
IBM XT / AT           MS-DOS          INDES
                      VENIX, XENIX    ING_DB
                      
P 8000                WEGA            ING_DB

K 1630, SM 4, I 100   MUTOS           ING_DB

SM 1420, SM 52/11     UNIX            ING_DB

HP 9000               HP-UX           ING_DB

AT 386                XENIX 386       ING_DB

K 1840,               MUTOS 1800      DABA 32
VAX-Rechner           ULTRIX          ING_DB
                      UNIX (BSD 4.x)  ING_DB
                      SVP bzw. VMS    Entwicklung geplant
                      
ESER-Rechner          VMX             ING_DB
                      PSU             z.Z. in Entwicklung
.DE
Der Vertrieb erfolgt z. Z. durch verschiedene Einrichtungen. ING_DB wird vom
ZKI, INDES von der NVA und DABA 32 vom Kombinat ROBOTRON /3/
.FS /3/
Neue Technik im Buero 3/88 S. 84-86
.FE
angeboten.
Es ist vorgesehen, dass alle Systeme in Zukunft vom Kombinat
ROBOTRON vertrieben werden.
Entsprechende Verhandlungen stehen kurz vor dem Abschluss.
.H 1 "Moeglichkeiten zur Datenuebernahme aus anderen Systemen"
Zur Uebernahme von Daten aus anderen Systemen steht ein leistungsfaehiges
Kopierkommando zur Verfuegung. Es ermoeg^licht nicht nur die Uebernahme
von Daten aus ASCII-Dateien, sondern auch das Ueberspielen von
Integer- bzw. Gleitkommazahlen.
Zur Uebernahme von dBASE-Dateien wird in Kuerze ein spezielles Kopierprogramm
zur Verfuegung stehen.
.br
Da die Moeglichkeit besteht, Datenbankanweisungen in C-Programme zu
integrieren (siehe Abschnitt Programmierung), koennen Daten problemlos
in jedes beliebige Format konvertiert werden.
.H 1 "Programmierung und Arbeit mit der Datenbank"
Zur Nutzung der Datenbank stehen verschiedene Nutzerschnittstellen
zur Verfuegung.
.DS I
QUEL   - QUEry Language
EQUEL  - Embedded QUEL interface to C
AMX    - Access Method - an eXtended interface
RC, RG - ReportCompiler, ReportGenerator
.DE
Die Sprache QUEL wird nicht nur fuer Anfragen an die Datenbank benutzt,
sie ermoeglicht auch jegliche Manipulationen in der Datenbank.
Die Query-Kommandos werden ueber einen interaktiven Terminal-Monitor
eingegeben, dann an einen Makro-Prozessor weitergeleitet und gelangen von
dort zum Datenbanksystem.
.br
Es besteht die Moeglichkeit, den Monitorpuffer durch einen beliebigen
im Betriebssystem verfuegbaren Texteditor zu bearbeiten. Weiterhin koennen Kommandofolgen in einer
Datei abgelegt und jederzeit in den Monitorpuffer geladen werden.
.br
Da mehrere Kommandos zu einer Query zusammengestellt werden koennen,
Eingaben und Verzweigungen moeglich sind, besteht mit QUEL bereits
die Moeglichkeit der Programmierung.
.P 1
Da aber viele Anwendungen den Einsatz komplexerer Programme erfordern,
wird mit EQUEL die Moeglichkeit bereitgestellt, QUEL-Kommandos
in C-Programme zu integrieren. Dabei stehen alle QUEL-Kommandos zur
Verfuegung. Variable Angaben in den EQUEL-Kommandos koennen aus
C-String-Variablen gelesen werden. Das Ergebnis eines retrieve-Kommandos
wird in C-Variablen abgelegt.
Damit besteht keine Einschraenkung bei der Nutzung der Datenbank
auf eine reine Datenbanksprache.
.P 1
Fuer sich staendig wiederholende Aufgaben mit der gleichen Datenbank
steht der AMX-Compiler zur Verfuegung. Mit ihm koennen sehr schnelle und
kurze Programme erstellt werden, da die Zugriffsmethoden des DBS
fest in die Programme integriert werden. Die Auswertung des Aufbaus
der einzelnen Relationen (Tabellen) erfolgt bereits waehrend der Uebersetzung, so
dass zur Laufzeit nicht mehr das DBS benoetigt wird. Dies hat
zur Folge, das AMX-Programme nach einer Veraenderung der Struktur benutzter
Relationen neu uebersetzt werden muessen.
.P 1
Der Reportgenerator stellt ein leistungsfaehiges Hilfsmittel
zur Aufbereitung von Listen zur Verfuegung. Er benutzt eine
eigene "Programmiersprache", die von einem Programmierer sicherlich
schnell zu erlernen, aber fuer den EDV-Laien nicht handhabbar ist.
Dafuer bietet er alles, was man sich von einem Reportgenerator
wuenscht.
.P 1
Viele relationale Datenbanksysteme stellen ein SQL-Interface zur
Verfuegung. Damit wird der Eindruck erweckt, dass SQL bereits eine
Standardsprache fuer relationale Datenbanken darstellt.
.br
Bei den hier genannten Systemen steht diese Sprache nicht zur
Verfuegung. Die Sprache QUEL ist in seinen wesentlichen Eigenschaften
mit SQL vergleichbar und bietet alle Moeg^lich^keiten einer modernen
Datenbanksprache.
.br
Bender und Bien weisen in ihrem Artikel "Kriterien fuer die Auswahl von
Datenbanksystemen in der Sozialwissenschaft" /4/
.FS /4/
ZA-Informationen 23 (November 1988) S. 113-118 Universitaet zu Koeln
.FE
darauf hin, dass die einzelnen SQL-Pakete nur bedingt kompatibel sind.
Da somit SQL-Programme nur bedingt portabel sind, besteht bis zur
Erarbeitung eines verbindlichen SQL-Standards keine Notwendigkeit
zur Bereitstellung dieser Sprache.
.H 1 "Speicherstrukturen fuer Relationen"
Die Effizienz eines DBS haengt im wesentlichen von den benutzten
Speicherstrukturen ab. Bei den hier genannten Systemen stehen folgende
Speicherstrukturen zur Auswahl:
.DS I
heap - unstrukturiert und ohne Schluessel
isam - index sequentielle Struktur
hash - hash Struktur
.DE
Es besteht fuer alle drei Speicherstrukturen die Moeg^lich^keit,
Zeichenketten in verdichteter Form abzulegen, d. h. nachlaufende
Leerzeichen werden nicht abgespeichert.
.P 1
Zu jeder Relation koennen eine beliebige Anzahl von Sekundaerindizes
bestehen. Diese koennen wieder in einer der drei Speicherstrukturen
organisiert sein.
.P 1
Vom Datenbanksystem wird fuer jede Anfrage der optimale Zugriffspfad
ausgewaehlt.
.H 1 "Grenzwerte und Formate der Domaenen"
Jede Datenbank kann aus einer beliebigen Anzahl von Relationen bestehen.
Die Zahl der gleichzeitig benutzten Relationen wird nur durch das jeweilige
Betriebssystem beschraenkt. Ebenso ist die Groesse einer Relation durch
die maximale Groesse eines Dateisystems auf dem benutzten Rechner
eingeschraenkt.
Die Anzahl der Domaenen (Spalten) fuer eine Relation ist
fuer 16-Bit-Rechner auf
49 bzw. fuer 32-Bit-Rechner auf 128 begrenzt.
Die maximale Laenge der Tupel (Zeilen) betraegt 1010 Bytes.
Fuer die Abspeicherung der Domaenen koennen folgenden Formate gewaehlt
werden:
.DS I
c1 - c255  Zeichenketten von 1-255 Bytes Laenge
i1         1-Byte Integer
i2         2-Byte Integer
i4         4-Byte Integer
f4         4-Byte floating
f8         8-Byte floating
.DE
Numerische Domaenen werden mit Null initialisiert.
Die Behandlung von nicht vorhandenen Werten bleibt dem Nutzer ueberlassen.
.P 1
Die genannten Grenzwerte werden oftmals als zu eng bewertet.
Da die maximale Laenge einer Zeichenkette
auf 255 Bytes beschraenkt ist, erscheint es empfehlenswert, Texte
zeilenweise abzuspeichern bzw. eine Relation mit Textbausteinen
(z. B. Saetzen) anzulegen und ueber einen Schluessel mit anderen
Relationen zu verbinden.
.br
Sollte jedoch jemand mehr als 49 bzw. 128 Domaenen in einer
Relation benoetigen,
ist er in den meisten Faellen gut beraten, wenn er sein Datenbankmodell
noch einmal ueberdenkt. Gegebenenfalls koennen mehrere Relationen
ueber einen Schluessel miteinander verbunden werden.
.H 1 "Zugriffsschutz und Datensicherheit"
Voraussetzung fuer die Benutzung des Datenbanksystems ist ein
Eintrag im Nutzerverzeichnis. Dieser legt auch
die Rechte des Nutzers innerhalb des Datenbanksystems fest.
.br
Fuer Relationen kann der Datenbankadministrator Zugriffsrechte
vergeben. Diese koennen sich auf bestimmte
Kommandos,
Zeitraeume und Terminals beziehen.
Weiterhin ist es moeg^lich, die Rechte der einzelnen Nutzer auf einzelne
Domaenen und datenabhaengig sogar auf einzelne Tupel einzuschraenken.
.P 1
Zur Sicherung der Integritaet der Datenbank bei Systemabstuerzen wird
garantiert, dass jede Veraenderung am Datenbestand zuerst in
eine Protokolldatei geschrieben wird, welches erst nach
erfolgreicher Aenderung
geloescht wird. Fuer den Fall eines Systemabsturzes waehrend einer
Aktu^ali^sie^rung steht ein "restore"-Programm zur Verfuegung, welches die
Integritaet der Datenbank gegebenenfalls wieder herstellt.
.P 1
Da die genannten Systeme mehrplatzfaehig sind, besteht die Notwendigkeit
einer "Gleichzeitigkeitskontrolle". Es wird gesichert, dass bei
Aenderungen in einer Relation die betreffende Seite bzw. die
gesamte Relation fuer diese Zeit gesperrt wird.
.H 1 "Speicherbedarf"
Die Abspeicherung der Daten auf externen Datentraegern erfolgt nach einem
Seitenkonzept. Jede Seite enthaelt Tupel einer Relation und zwar so,
dass jedes Tupel ganz auf einer Seite untergebracht ist.
Der Fuellfaktor einer Seite ist im wesentliche von der gewaehlten
Speicherstruktur abhaengig. Er
kann durch den Nutzer festgelegt werden.
Damit hat der Nutzer die Moeglichkeit, fuer Relationen, in denen keine bzw.
nur wenige Aenderungen zu erwarten sind, einen hoeheren Fuellfaktor anzugeben
und damit den
Speicherbedarf zu beeinflussen.
Als vorteilhaft erweist sich die Tatsache, dass die Daten entsprechend der
gewaehlten Speicherstruktur nach ihrem Hauptschluessel sortiert in den
Dateien abgelegt werden und somit fuer den jeweiligen Hautschluessel
keine sekundaere Indexdatei notwendig ist.
Weiterhin ist darauf hinzuweisen, dass numerische Daten
nicht als Zeichenketten in den Dateien gespeichert werden (wie
es z. B. in dem weit verbreiteten dBASE erfolgt).
.P 1
Ein weiterer Gesichtspunkt bei der Bewertung eines DBS ist der
Hauptspeicherbedarf bei Mehrplatzsystemen, sowie der benoetigte externe
Speicherplatz fuer die Programme des DBS. Bei den hier genannten Systemen
wird ein externer Spei^cher^platz von 1,2 MByte (MS-DOS) bis etwa 2 MByte
(ESER) benoetigt. Es besteht also auch auf kleinen Systemen noch die
Moeglichkeit, Daten abzuspeichern. 
.P 1
Der Hauptspeicherbedarf des DBS ist als ausgesprochen gering einzuschaetzen.
Auf einem P 8000 mit einem Hauptspeicher von 1 MByte wurde von 3 Terminals
gleichzeitig ING_DB gestartet. Es konnte dabei keine zusaetzliche Behinderung
der einzelnen Prozesse durch ein notwendiges "swappen" festgestellt
werden. Natuerlich war aber eine Verlangsamung der einzelnen Prozesse
durch die notwendige Resourcenteilung zu bemerken. Ein sinnvolles Arbeiten
war jedoch noch moeglich.
.H 1 "Literatur"
Zum Datenbanksystem ING_DB steht eine umfangreiche, stellenweise
nicht gerade leicht verstaendliche Dokumentation zur Verfuegung. Sie zeichnet
sich dadurch aus, dass insbesondere die Speicherstrukturen
und Zugriffsmethoden des DBS sehr gut dargestellt werden. Die Entscheidung
des Nutzers fuer eine der moeglichen Benutzerschnittstellen wird durch
ein Kapitel ueber die Einbettung von Datenbanksprachen in hoehere
Programmiersprachen erleichtert. Hilfreich erscheint auch die Tatsache,
dass die Arbeitsweise des Makroprozessors ausfuerlich erlaeutert wird.
Dadurch ist diese Dokumentation sehr gut geeignet, die Arbeitsweise
des DBS verstehen zu lernen und beim Entwurf des Datenbankmodells
zu beruecksichtigen.
.P 1
In der Zeitschrift "Neue Technik im Buero", Heft 3/1988 ist ein
Ueberblicksartikel von Gruenhagen (DABA 32 - relationales Datenbanksystem
unter MUTOS 1800) erschienen, der in knapper Form auf die Moeglichkeiten
von DABA 32 eingeht.
.P 1
Zum aktuellen Entwicklungsstand von INGRES ist vom Vogel-Verlag fuer
voraussichtlich April 1989 ein Chip-Sonderheft geplant.
.P 1
Das Buch von Date "A Guide to INGRES" (Addison Wesly 1987) kann
als Standardwerk bezeichnet werden.
.H 1 "Zusammenfassung"
In der DDR steht fuer alle 16- und 32-Bit-Rechner ein INGRES-kompatibles
DBS zur Verfuegung. Alle diese Systeme sind in der Nutzerschnittstelle
und im Leistungsvermoegen vollstaendig identisch.
Der Uebergang zwischen verschiedenen Rechnern
wird dadurch betraechtlich vereinfacht.
Da im ZKI die Quellen fuer das DBS vorhanden sind, kann davon ausgegangen
werden, dass dieses DBS auch fuer jeden exotischen (ROBOTRON-) Rechner
bereitgestellt werden kann. Damit besteht nicht die Abhaengigkeit
vom Import westlicher Systeme.
.br
Die Systeme werden vom ZKI staendig aktualisiert und gewartet.
.br
Die genannten DBS sind sehr leistungsstarke, mehrplatzfaehige
Systeme, die sich vor
allem fuer den professionellen Einsatz eignen. Fuer die Benutzung durch
EDV-Laien koennte man sich eine komfortablere Benutzerschnittstelle
wuenschen. Mit EQUEL- bzw. AMX-Programmen koennen jedoch sehr
leistungsfaehige und benutzerfreundliche Bedienoberflaechen fuer den
jeweiligen Einsatzfall geschaffen werden.

.DS I

Anschrift der Autoren:

B.-M. Paschke

Institut fuer Krankenhaushygiene
des Bereiches Medizin (Charite)
der Humboldt-Universitaet zu Berlin

Otto-Grotewohl-Str.1
Postfach 140
Berlin
1 0 4 0                  Tel. 2202411  App. 319



J. Wenzlaff

Zentralinstitut fuer Kybernetik
und Informationsprozesse der
Akademie der Wissenschaften

Kurstrasse 30 - 34
Berlin
1 0 8 0                   Tel. 2072 322
.TC
