#include	<isam.h>
#include	"isint.h"

btinsert(it, kp)
struct item	*it;
struct keydesc	*kp;
{
	register struct tree	*tp;
	struct buf	*bp, *nbp;
	char	*p, *np;
	struct item	it1, it2;
	struct item	*ip1, *ip2;
	int	half, max, off, off2, len, len2;
	long	blk;

	it->it_flags = 0;
	tp = &trees[treelev];
	if ((bp = bfread(tp->t_blk)) == 0)
		return;
	p = bp->bf_buf + 2;
	ip1 = &it1;
	ip2 = &it2;
	max = filep->f_nodsiz - 4;
	off = 0;
	if (tp->t_off > 0) {
		itclear(ip1);
		while (off < tp->t_off)
			off = itunpack(p, off, ip1, kp);
		bycopy(p, tempbuf, off);
		len = off;
		itcopy(ip1, ip2);
		itpress(ip1, it, kp);
	} else {
		itclear(ip2);
		itpress(it, it, kp);
		len = 0;
	}
	len = itpack(it, tempbuf, len, kp);
	if (off < tp->t_used) {
		off = itunpack(p, off, ip2, kp);
		if (off == tp->t_used && treelev < treeleaf)
			bycopy(it->it_key, ip2->it_key, 120);
		itpress(it, ip2, kp);
		len = itpack(ip2, tempbuf, len, kp);
		bycopy(p + off, tempbuf + len, tp->t_used - off);
		len += tp->t_used - off;
	}
	if (len <= max) {
		bycopy(tempbuf, p, len);
		bp->bf_used = len + 2;
		bfdwrite(bp);
		return;
	}
	blk = allocrec(0);
	nbp = bfgetclr(blk);
	bfwrite(nbp);
	if (iserrno)
		return;
	nbp = bfread(blk);
	wdfill(bp->bf_buf, 512, 0);
	np = nbp->bf_buf + 2;
	half = max >> 1;
	off = 0;
	while ((off2 = itunpack(tempbuf, off, ip1, kp)) < half)
		off = off2;
	bycopy(tempbuf, np, off);
	len2 = off;
	if (treelev < treeleaf)
		itlast(ip1, kp);
	len2 = itpack(ip1, np, len2, kp);
	itcopy(ip1, ip2);
	off = itunpack(tempbuf, off2, ip2, kp);
	itpress(ip2, ip2, kp);
	off2 = itpack(ip2, p, 0, kp);
	bycopy(tempbuf + off, p + off2, len - off);
	len += off2 - off;
	bp->bf_used = len + 2;
	nbp->bf_used = len2 + 2;
	nbp->bf_level = bp->bf_level;
	nbp->bf_type = bp->bf_type;
	bfdwrite(nbp);
	ip1->it_ptr = blk;
	if (treelev == 0) {
		blk = allocrec(0);
		nbp = bfgetblk(blk);
		wdcopy(bp->bf_buf, nbp->bf_buf, 512);
		nbp->bf_used = bp->bf_used;
		nbp->bf_level = bp->bf_level;
		nbp->bf_type = bp->bf_type;
		wdfill(bp->bf_buf, 512, 0);
		itpress(ip1, ip1, kp);
		len = itpack(ip1, p, 0, kp);
		ip1->it_ptr = blk;
		itlast(ip1, kp);
		len = itpack(ip1, p, len, kp);
		bp->bf_level++;
		bp->bf_used = len + 2;
		bfdwrite(bp);
		bfdwrite(nbp);
		return;
	}
	bfdwrite(bp);
	treelev--;
	btinsert(ip1, kp);
	treelev++;
}

btremove(kp)
struct keydesc	*kp;
{
	struct buf	*bp;
	register struct tree	*tp;
	register char	*p;
	struct item	it1, it2;
	struct item	*ip1, *ip2;
	register int	off2, off;

	tp = &trees[treelev];
	if ((bp = bfread(tp->t_blk)) == 0)
		return;
	p = bp->bf_buf + 2;
	ip1 = &it1;
	ip2 = &it2;
	if (tp->t_off) {
		off2 = itprevious(p, tp->t_off, ip2, kp);
		itcopy(ip2, ip1);
	} else
		itclear(ip1);
	itunpack(p, tp->t_off, ip1, kp);
	off = tp->t_off;
	if (tp->t_next != tp->t_used) {
		off2 = itunpack(p, tp->t_next, ip1, kp);
		itpress(tp->t_off ? ip2 : ip1, ip1, kp);
		off = itpack(ip1, p, off, kp);
		bycopy(p + off2, p + off, tp->t_used - off2);
		off += tp->t_used - off2;
	}
	byfill(p + off, tp->t_used - off, 0);
	bp->bf_used = off + 2;
	if (off == 0)
		bp->bf_level = 0;
	bfdwrite(bp);
	if (treelev && off == 0) {
		freerec(tp->t_blk, 0);
		treelev--;
		btremove(kp);
		treelev++;
	}
}
