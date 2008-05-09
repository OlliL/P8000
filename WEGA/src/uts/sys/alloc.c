/******************************************************************************
*******************************************************************************

	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1988

	KERN 3.2	Modul: alloc.c


	Bearbeiter:
	Datum:		$D$
	Version:	$R$

*******************************************************************************
******************************************************************************/

char allocwstr[] = "@[$]alloc.c		Rev : 4.1 	09/15/83 07:13:22";

#include <sys/param.h>
#include <sys/sysinfo.h>
#include <sys/systm.h>
#include <sys/mount.h>
#include <sys/filsys.h>
#include <sys/fblk.h>
#include <sys/conf.h>
#include <sys/buf.h>
#include <sys/inode.h>
#include <sys/ino.h>
#include <sys/dir.h>
#include <sys/mmu.h>
#include <sys/s.out.h>
#include <sys/user.h>
extern int Ninode;
extern int Nmount;

/*------------------------------------------ Typdefinitionen ----------------*/

typedef	struct fblk *FBLKP;

/*------------------------------------------ Funktionen ---------------------*/

/*---------------------------------------------------------------------------*/
/*
 * alloc
 * alloc weist den naechsten verfuegbaren (freien) Diskblock aus der
 * Freispeicherliste des Superblockes des entsprechenden Geraetes (dev) zu.
 * 
 * Der Superblock enthaelt bis zu NICFREE freie Bloecke.
 * Die letzte Eintragung verweist auf einen weiteren Ankerblock der
 * Freispeicherliste.
 * "no space on dev x/y" --> wenn die Freiliste erschoepft ist
/*---------------------------------------------------------------------------*/
struct buf *
alloc(dev)
dev_t dev;
{
	daddr_t bno;
	register struct filsys *fp;
	register struct buf *bp;

	fp = getfs(dev);
	while(fp->s_flock)
		sleep((caddr_t)&fp->s_flock, PINOD);
	do {
		if(fp->s_nfree <= 0)
			goto nospace;
		if (fp->s_nfree > NICFREE) {
			prdev("Bad free count", dev);
			goto nospace;
		}
		bno = fp->s_free[--fp->s_nfree];
		if(bno == 0)
			goto nospace;
	} while (badblock(fp, bno, dev));
	if(fp->s_nfree <= 0) {
		fp->s_flock++;
		bp = bread(dev, bno);
		if ((bp->b_flags&B_ERROR) == 0) {
			fp->s_nfree = ((FBLKP)(bp->b_un.b_addr))->df_nfree;
			bcopy((caddr_t)((FBLKP)(bp->b_un.b_addr))->df_free,
			    (caddr_t)fp->s_free, sizeof(fp->s_free));
		}
		brelse(bp);
		fp->s_flock = 0;
		wakeup((caddr_t)&fp->s_flock);
		if (fp->s_nfree <=0)
			goto nospace;
	}
	bp = getblk(dev, bno);
	clrbuf(bp);
	if(fp->s_tfree) 		/* Decrem. freelist-counter */
		fp->s_tfree--;
	fp->s_fmod = 1;
	return(bp);

nospace:
	fp->s_nfree = 0;
	fp->s_tfree = 0;	/* Total Free Blocks zero */
	prdev("no space", dev);
	u.u_error = ENOSPC;
	return(NULL);
}

/*---------------------------------------------------------------------------*/
/*
 * free
 * Rueckketten des Blockes (bno) in die Freispeicherliste des
 * Superblockes des entsprechenden Geraetes (dev)
 */
/*---------------------------------------------------------------------------*/
free(dev, bno)
dev_t dev;
daddr_t bno;
{
	register struct filsys *fp;
	register struct buf *bp;

	fp = getfs(dev);
	fp->s_fmod = 1;
	while(fp->s_flock)
		sleep((caddr_t)&fp->s_flock, PINOD);
	if (badblock(fp, bno, dev))
		return;
	if(fp->s_nfree <= 0) {
		fp->s_nfree = 1;
		fp->s_free[0] = 0;
	}
	if(fp->s_nfree >= NICFREE) {
		fp->s_flock++;
		bp = getblk(dev, bno);
		((FBLKP)(bp->b_un.b_addr))->df_nfree = fp->s_nfree;
		bcopy((caddr_t)fp->s_free,
			(caddr_t)((FBLKP)(bp->b_un.b_addr))->df_free,
			sizeof(fp->s_free));
		fp->s_nfree = 0;
		bwrite(bp);
		fp->s_flock = 0;
		wakeup((caddr_t)&fp->s_flock);
	}
	fp->s_free[fp->s_nfree++] = bno;
	fp->s_tfree++;		/* Total Free Blocks */
	fp->s_fmod = 1;
}

/*---------------------------------------------------------------------------*/
/*
 * badblock
 * Test, ob Blocknummer bn zwischen dem Ende der I-Liste und dem
 * ersten nicht vorhandenen Block des Geraetes (dev) liegt
 * Wird hauptsaechlich genutzt, um zu kontrollieren, ob ein "Garbage"-
 * Filesystem gemounted ist 
 * "bad block on dev x/y" ==> Blck liegt ausserhalb zulaess. Bereich
 */
/*---------------------------------------------------------------------------*/
badblock(fp, bn, dev)
register struct filsys *fp;
register daddr_t bn;
dev_t dev;
{

	if (bn < fp->s_isize || bn >= fp->s_fsize) {
		prdev("bad block", dev);
		return(1);
	}
	return(0);
}

/*---------------------------------------------------------------------------*/
/*
 * ialloc
 * Weist einen unbenutzten Inode aus der Inode-Freiliste des Superblockes
 * fuer das angegebene Geraet (dev) zu und stellt ihn in der Inode-Tabelle
 * bereit
 * Wird zur File-Kreation benoetigt
 * Der Algorithmus durchlaeuft die Inodes, die im Superblock eingetragen
 * sind (NICINODE Stueck). Wenn diese nicht ausreichen, so erfolgt eine
 * lineare Suche durch die Inodes in den Inode-Bloecken des Geraetes,
 * um NICINODE freie Inodes bereitzustellen
 */
/*---------------------------------------------------------------------------*/
struct inode *
ialloc(dev)
dev_t dev;
{
	register struct filsys *fp;
	register struct buf *bp;
	register struct inode *ip;
	int i;
	struct dinode *dp;
	ino_t ino;
	daddr_t adr;

	fp = getfs(dev);
	while(fp->s_ilock)
		sleep((caddr_t)&fp->s_ilock, PINOD);
loop:
	if(fp->s_ninode > 0) {
		ino = fp->s_inode[--fp->s_ninode];
		if (ino < ROOTINO)
			goto loop;
		ip = iget(dev, ino);
		if(ip == NULL)
			return(NULL);
		if(ip->i_mode == 0) {
			for (i=0; i<NADDR; i++)
				ip->i_blks.i_p.i_a[i] = 0;
			if(fp->s_tinode)	/* Decr. Inode Counter */
				fp->s_tinode--;
			fp->s_fmod = 1;
			return(ip);
		}
		/*
		 * Inode was allocated after all.
		 * Look some more.
		 */
		iput(ip);
		goto loop;
	}
	fp->s_ilock++;
	ino = 1;
	for(adr = SUPERB+1; adr < fp->s_isize; adr++) {
		bp = bread(dev, adr);
		if (bp->b_flags & B_ERROR) {
			brelse(bp);
			ino += INOPB;
			continue;
		}
		dp = bp->b_un.b_dino;
		for(i=0; i<INOPB; i++) {
			if(dp->di_mode != 0)
				goto cont;
			for(ip = &inode[0]; ip < &inode[Ninode]; ip++)
			if(dev==ip->i_dev && ino==ip->i_number)
				goto cont;
			fp->s_inode[fp->s_ninode++] = ino;
			if(fp->s_ninode >= NICINOD)
				break;
		cont:
			ino++;
			dp++;
		}
		brelse(bp);
		if(fp->s_ninode >= NICINOD)
			break;
	}
	fp->s_ilock = 0;
	wakeup((caddr_t)&fp->s_ilock);
	if(fp->s_ninode > 0)
		goto loop;
	prdev("Out of inodes", dev);
	u.u_error = ENOSPC;
	fp->s_tinode = 0;		/* Clear Inode Counter */
	return(NULL);
}

/*---------------------------------------------------------------------------*/
/*
 * ifree
 * Freigabe der spezifizierten Inode-Nr.(ino) auf dem spezifizierten Device
 * Der Algorithmus speichert bis zu NICINOD Inodes im Superblock ab; folgende
 * Inodes werden uebergangen
 */
/*---------------------------------------------------------------------------*/
ifree(dev, ino)
dev_t dev;
ino_t ino;
{
	register struct filsys *fp;

	fp = getfs(dev);
	fp->s_tinode++;		/* Total free inodes */
	if(fp->s_ilock)
		return;
	if(fp->s_ninode >= NICINOD)
		return;
	fp->s_inode[fp->s_ninode++] = ino;
	fp->s_fmod = 1;
}

/*---------------------------------------------------------------------------*/
/*
 * getfs
 * Bereitstellen eines Pointers auf den im Kern befindlichen Superblock
 * fuer eine angegebene Devicenummer "dev"
 * Der Alogorithmus stellt eine lineare Suche durch die Mount-Tabelle dar.
 * Konsistenzcheck der sich im Kern befindenden Freispeicherliste und des
 * Inode-Zaehlers
 *
 * "bad count on dev x/y" --> fehlerhafter Inode-Check
 *	Ab diesem Punkt werden die Inode- Zaehler auf Null gesetzt, was immer
 *	zu der Diagnose - Ausschrift "no space" fuehrt
 * "panic: no fs" --> Device ist nicht gemounted.
 */
/*---------------------------------------------------------------------------*/
struct filsys *
getfs(dev)
dev_t dev;
{
	register struct mount *mp;
	register struct filsys *fp;
	struct filsys *fp1;		/* ??? 4 Byte */

	for(mp = &mount[0]; mp < &mount[Nmount]; mp++)
	if(mp->m_bufp != NULL && mp->m_dev == dev) {
		fp = mp->m_bufp->b_un.b_filsys;
		if(fp->s_nfree > NICFREE || fp->s_ninode > NICINOD) {
			prdev("bad count", dev);
			fp->s_nfree = 0;
			fp->s_ninode = 0;
		}
		return(fp);
	}
	panic("no fs");
	return(NULL);
}

/*---------------------------------------------------------------------------*/
/*
 * update
 * update ist der interne Name von 'sync;
 * -durchlaeuft Disk-Queues zur Aktualisierung zwischengepufferter I/O-Bloecke
 * -durchlaeuft Inodes zum Schreiben modifizierter Inodes
 * -durchlaeuft Mount-Tabelle zur Aktualiseirung modifizierter Superbloecke
 */
/*---------------------------------------------------------------------------*/
update()
{
	register struct inode *ip;
	register struct mount *mp;
	register struct buf *bp;
	struct buf *bp1;		/* 4 Byte recomp ???? */
	struct filsys *fp;
	int fcw;

	if(updlock)
		return;
	updlock++;
	for(mp = &mount[0]; mp < &mount[Nmount]; mp++)
		if(mp->m_bufp != NULL) {
			fp = mp->m_bufp->b_un.b_filsys;
			if(fp->s_fmod==0 || fp->s_ilock!=0 ||
			   fp->s_flock!=0 || fp->s_ronly!=0)
				continue;
			bp = getblk(mp->m_dev, SUPERB);
			if (bp->b_flags & B_ERROR)
				continue;
			fp->s_fmod = 0;
			fp->s_time = time;
			bcopy((caddr_t)fp, bp->b_un.b_addr, BSIZE);
			bwrite(bp);
		}
	for(ip = &inode[0]; ip < &inode[Ninode]; ip++) {
		fcw=dvi();
		if((ip->i_flag&ILOCK)==0 && ip->i_count) {
			ip->i_flag |= ILOCK;
			ip->i_count++;
			iupdat(ip, &time, &time);
			iput(ip);
		}
		rvi(fcw);
	}
	updlock = 0;
	bflush(NODEV);
}
