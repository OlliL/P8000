h05673
s 00005/00005/00619
d D 3.1.1.4 89/11/08 10:14:50 wega 5 4
c tolower
e
s 00005/00005/00619
d D 3.1.1.3 89/11/08 09:59:06 wega 4 3
c _tolower
e
s 00002/00000/00622
d D 3.1.1.2 89/11/08 08:16:00 klatte 3 2
c include
e
s 00008/00006/00614
d D 3.1.1.1 89/11/08 07:51:10 klatte 2 1
c Ueberschrift;tolower
e
s 00620/00000/00000
d D 3.1 89/11/07 14:57:48 klatte 1 0
c date and time created 89/11/07 14:57:48 by klatte
e
u
U
f b 
t
T
I 1
/**************************************************************************
***************************************************************************

	UDOS - Quelle          (c) ZFT/KEAW Abt. Basissoftware 1988
	P8000-UDOS
	Programm:			EIGENTEST
	Modul:    			%M%
	Bearbeiter: 			J.Kubisch, M.Klatte
	Datum der letzten Release:	%G%
	UDOS-Release:			%R%.%L%
	Zweignummer:			%B%.%S%

***************************************************************************
**************************************************************************/

char ident[] = "%W%";

I 3
D 5
#include <ctype.h>
E 5
I 5
#define tolower(c)	(('A'<=c && c<='Z')?c-'A'+'a':c)
E 5

E 3
#define JA	1
#define NEIN	0

extern unsigned int errcnt; /* Fehlerzaehler */

unsigned char ertab[1000]; /* Fehlerliste   */
unsigned int z,errct,adress,sollcrc,istcrc;
unsigned int sollwert,istwert;
char fehnr;
char eink[5];     /* Kommandoeingabe*/
char eint[2];	  /*Testeingabe*/
char komm;	  /*Letztes Kommando der Kommando-Ebene*/
int einzelt;	  /*aktueller Einzeltest*/
int rep;	  /*Wiederholungszaehler fuer akt. Test*/
int noend;	  /*Merker fuer Endlostest*/
int testl;	  /*Ungleich Null, wenn Liste abgearbeitet wird.*/
int aktze;	  /*aktuelle Zeile fuer vollstaendige Listenabarbeitung*/
int i,j,n;	  
char	*gets();	/*Zeichenketteneingabe von Konsole*/

struct testz
{	char name [11];
	int wdhlg;
} tz[8] = {{"EPROM     ",1},
           {"SRAM      ",1},
           {"PIO       ",1},
           {"CTC       ",1},
           {"SIO       ",1},
	   {"FDC       ",1},
           {"DMA       ",1},
           {"DRAM      ",1}};

struct testz *pt;	/*Strukturpointer*/


main()
{
	einzelt = 0;	/*1.Test (EPROM) voreingestellt*/
	aktze = 1;	/*1 = aktuelle Zeile,voreingestellt*/
	rep = 1;
	noend = 0; 
	testl = 0;
	komm = ' ';	/*Kommandotaste*/
	
	printf("\n\0\0\0\n\0\0\0\n\0\0\0\n\0\0\0\n\0\0\0\n\0\0\0\n\0\0\0");
	printf("\n\0\0\0\n\0\0\0\n\0\0\0\n\0\0\0\n\0\0\0\n\0\0\0\n\0\0\0");
	printf("\n\0\0\0\n\0\0\0\n\0\0\0\n\0\0\0\n\0\0\0\n\0\0\0\n\0\0\0");
	printf("\n\0\0\0\n\0\0\0\n\0\0\0\n");	
	printf("\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\n");
	printf("***************************************************\n");
I 2
D 4
	printf("* UDOS %R%.%L%                                    *\n");
E 4
I 4
	printf("* UDOS %R%.%L%                                        *\n");
E 4
E 2
	printf("*                                                 *\n");
D 2
	printf("*       Stand alone Hardware-Eigentest            *\n");
E 2
I 2
	printf("*     H a r d w a r e - E i g e n t e s t  V%B%.%S%   *\n");
E 2
	printf("*                                                 *\n");
I 2
	printf("*                                                 *\n");
E 2
	printf("***************************************************\n");
	
	INI_ERR_LST ();		/*UP zur Initialisierung der Fehlerliste*/

	do ; while (tke());
	printf("\nEnde Eigentest - Resettaste betaetigen!\n");
	do ; while (1);
}	
tke()		/*Testkommando-Eingabe*/
{
	printf("\n\n\n\t*** TESTKOMMANDO-EBENE ***");
	printf("\n\nt  Auswaehlen und Abarbeiten von Einzeltests");
	printf("\nr  Wiederholen des aktuellen Einzeltests");
	printf("\nl  Abarbeiten der vollstaendigen Testliste");
	printf("\nf  Anzeige der Fehlerliste");
 	printf("\nq  Quitt");  
	printf("\n\n\tGeben Sie Ihre Auswahl ein ]=> ");

	gets(eink,sizeof eink);		/*Kommando-Eingabe)*/
D 2
	komm = eink[0];
E 2
I 2
D 4
	komm = tolower(eink[0]);
E 4
I 4
D 5
	komm = _tolower(eink[0]);
E 5
I 5
	komm = tolower(eink[0]);
E 5
E 4
E 2

D 2
	switch(eink[0]) {
E 2
I 2
	switch(komm) {
E 2
	case 't':
	       do ; while (aae());	/*Auswaehlen und /Abarbeiten von 
					  Einzeltests*/
		return(1);
	case 'r':
		do ; while (vtz());	/*Veraendere Testzeile*/
		return(1);
	case 'l':
		testl=1;
		do ; while (avt());	/*Abarbeiten der vollst. Testliste*/
		return(1);
	case 'f':
		printf("\n\n\t*** Ausgabe der Fehlerliste ***\n\n");
		MEM_MSG_OUT ();		/*UP zur Fehlerlistenausgabe*/
		return(1);
	case 'q':
		return(0);  /* Ende des Tests*/
	default:
		return(1);
	}
}

aae()		/*Auswaehlen und Abarbeiten von Einzeltests*/
{
	printf("\n\n	*** WAEHLE TESTZEILE ***");
	printf("\n\n1  Test EPROM");
	printf("\n2  Test SRAM");
	printf("\n3  Test PIO");
	printf("\n4  Test CTC");
	printf("\n5  Test SIO");
	printf("\n6  Test FDC");
	printf("\n7  Test DMA");
	printf("\n8  Test DRAM");
	printf("\n\n#  Auswahl des Tests mit der Nr. #");
	printf("\n^  Rueckkehr zur \"Testkommando-Ebene\"");
	printf("\n\n	  Geben Sie Ihre Auswahl ein ]=> ");

	gets(eint,sizeof eint);

	switch (eint[0]){
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
		einzelt=eint[0]-0x30; /*Testzeilennr.: Ascii zu int konv.*/
		aktze = einzelt;	/*Aktuelle Zeile*/

		do ; while (vtz());	  /*Veraendere Testzeile*/	
                return(1);	/*aae bleibt, bis ^ gegeben wird*/
	case '^':
		return(0);	/*Ende aae*/
	default:
		return(1);      /*aae soll wiederholt werden*/
	}
}
vtz()		/*Veraendere Testzeile*/
{
	printf("\n\n    *** Veraendere Testzeile (optional) ***");
	printf("\n\nDie Testzeile hat folgenden aktuellen Stand:");
	printf("\n\nName          Anzahl der Wiederholungen");
	
	pt = tz + aktze - 1;	/*Strukturzeiger setzen*/

	printf("\n%s               %d",pt->name,pt->wdhlg);
	printf("\n\n w   Wiederholungszaehler veraendern");
	printf ("\n p   Testparameter veraendern");
	printf("\n<CR> Testzeile ist korrekt, Test abarbeiten!");  
	if (komm == 'r')	/*Wdhlg. des aktuellen Einzeltests*/
	printf("\n ^   Rueckkehr zur \"Testkommando-Ebene\"");
	else
	printf("\n ^   Rueckkehr zu \"Waehle Testzeile\"");
	printf("\n\n	Geben Sie Ihre Auswahl ein ]=>");
	gets (eink,sizeof eink);	/*Eingabe fuer "veraendere Testzeile"*/

D 2
	switch (eink[0]){
E 2
I 2
D 4
	switch (tolower(eink[0])){
E 4
I 4
D 5
	switch (_tolower(eink[0])){
E 5
I 5
	switch (tolower(eink[0])){
E 5
E 4
E 2

	case 'w':
		vw();	/*Progr. \"Wiederholungszaehler setzen\""*/
		return (1);

	case 'p':
		vp();	/*Progr. \"Testparameter veraendern\""*/
		return(1);

	case '^':
		return(0);	/*Rueckkehr zu "Waehle Testzeile"*/

	case '\0':		/*CR, Testzeile o.k., Test abarbeiten*/

		tl();		/*Testabarbeitung*/

		return(1);
	default:
		return(1);
	}	
}
vw()			/*Setze Wiederholungszaehler*/
{
	printf("\n\n    *** Veraendere Wiederholungszaehler ***");
	printf("\nAktueller Wiederholungszaehler = %d",pt->wdhlg);
	printf("\n\n 0   Test laeuft, bis Reset-Taste betaetigt wird");
	printf("\n #   Test laeuft # mal (# positiv <= 9999)");
	printf("\n ^   Rueckkehr zu \"Veraendere Testzeile\"");
	printf("\n<CR> Wiederholungszaehler bleibt unveraendert");
	printf("\n\n	  Geben Sie Ihre Auswahl ein ]=>");

	gets(eink,sizeof eink);
	switch (eink[0]){
	case '\0':	/*CR:Wiederholungszaehler bleibt unveraendert*/
	case '^':
		return(0);	/*Rueckkehr zu "Veraendere Testzeile"*/
	}

	rep=katoi(eink);	/*Konvertierung Wiederholungszaehler*/
	if (rep>=0 && rep<=9999)
		pt->wdhlg=rep;
	return(0);
}

katoi(s)			/*Zeichenkette s in int umwandeln*/
	char s[];
{
		int n;
		for (i=0;s[i]==' '||s[i]=='\n'||s[i]=='\t'||s[i]=='+'||s[i]=='-';i++)
		;	/*Zwischenraeume ueberlesen*/

		for(n=0;s[i]>='0'&&s[i]<='9';i++)
		n=10*n+s[i]-'0';
		return(n);
}

vp()		/*Veraendere Testparameter*/
{
	printf("\n\n    *** Veraendere Testparameter ***");
	printf("\nFuer diesen Test gibt es keine Parameter ");
	printf("\n ^   Rueckkehr zu \"Veraendere Testzeile\"");
	printf("\n\n	Geben Sie Ihre Auswahl ein ]=>");
	do
	   gets(eink,sizeof eink);
	   while (eink[0]!='\0' && eink[0]!='^');
	  return(0);
}

tl()		/*Testlistenabarbeitung*/
{
	printf("\n\nDer folgende Test wird jetzt abgearbeitet:");
	printf("\n\nName          Anzahl der Wiederholungen\n");

	if (komm == 'l')	/*vollstaendige Liste abarbeiten*/
		j=8;
	else
		j=aktze;	/*nur fuer akt. Zeile abarbeiten*/

	for (i = aktze; i <= j; i++)
	{ pt = tz+i-1;
          printf("\n%s               %d\n",pt->name,pt->wdhlg);
	  rep = pt->wdhlg;	/*Wiederholungszaehler fuer aktuellen Test*/

	  if (rep == 0)
	      noend = 1;
	  else
	      noend = 0;

/* Aufruf der Assembler-UPs fuer die entspr. Tests */
/* Geben Fehlermeldungen aus, die im Abarbeitungstempo weiterrollen */
	  
	  while (rep > 0 || noend == 1)
	  {	switch (i)
		{   case 1:
			   printf("EPROM_ \n");   /*Aufruf des Assembl.-UPs*/
		           EPROM_ ();
			   break;
			
		    case 2:
			   printf ("SRAM_ \n");
			   SRAM_ ();
			   break;

		    case 3:
			   printf ("PIO_ \n");
			   PIO_ ();
			   break;

		    case 4:
			   printf ("CTC_ \n");
			   CTC_ ();
			   break;
		   
		    case 5:
			   printf ("SIO_ \n");
			   SIO_ ();
			   break;

		    case 6:
			   printf ("FDC_ \n");
     			   FDC_ ();
			   break;

		    case 7:
			   printf ("DMA_ \n");
			   DMA_ ();
			   break;

		    case 8:
			   printf ("DRAM_ \n");  
		           DRAM_ ();
			   break;

		    default:
			   printf ("\nFehlerh. Eingabe");
 		    }
	   rep --;
	   }
     }
     if (komm == 'l')		/*Testlistenabarbeitung*/
     printf ("\n\nRueckkehr zur \"Testkommando-Ebene\" durch CR-Eingabe :");
     else
     printf ("\n\nRueckkehr zu \"Veraendere Testzeile\" durch CR-Eingabe :");

     do
	  gets (eink,sizeof eink);
	  while (eink[0] != '\0' && eink[0] !='^');
     return (0);
}


avt()		/*Abarbeiten der volsstaendigen Testliste*/
{
	printf("\n\n	*** Testliste abarbeiten ***\n\n");
	for (i=1; i<=8; i++)  /*Vollst.Testliste abarbeiten*/
	{if (i == aktze)
		printf("*");
	 else	printf(" ");
	pt = tz+i-1;
	printf(" %d %s\n",i,pt->name);
	}
	printf("\n<CR> Testlistenabarbeitung beginnt bei aktueller Zeile (*)");
	printf("\n s   Testlistenabarbeitung beginnt bei Zeile 1");
	printf("\n #   # wird zur aktuellen Zeile");
	printf("\n ^   Rueckkehr zur \"Testkommando-Ebene\"");

	printf("\n\n	Geben Sie Ihre Auswahl ein ]=> ");

	gets(eink, sizeof eink);
	
D 2
	switch (eink[0]){
E 2
I 2
D 4
	switch (tolower(eink[0])){
E 4
I 4
D 5
	switch (_tolower(eink[0])){
E 5
I 5
	switch (tolower(eink[0])){
E 5
E 4
E 2
		
		case'\0':	/*CR-Testabarbeitung beginnt bei akt. Zeile*/
			tl();
			return(0);
		case's':
			aktze=1;  /*Testabarb. ab Zeile 1*/
			tl();
			return(0);
		case'1':
		case'2':
		case'3':
		case'4':
		case'5':
		case'6':
		case'7':
		case'8':
			aktze = katoi(eink);
			return(1);
		case'^':
			return(0);
		default:
			return(1);	/*Wiederholung avt*/
		}
}			/*Ende avt*/




/*---------------------------------------------------------------------------*/
/*
 *gets
 * Eingabe einer Zeichenkette von der Console
 *
 * in einen vorgegebenen Puffer bis CR oder LF
 * oder bis der Eingabepuffer gefuellt ist
 * Das letzte Zeichen im Puffer ist 0x00. CR und LF werden
 * nicht in den Puffer geschrieben
 */
/*---------------------------------------------------------------------------*/
char *gets(buf, buflen)
char *buf;
unsigned buflen;
{
	 char *lp;
	 int c;

	lp = buf;
	for (;;) {
		c = getchar() & 0177;
		putchar (c);	/* Ausgabe an Terminal */
		switch(c) {	 
		case '\n':	/* lf */
		case '\r':	/* cr */
			*lp = '\0';
			return(buf);
		case '\b':	/* back space */
			putchar(' ');
			putchar('\b');
			lp--;
			if (lp < buf)
				lp = buf;
			continue;
		case 0x18:
		case 0x7f:
			lp = buf;
			putchar('\n');
			continue;
		default:
			if (lp>=buf+buflen-1) {
				*lp = '\0';
				return(buf);
			}
			*lp++ = c;
		}
	}
}

/* Initialisierung der Fehlerliste */

INI_ERR_LST ()
{
	for(n = 0; n<1000; n++) ertab[n] = 0;
	errcnt = 0;
}

/* Fehlerliste ausgeben */

MEM_MSG_OUT ()
{
errct = errcnt;

if (errct==0)
      printf("\n Es sind keine Fehler gespeichert !\n");
else {
      printf("\nLfd.-Nr.  Fehler-Nr.  Adresse   Sollwert  Istwert\n");
      for(n = 0; (errct>0)&&(n<1000); errct--, n+=5) {
      fehnr = ertab[n];
      if (fehnr<5) {
            sollcrc = ertab[n+1]*256+ertab[n+2];
	    istcrc = ertab[n+3]*256+ertab[n+4];
	    printc(" %5d     %x                   %0x      %0x\n",errct,fehnr,sollcrc,istcrc,n);
		   }
      else {
	    adress = ertab[n+1]*256+ertab[n+2];
	    sollwert = ertab[n+3];
	    istwert = ertab[n+4];
	    printc(" %5d     %x         %0x      %x      %x\n",errct,fehnr,adress,sollwert,istwert);
	   }
	if (((errcnt-errct+1)%22)==0) {
	     printf("        weiter ---> (ENTER)             beenden ---> (q)");
	     gets(eink,sizeof eink);
D 2
	     if (eink[0]=='q') return;
E 2
I 2
D 4
	     if (tolower(eink[0])=='q') return;
E 4
I 4
D 5
	     if (_tolower(eink[0])=='q') return;
E 5
I 5
	     if (tolower(eink[0])=='q') return;
E 5
E 4
E 2
	     printf("\n");
	     } /* end if */
	  } /* next errct */
      printf("\ndruecke (ENTER) :\n");
      gets(eink,sizeof eink);
      } /* end else */
} /* end MEM_MSG_OUT () */



/*****************************************/
/*                                       */
/*  simulation der Druckausgabe 'printf' */
/*                                       */
/*****************************************/

/* conventions:                                                   */
/*	Variable duerfen nur vom Typ int, char oder pointer sein. */
/*	Die Anzahl der aktuellen Parameter muss mit der Anzahl der*/
/*	formalen Parameter uebereinstimmen (bei printc sind es 5) */

int links;	/* Zeichen linksbuendig */
int stellen;	/* Anzahl der Stellen   */
int vnull;	/* vornullen ausgeben   */
int pnull;	/* Null ausgabe		*/
int k,c;

printc(tp,a,b,c,d,e)
unsigned char *tp;
int a,b,c,d,e;
{
int *pp;

pp = &a;

while (*tp) {

	if ( *tp == '%' ) {

		tp++;
		if (*tp=='-') {links = JA;tp++;} else links = NEIN;
		if (*tp=='0') {vnull = JA;tp++;} else vnull = NEIN;
		if ('0'<*tp && *tp<'6') {stellen = *tp-'0'; tp++;}
				else stellen = 5;
		pnull = NEIN;
		switch (*tp) {
		case'd': putdec(*pp--); break;
		case'u': putudec(*pp--); break;
		case'x': puthex(*pp--); break;
		case'o': putoct(*pp--); break;
		case'c': putchar(*pp--); break;
		case's': putstring(*pp--); break;
		case'%': putchar(*tp);break;
		default: break;
		}
	}
	else putchar(*tp);

	tp++;
}
}



/* Ausgabeconvertierung DEZIMAL mit Vorzeichen */
/*---------------------------------------------*/
putdec(zahl)
int zahl;
{
int z;
int za;

if ( zahl < 0 ) {
	za = -zahl;
	putchar('-');
	}
else za = zahl;

for ( k = z = 1; z<stellen; z++) k*=10; 
for( z = 0; k; k/=10) {
	za = (za - z*10*k);
	z = za/k;
	c = z+'0'; /* int to ascii */
	putform(c);
}
}	/* end of procedure */


/* Ausgabe vorzeichenlos DECIMAL */
/*------------------------------*/

putudec(zahl)
int zahl;
{
int za;
za = zahl<0 ? -zahl:zahl;
putdec(za);
}


/* Ausgabeconvertierung OKTAL mit Vorzeichen */
/*---------------------------------------------*/
putoct(zahl)
int zahl;
{
int z;
int za;

za = zahl;
if ( zahl < 0 ) {
	za = -za;
	putchar('-');
	}
if (stellen==0) stellen = 5;

for ( k = z = 1; z<stellen; z++) k*=8; 
for( z = 0; k; k/=8) {
	za = (za - z*8*k);
	z = za/k;
	c = z+'0'; /* int to ascii */
	putform(c);

}
}	/* end of funktion */

/* Ausgabe in HEX */

puthex(za)
int za;
{

for ( k = 4; k>0; k--) {
	c = (((za&0xf000)>>12)&0x000f)+'0';
	za <<= 4;
	if(c > '9') c += 7;
	putform(c);

}
}  /* end of funktion */

/* Ausgabe eines strings */

putstring(s)
char *s;
{
while (*s) { putchar(*s); s++; }
}


/* formatierte Ausgabe eines Zeichens */

putform(c)
int c;
{

	if (links) {
		if ((c>'0') || pnull || k==1) { putchar(c); pnull = JA; }
	}
	else {
		if (c=='0') {
			if (vnull || pnull || k==1) putchar('0');
				else putchar(' ');
		}
		else { putchar(c); pnull = JA;}
	}
}
E 1
