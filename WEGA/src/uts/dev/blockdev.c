/**********************************************************
*
* Rahmenprogramm fuer ein Blockgeraet
*
**********************************************************/
 
#include "sys/param.h"
#include "sys/buf.h"
#include "sys/iobuf.h"
#include "sys/conf.h"
#include "sys/dir.h"
#include "sys/state.h"
#include "sys/mmu.h"
#include "sys/s.out.h"
#include "sys/user.h"
#include "sys/sysinfo.h"
#include "sys/systm.h"
 
/**************************************************
* Geraete-abhaengige Definitionen
**************************************************/

/* Maske fuer Fehler-Bits im Int-Verktor (s->s_eventid)*/
#define USR1ERROR	0xFF00
 
/* Maske, wenn kein Fehler*/
#define USR1OK		0x0000
 
/* Definition der Fehlerkodes
 * Diese werden normalerweise ueber in/inb vom E/A-Geraet
 * gelesen */
#define USR1ERR1	0
#define USR1ERR2	0

/* maximale Anzahl der Minor-Geraete */
#define USR1MINORMAX	2

/* maximal moegliche Anzahl der uebertragbaren Bytes */
#define USR1MAXFR	20 * 512

/* Anzahl der Wiederholungen bei einem Fehler */ 
#define USR1RETRYLIMIT	10
 
/**************************************************
* Deklaration von Symbolen
**************************************************/

/* Puffer-Header fuer Raw-Interface */
struct buf rusr1buf;

/* Puffer-Header fuer Geraetepufferwarteschlange */
struct iobuf usr1tab;

/* Pointer zum augenblicklich aktivem Puffer */
struct buf *usr1current = (struct buf *)&usr1tab;

/* Richtung der Kopfbewegung eines Disklaufwerkes
 * 1 = in Richtung hoeherer Zylindernummern
 * 0 = in Richtung kleinerer Zylindernummern */
unsigned int direction = 1;
 
/* Tabelle fuer die Diskaufteilung, nur fuer Disktreiber
 * fuer den Standarddisktreiber wird die Tabelle von
 * sysgen in wpar.c erstellt. */
struct vd_size usr1sizes[USR1MINORMAX] =
{
/*Unit, Offset fuer ersten Block, Anzahl der Bloecke */
0,	0,				1000,
0,	10000,				10000
};

extern int last_int_serv;
 
/**************************************************
* Interrupt-Routine
**************************************************/
usr1int(s)
register struct state *s;
{
    register struct buf *bp;/*Pointer zum aktiven Puffer*/

    last_int_serv = USR1INT;
 
    /**************************************************
    * Geraete-spezifischer Kode
    * Reset Hardware und Interrupt (reti)
    * Interrupts sind in dieser Routine gesperrt, damit
    * treten keine Probleme auf
    **************************************************/
 
    /* wenn wir keinen Int erwartet haben */
    if (usr1tab.b_active == NULL) {
	printf("usr1: unexpected interrupt\n");
	return;
    }
    /* Controler frei setzen */
    usr1tab.b_active == NULL;
    bp = usr1current;	/* Pointer zu aktivem Puffer */

    /* Im oberen Byte des Int-Vektors ist hier der
     * Fehlerkode enthalten */
    if ((s->s_eventid & USR1ERROR) == USR1OK) {
	/*Transfer OK, Ueberpruefen ob alles Uebertragen
	 *Das E/A-Geraet hat ein Limit von USR1MAXFR Bytes
	 *fuer eine Uebertragung */
	if ((bp->b_resid -=min(bp->b_resid,USR1MAXFR))>0) {
 
	    /* Uebertragung ist noch nicht fertig,
	     * naechsten Teil uebertragen */
	    bp->b_blkno = USR1MAXFR / BSIZE;
	    bp->b_paddr += USR1MAXFR;
	    usr1tab.b_errcnt = 0;
	    usr1start();
	    return;
	}
    } else  {  /*Fehler in der Uebertragung*/
	/******************************************
	* Geraete-abhaengiger Kode zum Einlesen des
	* Fehlerstatus und zum Loeschen des Fehlers
	*******************************************
 
	/*USR1RETRYLIMIT malige Wiederholung des Auftrags*/
	if (++usr1tab.b_errcnt <  USR1RETRYLIMIT) {

	    deverror(bp, USR1ERR1, USR1ERR2);
	    /*USR1ERR1 und USR1ERR2 sind Geraete-
	     *spezifische Status-Indikatoren, gewoehnlich
	     *Geraete-Fehlerregister, die ueber in/inb-
	     *Calls eingelesen werden */

	    usr1start();
	    return;
	}
	else {   /*zu viele Wiederholungen*/
	    printf("usr1: unrecoverable error\n");
	    bp->b_flags |= B_ERROR;
	}
    }	/*Ende der Fehlerbehandlung*/
 
    /* Puffer aus Auftragswarteschlange streichen */
    (bp->av_forw)->av_back = bp->av_back;
    (bp->av_back)->av_forw = bp->av_forw;
 
    /* Start einer neuen Uebertragung */
    if (direction)
	usr1current = bp->av_forw; /*Zyl.Nr. steigend*/
    else
	usr1current = bp->av_back; /* Zyl.-Nr fallend */

    /*kein weiterer Puffer in der aktuellen Richtung ?*/
    if (usr1current = (struct buf *)&usr1tab) {
	/* Wechsel der Richtung */
	if (direction == 0) {
	    direction = 1;
	    usr1current = bp->av_forw;
	} else {
	    direction = 0;
	    usr1current = bp->av_back;
	}
    }
 
    usr1tab.b_errcnt = 0;
    bp->b_error = 0;

    iodone(bp); /* alten Auftrag als abgearbeitet
		 * kennzeichnen */
    usr1start(); /* neuer Auftrag */
}
 
/**************************************************
* Start-Routine
**************************************************/
usr1start()
{
    register struct buf *bp; /* Zeiger zum aktuellen
			      * Puffer */
 
    if ((bp = usr1current) == (struct buf *)&usr1tab) {
	return; /* Auftragsschlange ist leer */
    }
    usr1tab.b_active++; /* Geraet als aktiv kennzeichnen */
 
    /*****************************************************
    * Geraete-abhaengiger Kode
    *
    *	Zur Erstellung des Geraetekommondos benutze:
    *
    *	bp->b_blkno	Blocknummer auf Minor-Geraet
    *	bp->b_dev	Geraete-Nummer (Major/Minor)
    *	   major(bp->bdev) und minor(bp->b_dev)
    *	   sind Macros zur Selektion der Nummern
    *	bp->b_resid	Zaehler der noch zu uebertragenen
    *			Bytes
    *	bp->b_paddr	24 bit physische Adresse, wo
    *			Uebertragung startet.
    *	bp->b_flags	neben anderen Dingen enthaelt es
    *			die Uebertragungsrichtung
    *	command = (bp->b_flags&B_READ ) ? READ:WRITE;
    *******************************************************/
}
 
/**************************************************
* Strategy-Routine
**************************************************/
usr1strategy(bp)
register struct buf *bp;
{
    register struct buf *ip; /* Pointer zum Einfuegungs-
			      * punkt in der Pufferliste */
    register int zyl;	     /* Zylindernummer */

    /* Ueberpruefen, ob gerade Adresse und gerade Anzahl
     * der zu uebertragenden Bytes
     * einige Geraete erfordern dieses nicht */

    if ((int)(bp->b_paddr & 1) || (bp->b_bcount & 1)) {
	u.u_error = EFAULT;
	iodone(bp);
	return;
    }
 
    /* Ueberpruefung der Gueltigkeit des Requests fuer
     * das Minor-Geraet */

    if (minor(bp->b_dev) >= USR1MINORMAX ||
	bp->b_blkno < 0 ||
	bp->b_blkno + ((bp->b_bcount + BSIZE - 1) >>
	 BSHIFT) > usr1sizes[minor(bp->b_dev)].vd_nblocks)
    {
	bp->b_flags | = B_ERROR;
	bp->b_error = DEMEDIA;
	iodone(bp);
	return;
    }
    /* Request ist OK, Aufrag in Auftragsschlange
     * einordnen */
    bp->b_resid = bp->b_bcount;

    /* Zylindernummer des physischen Blocks
     * 72 = Anzahl der Sektoren je Zylinder
     * (Laufwerksabhaengig) */
    zyl = (usr1sizes[minor(bp->b_dev)].vd_blkoff
	   + bp->b_blkno) / 72;
    bp->b_error = zyl; /* b_error wird missbraucht */
 
    /* Zugriff auf Warteschlange nur im DI, da auch die
     * Int-Routine darauf zugreift */
    dvi();
    /* Ein haeufiges Kriterium zum Einsortieren in
     * die Auftragswarteschlange ist die Zylindernummer.
     * Damit werden die Kopfbewegungen des Laufwerks
     * minimiert */
    for (ip = usr1tab.b_actf; ip != (struct buf *)&usr1tab;
		ip=ip->av_forw)
    {
	if (zyl < ip->b_error)
		break;	/*Stelle zum Einordnen gefunden*/
    } 
    /* Der neue Puffer auf den bp zeigt, wird vor dem
     * Puffer auf den ip zeigt, in die Warteschlange
     * eingefuegt */
    /* Vorwaertsverbindung im neuen Puffer */
    bp->av_forw = ip;
    /* Rueckverbindung im neuen Puffer */
    bp->av_back = ip->av_back;
    /* Vorwartsverbindung im Puffer vor bp */
    (bp->av_back)->av_forw = bp;
    /* Rueckverbindung im ip Puffer */
    ip->av_back = bp;

    /* ist die Auftragsschlange leer ?*/
    if (usr1current == (struct buf *)&usr1tab) {
	usr1current = bp;
    }
    /*wenn das Geraet nicht beschaeftigt ist, starte es */
    if (usr1tab.b_active == NULL)
	usr1start();
    evi();
}
 
/**************************************************
* Open-Routine
**************************************************/
usr1open(dev, flag)
dev_t dev;
int flag;    /* kennzeichnet Open fuer Read oder Write */
{
    static int usr1first = 0;	/* Geraete-Init-Flag */

    if (usr1first++ == 0) {	/* erstmaliger Aufruf ? */
	/* Auftrags-Warteschlange initialisieren */
	usr1tab.b_actf = (struct buf *)&usr1tab;
	usr1tab.b_actl = (struct buf *)&usr1tab;
 
	/**************************************
	* Geraete-abhaengiger Initialisierungs-
	* Kode (wird nur einmal abgearbeitet)
	* z.B. Init des Int-Vektors
	**************************************/
    }
    /*******************************************
    * Geraete-spezifischer Kode zum Ueberpruefen
    * des Open-Request, z.B Write zu einem
    * schreibgeschuetzten Geraet
    *******************************************/
}
 
/**************************************************
* Close-Routine
**************************************************/
usr1close(dev, flag)
dev_t dev;
int flag;
{
    /*******************************
    * Geraete-abhaengiger Close-Kode
    * (oft eine "Null"-Prozedur
    *******************************/
}

/**************************************************
* Read-Routine
**************************************************/
usr1read(dev)
dev_t dev;
{
    physio(usr1strategy, &rusr1buf, dev, B_READ);
}
 
/**************************************************
* Write-Routine
**************************************************/
usr1write(dev)
dev_t dev;
{
    physio(usr1strategy, &rusr1buf, dev, B_WRITE);
}
