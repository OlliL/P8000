#include	<isam.h>
#include	"isint.h"

long	unlk(), lkdata();

islock(isfd)
int	isfd;
{
	if (isenter(isfd, -1) == -1)
		return(-1);
	lkfile(2);
	return(isexit());
}

isunlock(isfd)
int	isfd;
{
	if (isenter(isfd, -1) == -1)
		return(-1);
	lkfile(0);
	return(isexit());
}

lksemaphore(mode)
int	mode;
{
	if (lseek(filep->f_idxfd, 512L, 0) != 512L) {
		iserrno = errno;
		iserrio = 0x31;
		return(-1);
	}
	if (locking(filep->f_idxfd, mode, 0x3fff0000L) == -1) {
		iserrno = errno;
		iserrio = 0x61;
		return(-1);
	}
	if (mode == 0)
		openp->o_flags &= ~0100;
	else
		openp->o_flags |= 0100;
	return(0);
}

lkopenlock(mode)
int	mode;
{
	long	off, len;

	off = 200L;
	len = 1L;
	if (mode == 04000 || mode == 0)
		len = 200L;
retry:
	if (lseek(filep->f_idxfd, off, 0) != off) {
		iserrno = errno;
		iserrio = 0x31;
		return(-1);
	}
	if (locking(filep->f_idxfd, mode == 0 ? 0 : 2, len) == -1) {
		if (errno == 13 && len == 1L && (off += 2) < 400L)
			goto retry;
		if (errno != 13) {
			iserrno = errno;
			iserrio = 0x61;
		} else
			iserrno = 113;
		return(-1);
	}
	return(0);
}

lkrecord(recnum, mode)
long	recnum;
register int	mode;
{
	long	off, len;
	register int	lmode;

	if (openp->o_flags & 040)
		return(0);
	if (openp->o_mode & 04000)
		return(0);
	len = filep->f_recsize;
	off = (recnum - 1) * len;
	if (lseek(filep->f_datfd, off, 0) != off) {
		iserrno = errno;
		iserrio = 0x32;
		return(-1);
	}
	lmode = mode == 0 ? 0 : 2;
	if (locking(filep->f_datfd, lmode, len) == -1) {
		if (errno != 13) {
			iserrno = errno;
			iserrio = 0x62;
		} else
			iserrno = 107;
		return(-1);
	}
	if (mode == 3)
		locking(filep->f_datfd, 0, len);
	return(0);
}

lkfile(mode)
int	mode;
{
	long	off, len;

	if (openp->o_mode & 04000)
		return(0);
	off = 0;
	len = 0;
	if (lseek(filep->f_datfd, off, 0) != off) {
		iserrno = errno;
		iserrio = 0x32;
		return(-1);
	}
	if (locking(filep->f_datfd, mode, len) == -1) {
		if (errno != 13) {
			iserrno = errno;
			iserrio = 0x62;
		}
		if (iserrno == 0)
			iserrno = 107;
		return(-1);
	}
	if (mode == 0)
		openp->o_flags &= ~040;
	else
		openp->o_flags |= 040;
	return(0);
}

locking(fd, mode, len)
int	fd, mode;
long	len;
{
	struct {
		long	l_len;
		long	l_x;
		int	l_y;
	} lk;
	long	r;

	lk.l_len = len;
	lk.l_x = 0;
	lk.l_y = 0;
	switch (mode) {
	case 0:
		r = unlk(fd, 4, &lk);
		break;
	case 1:
		r = lkdata(fd, 1, &lk);
		break;
	case 2:
		r = lkdata(fd, 4, &lk);
		break;
	}
	if (r == -1)
		return(-1);
	return(0);
}
