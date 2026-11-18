#include	<isam.h>
#include	"isint.h"
#include	<sys/types.h>
#include	<sys/stat.h>

allocopen()
{
	register int	i;

	for (openp = opens, i = 0; i < nopens; openp++, i++)
		if (openp->o_flags == 0) {
			wdfill(openp, sizeof(struct open), 0);
			openp->o_pid = getpid();
			openp->o_uid = getuid();
			openp->o_flags = 1;
			return(i);
		}
	iserrno = 104;
	return(-1);
}

freeopen(op)
register struct open	*op;
{
	if (op->o_filep)
		freefile(op->o_filep);
	openp->o_flags = 0;
}

struct file *
allocfile(name, mode)
char	*name;
register int	mode;
{
	register struct file	*fp, *free;
	char	buf[64];
	long	id;

	mkidxname(name, buf);
	if ((id = getfileid(buf)) == -1L)
		return(0);
	for (free = 0, fp = files; fp < &files[nfiles]; fp++) {
		if (fp->f_refcnt > 0 && fp->f_fileid == id)
			goto found;
		if (free == 0 && fp->f_refcnt == 0)
			free = fp;
	}
	if (free == 0) {
		iserrno = 104;
		return(0);
	}
	fp = free;
	wdfill(fp, sizeof(struct file), 0);
	if ((fp->f_idxfd = open(buf, 2)) == -1) {
		iserrno = errno;
		iserrio = 0x11;
		return(0);
	}
	mkdatname(name, buf);
	if ((fp->f_datfd = open(buf, mode == 0 ? 0 : mode == 1 ? 1 : 2)) == -1) {
		iserrno = errno;
		iserrio = 0x12;
		close(fp->f_idxfd);
		return(0);
	}
	fp->f_fileid = id;
found:
	fp->f_refcnt++;
	return(fp);
}

freefile(fp)
struct file	*fp;
{
	if (!--fp->f_refcnt) {
		bfignor(filep->f_fileid);
		if (fp->f_flags & 04)
			close(fp->f_audfd);
		fp->f_flags = 0;
		fp->f_fileid = -1L;
		close(fp->f_idxfd);
		close(fp->f_datfd);
	}
}

long
allocrec(type)
int	type;
{
	struct buf	*bp;
	char	*freep, *cntp;
	long	blk, blk2;

	if (type == 0) {
		freep = dictp + 29;
		cntp = dictp + 37;
	} else {
		freep = dictp + 25;
		cntp = dictp + 33;
	}
	for (;;) {
		if ((blk = ldlong(freep)) == 0) {
newblk:
			blk = ldlong(cntp) + 1;
			stlong(blk, cntp);
			filep->f_dictbp->bf_flags |= BFMODF;
			return(blk);
		}
		if ((bp = bfread(blk)) == 0)
			goto newblk;
		bp->bf_used -= 4;
		blk2 = ldlong(bp->bf_buf + bp->bf_used);
		if (bp->bf_used <= 2) {
			stlong(blk2, freep);
			filep->f_dictbp->bf_flags |= BFMODF;
			if (type) {
				bfrelease(bp);
				freerec(blk, 0);
				continue;
			}
		} else
			blk = blk2;
		bfdwrite(bp);
		return(blk);
	}
}

freerec(blk, type)
long	blk;
int	type;
{
	register struct buf	*bp;
	register char	*freep, *cntp;
	long	blk2, nblk;

	if (type == 0) {
		freep = dictp + 29;
		cntp = dictp + 37;
	} else {
		freep = dictp + 25;
		cntp = dictp + 33;
	}
	if (ldlong(cntp) == blk) {
		blk -= 1;
		stlong(blk, cntp);
		filep->f_dictbp->bf_flags |= BFMODF;
		return;
	}
	if ((blk2 = ldlong(freep)) == 0) {
new:
		nblk = type ? allocrec(0) : blk;
		stlong(nblk, freep);
		filep->f_dictbp->bf_flags |= BFMODF;
		bp = bfgetclr(nblk);
		bp->bf_used = 6;
		bp->bf_level = 0177;
		bp->bf_type = type == 0 ? 0376 : 0377;
		stlong(blk2, bp->bf_buf + 2);
		if (type) {
			stlong(blk, bp->bf_buf + 6);
			bp->bf_used = 10;
		}
	} else {
		if ((bp = bfread(blk2)) == 0)
			return;
		if (bp->bf_used + 4 < filep->f_nodsiz - 2) {
			stlong(blk, bp->bf_buf + bp->bf_used);
			bp->bf_used += 4;
		} else {
			bfrelease(bp);
			goto new;
		}
	}
	bfdwrite(bp);
}

makefile(name)
register char	*name;
{
	register int	fd;
	struct stat	st;

	errno = 17;
	if (stat(name, &st) == -1 && errno == 2 && (fd = creat(name, 0664)) != -1) {
		close(fd);
		return(0);
	}
	iserrno = errno;
	return(-1);
}

long
getfileid(name)
char	*name;
{
	struct stat	st;
	union {
		long	l;
		struct {
			int	dev, ino;
		} s;
	} id;

	if (stat(name, &st) == 0) {
		id.s.dev = st.st_dev;
		id.s.ino = st.st_ino;
	} else {
		id.l = -1L;
		iserrno = errno;
	}
	return(id.l);
}

dataread(rec, recnum)
register char	*rec;
long	recnum;
{
	long	off;
	register int	len;
	register int	c, n;

	len = filep->f_recsize + 1;
	off = longmult(recnum - 1, (long)len);
	if (lseek(filep->f_datfd, off, 0) != off) {
		iserrno = errno;
		iserrio = 0x32;
		return(-1);
	}
	c = rec[len - 1];
	if ((n = read(filep->f_datfd, rec, len)) == -1) {
		iserrno = errno;
		iserrio = 0x42;
	}
	if (n == len)
		n = rec[len - 1] == '\n' ? 0 : -1;
	else
		n = -1;
	rec[len - 1] = c;
	return(n);
}

datawrite(rec, recnum)
register char	*rec;
long	recnum;
{
	long	off;
	register int	len;
	register int	c;

	len = filep->f_recsize + 1;
	c = rec[len - 1];
	rec[len - 1] = '\n';
	off = longmult(recnum - 1, (long)len);
	if (lseek(filep->f_datfd, off, 0) != off) {
		iserrno = errno;
		iserrio = 0x32;
	} else if (write(filep->f_datfd, rec, len) != len) {
		iserrno = errno;
		iserrio = 0x52;
	}
	rec[len - 1] = c;
	return(iserrno ? -1 : 0);
}

datadelete(recnum)
long	recnum;
{
	long	off;
	register int	len;

	len = filep->f_recsize + 1;
	byfill(temprecord, len, 0);
	off = longmult(recnum - 1, (long)len);
	if (lseek(filep->f_datfd, off, 0) != off) {
		iserrno = errno;
		iserrio = 0x32;
	} else if (write(filep->f_datfd, temprecord, len) != len) {
		iserrno = errno;
		iserrio = 0x52;
	}
	return(iserrno ? -1 : 0);
}
