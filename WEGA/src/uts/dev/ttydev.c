/**********************************************************
*
* Rahmenprogramm fuer ein Terminalgeraet
*
**********************************************************/

/* Dieser Treiber kann nur implementiert werden, wenn
 * die 3 bzw. 4 Interruptquellen in die Interrupt-Tabelle
 * (in event.s) eingetragen werden koennen. */
 
#include "sys/param.h"
#include "sys/conf.h"
#include "sys/dir.h"
#include "sys/state.h"
#include "sys/mmu.h"
#include "sys/s.out.h"
#include "sys/user.h"
#include "sys/tty.h"
#include "sys/sysinfo.h"
#include "sys/systm.h"
 
/**************************************************
* Geraete-abhaengige Definitionen
**************************************************/

/* Anzahl der Leitungen (Kanaele) */
#define U1MAXDEV	1
 
#define U1DEVREADY(x)	1
/* ist das Geraet ready, normalerweise ist dies ein
 * in/inb vom Geraet, hier eine Dummy-Definition */
 
#define U1INTDECODE(x)	1
/* Ermittlung der Minor-Nummer bei einem Interrupt,
 * normalerweise aus dem Interruptvektor, hier eine
 * Dummy-Definition */
 
#define U1RCVVALID(x)	1
/* Die Funktion gibt einen Wert ungleich 0 zurueck,
 * wenn ein gueltiges Zeichen im Geraet vorhanden ist,
 * sonst 0, hier eine Dummy-Definition */
 
#define U1TXRDY(x)	1
/* Die Funktion gibt einen Wert ungleich 0 zurueck, wenn
 * das Geraet bereit ist, das naechste Zeichen anzunehmen,
 * hier eine Dummy-Definition */

#define TTLOWAT		50
/* Wenn weniger als diese Anzahl von Zeichen in der
 * Ausgabeschlange sind, wird der Prozess wieder geweckt */
 
/**************************************************
* Deklaration von Symbolen
**************************************************/

/* Jede Leitung benoetigt eine Stuktur */
struct tty u1tty[U1MAXDEV];
 
int u1proc();
int ttrstrt();

/* Fuer die Abspeicherung der I/O-Adressen */
struct ioaddr {
    unsigned int sio;
    unsigned int ctc;
}ioaddr[U1MAXDEV];
 
/**************************************************
* Open-Routine
**************************************************/
u1open(dev, flag)
dev_t dev;
int flag;
{
    register int d; /*locale Kopie der Minor-Nummer */
    register struct tty *tp; /*ptr zu tty struct*/

    /* Dekodieren der Minor-Nummer */ 
    d = minor(dev);

    /* Ist Minor-Nummer gueltig ? */ 
    if (d >= U1MAXDEV || d < 0) {
	u.u_error = ENXIO; /*Fehler*/
	return;
    }

    /* tp zeigt auf die tty-Stuktur der Leitung */
    tp = &u1tty[d];
    /* Ist die Leitung schon eroeffnet ? */
    if ((tp->t_state & ISOPEN) == 0) { 
    
	/* nein, es erfolgen Initialisierungen */
 
	/* Setzen der I/O-Adressen z.B. fuer
	 * U880 SIO und CTC */
	ioaddr[d].sio = NULL;	/*DEVICE SPECIFIC*/
	ioaddr[d].ctc = NULL;	/*DEVICE SPECIFIC*/
 
	tp->t_proc = u1proc;
 
	/* Setzen der Standard-Initialisierung
	 * z.B. der Sonderzeichen, Flags */
	ttinit(tp); /*sollte besser ueber linesw erfolgen*/
 
	/* Setzen der Standard-Baudrate */
	tp->t_cflag = (tp->t_cflag & ~CBAUD) | B9600;

	/* Routine zum Setzen des I/O-Ports */
	u1param(d);
 
	/******************************************
	* Geraete-spezifischer Initialisierungskode
	******************************************/
  
	/* Ueberpruefen, ob E/A-Geraet bereit ist */
 
	if (U1DEVREADY(tp)) {
	    tp->t_state |= CARR_ON;
	}
 
	/* Da die state Variable auch in den Interrupt-
	 * Routinen benoetigt wird, tritt bei der
	 * Ueberpruefung von state und bei sleep eine
	 * kritische Situation ein */
	dvi();

	/* wenn Geraet nicht aktiv, sleep bis es es ist */
	while ((tp->t_state & CARR_ON) == 0)
	    sleep((caddr_t)&tp->t_rawq, TTIPRI);
	evi();
    }
    /* Protokoll-Handler Open-Kode */
    (*linesw[tp->t_line].l_open)(tp);
}
 
/**************************************************
* Close-Routine
**************************************************/
u1close(dev, flag)
dev_t dev;
int flag;
{
    register struct tty *tp;
    register unsigned int carrier;
 
    tp = &u1tty[minor(dev)];
    carrier = tp->t_state & CARR_ON;
    (*linesw[tp->t_line].l_close)(tp);
    tp->t_state = carrier;

    /******************************** 
    * Geraete-spezifischer Close-Kode
    ********************************/
}
 
/**************************************************
* Read-Routine
**************************************************/
u1read(dev)
dev_t dev;
{
    register struct tty *tp;
 
    tp = &u1tty[minor(dev)];
    (*linesw[tp->t_line].l_read)(tp);
}
 
/**************************************************
* Write-Routine
**************************************************/
u1write(dev)
dev_t dev;
{
    register struct tty *tp;
 
    tp = &u1tty[minor(dev)];
    (*linesw[tp->t_line].l_write)(tp);
}
 
/**************************************************
* Transmitter-Interrupt-Routine
**************************************************/
u1xint(s)
struct state *s;
{
    register struct tty *tp;

    /* Port, der Int ausgeloest hat, ermitteln */ 
    tp = &u1tty[U1INTDECODE(s->s_eventid)];

    /********************************
    * Geraete-spezifischer Kode
    * Reset Port und Interrupt (reti)
    ********************************/
 
    /* Leitung frei setzen, und neue Ausgabe starten */
    tp->t_state &= ~BUSY;
    u1proc(tp, T_OUTPUT);
    /* oder ueber:
    (*linesw[tp->t_line].l_start)(tp);
    */
}
 
/**************************************************
* External/Status-Interrupt-Routine
**************************************************/
u1eint(s)
struct state *s;
{
    register struct tty *tp;
 
    tp = &u1tty[U1INTDECODE(s->s_eventid)];
    /* Geraetespezifische Ueberpruefung von Ready */
    if (U1DEVREADY(tp)) {
	if ((tp->t_state & CARR_ON) == 0) {
	    wakeup((caddr_t)&tp->t_rawq);
	    tp->t_state |= CARR_ON;
	} else {
	/* alle anderen Interrupt-Moeglichkeiten
	 * generieren ein Hangup-Signal */
	    if (tp->t_state & CARR_ON) {
		if (tp->t_state & ISOPEN) {
		    signal(tp->t_pgrp, SIGHUP);
		    flushtty(tp);
		}
		tp->t_state &= ~CARR_ON;
	    }
	}
    }
    /**************************
    * Geraete-spezifischer Kode
    * Reset Interrupt (reti)
    **************************/
}
 
/**************************************************
* Receiver-Interrupt-Routine
**************************************************/
u1rint(s)
struct state *s;
{
    register struct tty *tp;
    register char c;
 
    tp = &u1tty[U1INTDECODE(s->s_eventid)];

    /* Einlesen aller verfuegbaren Zeichen */
    while (U1RCVVALID(tp)) {
	/****************************
	* Geraete-spezifischer Kode
	* ein Zeichen nach c einlesen
	****************************/
	(*linesw[tp->t_line].l_input)(tp, (long)c, 0);
   } 
    /**************************
    * Geraete-spezifischer Kode
    * Reset Interrupt (reti)
    **************************/
}
 
/**************************************************
* I/O-Control-Routine
**************************************************/
u1ioct1(dev, cmd, addr, flag)
dev_t dev;
int cmd;
caddr_t addr;
int flag;
{
    register struct tty *tp;
 
    tp = &u1tty[minor(dev)];
    /* folgender Aufruf sollte besser ueber linesw gehen*/
    if (ttioccom(cmd, tp, (caddr_t)addr, dev, flag)) {
	/********************************************
	* Geraete-spezifischer Kode
	* siehe Rahmenprogramm fuer Character-Geraete
	********************************************/
	switch (cmd) {
	    default:
		u.u_error = EINVAL;
		return;
	}
    } else {
	u.u_error = ENOTTY;
    }
}
 
/**************************************************
* Allgemeine Geraetebehandlungs-Routine
**************************************************/
u1proc(tp, cmd)
register struct tty *tp;
register cmd;
{
    register int c;
    register unsigned int x;

    switch (cmd) {
     case T_OUTPUT: { /* Transmitter Start */

	/* Geraete-spezifischer Test ob Transmitter ready*/ 
	if (U1TXRDY(tp))
	    return; /*nein*/
	x = dvi();

	/* Ueberpruefen: Restart der Leitung
	 * nur moeglich, wenn sie nicht auf Timeout
	 * wartet, nicht BUSY ist und nicht im gestoppten
	 * Zustand (xon/xoff) ist */
	if (tp->t_state & (TIMEOUT | BUSY | TTSTOP))
	    goto out;
 
	/* ein Zeichen aus Ausgabeschlange holen */
	if ((c = getc(&tp->t_outq)) >= 0) {
	    if (c <= 0177) {
		tp->t_state |= BUSY;
		/***************************
		 * Geraete-spezifischer Kode
		 * Ausgabe des Zeichens c 
		 ***************************
	    } else {
		/*Verzoegerung c&0177 ticks */
		tp->t_state |= TIMEOUT;
		timeout(ttrstrt, (caddr_t)tp, c & 0177);
	    }
 
	    /* wakeup des Prozesses ? */
	    if (tp->t_outq.c_cc <= TTLOWAT &&
	       tp->t_state & OASLP)
	    {
		tp->t_state &=  ~OASLP;
		wakeup((caddr_t)&tp->t_outq);
	    }
	}
       out:
	rvi(x);
	return;
     }
    }
}
