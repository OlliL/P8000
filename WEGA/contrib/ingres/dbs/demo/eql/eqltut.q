# include <stdio.h>
/*
** Dieses Dokument will als Lehrbeispiel eines EQL-Programmes
** in C verstanden werden.
** Als Voraussetzung sollten Sie sowohl mit der Programmiersprache
** C als auch mit der DABA16-Anfragesprache QL vertraut sein,
** bevor Sie die Beispiele durcharbeiten.
** Das Programm kann abgearbeitet werden, um zu sehen, wie die 
** Beispiele bei entsprechenden Eingaben exakt funktionieren.
** Um dieses Programm abzuarbeiten, sollten Sie folgende 
** Shell-Kommandos spezifizieren:
**
**		eqc eqltut.q
**		cc eqltut.c -lq
**		a.out
**
** Das erste Kommando ruft den EQL-Praecompiler  fuer C (eqc(mutos))
** auf, der in das Programm Code einfuegt, mit dem Anfragen an DABA16
** gesendet werden.
** Die Ausgabe des Praecompilers erfolgt im vorliegenden Fall
** in das File eqltut.c.
** Allgemein wird der Praecompiler wie folgt aufgerufen:
**
**		eqc [-d] [-f]	[-r] file1.q [file2.q ...]
**
** Die Ausgabe erscheint in "file1.c" usw..
** Die -d-Option veranlasst "eqc" die Zeilennummern-Informationen
** in das File zu stellen, so dass bei Laufzeitfehlern diese
** mit den entsprechenden Anfragen assoziiert werden koennen.
**
** Es ist moeglich den C-Praecompiler zu benutzen, um Files mit
** EQL-Anweisungen und/ oder Deklarationen einzuschliessen, wenn
** die Namen dieser Files auf ".q.h" enden.
** Solche durch "eqc" bearbeiteten Files erhalten den Filesuffix
** ".q.h" und werden durch den C-Praecompiler durch #include
** eingeschlossen.
** Files, fuer die zwar #include spezifiziert wird, die aber
** obigen Namenskonventionen nicht genuegen, werden durch "eqc"
** ignoriert.
*/


/*
** EQL verwendet i.allg. die gleiche Syntax wie QL.
** Es gibt jedoch einige wenige Differenzen zwischen EQL und QL
** und somit Unterschiede in den QL- und C-Konstruktionen.
** Folgende Differenzen bestehen:
**
** C-variablen, die fuer EQL deklariert werden, sind i.allg. wie
** solche ausserhalb von EQL-Anweisungen zu verwenden.
** Das gilt nicht innerhalb von Strings bzw., wenn ihnen ein
** sogenannter Entreferenzierungsoperator '#' vorangeht.
** Im allgemeinen ist Vorsicht geboten, wenn Variablennamen
** mit Domaenen-Namen uebereinstimmen.
**
** Alle Strings, die C-Variablen von DABA16 uebergeben werden,
** werden durch Null abgeschlossen.
** Damit werden sie um ein Byte laenger als innerhalb der Relation.
** Das bedeutet, dass eine Zeichenkette, in die Daten uebertragen
** werden sollen, um ein Byte groesser definiert werden muss
** als die Domaene, aus der die Daten kommen.
** 
**
** Retrieve-Anweisungen ohne Ergebnisrelation haben in EQL
** und QL unterschiedliche Interpretationen.
** Dazu einige Beispiele.
*/

/*
** Zuerst einige Anfragen, die auch in der Schrift
** "IQL-Lehrbeispiele, Anleitung fuer den IQL-Nutzer"
** enthalten sind. Somit werden die Gemeinsamkeiten und die
** Unterschiede zwischen QL und EQL sichtbar. 
*/

/*
** Es wird mit der Deklaration einiger Variablen begonnen,
** die fuer die Interaktionen benoetigt werden. Beachten Sie bitte,
** dass die Variablen fuer EQL global sind und, dass die
** Deklarationen fuer das gesamte File gelten.
*/

## char	tname[21];	/*
			** tname wird so dimensioniert, dass das
			** tname-Feld der teile-Relation plus
			** das Nullbyte gehalten werden koennen.
			** Der Entreferenzierungsoperator '#'
			** muss verwendet werden, wenn "tname"
			** sowohl als Feldname als auch als 
			** Variablenname verwendet wird und 
			** der Feldname gemeint ist. Bei einer
			** Spezifikation "tname." wuerde EQL an-
			** nehmen, dass "tname" als Variable 
			** benutzt werden soll.
			*/

## char	farb[9];		/*
			** Damit kann das farbe-Attribut der teile-
			** Relation gehalten werden. Es wurde "farb"
			** statt "farbe" genannt, so dass 
			** der Term "t.farbe" kein Verweis auf eine
			** Variable darstellt und somit auch kein 
			** Entreferenzierungsoperator benoetigt
			** wird.
			*/

main(argc, argv)
int	argc;
char	*argv[];

{

	/*
	** Die Interaktion mit DABA16 unter Benutzung der dt-
	** Datenbank wird wie folgt eingeleitet.
	*/


##   	daba "-i210" dt

	/*
	** Beim daba-Aufruf koennen bis zu 9 Argumente spezifi-
	** ziert werden. 
	** Hier wird das Integer-Ausgabeformat modifiziert.
	** Optionen muessen in Anfuehrungszeichen eingeschlossen
	** werden.
	*/

	/*
	** Wie in den IQL-Lehrbeispielen, kann die teile-Relation 
	** durch folgende Spezifikation ausgegeben werden.
	*/

##	print teile

	/*
	** Das stimmt mit der entsprechenden QL-Anweisung ueberein.
	** Die EQL-Anweisung ist jedoch durch "##" gekennzeichnet,
	** um den "eqc"-Praecompiler zu veranlassen, diese Zeile
	** in C-Anweisungen zu transformieren.
	*/


##	range of t is teile	/*
				** Dies stimmt mit der QL-Syntax
				** ueberein.
				** Beachten Sie bitte die 
				** Benutzung eines Kommentares
				** in EQL-Anweisungen.
				*/


	/*
	 ** Fuer das  folgende  Beispiel ist zu beachten, dass das erste
	 ** "tname" auf die Variable "tname" verweist, waehrend fuer das
	 ** zweite "tname" angenommen wird, dass es einen Konstantennamen
	 ** darstellt, weil der Entreferenzierungsoperator vorangestellt
	 ** wurde.
	*/
##	retrieve (tname	= t.#tname)
##	{
		/*
		** Alles innerhalb der geschweiften Klammern wird
		** fuer jedes wiederaufgefundene Tupel wiederholt.
		*/

		printf("%s\n", tname);

		/*
		** tname ist ein ordnungsgemaess abgeschlossener
		** String. EQL schliesst alle Strings, die von
		** DABA16 uebergeben werden, mit Null ab. Damit 
		** werden Strings gegenueber der Attributlaenge
		** um Eins groesser. Es wird vorausgesetzt, dass
		** der Nutzer genug Speicherplatz bereitgestellt
		** hat!
		*/
##	}

	/*
	** Jetzt sollen Farben und Namen der Teile wiedergefunden 
	** werden.
	** Der in den "IQL-Lehrbeispielen" dargestellte Fehler soll
	** uebergangen werden. Es ist nur zu bemerken, dass EQL
	** den Fehler erkennen wuerde und fuer die folgende
	** Anweisung nachfolgenden Kommentar geben wuerde.
	**	##	retrieve tname = t.#tname, farb = t.farbe
	**	IS = '=' : line 7, syntax error
	*/


##	retrieve (tname	= t.#tname, farb = t.farbe)
##		/*
		** Der Name "farb" wurde als Variablenname fuer
		** "farbe" verwendet. 
		**
		** Der Kommentar an dieser Stelle muss mit "##"
		** am Zeilenanfang beginnen, da EQL nach 
		** "## {" sucht, um das Wiederfinden zu wieder-
		** holen. Gleiches gilt fuer Leerzeilen. Auch sie
		** muessen mit "##" vor einem "## {" beginnen.
		*/
##	{
	 printf("Die Farbe des Teiles  %s ist %s\n", tname,farb);
##	}

	/*
	** Die Anweisungen ##{ und ##} sind stets notwendig, auch
	** wenn nur eine Zeile C-Code innerhalb des Wieder-
	** findens auftritt. 
	*/

	/*
	** Um die grauen Teile wiederzufinden und auszugeben, ist
	** folgendes zu spezifizieren.
	*/

	printf("Folgende Teile sind grau:\n");
##	retrieve (tname	= t.#tname)
##	where	t.farbe	= "grau"
##	{
		printf("\t%s\n", tname);
##	}

	/*
	** Obige Anfrage stimmt mit der entsprechenden Anfrage
	** der "IQL-Lehrbeispiele" ueberein.
	*/

	/*
	** In EQL gibt es den Begriff des "Anfragenpuffers" wie beim
	** DABA16-Terminal-Monitor nicht.
	** Wenn die Anfrage 6 der "IQL-Lehrbeispiele" ausgefuehrt 
	** werden soll, muss mit Ausnahme der range-Anweisung
	** die komplette Anfrage spezifiziert werden.
	*/

##	retrieve (tname	= t.#tname, farb= t.farbe)
##	where	t.farbe	= "grau"
##	or	t.farbe	= "rosa"
##	{
	printf("Die Farbe des Teiles %s ist %s\n", tname, farb);
##	}

	/*
	** Jetzt sollen keine Beispiele der "IQL-Lehrbeispiele" mehr
	** behandelt werden. Es soll auf EQL-Besonderheiten ein-
	** gegangen werden.
	*/

	example1();

	/*
	** Als naechstes ein interaktives Beispiel.
	*/

	raise();

	/*
	** Als naechstes ein Beispiel einer "parametrisierten"
	** Anweisung.
	*/

	param_ex();
}

/*
** Es sei angenommen, dass Teile einer Relation in den Haupt-
** speicher uebertragen werden sollen. Dies ist mit DABA16 sehr
** schwierig.
**
** Dieses Beispiel uebertraegt Elemente der "lieferung"-Relation
** in ein Feld von Strukturen.
*/
# define	MAXDATA	20
/*
** Im Folgenden werden die Felder "tnum", "lnum" und "menge"
** fuer EQL definiert.
*/
## struct  lieferung
## {
##	int tnum, lnum;
##	int menge;
## };

/*
** Die Zeichen ##{ und ##} am Anfang und Ende des Beispieles
** der example1()-Funktion kennzeichnen den Gueltigkeitsbereich
** der deklarierten Variablen. Deshalb werden diese Daten von
** EQL bezueglich example1 als lokal betrachtet.
** Irgendein freier Block (ein ##{...##}, nicht unmittelbar nach
** einem ##-retrieve ohne Ergebnisrelation [ohne into]) erklaert
** Variablen innerhalb des Blockes als lokal. Das bedeutet jedoch
** nur lokal bezueglich einer Stufe. Eine Variable ist entweder
** global fuer das File oder sie ist lokal bezueglich einge-
** schlossener freier Bloecke.
*/

	char x;

example1()
## {

##	struct	lieferung	data [MAXDATA +	1];
	register int	i;

	i = 0;

##	range of l is lieferung

	/*
	** Die Sruktur-Feldnamen sind als Strukturfelder bekannt,
	** weil sie als solche deklariert wurden und die Struktur-
	** variable "data" folgt.
	** Auf der rechten Seite des Gleichheitszeichens (=)
	** gibt es keine Strukturfelder, so dass sie als Domaenen-
	** namen betrachtet werden. 
	** Jedoch auch der Entreferenzierungsoperator koennte ver-
	** wendet werden, um Klarheit zu schaffen.
	*/

##	retrieve (data [i].tnum = l.tnum,
##		data [i].lnum	= l.lnum,
##		data [i].menge	= l.menge)
##	where l.liefdat <= "84-12-10"
##	{
	 printf("Lieferant Nr.%d, liefert %d Teil(e) der Teilenr.%d.\n",
	 data [i].lnum, data [i].menge, data [i].tnum);
	 if (i++	>= MAXDATA - 1)
	 {
			printf("Zu viele Daten!\n");
			break;
			/*
			** Die Unterbrechung ist berechtigt, da
			** die retrieve-Anweisung in eine "while"
			** Anweisung konvertiert wird. Die Unter-
			** brechung ist nur dann anwendbar, wenn
			** die Wiederauffindung bei einem vom
			** Anwender entdeckten Fehler verlassen
			** werden soll.
			** Nach dem "while" gibt es Code, um die
			** Uebertragung der Daten, die nicht vom
			** EQL-Prozess benutzt wurden, zu be-
			** enden.
			*/
		}

##	}
## }

/*
** Folgende Routine liefert eine interaktive Terminalsitzung, um
** Gehaelter zu aktualisieren. Dazu gibt es auch andere Moeglich-
** keiten. Hier sollen jedoch moegliche Fallen gezeigt werden.
*/

raise()
## {
	int		flag;
	int		per;
##	char		percent[10];
##	char		rname[21];
##	char		ename[21];
##	int		geh;
##	char		domain[20];
##	char		info[255];

/*
	extern	int (*IIinterrupt)();
	extern	reset();
*/
	

##	range of p is personal

	/*
	** Weil die range-Anweisung solange gueltig ist, wie DABA16
	** arbeitet, wird sie vor der Schleife deklariert.
	*/

	/*
	** Vor Eintritt in die Schleife wird Vorsorge fuer eine 
	** Fortsetzung der Verarbeitung getroffen, wenn durch 
	** den Anwender eine Unterbrechung erfolgt.
	** An dieser Stelle darf durch das Programm keine Signal-
	** behandlung erfolgen, da DABA16 diese ausfuehrt und ver-
	** sucht, eine Synchronisation mit dem EQL-Prozess zu
	** erreichen.
	** Wenn der EQL-Prozess synchronisiert wurde, wird
	** (*IIinterrupt)() aufgerufen.
	*/
/*

	setexit();
Iinterrupt = reset;
*/
	
loop:
	printf("Spezifizieren Sie bitte den Namen\n");

	if (eread(ename))
		return (0);

	if (ename[0] ==	'?' && ename[1]	== '\0')
##		print personal
	else
	{
		flag = 0;

		/*
		** In dieser Interaktion werden drei Anfragen
		** und Berechnungen  von DABA16 ausgefuehrt.
		** Der wiedergefundene Name wird nach "rname" ueber-
		** tragen, da "ename" Suchmuster-Zeichen enthalten
		** kann. So kann beispielsweise "Rose*" einge-
		** geben werden. Fuer "Rose, Peter" und "Rose,
		** Petra" wuerden die Gehaelter ausgegeben werden.
		*/

##		retrieve (rname	= p.name, geh =	p. gehalt)
##			where p.name = ename
##		{
			printf("Das Gehalt von %s betraegt %d\n",
				rname, geh);
			flag = 1;
##		}

		if (!flag)
		{
			printf("Diese Person gibt es nicht\n");
			goto loop;
		}
		printf("Gib Zunahme in Prozent:");
		if (eread(percent))
			goto loop;

		/*
		** Es gibt in EQL keine Moeglichkeit, um ein
		** Tupel zu pruefen, zu modifizieren und an-
		** schliessend zurueckzuschreiben. Bei "replace"
		** muss eine Qualifikation enthalten sein, weil
		** es keine Verbindung zwischen dem vorherigen
		** "retrieve" und dem "replace" gibt.
		*/
##		replace	p ( gehalt = p. gehalt + float8(percent)/100
##              * p. gehalt)
##			where p.name = ename


		per = atoi(percent);

##		retrieve (rname	= p.name, geh =	p. gehalt)
##			where p.name = ename
##		{
			printf("Diese Gehaltserhoehung ist ");
			if (per	< 5)
				printf("gering\n");
			else if	(per < 10)
				printf("maessig\n");
			else if	(per < 30)
				printf("gut\n");
			else
				printf("gewaltig\n");
			printf(" Gehalt von %s ist jetzt %d\n",rname,geh);
##		}


		printf("Andere Informationen ueber %s gewuenscht?\n"
			, ename);

		if (eread(domain) || domain[0] == 'n' )
			goto loop;

		printf("Domaenenname:  ");

		if (eread(domain))
			goto loop;

		/*
		** Wenn der Anwender mit '?' antwortet, werden
		** ihm alle vorhandenen Domaenen angezeigt.
		** Das erfolgt durch Ausgabe der Attribute der 
		** Relation der Tupel der "attribute"-Relation.
		*/

		if (domain[0] == '?' &&	domain[1] == '\0')
		{

##			range of a is attribute

##			retrieve(domain	= a.attname)
##				where a.attrelid = "personal"
##			{
				printf("\t%s\n", domain);
##			}
			printf("Domaenenname :  ");

			if (eread(domain))
				goto loop;
		}

		/*
		** Hier wird eine C-Variable als ein Domaenen-
		** name verwendet. Der Wert der Variablen wird
		** an DABA16 uebergeben und als Teil der Anfrage 
		** interpretiert.
		*/

		/*
		** Die ascii-Funktion wird verwendet, weil der
		** Domaenentyp nicht bekannt ist. Der Aufruf der
		** ascii-Funktion fuer Zeichen-Domaenen ist 
		** erlaubt.
		*/
##		retrieve (rname	= p.name, info = ascii(p.domain))
##			where p.name = ename
##		{

			printf("%s\t%s = %s\n",	rname, domain, info);
##		}

	}
	goto loop;
##}

/*
** Die folgende Routine zeigt die Benutzung parametrisierter
** EQL-Anweisungen, wobei die Target-Liste bis zur Ausfuehrungs-
** zeit unterbestimmt ist.
** Dies zeigt einen Weg, wie eine variable Anzahl von Domaenen
** oder variable Typen durch die gleiche EQL-Anweisung benutzt
** werden koennen.
*/

param_ex()
{
	char		name [25];	/*
					 ** Variable, verwendet
					 ** in der Target-Liste
					 ** der parametrisierten
					 ** Anweisung, die nicht 
					 ** fuer EQL deklariert
					 ** werden muss.
					*/
	register char	*string;
	union	var {
		short	empno;
		char	cempno[2];
	};
	union	var	v;
	char		*tl_vector [100];

	/*
	** Folgender alternative Weg ist moeglich :
	**  ##	retrieve (name = p.#name, empno	=p.num) 
	**  ##	{
	**		printf("Person Nr.%d  heisst  %s.\n",
	** 		empno, name);
	**  ##	}
	*/

	/*
	** Diese Anweisung initialisiert die Target-Listen-
	** Variable.
	** Das Prozentzeichen '%' kennzeichnet den Typ des
	** korrespondierenden, nachfolgenden Argumentes.
	** Erlaubte Typen sind:
	**	%c -- String beliebiger Laenge
	**	%i2, %i4 -- integer oder long
	**	%f4, %f8 -- float oder double
	*/

	string = "%c is	p.name,	%i2 = p.num";
	tl_vector [0] = name;
	tl_vector [1] = v.cempno;

##	param retrieve (string, tl_vector)
##	/*
	** Diese Anweisung koennte auch wie folgt formuliert
	** werden.
	** ## param retrieve ("%c is p.name, %i2 = p.num", 
	** ##    tl_vector)
	*/
##	{
		printf("Person Nr.%5d heisst %s.\n", v.empno, name);
##	}

	/*
	** Parametrisierte Anweisungen koennen fuer
	** "append", "copy", "create", "define view",
	** "retrieve ohne Ergebnisrelation" und 
	** "replace" benutzt werden.
	** Fuer "append" waere spezifizierbar :
	** ##  param append to personal ("name is %c, num is %i2",
	** ##	  tl_vector)
	*/
}

/*
** Nachfolgende Routine liest einen String vom Terminal und
** schliesst ihn durch Null ab.
** Es wird 1 zurueckgegeben, wenn EOF gelesen wird.
*/

eread(p)
char	*p;
{
	int 	c;
	while((c = getchar()) != EOF)
	{
		if(c ==	'\n')
		{
			*p = 0;
			return(0);
		}
		*p++ = c;
	}
	return(1);
}
