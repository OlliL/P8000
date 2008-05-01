/******************************************************************************
*******************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1988
	KERN 3.2		  Modul: rdwri.c
 
 
	Bearbeiter:	P.Hoge
	Datum:		23.6.88
	Version:	1.2
 
*******************************************************************************
******************************************************************************/

 
#include <sys/param.h>
#include <sys/sysinfo.h>
#include <sys/systm.h>
#include <sys/inode.h>
#include <sys/dir.h>
#include <sys/buf.h>
#include <sys/conf.h>
#include <sys/mmu.h>
#include <sys/s.out.h>
#include <sys/user.h>
#include <sys/file.h>

 
 
/*---------------------------------------------------------------------------*/
/*
 * readi
 * Liest die Datei, die durch den Inode (ip) bestimmt ist;
 * die aktuellen Leseargumente sind in folgenden Variablen zu finden:
 *	u_base		Speicherzieladrese
 *	u_offset	Offsetadresse in der Datei
 *	u_count		zu lesende Byteanzahl
 *	u_segflg	Zielbereich (kernel/user D/user I)
 */
/*---------------------------------------------------------------------------*/

readi(ip)
register struct inode *ip;
{
	register struct buf *bp;
	register int type;
	register unsigned n;
	daddr_t lbn, bn;
	dev_t dev;
	off_t diff;
	unsigned on;

	if (u.u_count == 0)
		return;
	if (u.u_offset < 0) {
		u.u_error = EINVAL;
		return;
	}
	dev = (dev_t)ip->i_rdev;
	type = ip->i_mode & IFMT;
	switch (type) {
	case IFCHR:
		ip->i_flag |= IACC;
		(*cdevsw[major(dev)].d_read)(dev);
		break;

	case IFIFO:
		while (ip->i_size == 0) {
			if (ip->i_fwcnt == 0)
				return;
			if (u.u_fmode & FNDELAY)
				return;
			ip->i_fflag |= IFIR;
			prele(ip);
			sleep((caddr_t)&ip->i_frcnt, PPIPE);
			plock(ip);
		}
		u.u_offset = ip->i_frptr;
	case IFBLK:
	case IFREG:
	case IFDIR:
	do {
		lbn = bn = u.u_offset >> BSHIFT;
		on = u.u_offset & BMASK;
		n = min((unsigned)(BSIZE-on), u.u_count);
		if (type != IFBLK) {
			if (type == IFIFO)
				diff = ip->i_size;
			else
				diff = ip->i_size - u.u_offset;
			if (diff <= 0)
				break;
			if (diff < n)
				n = diff;
			bn = bmap(ip, bn, B_READ);
			if (u.u_error)
				break;
			dev = ip->i_dev;
		} else
			rablock = bn+1;

		if ((long)bn < 0) {
			if (type != IFREG)
				break;
			bp = geteblk();
			clrbuf(bp);
		} else if (ip->i_lastr+1 == lbn &&
		    (type != IFIFO) && (on+n) == BSIZE)
			bp = breada(dev, bn, rablock);
		else
			bp = bread(dev, bn);
		if ((on+n) == BSIZE)
			ip->i_lastr = lbn;
		if (bp->b_resid) {
			n = 0;
		}
		if (n != 0)
			iomove(bp->b_un.b_addr+on, n, B_READ);
		if (type == IFIFO) {
			ip->i_size -= n;
			if (u.u_offset >= PIPSIZ)
				u.u_offset = 0;
			if ((on+n) == BSIZE && ip->i_size < (PIPSIZ-BSIZE))
				bp->b_flags &= ~B_DELWRI;
		}
		brelse(bp);
		ip->i_flag |= IACC;
	} while (u.u_error == 0 && u.u_count && n);

		if (type == IFIFO) {
			if (ip->i_size)
				ip->i_frptr = u.u_offset;
			else
				ip->i_frptr = ip->i_fwptr = 0;
			if (ip->i_fflag & IFIW) {
				ip->i_fflag &= ~IFIW;
				curpri = PPIPE;
				wakeup((caddr_t)&ip->i_fwcnt);
			}
		}
		break;

	default:
		u.u_error = ENODEV;
	}
}

/*---------------------------------------------------------------------------*/
/*
 * writei
 * Schreibt die Datei, die durch den Inode (ip) bestimmt ist;
 * die aktuellen Schreibargumente sind in folgenden Variablen zu finden:
 *	u_base		Speicherquelladresse
 *	u_offset	Offsetadresse in der Datei
 *	u_count		zu schreibende Byteanzahl
 *	u_segflg	Speicherbereich (kernel/user/user I)
 */
/*---------------------------------------------------------------------------*/
writei(ip)
register struct inode *ip;
{
	struct buf *bp;
	daddr_t bn;
	register dev_t dev;
	register type;
	register unsigned n;
	register int usave;
	unsigned on;

	if(u.u_offset < 0) {
		u.u_error = EINVAL;
		return;
	}
	dev = (dev_t)ip->i_rdev;
	type = ip->i_mode & IFMT;
	switch (type) {
	case IFCHR:		/* Character Special File */
		ip->i_flag |= IUPD|ICHG;
		(*cdevsw[major(dev)].d_write)(dev);
		return;

	case IFIFO:
floop:
		usave = 0;
		while(u.u_count + ip->i_size > PIPSIZ) {
			if (ip->i_frcnt == 0)
				break;
			if (u.u_count > PIPSIZ && ip->i_size < PIPSIZ) {
				usave = u.u_count;
				u.u_count = PIPSIZ - ip->i_size;
				usave -= u.u_count;
				break;
		   	} 
			if (u.u_fmode & FNDELAY)
				return;
			ip->i_fflag |= IFIW;
			prele(ip);
			sleep((caddr_t)&ip->i_fwcnt,PPIPE);
			plock(ip);
		}
		if (ip->i_frcnt == 0) {
			u.u_error = EPIPE;
			psignal(u.u_procp, SIGPIPE);
			break;
		}
		u.u_offset = ip->i_fwptr;

	case IFDIR:		/* Directory */
	case IFBLK:		/* Block Special File */
	case IFREG:		/* Regular File */
	while(u.u_error == 0 &&  u.u_count != 0) {
		bn = u.u_offset >> BSHIFT;
		on = u.u_offset & BMASK;
		n = min((unsigned)(BSIZE-on), u.u_count);
		if (type != IFBLK) {
			bn = bmap(ip, bn, B_WRITE);
			if (u.u_error)
				break;
			dev = ip->i_dev;
		}
		if (n == BSIZE)
			bp = getblk(dev, bn);
		else if (type==IFIFO && on==0 && ip->i_size < (PIPSIZ-BSIZE))
				bp = getblk(dev, bn);
			else
				bp = bread(dev, bn);
		iomove(bp->b_un.b_addr+on, n, B_WRITE);
		if(u.u_error != 0)
			brelse(bp);
		else
			bdwrite(bp);
		if (type == IFREG || type == IFDIR) {
			if (u.u_offset > ip->i_size)
				ip->i_size = u.u_offset;
		} else if (type == IFIFO) {
			ip->i_size += n;
			if (u.u_offset == PIPSIZ)
				u.u_offset = 0;
		}
		ip->i_flag |= IUPD | ICHG;
	}
		if (type == IFIFO) {
			ip->i_fwptr = u.u_offset;
			if (ip->i_fflag & IFIR) {
				ip->i_fflag &= ~IFIR;
				curpri = PPIPE;
				wakeup((caddr_t)&ip->i_frcnt);
			}
			if (u.u_error == 0 &&  usave != 0) {
				u.u_count = usave;
				goto floop;
			}
		}
		break;
	default:
			u.u_error = ENODEV;
	}
}

/*---------------------------------------------------------------------------*/
/*
 * max
 * Gibt das Maximum zweier Argumente zurueck
 */
/*---------------------------------------------------------------------------*/
max(a, b)
unsigned a, b;
{
	if (a > b)
		return(a);
	return(b);
}

/*---------------------------------------------------------------------------*/
/*
 * min
 * Gibt das Minimum zweier Argumente zurueck
 */
/*---------------------------------------------------------------------------*/
min(a, b)
unsigned a, b;
{
	if (a < b)
		return(a);
	return(b);
}

/*---------------------------------------------------------------------------*/
/*
 * iomove
 * Transportiert n Bytes von/nach (flag) cp zum/vom user/kernel (u_segflg)
 * Bereich beginnend ab u_base
 *
 * Es gibt dazu folgende Algorithmen:
 * - falls System-Modus (u.u_segflg == 1) Nutzung bcopy in Abh. von flag
 * - falls segmentierter Mode (u.u_segflg == 0 oder u.u_segmented == 1)
 *   Nutzung von maschinenabhaengigen Routinen copyin, copyout (mch.s)
 * - sonst copyiin, copyiout (mch.s)
 */
/*---------------------------------------------------------------------------*/
iomove(cp, n, flag)
register caddr_t cp;
register n;
register flag;
{
	register t;

	if (n == 0)
		return;
	t = 0;
	if (u.u_segflg == 1)		/* System-Forderung */
		if (flag == 0)
			bcopy((caddr_t)u.u_base, cp, n);
		else
			bcopy(cp, (caddr_t)u.u_base, n);
	else				/* User untersetzen */
		if (u.u_segflg == 0 || u.u_segmented)
			if (flag == 0)
				t = copyin((caddr_t)u.u_base, cp, n);
			else
				t = copyout(cp, (caddr_t)u.u_base, n);
		else
			if (flag==0)
				t = copyiin(u.u_base.right, cp, n);
			else
				t = copyiout(cp, u.u_base.right, n);
	if (t) {
		u.u_error = EFAULT;
		return;
	}
	u.u_base.right += n;
	u.u_offset += n;
	u.u_count -= n;
	return;
}
