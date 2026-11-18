#include	<isam.h>
#include	"isint.h"

static int	bfinited = 0;

bfinit()
{
	register struct buf	*bp;
	register int	i;

	avbufs.bf_forw = avbufs.bf_back = &avbufs;
	for (bp = bufs, i = 0; i < nbufs; i++, bp++) {
		bp->bf_flags = BFBUSY;
		bp->bf_fileid = -1L;
		bp->bf_buf = &buffers[i * 512];
		bfrelease(bp);
	}
	bfinited = 1;
}

struct buf *
bfread(blkno)
long	blkno;
{
	register struct buf	*bp;
	register char	*p;
	register int	n;
	long	off;
	struct buf	*bfgetblk();

	if (blkno <= 0) {
		iserrno = 105;
		goto out;
seekerr:
		iserrio = 0x31;
bad:
		iserrno = errno;
		bfrelease(bp);
out:
		return(0);
	}
	bp = bfgetblk(blkno);
	if (!(bp->bf_flags & BFVALID)) {
		off = (blkno - 1) << 9;
		if (lseek(bp->bf_fd, off, 0) != off)
			goto seekerr;
		p = bp->bf_buf;
		n = off ? filep->f_nodsiz + 1 : 54;
		if (read(bp->bf_fd, p, n) != n) {
			iserrio = 0x41;
			goto bad;
		}
		bp->bf_used = (((p)[0]<<8)+((p)[1]&0377)) & 077777;
		bp->bf_type = *(p + filep->f_nodsiz - 2) & 0377;
		bp->bf_level = *(p + filep->f_nodsiz - 1) & 0177;
		bp->bf_flags |= BFVALID;
	}
	return(bp);
}

struct buf *
bfgetclr(blkno)
long	blkno;
{
	register struct buf	*bp;
	register int	*p;
	register int	i;
	struct buf	*bfgetblk();

	bp = bfgetblk(blkno);
	p = (int *)bp->bf_buf;
	i = 256;
	do
		*p++ = 0;
	while (--i);
	bp->bf_flags |= BFMODF|BFVALID;
	return(bp);
}

struct buf *
bfgetblk(blkno)
long	blkno;
{
	register struct buf	*bp;
	register int	i;

	if (!bfinited)
		bfinit();
	for (bp = bufs, i = 0; i < nbufs; i++, bp++)
		if (bp->bf_blkno == blkno && bp->bf_fileid == filep->f_fileid)
			goto found;
	bp = avbufs.bf_back;
	if (bp == &avbufs)
		error("no more buffers");
	if (bp->bf_flags & BFMODF)
		bfwrite(bp);
	bp->bf_blkno = blkno;
	bp->bf_fileid = filep->f_fileid;
	bp->bf_flags = 0;
found:
	if (bp->bf_flags & BFBUSY)
		error("buffer not free");
	bp->bf_flags |= BFBUSY;
	bp->bf_fd = filep->f_idxfd;
	bp->bf_back->bf_forw = bp->bf_forw;
	bp->bf_forw->bf_back = bp->bf_back;
	return(bp);
}

bfrelease(bp)
register struct buf	*bp;
{
	if (bp->bf_flags & BFMODF)
		filep->f_flags |= 1;
	if (!(bp->bf_flags & BFBUSY))
		error("buffer not busy");
	bp->bf_flags &= ~BFBUSY;
	bp->bf_forw = avbufs.bf_forw;
	bp->bf_forw->bf_back = bp;
	avbufs.bf_forw = bp;
	bp->bf_back = &avbufs;
}

bfdwrite(bp)
register struct buf	*bp;
{
	bp->bf_flags |= BFMODF|BFVALID;
	bfrelease(bp);
}

bfwrite(bp)
register struct buf	*bp;
{
	register char	*p;
	register int	n;
	long	off;

	off = (bp->bf_blkno - 1) << 9;
	p = bp->bf_buf;
	if (off) {
		((p)[0]=(bp->bf_used)>>8,(p)[1]=(bp->bf_used));
		*(p + filep->f_nodsiz - 2) = bp->bf_type;
		*(p + filep->f_nodsiz - 1) = bp->bf_level;
		n = filep->f_nodsiz + 1;
	} else
		n = 54;
	if (lseek(bp->bf_fd, off, 0) != off) {
		iserrno = errno;
		iserrio = 0x31;
	} else if (write(bp->bf_fd, p, n) != n) {
		iserrno = errno;
		iserrio = 0x51;
	}
	bp->bf_flags &= ~BFMODF;
	bp->bf_flags |= BFVALID;
	if (bp->bf_flags & BFBUSY)
		bfrelease(bp);
}

bfflush(fileid)
long	fileid;
{
	register struct buf	*bp;
	register int	i;

	for (bp = bufs, i = 0; i < nbufs; i++, bp++)
		if (!(bp->bf_flags & BFBUSY) && bp->bf_fileid == fileid && bp->bf_flags & BFMODF)
			bfwrite(bp);
}

bfignor(fileid)
long	fileid;
{
	register struct buf	*bp;
	register int	i;

	for (bp = bufs, i = 0; i < nbufs; i++, bp++)
		if (!(bp->bf_flags & BFBUSY) && bp->bf_fileid == fileid) {
			if (bp->bf_flags & BFMODF)
				error("ignoring modified buffer");
			bp->bf_fileid = -1L;
		}
}
