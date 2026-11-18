#include	<isam.h>
#include	"isint.h"

kysearch(kp, num)
struct keydesc	*kp;
int	num;
{
	struct buf	*bp;
	register char	*p;
	struct keydesc	k;
	register int	off;
	int	end, len, cnt;
	long	blk;

	if (num <= 0 && kylegal(kp, filep->f_recsize) == -1)
		return(-1);
	cnt = 0;
	blk = ldlong(dictp + 15);
	while (blk != 0) {
		if ((bp = bfread(blk)) == 0)
			return(-1);
		p = bp->bf_buf;
		end = bp->bf_used - 6;
		for (off = 0; off != end; off += len) {
			cnt++;
			len = ldint(p + 6 + off);
			kyunpack(p + 6 + off, &k);
			if (num <= 0) {
				if (kycompare(kp, &k) == 0) {
					if (num == 0) {
						kp->k_flags = k.k_flags;
						kp->k_rootnode = k.k_rootnode;
						kp->k_len = k.k_len;
						bfrelease(bp);
						return(cnt);
					} else {
						end -= len;
						bycopy(p + 6 + off + len, p + 6 + off, end - off);
						byfill(p + 6 + end, len, 0);
						bp->bf_used = end + 6;
						bfdwrite(bp);
						stint(ldint(dictp + 8) - 1, dictp + 8);
						filep->f_dictbp->bf_flags |= BFMODF;
					}
					return(cnt);
				}
			} else if (num == cnt) {
				kycopy(&k, kp);
				bfrelease(bp);
				return(num);
			}
		}
		blk = ldlong(p + 2);
		bfrelease(bp);
	}
	return(-1);
}

kycompare(k1, k2)
register struct keydesc	*k1;
struct keydesc	*k2;
{
	register struct keypart	*p1, *p2;
	int	i;

	if (k1->k_nparts != k2->k_nparts)
		return(-1);
	for (p1 = k1->k_part, p2 = k2->k_part, i = 0; i < k1->k_nparts; p1++, p2++, i++)
		if (p1->kp_start != p2->kp_start || p1->kp_leng != p2->kp_leng || p1->kp_type != p2->kp_type)
			return(-1);
	return(0);
}

kypack(kp, p)
register struct keydesc	*kp;
register char	*p;
{
	register struct keypart	*kpp;
	int	i;

	stint(kp->k_nparts * 5 + 7, p);
	stlong(kp->k_rootnode, p + 2);
	p[6] = kp->k_flags >> 1;
	p += 7;
	if (kp->k_flags & ISDUPS)
		kp->k_leng |= 0100000;
	for (kpp = kp->k_part, i = 0; i < kp->k_nparts; kpp++, i++) {
		stint(kpp->kp_leng, p);
		stint(kpp->kp_start, p + 2);
		p[4] = kpp->kp_type;
		p += 5;
	}
	kp->k_leng &= ~0100000;
}

kyunpack(p, kp)
register char	*p;
register struct keydesc	*kp;
{
	register struct keypart	*kpp;
	int	i;

	kp->k_nparts = (ldint(p) - 7) / 5;
	kp->k_rootnode = ldlong(p + 2);
	kp->k_len = 0;
	kp->k_flags = (p[6] & 0377) * 2;
	p += 7;
	for (kpp = kp->k_part, i = 0; i < kp->k_nparts; kpp++, i++) {
		kpp->kp_leng = ldint(p);
		kp->k_len += kpp->kp_leng & 077777;
		kpp->kp_start = ldint(p + 2);
		kpp->kp_type = p[4] & 0377;
		p += 5;
	}
	if (kp->k_leng & 0100000)
		kp->k_flags |= ISDUPS;
	kp->k_leng &= 077777;
}

static int	typesize[] = { 0, 2, 4, 8, 4 };

kylegal(kp, recsize)
register struct keydesc	*kp;
register int	recsize;
{
	register struct keypart	*kpp;
	register int	i;
	register int	t;

	if (kp->k_nparts <= 0 || kp->k_nparts > NPARTS)
		goto bad;
	kpp = kp->k_part;
	kp->k_len = 0;
	for (i = 0; i < kp->k_nparts; kpp++, i++) {
		if (kpp->kp_start < 0 || kpp->kp_start > recsize || kpp->kp_leng < 0 || kpp->kp_leng > recsize ||
		    kpp->kp_start + kpp->kp_leng > recsize)
			goto bad;
		if ((t = kpp->kp_type & TYPEMASK) >= MAXTYPE)
			goto bad;
		if (typesize[t] && typesize[t] != kpp->kp_leng)
			goto bad;
		kp->k_len += kpp->kp_leng;
	}
	if (kp->k_len > 120)
		goto bad;
	return(0);
bad:
	iserrno = 103;
	return(-1);
}

kycopy(from, to)
struct keydesc	*from, *to;
{
	wdcopy((int *)from, (int *)to, sizeof(struct keydesc));
}

kyadd(kp)
struct keydesc	*kp;
{
	struct buf	*bp;
	register char	*p;
	register int	off;
	register int	need;
	long	blk;

	if (kysearch(kp, 0) != -1) {
		iserrno = 108;
		return(-1);
	}
	kp->k_rootnode = allocrec(0);
	bp = bfgetclr(kp->k_rootnode);
	bp->bf_used = 2;
	bp->bf_level = 0;
	bp->bf_type = 0;
	bfdwrite(bp);
	blk = ldlong(dictp + 15);
	need = kp->k_nparts * 5 + 7;
	while ((bp = bfread(blk)) != 0) {
		p = bp->bf_buf;
		off = bp->bf_used - 6;
		if (bp->bf_used + need < filep->f_nodsiz - 2) {
			kypack(kp, p + 6 + off);
			off += need;
			bp->bf_used = off + 6;
			bfdwrite(bp);
			stint(ldint(dictp + 8) + 1, dictp + 8);
			filep->f_dictbp->bf_flags |= BFMODF;
			return(0);
		}
		if ((blk = ldlong(p + 2)) == 0) {
			blk = allocrec(0);
			stlong(blk, p + 2);
			bfdwrite(bp);
			bp = bfgetclr(blk);
			bp->bf_used = 6;
			bp->bf_level = 0176;
			bp->bf_type = 0377;
			bfdwrite(bp);
		} else
			bfrelease(bp);
	}
	return(-1);
}

kydelete(kp)
struct keydesc	*kp;
{
	if (kysearch(kp, -1) > 0)
		return(0);
	iserrno = 103;
	return(-1);
}
