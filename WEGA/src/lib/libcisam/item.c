#include	<isam.h>
#include	"isint.h"

float	ldfloat();
double	lddbl();

itcompare(it1, it2, kp, len)
struct item	*it1, *it2;
struct keydesc	*kp;
int	len;
{
	register struct keypart	*kpp;
	register int	r;
	char	*p1, *p2;
	int	n;
	int	i1, i2;
	long	l1, l2;
	float	f1, f2;
	double	d1, d2;

	if (len <= 0)
		len = kp->k_len;
	kpp = kp->k_part;
	r = 0;
	p1 = it1->it_key;
	p2 = it2->it_key;
	n = kp->k_nparts;
	while (n > 0 && r == 0 && len > 0) {
		switch (kpp->kp_type & TYPEMASK) {
		case CHARTYPE:
			r = bycmpr(p1, p2, kpp->kp_leng < len ? kpp->kp_leng : len);
			break;
		case INTTYPE:
			i1 = ldint(p1);
			i2 = ldint(p2);
			if (i1 < i2)
				r = -1;
			else if (i1 > i2)
				r = 1;
			break;
		case LONGTYPE:
			l1 = ldlong(p1);
			l2 = ldlong(p2);
			if (l1 < l2)
				r = -1;
			else if (l1 > l2)
				r = 1;
			break;
		case FLOATTYPE:
			f1 = ldfloat(p1);
			f2 = ldfloat(p2);
			if (f1 < f2)
				r = -1;
			else if (f1 > f2)
				r = 1;
			break;
		case DOUBLETYPE:
			d1 = lddbl(p1);
			d2 = lddbl(p2);
			if (d1 < d2)
				r = -1;
			else if (d1 > d2)
				r = 1;
			break;
		}
		if (kpp->kp_type & ISDESC)
			r = -r;
		p1 += kpp->kp_leng;
		p2 += kpp->kp_leng;
		len -= kpp->kp_leng;
		kpp++;
		n--;
	}
	return(r);
}

itpack(it, buf, off, kp)
register struct item	*it;
char	*buf;
int	off;
register struct keydesc	*kp;
{
	register char	*p;

	p = buf + off;
	if ((it->it_flags & 1) == 0) {
		if (off)
			p[-4] &= 0177;
		if (kp->k_flags & LCOMPRESS)
			*p++ = it->it_leadcnt;
		if (kp->k_flags & TCOMPRESS)
			*p++ = it->it_trailcnt;
		bycopy(it->it_key + it->it_leadcnt, p, it->it_keylen);
		p += it->it_keylen;
	} else if (off)
		p[-4] |= 0200;
	if (kp->k_flags & ISDUPS) {
		stint((int)it->it_dupnum, p);
		p += 2;
	}
	stlong(it->it_ptr, p);
	if (it->it_flags & 2)
		*p |= 0200;
	return(off + it->it_totlen);
}

itunpack(buf, off, it, kp)
char	*buf;
int	off;
register struct item	*it;
register struct keydesc	*kp;
{
	register char	*p;
	char	*q;

	q = p = buf + off;
	if (off && p[-4] & 0200) {
		it->it_flags = 1;
		it->it_keylen = 0;
		it->it_leadcnt = 0;
		it->it_trailcnt = 0;
	} else {
		it->it_flags = 0;
		it->it_leadcnt = kp->k_flags & LCOMPRESS ? *p++ & 0377 : 0;
		it->it_trailcnt = kp->k_flags & TCOMPRESS ? *p++ & 0377 : 0;
		it->it_keylen = kp->k_len - it->it_leadcnt - it->it_trailcnt;
		bycopy(p, it->it_key + it->it_leadcnt, it->it_keylen);
		p += it->it_keylen;
		byfill(it->it_key + it->it_leadcnt + it->it_keylen, it->it_trailcnt, ' ');
	}
	if (kp->k_flags & ISDUPS) {
		it->it_dupnum = ldint(p);
		p += 2;
	} else
		it->it_dupnum = 0;
	if (*p & 0200)
		it->it_flags |= 2;
	it->it_ptr = ldlong(p) & 0x7fffffffL;
	p += 4;
	it->it_totlen = p - q;
	return(off + it->it_totlen);
}

itpress(it1, it2, kp)
register struct item	*it1, *it2;
struct keydesc	*kp;
{
	register int	i;

	if (it1 != it2 && kp->k_flags & DCOMPRESS && bycmpr(it1->it_key, it2->it_key, kp->k_len) == 0) {
		it2->it_flags |= 1;
		it1->it_flags |= 2;
		it2->it_keylen = 0;
		it2->it_totlen = 0;
		it2->it_trailcnt = 0;
		it2->it_leadcnt = 0;
	} else {
		it2->it_flags &= ~1;
		if (it1 != it2)
			it1->it_flags &= ~2;
		it2->it_keylen = it2->it_totlen = kp->k_len;
		if (kp->k_flags & TCOMPRESS) {
			for (i = it2->it_totlen; i != 0; i--)
				if (it2->it_key[i - 1] != ' ')
					break;
			it2->it_trailcnt = it2->it_totlen - i;
			it2->it_totlen = i + 1;
		} else
			it2->it_trailcnt = 0;
		if (kp->k_flags & LCOMPRESS) {
			if (it1 != it2)
				for (i = 0; i < it2->it_totlen - 1 && it2->it_key[i] == it1->it_key[i]; i++)
					;
			else
				i = 0;
			it2->it_leadcnt = i;
			if ((i = kp->k_len - it1->it_trailcnt) < it2->it_leadcnt)
				it2->it_leadcnt = i;
			it2->it_totlen -= it2->it_leadcnt - 1;
		} else
			it2->it_leadcnt = 0;
		it2->it_keylen -= it2->it_leadcnt + it2->it_trailcnt;
	}
	if (kp->k_flags & ISDUPS)
		it2->it_totlen += 2;
	it2->it_totlen += 4;
}

itlast(it, kp)
register struct item	*it;
struct keydesc	*kp;
{
	struct item	tmp;

	itcopy(it, &tmp);
	itpress(&tmp, it, kp);
}

itprevious(buf, off, it, kp)
char	*buf;
int	off;
register struct item	*it;
struct keydesc	*kp;
{
	register int	j;
	register int	i;

	itclear(it);
	i = 0;
	while ((j = itunpack(buf, i, it, kp)) < off)
		i = j;
	return(i);
}

itcopy(from, to)
struct item	*from, *to;
{
	wdcopy((int *)from, (int *)to, sizeof(struct item));
}

itclear(it)
register struct item	*it;
{
	wdfill(it, sizeof(struct item), 0);
	byfill(it->it_key, sizeof(it->it_key), ' ');
}

itmake(rec, it, kp)
char	*rec;
struct item	*it;
struct keydesc	*kp;
{
	register char	*p;
	register struct keypart	*kpp;
	register int	n;

	kpp = kp->k_part;
	itclear(it);
	p = it->it_key;
	for (n = kp->k_nparts; n; n--) {
		bycopy(rec + kpp->kp_start, p, kpp->kp_leng);
		p += kpp->kp_leng;
		kpp++;
	}
}
