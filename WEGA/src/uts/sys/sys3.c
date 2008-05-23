/******************************************************************************
*******************************************************************************
 
	W E G A - Quelle

	KERN 3.2	Modul: sys3.c
 
 
	Bearbeiter:	O. Lehmann
	Datum:		23.05.08
	Version:	1.0
 
*******************************************************************************
******************************************************************************/
 
char sys3wstr[] = "@[$]sys3.c		Rev : 4.1 	08/27/83 11:59:28";

#include <sys/param.h>
#include <sys/sysinfo.h>
#include <sys/systm.h>
#include <sys/mount.h>
#include <sys/ino.h>
#include <sys/buf.h>
#include <sys/filsys.h>
#include <sys/dir.h>
#include <sys/s.out.h>
#include <sys/mmu.h>
#include <sys/user.h>
#include <sys/inode.h>
#include <sys/file.h>
#include <sys/conf.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

extern int Nmount;
extern int Ninode;

/*
 * the fstat system call.
 */
fstat()
{
	register struct file *fp;
	register struct a {
		int	fdes;
		struct stat *sb;
	} *uap;

	uap = (struct a *)u.u_ap;
	fp = getf(uap->fdes);
	if(fp == NULL)
		return;
	stat1(fp->f_inode, uap->sb, fp->f_flag&FPIPE? fp->f_un.f_off: 0);
}

/*
 * the stat system call.
 */
stat()
{
	register struct inode *ip;
	register struct a {
		char	*fname;
		struct stat *sb;
	} *uap;

	uap = (struct a *)u.u_ap;
	ip = namei(uchar, 0);
	if(ip == NULL)
		return;
	stat1(ip, uap->sb, (off_t)0);
	iput(ip);
}

/*
 * The basic routine for fstat and stat:
 * get the inode and pass appropriate parts back.
 */
stat1(ip, ub, pipeadj)
register struct inode *ip;
struct stat *ub;
off_t pipeadj;
{
	register struct dinode *dp;
	register struct buf *bp;
	struct stat ds;

	iupdat(ip, &time, &time);
	/*
	 * first copy from inode table
	 */
	ds.st_dev = ip->i_dev;
	ds.st_ino = ip->i_number;
	ds.st_mode = ip->i_mode;
	ds.st_nlink = ip->i_nlink;
	ds.st_uid = ip->i_uid;
	ds.st_gid = ip->i_gid;
	ds.st_rdev = (dev_t)ip->i_rdev;
	ds.st_size = ip->i_size - pipeadj;
	/*
	 * next the dates in the disk
	 */
	bp = bread(ip->i_dev, itod(ip->i_number));
	dp = bp->b_un.b_dino;
	dp += itoo(ip->i_number);
	ds.st_atime = dp->di_atime;
	ds.st_mtime = dp->di_mtime;
	ds.st_ctime = dp->di_ctime;
	brelse(bp);
	if (copyout((caddr_t)&ds, (caddr_t)ub, sizeof(ds)) < 0)
		u.u_error = EFAULT;
}

/*
 * the dup system call.
 */
dup()
{
	register struct file *fp;
	register struct a {
		int	fdes;
		int	fdes2;
	} *uap;
	register i, m;
	int foo1,foo2,foo3;

	uap = (struct a *)u.u_ap;
	m = uap->fdes & ~077;
	uap->fdes &= 077;
	fp = getf(uap->fdes);
	if(fp == NULL)
		return;
	if ((m&0100) == 0) {
		if ((i = ufalloc(0)) < 0)
			return;
	} else {
		i = uap->fdes2;
		if (i<0 || i>=NOFILE) {
			u.u_error = EBADF;
			return;
		}
		u.u_r.r_val1 = i;
	}
	if (i!=uap->fdes) {
		if (u.u_ofile[i]!=NULL)
			closef(u.u_ofile[i]);
		u.u_ofile[i] = fp;
		fp->f_count++;
	}
}

/*
 * the file control system call.
 */
fcntl()
{
	register struct file *fp;
	register struct a {
		int	fdes;
		int	cmd;
		int	arg;
	} *uap;
	register i;

	uap = (struct a *)u.u_ap;
	fp = getf(uap->fdes);
	if (fp == NULL)
		return;
	switch(uap->cmd) {
	case 0:
		i = uap->arg;
		if (i < 0 || i > NOFILE) {
			u.u_error = EINVAL;
			return;
		}
		if ((i = ufalloc(i)) < 0)
			return;
		u.u_ofile[i] = fp;
		fp->f_count++;
		break;

	case 1:
		u.u_r.r_val1 = u.u_pofile[uap->fdes];
		break;

	case 2:
		u.u_pofile[uap->fdes] = uap->arg;
		break;

	case 3:
		u.u_r.r_val1 = fp->f_flag+FOPEN;
		break;

	case 4:
		fp->f_flag &= (FREAD|FWRITE);
		fp->f_flag |= (uap->arg-FOPEN) & ~(FREAD|FWRITE);
		break;

	default:
		u.u_error = EINVAL;
	}
}

/*
 * the mount system call.
 */
smount()
{
	register dev_t dev;
	register struct inode *ip;
	register struct mount *mp;
	struct mount *smp;
	register struct filsys *fp;
	struct buf *bp;
	register struct a {
		char	*fspec;
		char	*freg;
		int	ronly;
	} *uap;

	uap = (struct a *)u.u_ap;
	dev = getmdev();
	if(u.u_error)
		return;
	u.u_dirp.l = (caddr_t)(((long)uap->freg) & 0x7F00FFFF);	/* FIXME: this is not 100% compatible */
	ip = namei(uchar, 0);
	if(ip == NULL)
		return;
	if (ip->i_count != 1)
		goto out;
	if (ip->i_mode&2000)
		goto out;
	smp = NULL;
	for(mp = &mount[0]; mp < &mount[Nmount]; mp++) {
		if(mp->m_bufp) {
			if(dev == mp->m_dev)
				goto out;
		} else
		if(smp == NULL)
			smp = mp;
	}
	mp = smp;
	if(mp == NULL)
		goto out;
	(*bdevsw[major(dev)].d_open)(dev, !uap->ronly);
	if(u.u_error)
		goto out1;
	bp = bread(dev, SUPERB);
	if(u.u_error) {
		brelse(bp);
		goto out1;
	}
	mp->m_inodp = ip;
	mp->m_dev = dev;
	mp->m_bufp = geteblk();
	bcopy((caddr_t)bp->b_un.b_addr, mp->m_bufp->b_un.b_addr, BSIZE);
	fp = mp->m_bufp->b_un.b_filsys;
	mp->m_flags = MINUSE;
	fp->s_ilock = 0;
	fp->s_flock = 0;
	fp->s_ronly = uap->ronly & 1;
	brelse(bp);
	ip->i_flag |= IMOUNT;
	prele(ip);
	return;

out1:
	mp->m_flags = MFREE;
out:
	if (u.u_error == 0)
		u.u_error = EBUSY;
	iput(ip);
}

/*
 * the umount system call.
 */
sumount()
{
	register dev_t dev;
	register struct inode *ip;
	register struct mount *mp;
	struct buf *bp;
	register struct a {
		char	*fspec;
	};

	dev = getmdev();
	if(u.u_error)
		return;
	xumount(dev);	/* remove unused sticky files from text table */
	update();
	for(mp = &mount[0]; mp < &mount[Nmount]; mp++)
		if(mp->m_bufp)
			goto found;
	u.u_error = EINVAL;
	return;

found:
	for(ip = &inode[0]; ip < &inode[Ninode]; ip++)
		if(ip->i_number != 0 && dev == ip->i_dev) {
			u.u_error = EBUSY;
			return;
		}
	(*bdevsw[major(dev)].d_close)(dev, 0);
	binval(dev);
	ip = mp->m_inodp;
	ip->i_flag &= ~IMOUNT;
	plock(ip);
	iput(ip);
	bp = mp->m_bufp;
	mp->m_bufp = NULL;
	brelse(bp);
	/* FIXME: here has to be a clr @r8 - rr8 is mp */
}


/*
 * Common code for mount and umount.
 * Check that the user's argument is a reasonable
 * thing on which to mount, and return the device number if so.
 */
dev_t
getmdev()
{
	register dev_t dev;
	register struct inode *ip;

	ip = namei(uchar, 0);
	if(ip == NULL)
		return(NODEV);
	if((ip->i_mode&IFMT) != IFBLK)
		u.u_error = ENOTBLK;
	dev = (dev_t)ip->i_rdev;
	if(major(dev) >= nblkdev)
		u.u_error = ENXIO;
	iput(ip);
	return(dev);
}


/*
 * character special i/o control
 */
ioctl()
{
	register struct file *fp;
	register struct inode *ip;
	register struct a {
		int	fdes;
		int	cmd;
		int	arg;
	} *uap;
	register dev_t dev;

	uap = (struct a *)u.u_ap;
	if ((fp = getf(uap->fdes)) == NULL)
		return;
	ip = fp->f_inode;
	if ((ip->i_mode&IFMT) != IFCHR) {
		u.u_error = ENOTTY;
		return;
	}
	dev = (dev_t)ip->i_rdev;
	(*cdevsw[major(dev)].d_ioctl)(minor(dev),uap->cmd,uap->arg,fp->f_flag);
}

/*
 * old stty and gtty
 */
stty()
{
	register struct a {
		int	fdes;
		int	arg;
		int	narg;
	} *uap;

	uap = (struct a *)u.u_ap;
	uap->narg = uap->arg;
	uap->arg = TIOCSETP;
	ioctl();
}

gtty()
{
	register struct a {
		int	fdes;
		int	arg;
		int	narg;
	} *uap;

	uap = (struct a *)u.u_ap;
	uap->narg = uap->arg;
	uap->arg = TIOCGETP;
	ioctl();
}

