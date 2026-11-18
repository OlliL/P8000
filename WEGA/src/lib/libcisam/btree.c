#include	<isam.h>
#include	"isint.h"

btfirst(kp)
struct keydesc	*kp;
{
	register struct buf	*bp;
	register struct item	*ip;
	register struct tree	*tp;
	long	blk;

	blk = kp->k_rootnode;
	ip = &treeitem;
	tp = trees;
	treeflags = 0;
	treelev = 0;
	for (;;) {
		tp->t_off = 0;
		tp->t_blk = blk;
		if ((bp = bfread(blk)) == 0) {
			treelev = -1;
			break;
		}
		tp->t_used = bp->bf_used - 2;
		if (tp->t_used == 0) {
			tp->t_next = 0;
			treeflags = 3;
		} else {
			itclear(ip);
			tp->t_next = itunpack(bp->bf_buf + 2, 0, ip, kp);
			blk = ip->it_ptr;
		}
		bfrelease(bp);
		if (bp->bf_level == 0)
			break;
		treelev++;
		tp++;
	}
	treeleaf = treelev;
}

btlast(kp)
struct keydesc	*kp;
{
	register struct buf	*bp;
	register struct item	*ip;
	register struct tree	*tp;
	long	blk;

	blk = kp->k_rootnode;
	ip = &treeitem;
	tp = trees;
	treeflags = 0;
	treelev = 0;
	for (;;) {
		tp->t_blk = blk;
		if ((bp = bfread(blk)) == 0) {
			treelev = -1;
			break;
		}
		tp->t_used = bp->bf_used - 2;
		tp->t_next = tp->t_used;
		if (tp->t_used == 0) {
			tp->t_off = 0;
			treeflags = 3;
		} else {
			tp->t_off = itprevious(bp->bf_buf + 2, tp->t_next, ip, kp);
			blk = ip->it_ptr;
		}
		bfrelease(bp);
		if (bp->bf_level == 0)
			break;
		treelev++;
		tp++;
	}
	treeleaf = treelev;
}

btcurrent()
{
	register struct keydesc	*kp;
	register struct item	*ip;

	if (!(openp->o_flags & 04))
		return(-1);
	ip = &openp->o_item;
	kp = &openp->o_key;
	if (treelev < 0 || itcompare(&treeitem, ip, kp, 0) != 0 || treeitem.it_ptr != ip->it_ptr)
		btsearch(ip, kp, 0, 0);
	if (treelev < 0)
		return(-1);
	if (itcompare(&treeitem, ip, kp, 0) != 0 || treeitem.it_ptr != ip->it_ptr)
		openp->o_flags &= ~010;
	return(0);
}

btsearch(it, kp, len, mode)
struct item	*it;
register struct keydesc	*kp;
int	len, mode;
{
	struct buf	*bp;
	char	*p;
	register struct item	*ip;
	register struct tree	*tp;
	int	r;
	long	blk;

	blk = kp->k_rootnode;
	ip = &treeitem;
	tp = trees;
	treeflags = 0;
	treelev = 0;
	for (;;) {
		tp->t_blk = blk;
		if ((bp = bfread(blk)) == 0) {
			treelev = -1;
			break;
		}
		p = bp->bf_buf + 2;
		tp->t_used = bp->bf_used - 2;
		if (tp->t_used == 0) {
			tp->t_off = 0;
			tp->t_next = 0;
			treeflags = 3;
			bfrelease(bp);
			break;
		}
		itclear(ip);
		for (tp->t_off = 0; tp->t_off < tp->t_used; tp->t_off = tp->t_next) {
			tp->t_next = itunpack(p, tp->t_off, ip, kp);
			r = itcompare(it, ip, kp, len);
			if (r == 0 && mode == 2 && bp->bf_level == 0)
				if (kp->k_flags & ISDUPS) {
					openp->o_flags |= 020;
					it->it_dupnum = ip->it_dupnum + 1;
				} else {
					iserrno = 100;
					treelev = -1;
					break;
				}
			if (r == 0 && kp->k_flags & ISDUPS && mode == 0)
				if (it->it_dupnum < ip->it_dupnum)
					r = -1;
				else if (it->it_dupnum > ip->it_dupnum)
					r = 1;
			if (r < 0)
				break;
			if (r == 0 && mode == 0)
				break;
			if (tp->t_next >= tp->t_used) {
				if (bp->bf_level != 0)
					break;
				bfrelease(bp);
				treeleaf = treelev;
				if (mode != 2)
					btnext(kp);
				else
					tp->t_off = tp->t_used;
				return;
			}
		}
		blk = ip->it_ptr;
		bfrelease(bp);
		if (bp->bf_level == 0)
			break;
		treelev++;
		tp++;
	}
	treeleaf = treelev;
}

btnext(kp)
struct keydesc	*kp;
{
	register struct buf	*bp;
	register struct item	*ip;
	register struct tree	*tp;

	if (treeflags & 02)
		return;
	treeflags = 0;
	if (treelev < 0) {
		treeflags = 02;
		return;
	}
	ip = &treeitem;
	tp = &trees[treelev];
	if (tp->t_next >= tp->t_used) {
		if (treelev > 0) {
			treelev--;
			btnext(kp);
			if (treelev < 0)
				return;
			treelev++;
		} else
			treeflags = 02;
		if (treeflags & 02) {
			if (treelev == treeleaf)
				tp->t_off = tp->t_used;
			return;
		}
		tp->t_blk = ip->it_ptr;
		tp->t_off = 0;
	} else
		tp->t_off = tp->t_next;
	if ((bp = bfread(tp->t_blk)) == 0) {
		treelev = -1;
		return;
	}
	tp->t_used = bp->bf_used - 2;
	tp->t_next = itunpack(bp->bf_buf + 2, tp->t_off, ip, kp);
	bfrelease(bp);
}

btprevious(kp)
struct keydesc	*kp;
{
	register struct buf	*bp;
	register struct item	*ip;
	register struct tree	*tp;

	if (treeflags & 01)
		return;
	treeflags = 0;
	if (treelev < 0) {
		treeflags = 01;
		return;
	}
	ip = &treeitem;
	tp = &trees[treelev];
	if (tp->t_off == 0) {
		if (treelev > 0) {
			treelev--;
			btprevious(kp);
			if (treelev < 0)
				return;
			treelev++;
		} else
			treeflags = 01;
		if (treeflags & 01) {
			if (treelev == treeleaf)
				tp->t_next = 0;
			return;
		}
		tp->t_blk = ip->it_ptr;
	}
	if ((bp = bfread(tp->t_blk)) == 0) {
		treelev = -1;
		return;
	}
	tp->t_used = bp->bf_used - 2;
	tp->t_next = tp->t_off ? tp->t_off : tp->t_used;
	tp->t_off = itprevious(bp->bf_buf + 2, tp->t_next, ip, kp);
	bfrelease(bp);
}

btadditem(it, kp)
struct item	*it;
register struct keydesc	*kp;
{
	register struct tree	*tp;

	btsearch(it, kp, 0, 2);
	if (treelev < 0)
		return(-1);
	tp = &trees[treelev];
	if (tp->t_off == 0 && !(treeflags & 01)) {
		btprevious(kp);
		if (itcompare(it, &treeitem, kp, 0) == 0)
			if (kp->k_flags & ISDUPS) {
				openp->o_flags |= 020;
				it->it_dupnum = treeitem.it_dupnum + 1;
			} else
				goto duperr;
		btnext(kp);
	}
	if (treelev < 0)
		goto bad;
	btinsert(it, kp);
	treelev = -1;
	return(0);
duperr:
	iserrno = 100;
bad:
	treelev = -1;
	return(-1);
}

btdelitem(it, kp)
struct item	*it;
register struct keydesc	*kp;
{
	btsearch(it, kp, 0, 0);
loop:
	if (treelev < 0)
		goto bad;
	if (treeflags & 02 || itcompare(it, &treeitem, kp, 0) != 0) {
		iserrno = 111;
		goto bad;
	}
	if (it->it_ptr == treeitem.it_ptr) {
		btremove(kp);
		treelev = -1;
		return(0);
	}
	btnext(kp);
	goto loop;
bad:
	treelev = -1;
	return(-1);
}
