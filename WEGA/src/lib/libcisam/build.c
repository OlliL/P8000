#include	<isam.h>
#include	"isint.h"

isbuild(name, reclen, kp, mode)
char	*name;
int	reclen;
register struct keydesc	*kp;
register int	mode;
{
	struct buf	*bp;
	char	buf[64];
	int	isfd;
	register int	lmode;

	iserrio = 0;
	iserrno = 0;
	if (reclen <= 0 || reclen > 2048) {
		iserrno = EBADARG;
		return(-1);
	}
	if (ckfilname(name))
		return(-1);
	if (kylegal(kp, reclen))
		return(-1);
	if ((isfd = allocopen()) < 0)
		return(-1);
	openp->o_mode = mode;
	lmode = mode & 0177400;
	mode &= 03;
	switch (lmode) {
	case ISAUTOLOCK:
		break;
	case ISAUTOLOCK + ISLOCK:
		lmode = ISAUTOLOCK;
		break;
	case ISMANULOCK:
		break;
	case ISMANULOCK + ISLOCK:
		lmode = ISEXCLLOCK;
		break;
	case ISEXCLLOCK:
		break;
	default:
		goto badarg;
	}
	switch (mode) {
	case ISINPUT:
	case ISOUTPUT:
	case ISINOUT:
		break;
	default:
badarg:
		iserrno = EBADARG;
		goto bad;
	}
	mkidxname(name, buf);
	if (makefile(buf) == -1) {
		iserrio = 0x21;
		goto bad;
	}
	mkdatname(name, buf);
	if (makefile(buf) == -1) {
		iserrio = 0x22;
		goto bad;
	}
	if ((filep = openp->o_filep = allocfile(name, mode & 03)) == 0)
		goto bad;
	if (lksemaphore(1))
		goto bad;
	if (lkopenlock(lmode))
		goto bad;
	filep->f_dictbp = bfgetclr(1L);
	dictp = filep->f_dictbp->bf_buf;
	dictp[0] = 0376;
	dictp[1] = 0123;
	dictp[2] = 2;
	dictp[3] = 2;
	dictp[4] = 4;
	dictp[5] = 4;
	dictp[6] = 1;
	dictp[7] = 0377;
	dictp[8] = 0;
	dictp[9] = 0;
	dictp[10] = 7;
	dictp[11] = 2;
	stint(reclen, dictp + 13);
	stlong(2L, dictp + 15);
	stlong(2L, dictp + 37);
	filep->f_nodsiz = 511;
	filep->f_recsize = reclen;
	filep->f_dictbp->bf_flags |= BFMODF|BFVALID;
	bp = bfgetclr(2L);
	bp->bf_used = 6;
	bp->bf_level = 0176;
	bp->bf_type = 0377;
	bfdwrite(bp);
	if (kyadd(kp))
		iserrno = EBADKEY;
	kycopy(kp, &openp->o_key);
	if (iserrno == 0) {
		isexit();
		return(isfd);
	}
	lkopenlock(0);
	isexit();
bad:
	if (openp->o_flags & 0100)
		lksemaphore(0);
	freeopen(openp);
	return(-1);
}

isaddindex(isfd, kp)
int	isfd;
register struct keydesc	*kp;
{
	struct item	it;
	long	recnum, nrec;

	if (isenter(isfd, 2) == -1)
		return(-1);
	if (!(openp->o_mode & ISEXCLLOCK)) {
		iserrno = ENOTEXCL;
		goto out;
	}
	if (kylegal(kp, filep->f_recsize) || ~((dictp[10] & 0377) << 1) & kp->k_flags & COMPRESS || kyadd(kp)) {
		if (iserrno == 0)
			iserrno = EBADKEY;
		goto out;
	}
	nrec = ldlong(dictp + 33);
	for (recnum = 1; recnum <= nrec && iserrno == 0; recnum++) {
		if (dataread(temprecord, recnum) < 0)
			continue;
		itmake(temprecord, &it, kp);
		it.it_ptr = recnum;
		btadditem(&it, kp);
	}
	if (iserrno) {
		btrecover(kp->k_rootnode, kp);
		kydelete(kp);
	}
out:
	return(isexit());
}

isdelindex(isfd, kp)
int	isfd;
register struct keydesc	*kp;
{
	if (isenter(isfd, 2) == -1)
		return(-1);
	if (!(openp->o_mode & ISEXCLLOCK))
		iserrno = ENOTEXCL;
	else if (kysearch(kp, 0) > 1 && kydelete(kp) == 0)
		btrecover(kp->k_rootnode, kp);
	else
		iserrno = EBADKEY;
	return(isexit());
}

iserase(name)
register char	*name;
{
	char	buf[64];

	iserrio = 0;
	iserrno = 0;
	if (ckfilname(name))
		return(-1);
	mkidxname(name, buf);
	if (unlink(buf))
		iserrno = errno;
	mkdatname(name, buf);
	if (unlink(buf))
		iserrno = errno;
	if (iserrno)
		return(-1);
	return(0);
}

isremlock(name)
register char	*name;
{
	iserrio = 0;
	iserrno = 0;
	if (ckfilname(name))
		return(-1);
	return(0);
}

isrename(old, new)
register char	*old, *new;
{
	char	b1[64], b2[64];

	iserrio = 0;
	iserrno = 0;
	if (ckfilname(old) || ckfilname(new))
		return(-1);
	mkidxname(old, b1);
	mkidxname(new, b2);
	if (link(b1, b2) || unlink(b1))
		goto err;
	mkdatname(old, b1);
	mkdatname(new, b2);
	if (link(b1, b2) || unlink(b1))
		goto err;
	return(0);
err:
	iserrno = errno;
	return(-1);
}

btrecover(blk, kp)
long	blk;
register struct keydesc	*kp;
{
	struct buf	*bp;
	char	*p;
	register struct item	*ip;
	register int	off, end;

	if (blk == 0 || (bp = bfread(blk)) == 0)
		return;
	p = bp->bf_buf + 2;
	ip = &tempitem;
	if (bp->bf_level) {
		end = bp->bf_used - 2;
		itclear(ip);
		for (off = 0; off < end; ) {
			off = itunpack(p, off, ip, kp);
			btrecover(ip->it_ptr, kp);
		}
	}
	bfrelease(bp);
	freerec(blk, 0);
}
