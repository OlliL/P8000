/************************************************************************
*									*
*		Stand alone Hardware-Eigentest				*
*									*
*	Programm:   sa.diags						*
*	C-Modul:    menu.c						*
*	Bearbeiter: J.Kubisch, M.Klatte					*
*	Version:    3.1				                 	*
*	Datum:	    19.07.1989						*
*					           (c) ZFT-KEAW 1989	*
************************************************************************/



#define TESTANZ 9 /* Anzahl der Testkommandos */
 
char eink[5];     /* Kommandoeingabe*/
char eint[2];	  /* Testeingabe*/
char komm;	  /* Letztes Kommando der Kommando-Ebene*/
int einzelt;	  /* aktueller Einzeltest*/
int rep;	  /* Wiederholungszaehler fuer akt. Test*/
int noend;	  /* Merker fuer Endlostest*/
int testl;	  /* Ungleich Null, wenn Liste abgearbeitet wird.*/
int aktze;	  /* aktuelle Zeile fuer vollstaendige Listenabarbeitung*/
int i,j;	  

struct testz
{	char name [11];
	int wdhlg;
} tz[TESTANZ] =	{{"EPROM     ",1},
           	{"STAT. RAM ",1},
           	{"PIO       ",1},
           	{"CTC       ",1},
           	{"SIO       ",1},
           	{"DYN. RAM  ",1},
           	{"MMU       ",1},
	   	{"WDC       ",1},
	   	{"WDOS-MODUL",1}};

struct testz *pt;	/*Strukturpointer*/


main()
{
	einzelt = 0;	/*1.Test (EPROM) voreingestellt*/
	aktze = 0;	/*1 = aktuelle Zeile,voreingestellt*/
	rep = 1;
	noend = 0; 
	testl = 0;
	komm = ' ';	/*Kommandotaste*/

	/* Schirm loeschen */

	for (i=0;i<26;i++) printf("            \n");

	printf("* * * * * * * * * * * * * * * * * * * * * * * * * *\n");
	printf("*                                                 *\n");
	printf("*               P  8 0 0 0 - compact              *\n");
	printf("*                                                 *\n");
	printf("*            Hardware-Eigentest   V. 3.1          *\n");
	printf("*                                                 *\n");
	printf("*                 (c)1989 keaw/zft/basissoftware  *\n");
	printf("* * * * * * * * * * * * * * * * * * * * * * * * * *\n");
	
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
	printf("\nd  Anzeige der Fehlerliste");
 	printf("\nq  Quitt");  
	printf("\n\n\tGeben Sie Ihre Auswahl ein ]=> ");

	gets(eink,sizeof eink);		/*Kommando-Eingabe)*/
	komm = eink[0];
	switch(eink[0]) {
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
	case 'd':
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
	printf("\n2  Test STATISCHER RAM");
	printf("\n3  Test PIO");
	printf("\n4  Test CTC");
	printf("\n5  Test SIO");
	printf("\n6  Test DYNAMISCHER RAM");
	printf("\n7  Test MMU");
	printf("\n8  Test WDC");
	printf("\n9  Test WDOS-ERWEITERUNGS-MODUL (WDOS-DRAM, U80601, U80610)");
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
	case '9':
		einzelt=eint[0]-0x31; /*Testzeilennr.-1,Ascii zu int konv.*/
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
	
	pt = tz + aktze;	/*Strukturzeiger setzen*/

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

	switch (eink[0]){

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
		int i,n;
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
	printf("\n\nName          Anzahl der Wiederholungen");

	if (komm == 'l')	/*vollstaendige Liste abarbeiten*/
		j=TESTANZ-1;
	else
		j=aktze;

	for (i = aktze; i <= j; i++)
	{ pt = tz+i;
          printf("\n\n%s - T E S T     %d\n",pt->name,pt->wdhlg);
	  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	  rep = pt->wdhlg;	/*Wiederholungszaehler fuer aktuellen Test*/

	  if (rep == 0)
	      noend = 1;
	  else
	      noend = 0;

/* Aufruf der Assembler-UPs fuer die entspr. Tests */
/* Geben Fehlermeldungen aus, die im Abarbeitungstempo weiterrollen */
	  
	  while (rep > 0 || noend == 1)
	  {	switch (i)
		{   case 0:
			   printf("EPROM_ \n");   /*Aufruf des Assembl.-UPs*/
			   EPROM_ ();
			   break;
			
		    case 1:
			   printf ("STAT_RAM_ \n");
			   STAT_RAM_ ();
			   break;

		    case 2:
			   printf ("PIO_ \n");
			   PIO_ ();
			   break;

		    case 3:
			   printf ("CTC_ \n");
			   CTC_ ();
			   break;
		   
		    case 4:
			   printf ("SIO_ \n");
			   SIO_ ();
			   break;

		    case 5:
			   printf ("DYN_RAM_ \n");  
		            DYN_RAM_ ();        /*Assemblerprogr.*/
				if (rep == 1)
				{printf ("\n\0");
				 MSG_MAXSEG ();
				}
			   break;

		    case 6:
			   printf ("MMU_ \n");
			   MMU_ ();
			   break;

		    case 7:
			   printf ("WDC_ \n");
			   WDC_ ();
			   break;

		    case 8:
			   printf ("WDOS_EXT_MODUL_ \n");
			   WEM_ ();
			   break;

		    default:
			   printf ("\nFehlerhafter Wiederholungszaehler");
 		    }
           rep --;
	   }
     }
     if (komm == '1')		/* Testlistenabarbeitung */
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
	for (i=0; i<=TESTANZ-1; i++)
	{if (i == aktze)
		printf("*");
	 else	printf(" ");
	pt = tz+i;
	j = i+1;
	printf(" %d %s\n",j,pt->name);
	}
	printf("\n<CR> Testlistenabarbeitung beginnt bei aktueller Zeile (*)");
	printf("\n s   Testlistenabarbeitung beginnt bei Zeile 1");
	printf("\n #   # wird zur aktuellen Zeile");
	printf("\n ^   Rueckkehr zur \"Testkommando-Ebene\"");

	printf("\n\n	Geben Sie Ihre Auswahl ein ]=> ");

	gets(eink, sizeof eink);
	
	switch (eink[0]){
		
		case'\0':	/*CR-Testabarbeitung beginnt bei akt. Zeile*/
			tl();
			return(0);
		case's':
			aktze=0;
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
		case'9':
			aktze = katoi(eink) - 1;
			return(1);
		case'^':
			return(0);
		default:
			return(1);	/*Wiederholung avt*/
		}
}			/*Ende avt*/

