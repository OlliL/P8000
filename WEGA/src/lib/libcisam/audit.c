#include	<isam.h>
#include	"isint.h"

isaudit(isfd, filename, mode)
int	isfd;
char	*filename;
int	mode;
{
	register struct buf	*bp;
	long	blk;

	if (isenter(isfd, 1) == -1)
		return(-1);
	switch (mode) {
	case AUDSETNAME:
		if (dictp[DI_AUDIT] & 0200)
			goto bad;
		if (stleng(filename) > 64)
			goto bad;
		if (makefile(filename) && iserrno != 17)
			goto bad;
		iserrno = 0;
		if ((blk = ldlong(&dictp[DI_AUDIT])) == 0) {
			blk = allocrec(0);
			bp = bfgetclr(blk);
			stlong(blk, &dictp[DI_AUDIT]);
			filep->f_dictbp->bf_flags |= BFMODF;
		} else
			bp = bfread(blk & 0x7fffffffL);
		if (bp == 0)
			break;
		stcopy(filename, bp->bf_buf + 4);
		bp->bf_used = stleng(filename) + 5;
		bfdwrite(bp);
		break;
	case AUDGETNAME:
		if ((blk = ldlong(&dictp[DI_AUDIT])) == 0)
			goto bad;
		if ((bp = bfread(blk & 0x7fffffffL)) == 0)
			break;
		stcopy(bp->bf_buf + 4, filename);
		bfrelease(bp);
		break;
	case AUDSTART:
		if (ldlong(&dictp[DI_AUDIT]) == 0)
			goto bad;
		dictp[DI_AUDIT] |= 0200;
		filep->f_dictbp->bf_flags |= BFMODF;
		break;
	case AUDSTOP:
		if (dictp[DI_AUDIT] & 0200)
			if (filep->f_flags & 04) {
				close(filep->f_audfd);
				filep->f_flags &= ~04;
			}
		dictp[DI_AUDIT] &= 0177;
		filep->f_dictbp->bf_flags |= BFMODF;
		break;
	case AUDINFO:
		*filename = (dictp[DI_AUDIT] & 0200) != 0;
		break;
	}
	return(isexit());
bad:
	if (iserrno == 0)
		iserrno = 102;
	return(isexit());
}

audopen()
{
	long	blk;
	register struct buf	*bp;
	register char	*p;

	if (filep->f_flags & 04)
		return(0);
	if ((dictp[DI_AUDIT] & 0200) == 0)
		return(0);
	blk = ldlong(&dictp[DI_AUDIT]);
	if ((bp = bfread(blk & 0x7fffffffL)) == 0)
		return(-1);
	p = bp->bf_buf;
	if ((filep->f_audfd = open(p + 4, 1)) == -1) {
		iserrno = errno;
		iserrio = 0x13;
	} else
		filep->f_flags |= 04;
	bfrelease(bp);
	if (iserrno)
		return(-1);
	return(0);
}

audoutput(type, rec)
int	type;
char	*rec;
{
	struct audhead	ah;
	long	t;
	register int	n;
	register int	c;

	if (!(dictp[DI_AUDIT] & 0200))
		return;
	if (!(filep->f_flags & 04))
		return;
	if (lseek(filep->f_audfd, 0L, 2) == -1L) {
		iserrno = errno;
		iserrio = 0x33;
		return;
	}
	t = time(&t);
	ah.au_type[0] = type;
	stlong(t, ah.au_time);
	stint(openp->o_pid, ah.au_procid);
	stint(openp->o_uid, ah.au_userid);
	n = filep->f_recsize;
	c = rec[n];
	rec[n] = '\n';
	if (write(filep->f_audfd, &ah, 9) != 9 || write(filep->f_audfd, rec, n + 1) != n + 1) {
		iserrno = errno;
		iserrio = 0x53;
	}
	rec[n] = c;
}
