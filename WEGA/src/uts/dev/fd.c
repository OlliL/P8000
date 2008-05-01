/**************************************************************************
***************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW    Abt. Basissoftware - 1986
	KERN 3.2	Modul : fd.c
 
	Bearbeiter	: P. Hoge, U. Wiesner
	Datum		: 23.6.88
	Version		: 1.2
 
***************************************************************************
**************************************************************************/

#define  MXSIZE 20*0X200	/* maximale Blocklaenge	*/

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
#include  <sys/koppel.h>
#include  <sys/ioctl.h>

#define ACK 		6
#define FL_READ		1
#define FL_WRITE	2

#define FDPRI		25

extern unsigned mmut;
extern int last_int_serv;
extern fd_flag;
extern dev_t console;
extern udos;

struct iobuf fdtab;
struct buf *fdnext = (struct buf *)&fdtab;
struct buf rfdbuf;

long fd_addr;
unsigned fd_len;
int closeflg;
unsigned rw;			/* FL_READ oder FL_WRITE */


/*------------------------------------------------------------------------
 *
 *		OPEN - Routine
 *
 *------------------------------------------------------------------------
 */

fdopen(dev)
register dev_t dev;
{
	if (udos != 2 || minor(u.u_ttyd) != console || ksflag == UD) {
		u.u_error = ENXIO;
		return;
	}
	if (ksflag == 0) {
		/*** Reset der Auftrags-Warteschlange ***/
		fdtab.b_actf = (struct buf *)&fdtab;
		fdtab.b_actl = (struct buf *)&fdtab;
	}
	ksflag |= 1 << (dev & 0x0003);
}

/*------------------------------------------------------------------------
 *
 *	CLOSE - Routine
 *
 *------------------------------------------------------------------------
 */

fdclose(dev)
register dev_t dev;
{
	register unsigned s;

	s = dvi();
	if ((ksflag &= ~(1 << (dev & 03))) == 0) {
		while (fdtab.b_active != 0) {
			closeflg++;
			sleep(&fdtab.b_active, FDPRI);
		}
	}
	rvi(s);
}

/*----------------------------------------------------------------------
 *
 *		DISK - Handler
 *
 *  Die Routine fdstrat ordnet die Reihenfolge der eingehenden Zugriffs-
 *  Auftraege (Puffer) und initiiert den Zugriff auf die Floppy-Disk, wenn bis
 *  dahin kein Auftrag aktuell in Bearbeitung war.
 *-----------------------------------------------------------------------
 */
fdstrat(bp)
register struct buf *bp;
{
	register struct buf *ptr;

	bp->b_resid = bp->b_bcount;

	/* Einordnen des Auftrags */
	dvi();
	for (ptr = fdtab.b_actf; ptr != (struct buf *)&fdtab;ptr = ptr->av_forw)
		if (bp->b_dev < ptr->b_dev ||
		    (bp->b_dev == ptr->b_dev && bp->b_blkno < ptr->b_blkno))
			break;
	bp->av_forw = ptr;
	bp->av_back = ptr->av_back;
	ptr->av_back->av_forw = bp;
	ptr->av_back = bp;

	/*** Anstoss einer Uebertragung, wenn kein Auftrag vorliegt ***/
	if (fdnext == (struct buf *)&fdtab)
		fdnext = bp;
	if (fdtab.b_active == 0)
		fdstart();
	evi();
}

/*------------------------------------------------------------
 *
 *		Start einer Uebertragung
 *
 *------------------------------------------------------------
 */

fdstart()
{
	register struct buf *bp;
	static struct segd fdmmu = {0, 0xff, 0};

	if ((bp = fdnext) == (struct buf *)&fdtab) {
		if (closeflg != 0) {
			closeflg = 0;
			wakeup(&fdtab.b_active);
		}
		return;
	}

	fdtab.b_active++;

	/*** Programmierung MMU-Segment fuer Uebertragung ***/
	fdmmu.sg_base = (int)(bp->b_paddr >>8);
	loadsd(mmut, FDISKSEG, &fdmmu);

	if ((rw = bp->b_flags & B_READ) != B_READ)
		rw = FL_WRITE;

	hiword(fd_addr) = FDISKSEG <<8;
	loword(fd_addr) = ((int)bp->b_paddr)&0x00ff;
	fd_len = min(bp->b_resid, MXSIZE);
	fd_flag++;
	fd_disk(rw, bp->b_dev, bp->b_blkno);
	evi();
}

/*------------------------------------------------------------------
 *
 *	Behandlung des "Floppy-Disk-Ende"-Interrupts
 *
 *------------------------------------------------------------------
 */

fdint(r)
register r;		/* Rueckgabewert vom 8-Bit-Teil */
{
	register struct buf *bp;
	register unsigned com_code;

	last_int_serv = FDINT;
	fd_flag = 0;

	if (fdtab.b_active == 0) {
		printf ("fd: unexpected VI ignored\n");
		return;
	} 
	com_code = ((r==ACK)? 0 : r);
	if (r == ACK && rw == FL_READ)
		com_code = fdrint();	/* Daten einlesen */
	fdtab.b_active = 0;
	bp = fdnext;
	if (com_code != 0) {
		deverror(bp, 0, com_code);
		printf("fd: fatal error (%x)\n", com_code);
		bp->b_flags |= B_ERROR;
	} else {
		/*** Test, ob Uebertragung vereinzelt werden muss ***/
		if (bp->b_resid -= min(bp->b_resid, MXSIZE)) {
			bp->b_blkno += MXSIZE/BSIZE;
			bp->b_paddr += MXSIZE;
			fdstart();
			return;
		}
	} 
	/*** Puffer aus Auftrags-Warteschlange streichen ***/
	bp->av_forw->av_back = bp->av_back;
	bp->av_back->av_forw = bp->av_forw;

	/*** Bereitstellung des naechsten Auftrags ***/
	fdnext = bp->av_forw;
	if (fdnext == (struct buf *)&fdtab)
		fdnext = fdtab.b_actf;

	/*** Endemeldung des akt. Auftrags ***/
	bp->b_error = 0;
	iodone(bp);
	fdstart();
}

/*-------------------------------------------------------
 *
 *	Direktes Lesen von Floppy-Disk
 *
 *-------------------------------------------------------
 */

fdread(dev)
unsigned dev_t dev;
{
	physio(fdstrat, &rfdbuf, dev, B_READ);
}

/*------------------------------------------------------
 *
 *	Direktes Schreiben auf Floppy-Disk
 *
 *------------------------------------------------------
 */

fdwrite(dev)
unsigned dev_t dev;
{
	physio(fdstrat, &rfdbuf, dev, B_WRITE);
}
 
/*---------------------------------------------------------------------------*/
/*
 *fdioctl
 * Anzahl der Diskbloecke uebergeben
 */
/*---------------------------------------------------------------------------*/

int fdblocks[16] = {	0,	/* - */
			500,	/* Typ 1 */
			640,	/* Typ 2 (neu) */
			320,	/* Typ 3 */
			800,	/* Typ 4 (neu) */
			1280,	/* Typ 5 */
			1440,	/* Typ 6 */
			1600,	/* Typ 7 */
			 720,	/* Typ 8 */
			 360,	/* Typ 9 */
			 720,	/* Typ 10 (ehemals 7) */
			0,
			0,
			0,
			0,
			0, };

fdioctl(dev, cmd, addr)
register dev_t dev;
register cmd;
register caddr_t addr;
{
	register blocks;

	if (cmd == SHOWFD) {
		blocks = fdblocks[(dev & 0x00f0) >> 4];
		if (blocks != 0) {
			if ((suword(addr, blocks)) < 0)
				u.u_error = EFAULT;
			return;
		}
	}
	u.u_error = EINVAL;
}
