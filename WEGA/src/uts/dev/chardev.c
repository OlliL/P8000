/**********************************************************
 *
 * Rahmenprogramm fuer Charactergeraet
 *
 *********************************************************/
 
#include "sys/param.h"
#include "sys/tty.h"
#include "sys/sysinfo.h"
#include "sys/systm.h"
#include "sys/dir.h"
#include "sys/state.h"
#include "sys/mmu.h"
#include "sys/s.out.h"
#include "sys/user.h"
 
/**************************************************
* Geraete-abhaengige Definitionen
**************************************************/

/* Fehlermaske */
#define U1ERROR		0x8000
 
/* Maske, wenn Uebertragung OK */
#define U1OK		0x0000
 
/* Uebertragungsrichtung */
#define DIRECTION	0x7000
#define OUTPUT		0x4000
#define INPUT		0x2000
 
/* maximale Anzahl der Zeichen in der Ausgabeschlange,
 * bevor der Prozess schlafen geschickt wird */
#define U1HIWATER	100
 
/*der Anzahl der Zeichen, wenn der Prozess wieder geweckt
 * wird */
#define U1LOWATER	10
 
/* flag, wenn der Prozess schlaeft */
#define U1ASLEEP	1 /* fuer Ausgabe */
#define U1ISLEEP	2 /* fuer Eingabe */

/* Prioritaet fuer wakeup */
#define U1PRI 45

/* U1HOG ist ein Begrenzer, wenn mehr als U1HOG Zeichen in
 * der Eingabeschlange enthalten sind (Zeichen empfangen,
 * Prozess hat sie aber nicht abgeholt) werden alle Zeichen
 * in der Eingabeschlange geloescht */
#define U1HOG		200
 
/**************************************************
* Deklaration von Symbolen
**************************************************/

/* Header fuer Ein- und Ausgabeschlange */ 
struct clist u1_inq, u1_outq;
 
/* Status-Flag */
unsigned int u1flag;

extern int last_int_serv;
 
/**************************************************
* Interrupt-Routine
**************************************************/
u1int(s)
register struct state *s;
{
    register char c;

    last_int_serv = USR1INT;

    if ((s->s_eventid & U1ERROR) == U1OK) {
     /* U1ERROR ist eine Geraete-abhaengige Maske fuer das
      * obere Byte von s_eventid (falls der Status im
      * oberen Teil des Int-Vektors enthalten ist) */

	switch (s->s_eventid & DIRECTION) {
	    /* Diese Int-Routine behandelt Ein- und
	     * Ausgabe-Interrupts */
 
	     case OUTPUT:
	     /* Ausgabe Zeichen erfolgt, neues Zeichen
	      * kann ausgegeben werden */

		/**************************
		* Geraete-spezifischer Kode
		* Reset Int (reti)
		**************************/
		u1start();
 
	/* Die Ausgabe des naechsten Zeichens laeuft nun,
	 * falls noch Zeichen auszugeben waren
	 * Es wird nun ueberprueft, ob der Prozess
	 * geweckt werden muss */
		if ((u1_outq.c_cc <= U1LOWATER)
		   &&(u1flag & U1ASLEEP)) /*schlaeft er ?*/
		{
		    u1flag &= ~U1ASLEEP; 
		    wakeup(&u1_outq); /*Prozess wecken*/
		} 
		return;

	     case INPUT:
 
	/***************************************
	* Geraete-spezifischer Kode
	* ein Zeichen vom Geraet nach c einlesen
	***************************************/
 
	/* zu viele Zeichen im Eingabepuffer? */
		if (u1_inq.c_cc > U1HOG) {
 
	/* ja, loeschen aller Zeichen */
		    while(u1_inq.c_cc) {
			getc(&u1_inq);
		    }
		    return;
		}
 
	/* Zeichen in Eingabepuffer eintragen */
		putc(c, &u1_inq);

	/* Prozess wecken, falls er schlaeft */
		if (u1flag & U1ISLEEP) {
		    u1flag &= ~U1ISLEEP; 
		    wakeup(&u1_inq); /*Prozess wecken*/
		}
 
		/**************************
		* Geraete-spezifischer Kode
		* Reset Int (reti)
		**************************/
		return;
	}
    }
 
    else { /* Uebertragungsfehler */
 
	printf("Descriptive message\n");

	/************************************
	* Geraete-abhaengige Fehlerbehandlung
	* und Reset Int (reti)
	************************************/
	return;
    }
}
 
/**************************************************
* Open-Routine
**************************************************/

/* Dummy-Definition zur Ueberpruefung der Minor-Nummer */
#define INVALID(n) (n<0 || n>2)

u1open(dev)
dev_t dev;	/* dev_t ist eine typedef*/
{ 
    register int minordev;   /*local Minor-Nummer*/
    static int u1first = 0;  /*zur einmaligen Ausfuehrung*/
 
    minordev = minor(dev);
 
    if (u1first++ == 0)	{
	/******************************************
	* Geraete-spezifischer Kode, der nur einmal
	* ausgefuehrt wird
	******************************************/
    }
    /* Hier kann auch eine Verriegelung des Treibers zur
     * Verhinderung einer gleichzeitigen mehrfachen
     * Benutzung eingebaut werden */ 

    /* Ueberpruefung der Minor-Nummer */
    if (INVALID(minordev)) {
	u.u_error = ENXIO; /* Fehler */
	return;
    }
}
 
/**************************************************
* Close-Routine
**************************************************/
u1close(dev)
dev_t dev;
{
    /**************************
     * Geraete-abhaengiger Kode
     *************************/
}
 
/**************************************************
* Read-Routine
**************************************************/
u1read(dev)
dev_t dev;
{
    /* Zeichen von der Eingabeschlange einlesen, bis diese
     * leer ist oder der Prozess keine Zeichen mehr haben
     * will */

    /*Disable Ints, weil wir in die Eingabeschlage sehen */
    dvi();
    /* Zeichen in der Eingabeschlange ? */
    while(u1_inq.c_cc == 0) {
	/* nein, sleep bis Zeichen da */
	sleep(&u1_inq, U1PRI);
    }
    evi();
    /* Zeichen sind da, auslesen */
    while (u1_inq.c_cc && passc(getc(&u1_inq)) >= 0)
	;
    return;
}
 
/**************************************************
* Write-Routine
**************************************************/
u1write(dev)
dev_t dev;
{
    register char c;

    while (u.u_count) { /* fuer jedes Zeichen des
			 * Write-Calls */
	dvi();
	/* zu viele Zeichen in der Ausgabeschlange ? */
	while (u1_outq.c_cc > U1HIWATER) {
	    /* ja, Ausgabe starten und sleep */
	    u1start();
	    u1flag |= U1ASLEEP;
	    sleep(&u1_outq, U1PRI);
	}
	evi();
 
	/* naechstes Zeichen vom User-Prozess holen */
	if ((c = cpass() ) < 0) 
	    break; /* alles ausgegeben */
 
	/* Zeichen in Ausgabeschlange bringen */
	putc(c, &u1_outq);
    }
    /* Start der Ausgabe */ 
    u1start();
}
 
/**************************************************
* I/O-Control-Routine
**************************************************/
u1ioctl(dev, cmd, arg_addr, flag)
dev_t dev;
int cmd;		/*Kommando*/
caddr_t arg_addr;	/*Adresse vom Argument-Block*/
int flag;		/*Kopie des Open-Mode-Flags*/
{ 
    struct u1ioct1b {
	int device_specific;
    }u1ioct1b;
 
/* Dieses sind Dummy-Definitionen */
#define CMD1 1
#define CMD2 2
  
    switch (cmd) {
 
	case CMD1:	/* Geraete-spezifisch */

	    /* Dies ist ein typisches Fragment zum Setzen
	     * der Parameter */
	    if (copyin(arg_addr, (caddr_t)&u1ioct1b,
		sizeof (u1ioct1b)))
	    {
		u.u_error = EFAULT; /* Fehler bei copyin */
		break;
	    }
	    /****************************************
	     * Geraete-spezifische Interpretation vom
	     * u1ioct1b Inhalt
	     ***************************************/
	    break;
 
	case CMD2:
	    /* typisches Fragment zum Lesen der Parameter*/

	    if (copyout(arg_addr, (caddr_t)&u1ioct1b,
		sizeof(u1ioct1b)))
	    {
		u.u_error = EFAULT;
	    }
	    break;
 
	default:
	    u.u_error = EINVAL;
	    break;
    }
    return;
}
 
/**************************************************
* Start-Routine
**************************************************/
u1start()
{
    char c;

    /* Zeichen von Ausgabeschlange einlesen */
    c = getc(&u1_outq);
 
    /*********************************
    * Geraete-spezifischer Ausgabekode
    * sende Zeichen in c zum Geraet
    *********************************/
}
