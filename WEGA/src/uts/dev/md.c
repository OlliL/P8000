/**************************************************************************
***************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW    Abt. Basissoftware - 1986
	KERN 3.2	Modul : md.c
 
	Bearbeiter	: P. Hoge
	Datum		: 5.4.88
	Version		: 1.3
 
***************************************************************************
**************************************************************************/

/*-----------------------------------------------------------------------*/
/*
 *  Die Routinen dieses Moduls realisieren in Verbindung mit dem Disk-
 *  Treiber (disk.s) den Verkehr mit den Disk-Devices.
 *  Die von mdstrat uebernommenen Puffer-Adressen werden als Auftraege
 *  in einer solchen Reihenfolge realisiert, dass die Kopfbewegungen der
 *  Disk optimiert werden. Dazu werden die Puffer nach der Block-
 *  Nummer sortiert in die Auftrags-Warte-
 *  Schlange von mdtab (av_ Pointer) uebernommem; von dort werden die in
 *  den Puffer-Koepfen enthalteten Auftraege sequentiell ausgefuehrt, wo-
 *  bei die Blocknummern anhand der Disk-Belegungstabelle md_sizes auf 
 *  Plausibilitaet ueberprueft werden.
 *  Der Verkehr mit der Disk wird fuer den aktuellen Auftrag von mdstart
 *  angestossen, worauf dann die Steuerung an den Modul bio.c abgegeben
 *  wird. Mit den Ende des Verkehrs aktiviert ein "Disk-Ende"-Interrupt
 *  die Endebehandlung mdint, in der der aktuelle Auftrag als fertig ge-
 *  meldet und die Realisierung eines weiteren Auftrags angestossen wird.
 */
/*-----------------------------------------------------------------------*/

#define  MXSIZE 8*0X200	/* maximale Blocklaenge	*/

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

extern unsigned nmd;
extern unsigned mmut;
extern struct vd_size md_sizes[];
extern int last_int_serv;
extern md_flag;

struct iobuf mdtab;
struct buf *mdnext = (struct buf *)&mdtab;
struct buf rmdbuf;

unsigned mdcount;

/*
 * Open-Routine
 */
mdopen()
{
	static unsigned open_flag = 0;

	if (open_flag == 0) {
		mdinit();
		/* Reset der Auftrags-Warteschlange */
		mdtab.b_actf = (struct buf *)&mdtab;
		mdtab.b_actl = (struct buf *)&mdtab;
		open_flag++;
	}
}

/*
 * Strategy-Routine
 */
mdstrat(bp)
register struct buf *bp;
{
	register struct buf *ptr;
	register struct vd_size *vp;

	/* Ueberpruefung der Parameter */
	vp = &md_sizes[(unsigned char)bp->b_dev];
	if ((unsigned char)bp->b_dev >= nmd ||
	    bp->b_blkno + (bp->b_bcount + BSIZE -1)/BSIZE > vp->vd_nblocks) {
		bp->b_flags |= B_ERROR;
		bp->b_error = DEMEDIA;
		iodone(bp);
		return;
	}
	bp->b_resid = bp->b_bcount;

	/* Einordnen des Auftrags */
	dvi();
	for (ptr = mdtab.b_actf; ptr != (struct buf *)&mdtab;ptr = ptr->av_forw)
		if (bp->b_dev < ptr->b_dev ||
		    (bp->b_dev == ptr->b_dev && bp->b_blkno < ptr->b_blkno))
			break;
	bp->av_forw = ptr;
	bp->av_back = ptr->av_back;
	ptr->av_back->av_forw = bp;
	ptr->av_back = bp;

	/* Anstoss einer Uebertragung, wenn kein Auftrag vorliegt */
	if (mdnext == (struct buf *)&mdtab)
		mdnext = bp;
	if (mdtab.b_active == 0)
		mdstart();
	evi();
}

/*
 * Start einer Uebertragung
 */
mdstart()
{
	register struct buf *bp;
	register struct vd_size *vp;
	register rw;
	register n;
	static struct segd mdmmu = {0, 0xff, 0};

	if ((bp = mdnext) == (struct buf *)&mdtab)
		return;

	mdtab.b_active++;
	vp = &md_sizes[(unsigned char)bp->b_dev];
	rw = bp->b_flags & B_READ;

	md_flag++;
	if (bp->b_flags & B_PHYS) {
		/* Programmierung MMU-Segment fuer Uebertragung */
		mdmmu.sg_base = (int)(bp->b_paddr >>8);
		loadsd(mmut, DISKSEG, &mdmmu);
		n = min(bp->b_resid, MXSIZE);
		mdcount = n;
		mdcmd(rw, vp->vd_unit, vp->vd_blkoff+bp->b_blkno, n);
		if (rw == 0)
			mdout(((int)bp->b_paddr)&0x00ff, DISKSEG <<8, n);
	} else {
		n = mdtest(bp, rw);
		mdcount = n;
		mdcmd(rw, vp->vd_unit, vp->vd_blkoff+bp->b_blkno, n*BSIZE);
		if (rw == 0)
			while (n) {
				mdout(bp->b_un.b_addr, BSIZE);
				bp = bp->av_forw;
				n--;
			}
	}
}

/*
 * Behandlung des "Disk-Ende"-Interrupts
 */
mdint(state)
register struct state *state;
{
	register struct buf *bp;
	register rw;
	register n;

	last_int_serv = MDINT;
	reti();
	md_flag = 0;

	if (mdtab.b_active == 0) {
		printf ("md: unexpected VI 0x%x ignored\n",state->s_eventid);
		return;
	} 
	bp = mdnext;

	if (n = mdint2()) { /* Fehler */
		if (mdtab.b_errcnt == 0)
			deverror(bp, state->s_eventid, n);
		if (++mdtab.b_errcnt < 4) {
			mdstart();
			return;
		}
		else {
			printf("md: fatal error\n");
			bp->b_flags |= B_ERROR;
			n = 1;
		}
	} else {
		n = mdcount;
		rw = bp->b_flags;
		if (rw & B_PHYS) {
			if (rw & B_READ) 
				mdin(((int)bp->b_paddr)&0x00ff, DISKSEG <<8, n);
			/* Test, ob Uebertragung vereinzelt werden muss */
			if (bp->b_resid -= n) {
				bp->b_blkno += MXSIZE/BSIZE;
				bp->b_paddr += MXSIZE;
				mdtab.b_errcnt = 0;
				mdstart();
				return;
			}
			n = 1;
		} else {
			if (rw & B_READ) {
				while (n) {
					mdin(bp->b_un.b_addr, BSIZE);
					bp = bp->av_forw;
					n--;
				}
				bp = mdnext;
				n = mdcount;
			}
		}
	}

	/* Puffer aus Auftrags-Warteschlange streichen */
	while (n) {
		bp->b_resid = 0;
		bp->b_error = 0;
		bp->av_forw->av_back = bp->av_back;
		bp->av_back->av_forw = (mdnext = bp->av_forw);
		iodone(bp);
		bp = mdnext;
		n--;
	}
	/* Bereitstellung des naechsten Auftrags */
	if (mdnext == (struct buf *)&mdtab)
		mdnext = mdtab.b_actf;
	mdtab.b_active = 0;
	mdtab.b_errcnt = 0;
	mdstart();
}

/*
 * Ermittlung Anzahl hintereinander folgender Bloecke
 */
mdtest(bp, rw)
register struct buf *bp;
int rw;
{
	register daddr_t bnr;
	register dev_t dev;
	register n;
	
	for (n = 1; n < MXSIZE/BSIZE; n++) {
		bnr = bp->b_blkno + 1;
		dev = bp->b_dev;
		if ((bp = bp->av_forw) == (struct buf *)&mdtab)
			return(n);
		if (rw != (bp->b_flags & (B_READ|B_PHYS)))
			return(n);
		if (dev != bp->b_dev || bnr != bp->b_blkno)
			return(n);
	}
	return(n);
}

/*
 * Raw-Read
 */
mdread(dev)
unsigned dev_t dev;
{
	physio(mdstrat, &rmdbuf, dev, B_READ);
}

/*
 * Raw-Write
 */
mdwrite(dev)
unsigned dev_t dev;
{
	physio(mdstrat, &rmdbuf, dev, B_WRITE);
}
