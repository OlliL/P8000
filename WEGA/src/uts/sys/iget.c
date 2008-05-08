/******************************************************************************
*******************************************************************************

	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1988
	KERN 3.2		  Modul: iget.c


	Bearbeiter:
	Datum:		$D$
	Version:	$R$

*******************************************************************************
******************************************************************************/

#include <sys/param.h>
#include <sys/inode.h>

char igetwstr[]="@[$]iget.c		Rev : 4.1 	08/27/83 11:54:46";

#include <sys/sysinfo.h>
#include <sys/systm.h>
#include <sys/mount.h>
#include <sys/dir.h>
#include <sys/ino.h>
#include <sys/filsys.h>
#include <sys/conf.h>
#include <sys/buf.h>
#include <sys/mmu.h>
#include <sys/s.out.h>
#include <sys/user.h>
extern int Ninode;
extern int Nmount;


struct inode *ihash[INOHSZ] = 0 ;
struct inode *ifreelist;

ihinit()
{
	register i;
	register struct inode *ip;

	ifreelist = inode;
	for(ip=inode; ip < &inode[Ninode-1]; ip++)
		ip->i_link = (struct inode *) ip+1;
	ip->i_link = (struct inode *) 0;
	for(i=0;i< INOHSZ;i++)
		ihash[i] = (struct inode *) 0;
}

struct inode *
ifind(idev,inum)
register dev_t idev;
register ino_t inum;
{
	register struct inode *ip;

	ip=ihash[INOHASH(idev,inum)];
	while(ip !=(struct inode *)0) {
		if(inum == ip->i_number && idev == ip->i_dev)
			return(ip);
		ip=ip->i_link;
	}
	return((struct inode *)0);
}

struct inode *
iget(dev, ino)
register dev_t dev;
register ino_t ino;
{
	register struct inode *ip;
	register struct mount *mp;
	register h_index;
	register struct buf *bp;
	register struct dinode *dp;
	int i;

	sysinfo.iget++;
loop:
	h_index = INOHASH(dev, ino);
	for(ip=ihash[h_index];ip!=(struct inode *) 0;ip=ip->i_link) {
		if(ino == ip->i_number && dev == ip->i_dev) {
			while((ip->i_flag & ILOCK) != 0) {
				ip->i_flag |= IWANT;
				sleep((caddr_t)ip, PINOD);
				if(ino == ip->i_number && dev == ip->i_dev)
					continue;
				goto loop;
			}
			if((ip->i_flag & IMOUNT) !=0) {
			for(mp = &mount[0]; mp < &mount[Nmount]; mp++)
				if(mp->m_inodp == ip) {
					dev = mp->m_dev;
					ino = ROOTINO;
					goto loop;
				}
				panic("no imt");
			}
			ip->i_count++;
			ip->i_flag |= ILOCK;
			return(ip);
		}
	}

	if(ifreelist == NULL) {
		printf("Inode table overflow\n");
		u.u_error = ENFILE;
		return(NULL);
	}
	ip=ifreelist;
	ifreelist=ip->i_link;
	ip->i_link=ihash[h_index];
	ihash[h_index]=ip;
	ip->i_dev = dev;
	ip->i_number = ino;
	ip->i_flag = ILOCK;
	ip->i_count++;
	ip->i_blks.i_l = (daddr_t) 0;
	bp = bread(dev, itod(ino));

	if((bp->b_flags&B_ERROR) != 0) {
		brelse(bp);
		iput(ip);
		return(NULL);
	}
	dp = bp->b_un.b_dino;
	dp += itoo(ino);
	iexpand(ip, dp);
	brelse(bp);
	return(ip);
}

iexpand(ip, dp)
register struct inode *ip;
register struct dinode *dp;
{
	ip->i_mode = dp->di_mode;
	ip->i_nlink = dp->di_nlink;
	ip->i_uid = dp->di_uid;
	ip->i_gid = dp->di_gid;
	ip->i_size = dp->di_size;
	l3tol((char *) ip->i_blks.i_p.i_a, (char *)dp->di_addr, NADDR);
	ip->i_locklist = 0;
}

iput(ip)
register struct inode *ip;
{
	register struct inode *ip1;
	register h_index, i_unbenutzt;
	long unbenutzt;

	if(ip->i_count == 1) {
		ip->i_flag |= ILOCK;
		if(ip->i_nlink <= 0) {
			itrunc(ip);
			ip->i_mode = 0;
			ip->i_flag |= IUPD|ICHG;
			ifree(ip->i_dev, ip->i_number);
		}
		iupdat(ip, &time, &time);
		prele(ip);
		h_index = INOHASH(ip->i_dev, ip->i_number);
		if(ip == ihash[h_index])
			ihash[h_index] = ip->i_link;
		else {
			ip1 = ihash[h_index];
			for(;ip1;ip1=ip1->i_link)
				if(ip == ip1->i_link) {
					ip1->i_link = ip->i_link;
					goto m1;
				}
			panic("iput");
		}
m1:
		ip->i_link = ifreelist;
		ifreelist = ip;
		ip ->i_flag= NULL;
		ip->i_number = 0;
	} else
		prele(ip);
	ip->i_count--;
}

iupdat(ip, ta, tm)
register struct inode *ip;
time_t *ta, *tm;
{
	register struct buf *bp;
	register struct dinode *dp;
	char *di_addr;
	char *i_f;
	unsigned i;

	if((ip->i_flag&(IUPD|IACC|ICHG)) == 0)
		return;
	if(getfs(ip->i_dev)->s_ronly) {
		if(ip->i_flag & (IUPD | ICHG))
			u.u_error = EROFS;
		ip->i_flag &= ~(IUPD | ICHG | IACC);
		return;
	}
	bp = bread(ip->i_dev, itod(ip->i_number));
	if (bp->b_flags & B_ERROR) {
		brelse(bp);
		return;
	}
	dp = bp->b_un.b_dino;
	dp += itoo(ip->i_number);
	dp->di_mode = ip->i_mode;
	dp->di_nlink = ip->i_nlink;
	dp->di_uid = ip->i_uid;
	dp->di_gid = ip->i_gid;
	dp->di_size = ip->i_size;

	if((ip->i_mode & IFMT) == IFIFO) {
		di_addr = (char *) dp->di_addr;
		i_f = (char *) ip->i_faddr;
		i=0;
		do {
		*di_addr++ = *i_f++;
		if( * i_f++)
			printf("iaddress > 2^24\n");
		*di_addr++ = *i_f++;
		*di_addr++ = *i_f++;
		}while(++i < NFADDR);

		for(;i<13;i++) {
			*di_addr++ = 0;
			*di_addr++ = 0;
			*di_addr++ = 0;
		}
	} else
		ltol3((char *)dp->di_addr, (char *)ip->i_addr, NADDR);
	if(ip->i_flag&IACC)
		dp->di_atime = *ta;
	if(ip->i_flag&IUPD)
		dp->di_mtime = *tm;
	if(ip->i_flag&ICHG)
		dp->di_ctime = time;
	ip->i_flag &= ~(IUPD|IACC|ICHG);
	bdwrite(bp);
}

itrunc(ip)
register struct inode *ip;
{
	register i;
	register dev_t dev;
	register daddr_t bn;

	i = ip->i_mode & IFMT;
	if (i!=IFREG && i!=IFDIR)
		return;
	dev = ip->i_dev;
	for(i=NADDR-1; i>=0; i--) {
		bn = ip->i_addr[i];
		if(bn == (daddr_t)0)
			continue;
		ip->i_addr[i] = (daddr_t)0;
		switch(i) {

		default:
			free(dev, bn);
			break;

		case NADDR-3:
			tloop(dev, bn, 0, 0);
			break;

		case NADDR-2:
			tloop(dev, bn, 1, 0);
			break;

		case NADDR-1:
			tloop(dev, bn, 1, 1);
		}
	}
	ip->i_size = 0;
	ip->i_flag |= ICHG|IUPD;
}
 
tloop(dev, bn, f1, f2)
dev_t dev;
daddr_t bn;
{
	register i;
	register struct buf *bp;
	register daddr_t *bap;
	int unbenutzt, u2, u3, u4, u5;
	daddr_t nb;

	bp = NULL;
	for(i=NINDIR-1; i>=0; i--) {
		if(bp == NULL) {
			bp = bread(dev, bn);
			if (bp->b_flags & B_ERROR) {
				brelse(bp);
				return;
			}
			bap = bp->b_un.b_daddr;
		}
		nb = bap[i];
		if(nb == (daddr_t)0)
			continue;
		if(f1) {
			brelse(bp);
			bp = NULL;
			tloop(dev, nb, f2, 0);
		} else
			free(dev, nb);
	}
	if(bp != NULL)
		brelse(bp);
	free(dev, bn);
}
 
struct inode *
maknode(mode)
{
	register struct inode *ip;
	struct inode *ip1;

	ip = ialloc(u.u_pdir->i_dev);
	if(ip == NULL) {
		iput(u.u_pdir);
		return(NULL);
	}
	ip->i_flag |= IACC|IUPD|ICHG;
	if((mode&IFMT) == 0)
		mode |= IFREG;
	ip->i_mode = mode & ~u.u_cmask;
	ip->i_nlink = 1;
	ip->i_uid = u.u_uid;
	ip->i_gid = u.u_gid;
	wdir(ip);
	return(ip);
}

wdir(ip)
struct inode *ip;
{

	if (u.u_pdir->i_nlink <= 0) {
		u.u_error = ENOTDIR;
	} else {
		u.u_dent.d_ino = ip->i_number;
		bcopy((caddr_t)u.u_dbuf, (caddr_t)u.u_dent.d_name, DIRSIZ);
		u.u_count = sizeof(struct direct);
		u.u_segflg = 1;
		u.u_base.l= (caddr_t) &u.u_dent;
		writei(u.u_pdir);
		u.u_segflg = 0;
	}
	iput(u.u_pdir);
}
