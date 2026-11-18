#include	<isam.h>
#include	"isint.h"

isopen(name, mode)
char	*name;
register int	mode;
{
	register int	lmode, isfd;

	if (ckfilname(name))
		return(-1);
	if ((isfd = allocopen()) < 0)
		return(-1);
	openp->o_mode = mode & 0377;
	lmode = mode & 0177400;
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
	switch (openp->o_mode) {
	case ISINPUT:
	case ISOUTPUT:
	case ISINOUT:
		break;
	default:
badarg:
		iserrno = EBADARG;
		goto bad;
	}
	if ((filep = openp->o_filep = allocfile(name, openp->o_mode)) == 0)
		goto bad;
	if (isenter(isfd, openp->o_mode))
		goto bad;
	if (lkopenlock(lmode))
		goto exit;
	openp->o_mode += lmode;
	filep->f_nodsiz = ldint(dictp + 6);
	filep->f_recsize = ldint(dictp + 13);
	if ((((char)*(unsigned char *)dictp << 8) + ((char)((unsigned char *)dictp)[1] & 0377)) != 0xfe53 || filep->f_nodsiz != 511 || (dictp[11] & 0377) != 2 || kysearch(&openp->o_key, 1) < 0) {
		iserrno = EBADFILE;
		goto unlock;
	}
	kposition(temprecord, 0, 0);
	if (iserrno == 0) {
		isexit();
		return(isfd);
	}
unlock:
	lkopenlock(0);
exit:
	isexit();
bad:
	if (openp->o_flags & 0100)
		lksemaphore(0);
	freeopen(openp);
	return(-1);
}

iscleanup()
{
	register struct open	*op;
	register int	i;

	for (op = opens, i = 0; i < nopens; op++, i++)
		if (op->o_flags & 01)
			isclose(i);
	return(0);
}

isclose(isfd)
int	isfd;
{
	register int	r;

	if (isenter(isfd, -1) == -1)
		return(-1);
	if (!(openp->o_flags & 0100))
		lksemaphore(1);
	lkfile(0);
	lkopenlock(0);
	openp->o_flags &= ~0100;
	r = isexit();
	bfflush(filep->f_fileid);
	lksemaphore(0);
	freeopen(openp);
	return(r);
}

isrelease(isfd)
int	isfd;
{
	if (isenter(isfd, 0) == -1)
		return(-1);
	lkfile(0);
	return(isexit());
}

isflush(isfd)
int	isfd;
{
	if (isenter(isfd, 1) == -1)
		return(-1);
	isexit();
	bfflush(filep->f_fileid);
	if (iserrno)
		return(-1);
	return(0);
}

isenter(isfd, mode)
register int	isfd, mode;
{
	long	blk;

	iserrio = 0;
	iserrno = 0;
	if (isfd < 0 || isfd >= nopens)
		goto bad;
	openp = &opens[isfd];
	if (!(openp->o_flags & 01))
		goto bad;
	if (getpid() != openp->o_pid)
		goto bad;
	filep = openp->o_filep;
	if (((mode + 1) & (openp->o_mode + 1)) != mode + 1)
		goto bad;
	if (!(openp->o_mode & ISEXCLLOCK) && lksemaphore(1))
		return(-1);
	filep->f_flags &= ~01;
	openp->o_flags &= ~020;
	filep->f_dictbp = bfread(1L);
	if (filep->f_dictbp == 0) {
		if (!(openp->o_mode & ISEXCLLOCK))
			lksemaphore(0);
		return(-1);
	}
	dictp = filep->f_dictbp->bf_buf;
	blk = ldlong(dictp + 41);
	if (filep->f_chgcnt != blk) {
		filep->f_chgcnt = blk;
		bfignor(filep->f_fileid);
		treelev = -1;
	}
	if (isfd != treeisfd)
		treelev = -1;
	treeisfd = isfd;
	if (openp->o_mode & ISAUTOLOCK)
		lkfile(0);
	return(0);
bad:
	iserrno = ENOTOPEN;
	return(-1);
}

isexit()
{
	if (filep->f_flags & 01) {
		filep->f_flags &= ~01;
		stlong(++filep->f_chgcnt, dictp + 41);
		filep->f_dictbp->bf_flags |= BFMODF;
	}
	bfrelease(filep->f_dictbp);
	if (!(openp->o_mode & ISEXCLLOCK)) {
		bfflush(filep->f_fileid);
		filep->f_dictbp->bf_flags = 0;
	}
	switch (iserrno) {
	case 0:
		isstat1 = '0';
		isstat2 = openp->o_flags & 020 ? '2' : '0';
		break;
	case EDUPL:
		isstat1 = '2';
		isstat2 = '2';
		break;
	case EENDFILE:
		isstat1 = '1';
		isstat2 = '0';
		break;
	case ENOREC:
		isstat1 = '2';
		isstat2 = '3';
		break;
	case ENOCURR:
		isstat1 = '2';
		isstat2 = '1';
		break;
	default:
		isstat1 = '9';
		isstat2 = '0';
	}
	if (openp->o_flags & 0100)
		lksemaphore(0);
	return(iserrno ? -1 : 0);
}

isuniqueid(isfd, id)
int	isfd;
long	*id;
{
	long	uid;

	if (isenter(isfd, -1) == -1)
		return(-1);
	uid = ldlong(dictp + 45);
	*id = uid;
	uid++;
	stlong(uid, dictp + 45);
	filep->f_dictbp->bf_flags |= BFMODF;
	return(isexit());
}

isindexinfo(isfd, buf, num)
int	isfd;
char	*buf;
register int	num;
{
	struct dictinfo	*di;
	register struct buf	*bp;
	long	nrec, blk;

	if (isenter(isfd, -1) == -1)
		return(-1);
	di = (struct dictinfo *)buf;
	if (num == 0) {
		di->di_nkeys = ldint(dictp + 8);
		di->di_recsize = ldint(dictp + 13);
		di->di_idxsize = ldint(dictp + 6);
		nrec = ldlong(dictp + 33);
		blk = ldlong(dictp + 25);
		while (blk != 0) {
			if ((bp = bfread(blk)) == 0)
				break;
			nrec -= (bp->bf_used - 6) / sizeof(long);
			blk = ldlong(bp->bf_buf + 2);
			bfrelease(bp);
		}
		di->di_nrecords = nrec;
	} else if (kysearch(di, num) < 0)
		iserrno = EBADKEY;
	return(isexit());
}
