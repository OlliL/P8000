/**************************************************************************
***************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW    Abt. Basissoftware - 1988
	KERN 3.2	Modul : bio.c
 
	Bearbeiter	: P. Hoge
	Datum		: 1.12.89
	Version		: 1.5
 
***************************************************************************
**************************************************************************/

#include <sys/param.h>
#include <sys/sysparm.h>
#include <sys/sysinfo.h>
#include <sys/systm.h>
#include <sys/dir.h>
#include <sys/mmu.h>
#include <sys/buf.h>
#include <sys/conf.h>
#include <sys/proc.h>
#include <sys/s.out.h>
#include <sys/user.h>

#define NRBUF 8
#define NWBUF 8

extern unsigned Nhbuf;
struct buf swbuf1, swbuf2;

/*
 * Read in (if necessary) the block and return a buffer pointer.
 */

struct buf *
bqread(dev, flag, blkno)
register dev_t dev;
register flag;
register daddr_t blkno;
{
	register struct buf *bp;

	sysinfo.lread++;
	bp = getblk(dev, blkno);
	if (bp->b_flags & B_DONE)
		return(bp);
	bp->b_flags |= B_READ;
	bp->b_bcount = BSIZE;
	(*bdevsw[major(dev)].d_strategy)(bp);
	u.u_ior++;
	sysinfo.bread++;
	if (flag)
		iowait(bp);
	return(bp);
}

struct buf *
bread(dev, blkno)
dev_t dev;
daddr_t blkno;
{

	return(bqread(dev, 1, blkno));
}

/*
 * Read in the block, like bread, but also start I/O on the
 * read-ahead block (which is not allocated to the caller)
 */

struct buf *
breada(dev, blkno, rablkno)
register dev_t dev;
daddr_t blkno;
daddr_t rablkno;
{
	register i;
	register struct buf *bp, *rabp;

	bp = NULL;
	if (!incore(dev, blkno)) {
		sysinfo.lread++;
		bp = getblk(dev, blkno);
		if ((bp->b_flags & B_DONE) == 0) {
			bp->b_flags |= B_READ;
			bp->b_bcount = BSIZE;
			(*bdevsw[major(dev)].d_strategy)(bp);
			u.u_ior++;
			sysinfo.bread++;
		}
	}
	if (rablkno && !incore(dev, rablkno)) {
	   i = 1;
	   if (major(dev) == 0 && blkno+1 == rablkno)
		i = NRBUF;		/* nur fuer Harddisk */
	   while (i--) {
		if (!incore(dev, rablkno)) {
			rabp = getblk(dev, rablkno);
			if (rabp->b_flags & B_DONE)
				brelse(rabp);
			else {
				rabp->b_flags |= B_READ|B_ASYNC;
				rabp->b_bcount = BSIZE;
				(*bdevsw[major(dev)].d_strategy)(rabp);
				u.u_ior++;
				sysinfo.bread++;
			}
		}
		rablkno++;
	   }
	}
	if (bp == NULL)
		return(bqread(dev, 1, blkno));
	iowait(bp);
	return(bp);
}


/*
 * Write the buffer, waiting for completion.
 * Then release the buffer.
 */
bwrite(bp)
register struct buf *bp;
{
	register flag;

	sysinfo.lwrite++;
	flag = bp->b_flags;
	bp->b_flags &= ~(B_READ | B_DONE | B_ERROR | B_DELWRI | B_AGE);
	bp->b_bcount = BSIZE;
	(*bdevsw[major(bp->b_dev)].d_strategy)(bp);
	u.u_iow++;
	sysinfo.bwrite++;
	if ((flag & B_ASYNC) == 0) {
		iowait(bp);
		brelse(bp);
	} 
	else if (flag & B_DELWRI)
		bp->b_flags |= B_AGE;
	else if (bp->b_flags & B_ERROR && u.u_error == 0) {
		u.u_error = EIO;
		u.u_deverr = bp->b_error;
	}
}


/*
 * Release the buffer, marking it so that if it is grabbed
 * for another purpose it will be written out before being
 * given up (e.g. when writing a partial block where it is
 * assumed that another write for the same block will soon follow).
 * This can't be done for magtape, since writes must be done
 * in the same order as requested.
 */
bdwrite(bp)
register struct buf *bp;
{
	register struct buf *dp;
	register m;

	if ((m = major(bp->b_dev)) >= 3 && m <= 5) {
		bwrite(bp);
		return;
	}
	sysinfo.lwrite++;
	bp->b_flags |= B_DELWRI | B_DONE;
	brelse(bp);
}


/*
 * Release the buffer, start I/O on it, but don't wait for completion.
 */
bawrite(bp)
register struct buf *bp;
{
	bp->b_flags |= B_ASYNC;
	bwrite(bp);
}


/*
 * release the buffer, with no I/O implied.
 */
brelse(bp)
register struct buf *bp;
{
	register struct buf *backp;
	register s;

	if (bp->b_flags & B_WANTED)
		wakeup((caddr_t)bp);
	if (bfreelist.b_flags & B_WANTED) {
		bfreelist.b_flags &= ~B_WANTED;
		wakeup((caddr_t)&bfreelist);
	}
	if (bp->b_flags & B_ERROR)
		bp->b_dev = NODEV;  /* no assoc. on error */
	s = dvi();
	if (bp->b_flags & B_AGE)
		backp = bfreelist.av_forw;
	else
		backp = &bfreelist;
	bp->av_forw = backp;
	(bp->av_back = backp->av_back)->av_forw = bp;
	backp->av_back = bp;
	bp->b_flags &= ~(B_ASYNC | B_AGE | B_WANTED | B_BUSY);
	rvi(s);
}


/*
 * See if the block is associated with some buffer
 * (mainly to avoid getting hung up on a wait in breada)
 */

incore(dev, blkno)
dev_t dev;
register daddr_t blkno;
{
	register struct buf *bp;
	register struct buf *hp;

	hp = bhash(dev, blkno);
	for (bp = hp->b_forw; bp != hp; bp = bp->b_forw) {
		if (bp->b_blkno == blkno && bp->b_dev == dev)
			return(1);
	}
	return(0);
}


/*
 * Assign a buffer for the given block.  If the appropriate
 * block is already associated, return it; otherwise search
 * for the oldest non-busy buffer and reassign it.
 */
struct buf *
getblk(dev, blkno)
dev_t dev;
register daddr_t blkno;
{
	register struct buf *bp;
	register struct buf *hp;

	if (major(dev) >= nblkdev)
		panic("blkdev");
loop:
	evi();
	hp = bhash(dev, blkno);
	for (bp = hp->b_forw; bp != hp; bp = bp->b_forw) {
		if (bp->b_blkno!=blkno || bp->b_dev!=dev)
			continue;
		dvi();
		if (bp->b_flags & B_BUSY) {
			bp->b_flags |= B_WANTED;
			syswait.iowait++;
			sleep((caddr_t)bp, PRIBIO+1);
			syswait.iowait--;
			goto loop;
		}
		(bp->av_back->av_forw = bp->av_forw)->av_back = bp->av_back;
		bp->b_flags |= B_BUSY;
		evi();
		return(bp);
	}
	dvi();
	if (bfreelist.av_forw == &bfreelist) {
		bfreelist.b_flags |= B_WANTED;
		sleep(&bfreelist, PRIBIO+1);
		goto loop;
	}
	bp = bfreelist.av_forw;
	if (bp->b_flags & B_DELWRI) {
		bqflush(NODEV, NWBUF);
		goto loop;
	}
	(bp->av_back->av_forw = bp->av_forw)->av_back = bp->av_back;
	bp->b_flags = B_BUSY;
	evi();
	(bp->b_back->b_forw = bp->b_forw)->b_back = bp->b_back;
	bp->b_back = hp;
	(bp->b_forw = hp->b_forw)->b_back = bp;
	hp->b_forw = bp;
	bp->b_dev = dev;
	bp->b_blkno = blkno;
	return(bp);
}


/*
 * get an empty block,
 * not assigned to any particular device
 */
struct buf *
geteblk()
{
	register struct buf *bp;
	register struct buf *dp;

	for (;;) {
		dvi();
		while (bfreelist.av_forw == &bfreelist) {
			bfreelist.b_flags |= B_WANTED;
			sleep(&bfreelist, PRIBIO+1);
		}
		dp = &bfreelist;
		bp = bfreelist.av_forw;
		if (bp->b_flags & B_DELWRI)
			bqflush(NODEV, NWBUF);
		else
			break;
	}
	(bp->av_back->av_forw = bp->av_forw)->av_back = bp->av_back;
	bp->b_flags = B_BUSY;
	evi();
	(bp->b_back->b_forw = bp->b_forw)->b_back = bp->b_back;
	bp->b_back = dp;
	(bp->b_forw = dp->b_forw)->b_back = bp;
	dp->b_forw = bp;
	bp->b_dev = (dev_t)NODEV;
	return(bp);
}


/*
 * Wait for I/O completion on the buffer; return errors
 * to the user.
 */
iowait(bp)
register struct buf *bp;
{
	syswait.iowait++;
	dvi();
	while ((bp->b_flags & B_DONE) == 0)
		sleep((caddr_t)bp, PRIBIO);
	evi();
	syswait.iowait--;
	if ((bp->b_flags & B_ERROR) && u.u_error == 0) {
		u.u_error = EIO;
		u.u_deverr = bp->b_error;
	}
}


/*
 * Mark I/O complete on a buffer, release it if I/O is asynchronous,
 * and wake up anyone waiting for it.
 */
iodone(bp)
register struct buf *bp;
{
	bp->b_flags |= B_DONE;
	if (bp->b_flags & B_ASYNC)
		brelse(bp);
	else {
		bp->b_flags &= ~B_WANTED;
		wakeup(bp);
	}
}


/*
 * Zero the core associated with a buffer.
 */
clrbuf(bp)
struct buf *bp;
{
	bzero(bp->b_un.b_addr, BSIZE);
	bp->b_resid = 0;
}


/*
 * swap I/O
 */
swap(swapdev, blkno, coreaddr, count, rdflg)
register blkno;
register coreaddr;
register unsigned count;
{
	register tcount;
	register struct buf *bp;

	syswait.swap++;
	bp = &swbuf1;
	dvi();
	if ((bp->b_flags & B_BUSY) && ((swbuf2.b_flags & B_WANTED) == 0))
		bp = &swbuf2;
	while (bp->b_flags & B_BUSY) {
		bp->b_flags |= B_WANTED;
		sleep(bp, PSWP+1);
	}
	while (count) {
		bp->b_flags = B_BUSY | B_PHYS | rdflg;
		bp->b_dev = swapdev;
		tcount = count;
		if (count >= 0xfe)
			tcount = 0xfe;
		bp->b_bcount = tcount << 8;
		bp->b_blkno = swplo + blkno;
		bp->b_paddr = ((long)coreaddr)<<8;
		(*bdevsw[major(swapdev)].d_strategy)(bp);
		u.u_iosw++;
		if (rdflg)
			sysinfo.swapin++;
		else
			sysinfo.swapout++;
		dvi();
		while ((bp->b_flags & B_DONE) == 0)
			sleep(bp, PSWP);
		count -= tcount;
		coreaddr += tcount;
		blkno += ctod(tcount);
	}
	if (bp->b_flags & B_WANTED)
		wakeup(bp);
	bp->b_flags &= ~(B_WANTED | B_BUSY);
	evi();
	if (bp->b_flags & B_ERROR)
		panic("IO err in swap");
	syswait.swap--;
}

/*
 * make sure all write-behind blocks
 * on dev (or NODEV for all)
 * are flushed out.
 * (from umount and update)
 */
bqflush(dev, n)
register dev_t dev;
register unsigned n;
{
	register struct buf *bp;

loop:
	dvi();
	for (bp = bfreelist.av_forw; bp != &bfreelist && n--; bp = bp->av_forw)
	    if ((bp->b_flags & B_DELWRI) && (dev==NODEV || dev==bp->b_dev)) {
		(bp->av_back->av_forw = bp->av_forw)->av_back = bp->av_back;
		bp->b_flags |=  B_ASYNC|B_BUSY;
		bwrite(bp);
		goto loop;
	    }
	evi();
}

bflush(dev)
dev_t dev;
{
	bqflush(dev, -1);
}


/*
 * Raw I/O. The arguments are
 *	The strategy routine for the device
 *	A buffer, which will always be a special buffer
 *	  header owned exclusively by the device for this purpose
 *	The device number
 *	Read/write flag
 * Essentially all the work is computing physical addresses and
 * validating them.
*/

physio(strat, bp, dev, rw)
register int (*strat)();
register struct buf *bp;
register dev_t dev;
{
	syswait.physio++;
	if (physvio(u.u_base, u.u_count, u.u_segflg) == 0){
		dvi();
		while (bp->b_flags & B_BUSY) {
			bp->b_flags |= B_WANTED;
			sleep(bp,PRIBIO+1);
		}
		bp->b_flags = B_PHYS|B_BUSY|rw;
		evi();
		bp->b_dev = dev;
		bp->b_blkno = u.u_offset >> BSHIFT;
		bp->b_bcount = u.u_count;
		physxfer(strat, bp, u.u_base, u.u_segflg);
		bp->b_flags &= ~(B_BUSY|B_WANTED);
		u.u_count = bp->b_resid;
		if (bp->b_flags & B_ERROR && u.u_error == 0) {
			u.u_error = EIO;
			u.u_deverr = bp->b_error;
		}
	}
	syswait.physio--;
}


physvio(sbase, count, segflg)
saddr_t sbase;
register unsigned count;
register segflg;
{
	register unsigned endb;
	register segm;
	register segno;

	if (count == 0)
		return(0);
	endb = sbase.right + count - 1;
	if (endb+1 > sbase.right) {
		if (u.u_segmented == 0) {
			if (segflg == 2) {
				if (endb < ctob(u.u_tsize))
					return(0);
			} else {
				if (endb < ctob(u.u_dsize) ||
				    sbase.right >= ctob(256 - u.u_ssize))
					return(0);
			}
		}
		else {
			segm = STAK;
			segno = (sbase.left >> 8) & segm;
			if (segno != u.u_stakseg) {
				for (segm = 0; segm < u.u_nsegs; segm++)
					if ((u.u_segno[segm] & STAK) == segno)
						break;
				if ((u.u_segno[segm] & 0x7f) != segno) {
					u.u_error = EFAULT;
					return(1);
				}
			}
			if (segflg == 2) {
				if (endb < ctob(u.u_segmts[segm].sg_limit+1))
					return(0);
			} else {
				if (endb < ctob(u.u_segmts[segm].sg_limit+1) ||
			    	    (segno != u.u_stakseg) ||
			    	    (ctob(256-u.u_ssize) <= sbase.right))
					return(0);
			}
		}
	}
	u.u_error = EFAULT;
	return(1);
}

physxfer(strat, bp, sbase, segflg)
int (*strat)();
register struct buf *bp;
saddr_t sbase;
{
	register unsigned fl1;
	register bflag;
	register seg;
	register cseg;

	fl1 = (btoc(sbase.right+1)-1) & 0xff;
	if (u.u_segmented) {
		seg = STAK;
		cseg = ((sbase.left>>8) & seg);
		if (cseg != u.u_stakseg) {
			for (seg = 0; seg < u.u_nsegs; seg++)
				if ((u.u_segno[seg] & 0x7f) == cseg)
					break;
		}
		fl1 += u.u_segmts[seg].sg_base;
	}
	else if (segflg  == 2) /* non segmented code */
		fl1 += u.u_nsucode.sg_base;
	else if (ctob(256-u.u_ssize) > sbase.right)
		fl1 += u.u_segmts[OLDSEGSTAK].sg_base;
	else
		fl1 += u.u_segmts[STAK].sg_base;
	bp->b_paddr = (ctob((long)fl1)) + (sbase.right & 0xff);
	bp->b_error = 0;
	bflag = u.u_procp->p_flag & SLOCK;
	u.u_procp->p_flag |= SLOCK;

	(*strat)(bp);
	dvi();
	while  ((bp->b_flags & B_DONE) == 0)
		sleep(bp, PRIBIO);
	if (!bflag)
		u.u_procp->p_flag &= ~SLOCK;
	if (bp->b_flags & B_WANTED)
		wakeup(bp);
	evi();
}

binval(dev)
register dev_t dev;
{
	register n;
	register struct hbuf *hp;
	register struct buf *bp;

	for (n = 0; n < Nhbuf; n++) {
		hp = &hbuf[n];
		for (bp = hp->b_forw; bp != (struct buf *)hp; bp = bp->b_forw) 
			if (bp->b_dev == dev) {
				bp->b_flags |= B_AGE;
			}
	}
}
