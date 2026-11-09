/******************************************************************************
*******************************************************************************

	W E G A - Quelle

	KERN 3.2	Modul: lock.c


	Bearbeiter:	O. Lehmann
	Datum:		01.06.08
	Version:	1.0

*******************************************************************************
******************************************************************************/

char lockwstr[] = "@[$]lock.c		Rev : 4.1 	08/27/83 11:55:14";

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
#include <sys/lockblk.h>
#include <sys/callo.h>
#include <sys/inode.h>
#include <sys/file.h>

extern int Nflock;
struct inode *lkfront();


#define	LKWANT	0x10		/* ll_flag: a process sleeps on this lock */
#define	LKREAD	0x100		/* ll_flag: read-only lock */
#define	MAXLOCK	(1L<<30)	/* the end of a lock without length */

struct a {
	int	fildes;
	int	flag;
	struct lockblk *lkblk;
};

/*
 * System call: lock a region of a file. Every lock of this process in the list is classified by its place to the new
 * region [lb, ub) and by its type, the lock is merged, cut or split accordingly.
 */
long
lkdata()
{
	register struct inode *ip;
	register struct locklist *prev;
	register struct locklist *lp;
	off_t lb, ub;
	struct a *uap;
	int rflag;

	uap = (struct a *)u.u_ap;
	if ((ip = lkfront(uap, &lb, &ub)) == NULL)
		return;
	if (locked((uap->flag & LKNOBLOCK) ? 0x80 : 0, ip, lb, ub))
		return;
	prev = (struct locklist *)&ip->i_locklist;
	rflag = (uap->flag & LKEXCLUSIVE) ? 0 : LKREAD;
	while ((lp = prev->ll_link) != NULL) {
		if (ub < lp->ll_start)
			break;
		if (lp->ll_proc != u.u_procp) {
			prev = lp;
			continue;
		}
		switch (((lp->ll_start <= lb) ? 1 : 0) + ((lp->ll_end <= lb) ? 2 : 0) +
			((lp->ll_start <= ub) ? 4 : 0) + ((lp->ll_end <= ub) ? 8 : 0) +
			(((lp->ll_flag & LKREAD) == rflag) ? 16 : 0)) {
		case 4:
			if (rflag) {
				lockadd(prev, rflag, lb, lp->ll_start);
				return;
			}
			if (lockadd(prev, rflag, lb, ub) == 0) {
				lp->ll_start = ub;
				return;
			}
		case 5:
			if (rflag)
				return;
			if (lp->ll_start < lb) {
				if (lockadd(prev, LKREAD, lp->ll_start, lb) == 0) {
					if (lockadd(prev->ll_link, 0, lb, ub) == 0) {
						lp->ll_start = ub;
						return;
					}
					prev->ll_link = lp->ll_link;
					lockfree(prev->ll_link);
					return;
				}
			} else {
				if (lockadd(prev, 0, lp->ll_start, ub) == 0)
					lp->ll_start = ub;
				return;
			}
		case 12:
			if (rflag == 0) {
				prev->ll_link = lp->ll_link;
				lockfree(lp);
				break;
			}
			if (lockadd(prev, rflag, lb, lp->ll_start) == 0) {
				if (locklist[0].ll_link == NULL) {
					prev->ll_link = lp->ll_link;
					lockfree(prev->ll_link);
					u.u_error = EDEADLOCK;
					return;
				}
				lb = lp->ll_end;
				if (lb != ub)
					break;
				return;
			}
		case 13:
			if (rflag) {
				lb = lp->ll_end;
				if (lb != ub)
					break;
				return;
			}
			if (lp->ll_start >= lb) {
				prev->ll_link = lp->ll_link;
				lockfree(lp);
				break;
			}
			lp->ll_end = lb;
			break;
		case 20:
			lp->ll_start = lb;
			return;
		case 21:
			return;
		case 31:
			if (lp->ll_end == lb)
				goto merge;
		case 15:
			prev = lp;
			break;
		case 29:
		merge:
			lb = lp->ll_start;
		case 28:
			prev->ll_link = lp->ll_link;
			lockfree(lp);
			break;
		}
	}
	lockadd(prev, rflag, lb, ub);
}

/*
 * System call: unlock a region of a file.
 */
long
unlk()
{
	register struct inode *ip;
	register struct locklist *prev;
	register struct locklist *lp;
	off_t lb, ub;
	struct a *uap;

	uap = (struct a *)u.u_ap;
	if ((ip = lkfront(uap, &lb, &ub)) == NULL)
		return;
	prev = (struct locklist *)&ip->i_locklist;
	while ((lp = prev->ll_link) != NULL) {
		if (lp->ll_proc != u.u_procp) {
			prev = lp;
			continue;
		}
		if (ub <= lp->ll_start)
			return;
		if (lp->ll_end <= lb) {
			prev = lp;
			continue;
		}
		if (lb <= lp->ll_start && lp->ll_end <= ub) {
			prev->ll_link = lp->ll_link;
			lockfree(lp);
			continue;
		}
		if (lp->ll_flag & LKWANT) {
			lp->ll_flag &= ~LKWANT;
			wakeup(lp);
		}
		if (lp->ll_start < lb && ub < lp->ll_end) {
			if (lockadd(lp, lp->ll_flag, ub, lp->ll_end))
				return;
			lp->ll_end = lb;
			return;
		}
		if (lb <= lp->ll_start && ub < lp->ll_end) {
			lp->ll_start = ub;
			return;
		}
		lp->ll_end = lb;
		prev = lp;
	}
}

/*
 * Does a lock of another process stand in the way of the region [lb, ub)? Sleeps until it is gone, unless flag has 0x80.
 * flag 0x100: only a read is wanted, read-only locks do not count.
 */
locked(flag, ip, lb, ub)
int flag;
register struct inode *ip;
register off_t lb, ub;
{
	struct locklist *lp;

	lp = ip->i_locklist;
	while (lp != NULL) {
		if (ub <= lp->ll_start)
			break;
		if (lp->ll_proc == u.u_procp || lb >= lp->ll_end || (flag & LKREAD & lp->ll_flag)) {
			lp = lp->ll_link;
			continue;
		}
		if (flag & 0x80) {
			u.u_error = EACCES;
			return(1);
		}
		if (deadlock(lp))
			return(1);
		lp->ll_flag |= LKWANT;
		sleep((caddr_t)lp, PSLEP);
		lp = ip->i_locklist;
		if (u.u_error)
			return(1);
	}
	return(0);
}

/*
 * Follow the chain owner of lock - lock it sleeps on: a deadlock if it comes back to this process.
 */
deadlock(lp)
register struct locklist *lp;
{
	register struct locklist *wp;
	long unused;		/* in the frame of the original */

	while (lp->ll_proc->p_stat == SSLEEP) {
		if ((wp = (struct locklist *)lp->ll_proc->p_wchan) < locklist || wp >= &locklist[Nflock])
			return(0);
		if (wp->ll_proc == u.u_procp) {
			u.u_error = EDEADLOCK;
			return(1);
		}
		lp = wp;
	}
	return(0);
}

/*
 * The last close of a file by this process: give up its locks.
 */
unlock(ip)
register struct inode *ip;
{
	register struct locklist *lp;
	register struct locklist *prev;
	long unused1, unused2;	/* in the frame of the original */
	int i, n;

	if (ip->i_locklist == NULL)
		return;
	prev = (struct locklist *)&ip->i_locklist;
	n = 0;
	for (i = 0; i < NOFILE; i++)
		if (u.u_ofile[i] != NULL && u.u_ofile[i]->f_inode == ip)
			n++;
	if (n)
		return;
	while ((lp = prev->ll_link) != NULL) {
		if (lp->ll_proc == u.u_procp) {
			prev->ll_link = lp->ll_link;
			lockfree(lp);
			continue;
		}
		prev = lp;
	}
}

/*
 * Get a free lock cell. locklist[0] is the head of the free list; the first call builds the list.
 */
struct locklist *
lockalloc()
{
	register struct locklist *fl = locklist;
	register struct locklist *lp;

	if (fl->ll_proc == NULL) {
		fl->ll_proc = proc;
		for (lp = &locklist[1]; lp < &locklist[Nflock]; lp++)
			lockfree(lp);
	}
	if ((lp = fl->ll_link) == NULL) {
		u.u_error = EDEADLOCK;
		return(NULL);
	}
	fl->ll_link = lp->ll_link;
	lp->ll_link = NULL;
	return(lp);
}

lockfree(lp)
register struct locklist *lp;
{
	register struct locklist *fl = locklist;

	if (lp->ll_flag & LKWANT) {
		lp->ll_flag &= ~LKWANT;
		wakeup(lp);
	}
	lp->ll_link = fl->ll_link;
	fl->ll_link = lp;
}

/*
 * Insert a new lock behind cl.
 */
lockadd(cl, flag, lb, ub)
register struct locklist *cl;
register int flag;
register off_t lb;
off_t ub;
{
	struct locklist *nl;

	if ((nl = lockalloc()) == NULL) {
		u.u_error = EDEADLOCK;
		return(1);
	}
	nl->ll_link = cl->ll_link;
	cl->ll_link = nl;
	nl->ll_proc = u.u_procp;
	nl->ll_start = lb;
	nl->ll_end = ub;
	nl->ll_flag = flag;
	return(0);
}

/*
 * Common front of lkdata and unlk: check the arguments, compute the region [*lb, *ub).
 */
struct inode *
lkfront(uap, lb, ub)
register struct a *uap;
register off_t *lb, *ub;
{
	struct lockblk lkblk;
	struct file *fp;
	struct inode *ip;
	int mode;

	if ((fp = getf(uap->fildes)) == NULL)
		return(NULL);
	if ((ip = fp->f_inode) == NULL) {
		printf("PANIC ip null in lkfront\n");
		return(NULL);
	}
	if ((fp->f_flag & FWRITE) == 0 && access(ip, IWRITE)) {
		u.u_error = EACCES;
		return(NULL);
	}
	mode = 0x7000;
	if ((ip->i_mode & IFMT) & mode) {
		if (ip->i_mode & 0x1000)
			u.u_error = ESPIPE;
		else
			u.u_error = EACCES;
		return(NULL);
	}
	if (copyin(uap->lkblk, &lkblk, sizeof lkblk)) {
		u.u_error = EFAULT;
		return(NULL);
	}
	if (uap->flag < 0 || uap->flag > 7) {
		u.u_error = EINVAL;
		return(NULL);
	}
	if (uap->flag & LKEXP) {
		if (lkblk.lkwhnce == 1)
			lkblk.lkoff += fp->f_offset;
		else if (lkblk.lkwhnce == 2)
			lkblk.lkoff += ip->i_size;
		*lb = lkblk.lkoff;
	} else
		*lb = fp->f_offset;
	if (*lb < 0) {
		u.u_error = EINVAL;
		return(NULL);
	}
	if (lkblk.lklen)
		*ub = *lb + lkblk.lklen;
	else
		*ub = MAXLOCK;
	if (*lb >= *ub) {
		u.u_error = EINVAL;
		return(NULL);
	}
	u.u_r.r_off = *lb;
	return(ip);
}
