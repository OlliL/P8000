/**************************************************************************
***************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW    Abt. Basissoftware - 1988
	KERN 3.2	Modul : mt.c
 
	Bearbeiter	: U. Oefler, P. Hoge
	Datum		: 15.2.88
	Version		: 1.1
 
***************************************************************************
**************************************************************************/

/*-------------------------------------------------------------------------*/
/*
 * Tape-Treiber fuer P8000
 */
/*-------------------------------------------------------------------------*/

#define NMT	2	/* Anzahl der Laufwerke */

#include  <sys/param.h>
#include  <sys/buf.h>
#include  <sys/iobuf.h>
#include  <sys/mmu.h>
#include  <sys/s.out.h>
#include  <sys/dir.h>
#include  <sys/user.h>
#include  <sys/conf.h>
#include  <sys/state.h>
#include  <sys/sysinfo.h>
#include  <sys/systm.h>
#include  <sys/file.h>
#include  <sys/ioctl.h>

#define b_repcnt b_bcount
#define b_command b_resid

/* Magnetband-Kommandos */
#define MT_RCOM		0x0006	/* Read */
#define MT_WCOM		0x000a	/* Write */
#define MT_WEOF		0x0012	/* Write EOF */
#define MT_SFORW	0x0022	/* Block vorwaers */
#define MT_SREV		0x8022	/* Block rueckwarts */
#define MT_FSF		0x0042	/* 1 Datei vorwaerts */
#define MT_BSF		0x8042	/* 1 Datei rueckwaerts */
#define MT_REW		0x0082	/* Rewind */

/* Return-Status */
#define MTS_NRDY	0x01	/* Geraet nicht bereit */
#define MTS_BUSY	0x02	/* Geraet beschaeftigt */
#define MTS_RLE		0x04	/* Blocklaengen-Fehler */
#define MTS_BOT		0x08	/* am Anfang des Bandes */
#define MTS_EOT		0x10	/* am Ende des Bandes */
#define MTS_EOF		0x20	/* End Of File */
#define MTS_WRP		0x40	/* Schreibgeschuetzt */
#define MTS_ERR		0x80	/* Aufzeichnungs-, Wiedergabe-, Geraetefehler */

extern unsigned mmut;			/* Adresse Code-MMU */
extern last_int_serv;
extern mt_flag;
extern dev_t console;

extern unsigned char mtcom[];		/* Kommando-Puffer */
extern unsigned char mtbuf[];		/* Zwischenspeicher fuer IO */

struct buf cmtbuf;			/* fuer ioctl und interne Kommandos */
struct buf rmtbuf;			/* fuer raw i/o */

/*
 * Queue-Header fuer mt-Queue
 */
struct iobuf mttab;

/*
 * Bits in Minor-Geraetenummer
 */
#define MTUNIT(dev)	(minor(dev)&1)
#define T_NOREWIND	0x80
#define INF		32767		/* Count bei Skip-Operationen */

/*
 * Software-Zustaende beim Band-Transport:
 *
 * 1. Ein Bandgeraet kan nur einmal eroeffnet werden, weiterer Open-Requests
 * werden zurueckgewiesen
 * 2. Beim Blockgeraete-Treiber wird die aktuelle Bandposition vermerkt.
 * Seek-Opetationen sind erlaubt.
 * 3. Bei Close wird eine EOF-Bandmarke geschrieben, wenn dar Treiber fuer
 * Schreiben eroeffnet war bzw. wenn der Treiber fuer Lesen und Schreiben
 * eroeffnet war und die letzte Operation ein Write war.
 */
struct mt_softc {
	int		sc_openf;	/* Verriegelung fuer weitere Open */
	unsigned	sc_lastiow;	/* letzte Operation war Write */
	daddr_t		sc_blkno;	/* aktuelle Blocknummer */
	unsigned	sc_nxrec;	/* Posit. des Bandendes, wenn bekannt */
	unsigned	sc_status;	/* Kopie des letzten Status */
	unsigned	sc_resid;	/* Kopie der letzten Return-Anzahl */
} mt_softc[NMT];

/*
 * Status fuer mttab.b_active
 */
#define SSEEK	1		/* Seek in Ausfuehrung */
#define SIO	2		/* sequent. I/O wird ausgefuehrt */
#define SCOM	3		/* Control-Kommando wird ausgefuehrt */
#define SREW	4		/* Rewind wird ausgefuehrt */
#define RSEEK	5		/* bei EOF bei Read Seek hinter EOF-Marke */

/*
 * Open
 *
 * Ein Bandgeraet kann nur einmal eroeffnet werden.
 * Zur Verhinderung von gegenseitigen Stoerungen der Benutzer
 * kan dies nur von der Systemkonsole aus erfolgen.
 */
mtopen(dev)
register dev_t dev;
{
	register struct mt_softc *sc;
	register unsigned mtunit;

	static unsigned mtopenflg = 0;

	if (mtopenflg == 0) {
		mt_init();
		mtopenflg++;
	}
	mtunit = MTUNIT(dev);
	if (mtunit >= NMT || (sc = &mt_softc[mtunit])->sc_openf
	   || minor(u.u_ttyd) != console) {
		u.u_error = ENXIO;
		return;
	}
/*
 * Tape ist WEGA durch Major-Nummer 3 bekannt
	mttab.b_flags |= B_TAPE;
*/
	sc->sc_openf = 1;
	sc->sc_blkno = 0;
	sc->sc_nxrec = INF;
	sc->sc_lastiow = 0;
}

/*
 * Close
 *
 * Bei Close wird erforderlich (siehe oben) eine EOF-Bandmarke
 * geschrieben und das Band beim Rewind-Geraet zurueckgespult.
 */
mtclose(dev, flag)
register dev_t dev;
register flag;
{
	register struct mt_softc *sc = &mt_softc[MTUNIT(dev)];

	if (flag == FWRITE || (flag & FWRITE) && sc->sc_lastiow)
		mtcommand(dev, MT_WEOF, 1);	/* Write EOT */
	if ((minor(dev)&T_NOREWIND) == 0)
		/*
		 * 0 mal MT_REW bedeutet, dass nicht auf die Beendigung
		 * des Rueckspulens gewartet wird. Der Puffer ctmbuf
		 * bleibt jedoch bis zur Beendigung des Rueckspulens als
		 * Busy gekennzeichnet. (Ein neuer Request wartet)
		 */
		 mtcommand(dev, MT_REW, 0);
	sc->sc_openf = 0;
}

/*
 * Ausfuehrung eines Kommandos, count mal
 */
mtcommand(dev, com, count)
register dev_t dev;
register com;
register count;
{
	register struct buf *bp;

	bp = &cmtbuf;
	dvi();
	while (bp->b_flags & B_BUSY) {
		/*
		 * Dieser Test wird durchgefuehrt, weil B_BUSY
		 * niemals bei "non-waiting rewind" rueckgesetzt wird
		 */
		if (bp->b_repcnt == 0 && (bp->b_flags & B_DONE))
			break;
		bp->b_flags |= B_WANTED;
		sleep((caddr_t)bp, PRIBIO);
	}
	bp->b_flags = B_BUSY | B_READ;
	evi();
	bp->b_dev = dev;
	bp->b_repcnt = count;
	bp->b_command = com;
	bp->b_blkno = 0;
	bp->b_error = 0;
	mtistrategy(bp);
	/*
	 * Kein Wait, wenn Rewind bei Close ausgefuehrt wird.
	 * Dies ist der einzige Fall, wo count = 0 sein kann.
	 */
	if (count == 0)
		return;
	iowait(bp);
	if (bp->b_flags & B_WANTED)
		wakeup((caddr_t)bp);
	bp->b_flags &= B_ERROR;
}

/*
 * Strategie-Routine
 */
mtstrategy(bp)
register struct buf *bp;
{
	register struct mt_softc *sc;
	register unsigned mtunit;

	/* Ueberpruefung der Gueltigkeit*/
	mtunit = MTUNIT(bp->b_dev);
	if (mtunit >= NMT || bp->b_bcount > 20*0x200) {
		u.u_error = ENXIO;
		iodone(bp);
		return;
	}
	sc = &mt_softc[mtunit];
	if ((bp->b_flags & B_READ) == 0)
		sc->sc_lastiow = 1;
	else
		sc->sc_lastiow = 0;
	mtistrategy(bp);
}

/*
 * interne Strategy-Routine
 */
mtistrategy(bp)
register struct buf *bp;
{
	bp->av_forw = NULL;
	dvi();
	if (mttab.b_actf == NULL)
		mttab.b_actf = bp;
	else
		mttab.b_actl->av_forw = bp;
	mttab.b_actl = bp;
	/*
	 * Start Controler, wenn nicht beschaeftigt
	 */
	 if (mttab.b_active == 0)
		mtstart();
	evi();
}

/*
 * Start-Routine
 */
mtstart()
{
	register struct mt_softc *sc;
	register struct buf *bp;
	register daddr_t blkno;
	unsigned count;

loop:	/* Wenn ein Dummy-Request den naechsten Request startet */
	if ((bp = mttab.b_actf) == NULL)
		return;
	sc = &mt_softc[MTUNIT(bp->b_dev)];

	/*
	 * wenn beim letzten Read ein EOF erkannt wurde, weil
	 * vor EOF positioniert wurde, muss der EOF-Record
	 * uebersprungen werden
	 */
	if ((sc->sc_status & MTS_EOF) && mttab.b_active == SIO) {
		bp->b_command = MT_SFORW;
		count = 1;
		mttab.b_active = SSEEK;
		goto dobpcmd;
	}
	if (sc->sc_openf == -1) {
		/*
		 * Hard-Error auf non-raw-Tape
		 * oder Geraet nicht verfuegbar
		 */
		bp->b_flags |= B_ERROR;
		goto next;
	}
	if (bp == &cmtbuf) {
		/*
		 * Ausfuehren einer Steueroperation mit angegebenen count
		 */
		if (bp->b_command == MT_REW)
			mttab.b_active = SREW;
		else
			mttab.b_active = SCOM;
		count = bp->b_repcnt;
		goto dobpcmd;
	}
	/*
	 * Ueberpruefung der Uebertragungsgrenzen fuer nicht-raw-Tapes,
	 * bei raw-Tapes wird sc->nxrec durch mtphys initialisiert und
	 * damit die normale Bewegung des Bandes gesichert
	 */
	if (bp->b_blkno > sc->sc_nxrec) {
		/*
		 * kann nicht nach bekanntem EOF lesen
		 */
		bp->b_flags |= B_ERROR;
		bp->b_error |= DEMEDIA;
		goto next;
	}
	if (bp->b_blkno == sc->sc_nxrec && (bp->b_flags & B_READ)) {
		/*
		 * Read bei EOF, Return 0 Bytes
		 */
		bp->b_resid = bp->b_bcount;
		clrbuf(bp);
		goto next;
	}
	if ((bp->b_flags & B_READ) == 0)
		/*
		 * Write setzt EOF
		 */
		sc->sc_nxrec = bp->b_blkno + 1;
	/*
	 * Wenn das Band fuer das Daten-Transfer-Kommando richtig
	 * positioniert ist, werden
	 * alle Register zum Start der E/A gesetzt
	 */
	if ((blkno = sc->sc_blkno) == bp->b_blkno) {
		mttab.b_active = SIO;
		count = bp->b_bcount;
		if ((bp->b_flags & B_READ) == 0) {
			bp->b_command = MT_WCOM;
			mtcopy(bp, count);	/* User-Adrese -> mtbuf */
			if (count == 1) { /* CM kann nicht 1 Byte schreiben */
				count = 2;
				mtbuf[1] = 0;
			}
		} else
			bp->b_command = MT_RCOM;
		goto dobpcmd;
	}
	/*
	 * Band ist nicht richtig positioniert, Seek vorwaerts oder
	 * rueckwaerts zur richtigen Stelle
	 * (tritt bei raw-I/O niemals auf)
	 */
	mttab.b_active = SSEEK;
	if (blkno < bp->b_blkno) {
		bp->b_command = MT_SFORW;
		count = bp->b_blkno - blkno;
	} else {
		bp->b_command = MT_SREV;
		count = blkno - bp->b_blkno;
	}

dobpcmd: /* Kommando ausfuehren */

	mt_flag++;
	mtsend(bp->b_dev, bp->b_command, count);
	return;

next:	/*
	 * Error trat auf oder es war ueberhaupt nichts zu tun
	 * Austragen aus Warteschlange und naechsten Request ausfuehren
	 */
	mttab.b_actf = bp->av_forw;
	iodone(bp);
	goto loop;
}

/*
 * Interrupt-Routine
 */
mtint(state)
struct state *state;
{
	register struct buf *bp;
	register struct mt_softc *sc;
	register unsigned mtunit;
	register unsigned stat;

	last_int_serv = MTINT;
	reti();
	mt_flag = 0;
	if ((bp = mttab.b_actf) == NULL) {
		printf ("mt: unexpected VI 0x%x ignored\n",state->s_eventid);
		return;
	}
	mtunit = MTUNIT(bp->b_dev);
	sc = &mt_softc[mtunit];
	/*
	 * Operation fertig, Status lesen
	 */
	if (mtunit == 0)
		sc->sc_status = mtcom[7];
	else
		sc->sc_status = mtcom[8];

	if (bp->b_command == MT_WCOM)
		sc->sc_resid = bp->b_bcount;
	else
		sc->sc_resid = (mtcom[6] << 8) | mtcom[5];

	stat = mttab.b_active;
	mttab.b_active = 0;
	/*
	 * Test auf Errors
	 */
	if ((sc->sc_status & (MTS_ERR|MTS_BUSY|MTS_NRDY)) == 0) {
	  /*
	   * bei Rewind
	   */
	   if (stat == SREW)
		goto ignoreerr;
	  /*
	   * bei EOF der Datei, Position aktualisieren
	   */
	  if (sc->sc_status & MTS_EOF) {
		mtseteof(bp);		/* set blkno und nxrec */
		if (stat == SIO) {	/* 1. EOF-Interrupt */
			mttab.b_active = stat;
			mtstart();	/* bei Read-Operation */
			return;
		}
		if (stat == RSEEK)	/* 2. EOF-Interrupt */
			sc->sc_resid = 0;
		goto opdone;
	  }
	  /*
	   * Wenn beim Lesen von raw-Tape der Record zu lang war, wird
	   * der Fehler ignoriert
	   */
	  if (bp == &rmtbuf && (bp->b_flags&B_READ) && (sc->sc_status&MTS_RLE)){
		sc->sc_resid = min(sc->sc_resid, bp->b_bcount);
		goto ignoreerr;
	  }
	  /*
	   *  wenn Bandanfang erkannt
	   */
	  if (sc->sc_status & MTS_BOT) {
	    if (stat == SCOM) {
		if (sc->sc_resid == bp->b_repcnt)
			goto ignoreerr;
		else if (bp->b_command==MT_BSF && sc->sc_resid==bp->b_repcnt-1)
			goto ignoreerr;
	    }
	    if ((stat == SSEEK) && (bp->b_blkno == 0))
			goto ignoreerr;
	    sc->sc_blkno = 0;
	    bp->b_flags |= B_ERROR;
	    bp->b_error |= DNMEDIA;
	    goto opdone;
	  }
	}
	/*
	 * wenn I/O-Operation und kein "harter" Fehler
	 * bis zu 2 mal wiederholen
	 */ 
	if (((sc->sc_status & (MTS_NRDY|MTS_BUSY|MTS_EOT|MTS_WRP))==0)
	      && (sc->sc_status & MTS_ERR) && stat == SIO) {
		if (mttab.b_errcnt == 0)
			deverror(bp, state->s_eventid, sc->sc_status);
		if (++mttab.b_errcnt < 2) { /* Tape hat 8 Wiederholungen */
			/*
			 * sc->sc_blkno++;
			 * unser Tape positioniert vor den fehlerhaften Block
			 */
			goto opcont;
		}
	}
	if (sc->sc_status != 0) {
		/*
		 * Fehler kann nicht beseitigt werden
		 */
		if ((sc->sc_openf > 0) && (bp != &rmtbuf))
			sc->sc_openf = -1; /* Close, wenn nicht raw-Tape */

		printf ("mt%d: fatal error %x\n", mtunit, sc->sc_status);
		bp->b_flags |= B_ERROR;
		goto opdone;
	}

	/*
	 * Tape-control-FSH
	 */
ignoreerr:
	switch (stat) {
	case SIO:
		/*
		 * Tape-Blocknummer bei Read/Write inkrementieren
		 */
		sc->sc_blkno++;
		if (bp->b_command == MT_RCOM)	  /* bei Read: */
			mtcopy(bp, sc->sc_resid); /* mtbuf -> User-Adresse */
		goto opdone;
	case SCOM:
		/* bei Record-weisem Vor- Rueckspulen
		 * aktuelle Position aktualisieren
		 */
		switch (bp->b_command) {
		case MT_SFORW:
			sc->sc_blkno += bp->b_repcnt;
			break;
		case MT_SREV:
			sc->sc_blkno -= bp->b_repcnt;
			break;
		}
		goto opdone;
	case SSEEK:
		sc->sc_blkno = bp->b_blkno;
		goto opcont;
	case SREW:
		sc->sc_blkno = 0;
		sc->sc_nxrec = INF;
	}
opdone:
	/*
	 * Ruecksetzen Error-Zaehler und Austragen aus Warteschlange
	 */
	mttab.b_errcnt = 0;
	mttab.b_actf = bp->av_forw;
	bp->b_resid = bp->b_bcount - sc->sc_resid;
	iodone(bp);
opcont:
	mtstart();
}

/*
 * mtseteof
 */
mtseteof(bp)
register struct buf *bp;
{
	register struct mt_softc *sc = &mt_softc[MTUNIT(bp->b_dev)];

	if ((bp == &cmtbuf)
	     && (bp->b_command == MT_SFORW || bp->b_command == MT_SREV)) {
		if (sc->sc_blkno > bp->b_blkno) {
			/* Operation war Rueckspulen zum gewuenschten Block */
			/* steht vor der BM (am Ende der vorherigen Datei)  */
			sc->sc_blkno -= sc->sc_resid;
			sc->sc_nxrec = sc->sc_blkno;
		} else {
			/* Operation war Vorspulen */
			/* steht nach BM (am Anfang der neuen Datei */
			sc->sc_blkno += sc->sc_resid;
			sc->sc_nxrec = sc->sc_blkno -1;
		}
		return;
	}
	/*
	 * EOF bei Read
	 */
	/* 1.Int: steht vor BM (am Ende der Datei) */
	/* 2.Int: steht nach BM (am Anfang der naechsten Datei) */
	sc->sc_nxrec = bp->b_blkno;
}

/*
 * raw-Read
 */
mtread(dev)
dev_t dev;
{
	mtphys(dev);
	if (u.u_error)
		return;
	physio(mtstrategy, &rmtbuf, dev, B_READ);
}

/*
 * raw-Write
 */
mtwrite(dev)
dev_t dev;
{
	mtphys(dev);
	if (u.u_error)
		return;
	physio(mtstrategy, &rmtbuf, dev, B_WRITE);
}

/*
 * Ueberpruefen, ob ein raw-Geraet existiert.
 * Wenn ja, sc_blkno und sc_nxrec so setzen, dass Band korrekt
 * positioniert wird
 */
mtphys(dev)
register dev_t dev;
{
	register unsigned mtunit = MTUNIT(dev);
	register daddr_t a;
	register struct mt_softc *sc;

	if (mtunit >= NMT) {
		u.u_error = ENXIO;
		return;
	}
	sc = &mt_softc[mtunit];
	a = (u.u_offset >> BSHIFT);
	sc->sc_blkno = a;
	sc->sc_nxrec = a + 1;
}

/*
 * IO-Control
 */
mtioctl(dev, cmd, callcount)
register dev_t dev;
register cmd;
register callcount;
{
	register fcount;
	register struct mt_softc *sc = &mt_softc[MTUNIT(dev)];
	struct buf *bp = &cmtbuf;

	static mtops[] = {MT_FSF, MT_BSF, MT_SFORW, MT_SREV, 0, MT_WEOF,
			  MT_REW,};

	switch (cmd) {
	case MTIOWFM:
		fcount = 1;
		break;
	case MTIOFF:
	case MTIOFR:
	case MTIOBF:
	case MTIOBR:
		fcount = callcount;
		callcount = 1;
		break;
	case MTIORW:
		callcount = 1;
		fcount = 1;
		break;
	default:
		u.u_error = ENXIO;
		return;
	}
	if (callcount <= 0 || fcount <= 0) {
		u.u_error = ENXIO;
		return;
	}
	while (--callcount >= 0) {
		mtcommand(dev, mtops[cmd & 0xff], fcount);
		u.u_r.r_val1 = sc->sc_resid;
		if ((bp->b_flags & B_ERROR)||(sc->sc_status & MTS_BOT))
			return;
	}
}

mt_init()
{
	outb(0xd001, 0);	/* Segment-Nr an DMA-Karte */
	outb(0xffb3, 0x03);	/* CTC 1 Kanal 1 auf ext. Int */
	outb(0xffb3, 0xd7);
	outb(0xffb3, 0x01);
}

mtcopy(bp, count)
register struct buf *bp;
register count;
{
	struct segd mmuout;

	/* Programmierung MMU-Segment fuer Uebertragung */
	mmuout.sg_base = (int)(bp->b_paddr >>8);
	mmuout.sg_limit = 0xff;
	mmuout.sg_attr = 0;
	loadsd(mmut, MTSEG, &mmuout);

	if (bp->b_command == MT_RCOM)
		/* Zwischenpuffer zur physischen Adresse kopieren */
		bcopy(mtbuf, ((int)bp->b_paddr)&0x00ff, MTSEG<<8, count);
	else
		/* von physischer Adresse zum Zwischenspeicher kopieren */
		bcopy(((int)bp->b_paddr)&0x00ff, MTSEG<<8, mtbuf, count);
}

mtsend(unit, command, count)
register unsigned unit;
register unsigned command;
register unsigned count;
{
	mtcom[0] = command | (unit & 1);
	mtcom[1] = (unsigned)mtbuf & 0xff;	/* low Adresse */
	mtcom[2] = (unsigned)mtbuf >> 8;	/* high Adresse */
	mtcom[3] = count;			/* Laenge low */
	mtcom[4] = (count | (command & 0x8000)) >> 8;
	outb(0xd003, 0);			/* Start DMA */
	outb(0xffe1, 0);			/* M1 ausgeben */
}
