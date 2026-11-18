#include	<isam.h>
#include	"isint.h"

isread(isfd, rec, mode)
int	isfd;
char	*rec;
register int	mode;
{
	register struct keydesc	*kp;
	int	lock;

	if (isenter(isfd, 0) == -1)
		return(-1);
	lock = mode & 0177400;
	if (lock) {
		if (lock != ISLOCK) {
			iserrno = EBADARG;
			goto out;
		}
	} else if (openp->o_mode & ISAUTOLOCK)
		lock = ISLOCK;
	mode &= 0377;
	kp = &openp->o_key;
	switch (mode) {
	case ISFIRST:
	case ISLAST:
	case ISEQUAL:
	case ISGREAT:
	case ISGTEQ:
		kposition(rec, 0, mode);
		break;
	case ISNEXT:
	case ISPREV:
	case ISCURR:
		if (btcurrent()) {
			if (iserrno == 0)
				iserrno = ENOCURR;
			goto out;
		}
		break;
	default:
		iserrno = EBADARG;
	}
	if (iserrno)
		goto out;
	if (mode == ISPREV || mode == ISLAST)
		btprevious(kp);
	if (mode == ISNEXT && (openp->o_flags & 010 || treeflags & 01))
		btnext(kp);
	if (treeflags & 03) {
		iserrno = EENDFILE;
		goto out;
	}
	itcopy(&treeitem, &openp->o_item);
	if (lock && lkrecord(openp->o_item.it_ptr, 2))
		goto locked;
	if (dataread(rec, openp->o_item.it_ptr))
		goto norec;
	if (mode == ISPREV)
		btprevious(kp);
	else
		btnext(kp);
	if ((treeflags & 03) == 0 && itcompare(&openp->o_item, &treeitem, kp, 0) == 0)
		openp->o_flags |= 020;
	if (mode == ISPREV)
		btnext(kp);
	else
		btprevious(kp);
	openp->o_flags |= 010;
	goto out;
locked:
	if (iserrno == 0)
		iserrno = ELOCKED;
	goto out;
norec:
	if (iserrno == 0)
		iserrno = ENOREC;
out:
	return(isexit());
}

isstart(isfd, kp, len, rec, mode)
int	isfd;
struct keydesc	*kp;
int	len;
char	*rec;
int	mode;
{
	if (isenter(isfd, 0) == -1)
		return(-1);
	kycopy(kp, &tempkey);
	if (kysearch(&tempkey, 0) <= 0)
		iserrno = EBADKEY;
	else {
		kycopy(&tempkey, &openp->o_key);
		kp->k_flags = tempkey.k_flags;
		kposition(rec, len, mode);
	}
	return(isexit());
}

kposition(rec, len, mode)
char	*rec;
int	len, mode;
{
	register struct keydesc	*kp;

	kp = &openp->o_key;
	switch (mode) {
	case ISFIRST:
		btfirst(kp);
		break;
	case ISLAST:
		btlast(kp);
		btnext(kp);
		break;
	case ISEQUAL:
	case ISGREAT:
	case ISGTEQ:
		itmake(rec, &tempitem, kp);
		btsearch(&tempitem, kp, len, mode == ISGREAT);
		if (treeflags & 03)
			iserrno = ENOREC;
		if (mode == ISEQUAL && itcompare(&tempitem, &treeitem, kp, len) != 0) {
			iserrno = ENOREC;
			goto bad;
		}
		break;
	default:
		iserrno = EBADARG;
	}
	if (iserrno) {
bad:
		openp->o_flags &= ~014;
		return;
	}
	itcopy(&treeitem, &openp->o_item);
	openp->o_flags |= 04;
	openp->o_flags &= ~010;
}

iswrite(isfd, rec)
int	isfd;
char	*rec;
{
	if (isenter(isfd, 1) == -1)
		return(-1);
	wrtrecord(rec, 0);
	return(isexit());
}

iswrcurr(isfd, rec)
int	isfd;
char	*rec;
{
	if (isenter(isfd, 1) == -1)
		return(-1);
	wrtrecord(rec, 1);
	if (iserrno == 0)
		openp->o_flags |= 014;
	return(isexit());
}

wrtrecord(rec, cur)
char	*rec;
int	cur;
{
	struct item	it;
	register struct keydesc	*kp;
	long	recnum;
	int	del;
	register int	i, n;

	if (audopen())
		return;
	recnum = allocrec(1);
	if (lkrecord(recnum, 3)) {
bad:
		freerec(recnum, 1);
		return;
	}
	del = 0;
	kp = &tempkey;
	n = ldint(dictp + 8);
	goto loop;
fail:
	n = i - 1;
undo:
	del = 1;
loop:
	for (i = 1; i <= n; i++) {
		if (kysearch(kp, i) <= 0)
			break;
		itmake(rec, &it, kp);
		it.it_ptr = recnum;
		if (del == 0) {
			if (btadditem(&it, kp))
				goto fail;
			if (cur && kycompare(kp, &openp->o_key) == 0)
				itcopy(&it, &openp->o_item);
		} else
			btdelitem(&it, kp);
	}
	if (iserrno)
		goto bad;
	if (datawrite(rec, recnum)) {
		datadelete(recnum);
		goto undo;
	}
	audoutput('A', rec);
}

isrewrite(isfd, rec)
int	isfd;
char	*rec;
{
	if (isenter(isfd, 2) == -1)
		return(-1);
	if (getrecord(rec) == 0)
		rewrecord(temprecord, rec, treeitem.it_ptr, 0);
	return(isexit());
}

isrewcurr(isfd, rec)
int	isfd;
char	*rec;
{
	if (isenter(isfd, 2) == -1)
		return(-1);
	if (getcurrent() == 0)
		rewrecord(temprecord, rec, treeitem.it_ptr, 1);
	return(isexit());
}

rewrecord(old, new, recnum, cur)
char	*old, *new;
long	recnum;
int	cur;
{
	struct item	it1, it2;
	register struct keydesc	*kp;
	register int	i, n;
	char	*tmp;
	int	pass;

	if (audopen())
		return;
	pass = 0;
	kp = &tempkey;
	n = ldint(dictp + 8);
again:
	for (i = 1; i <= n || pass == 0; i++) {
		if (i > n)
			pass = i = 1;
		if (kysearch(kp, i) <= 0)
			continue;
		if (((kp->k_flags & ISDUPS) != 0) != (pass == 1))
			continue;
		itmake(old, &it1, kp);
		itmake(new, &it2, kp);
		if (itcompare(&it1, &it2, kp, 0) == 0)
			continue;
		it1.it_ptr = recnum;
		it2.it_ptr = recnum;
		if (btadditem(&it2, kp)) {
			pass = 2;
			n = i - 1;
			tmp = old;
			old = new;
			new = tmp;
			goto again;
		}
		btdelitem(&it1, kp);
		if (cur && kycompare(kp, &openp->o_key) == 0)
			itcopy(&it2, &openp->o_item);
	}
	if (pass == 2)
		return;
	datawrite(new, recnum);
	audoutput('R', old);
	audoutput('W', new);
}

isdelete(isfd, rec)
int	isfd;
char	*rec;
{
	if (isenter(isfd, 2) == -1)
		return(-1);
	if (getrecord(rec) == 0)
		delrecord(temprecord, treeitem.it_ptr);
	return(isexit());
}

isdelcurr(isfd)
int	isfd;
{
	if (isenter(isfd, 2) == -1)
		return(-1);
	if (getcurrent() == 0)
		delrecord(temprecord, treeitem.it_ptr);
	return(isexit());
}

delrecord(rec, recnum)
char	*rec;
long	recnum;
{
	struct item	it;
	register struct keydesc	*kp;
	register int	i, n;

	if (audopen())
		return;
	kp = &tempkey;
	n = ldint(dictp + 8);
	for (i = 1; i <= n; i++) {
		if (kysearch(kp, i) <= 0)
			break;
		itmake(rec, &it, kp);
		it.it_ptr = recnum;
		btdelitem(&it, kp);
	}
	datadelete(recnum);
	audoutput('D', rec);
	freerec(recnum, 1);
}

getrecord(rec)
char	*rec;
{
	register struct keydesc	*kp;
	register struct item	*ip;

	kp = &tempkey;
	ip = &tempitem;
	if (kysearch(kp, 1) <= 0 || kp->k_flags & ISDUPS)
		goto bad;
	itmake(rec, ip, kp);
	btsearch(ip, kp, 0, 0);
	if (itcompare(ip, &treeitem, kp, 0))
		goto bad;
	if (lkrecord(treeitem.it_ptr, 3))
		return(-1);
	if (dataread(temprecord, treeitem.it_ptr))
		goto bad;
	return(0);
bad:
	if (iserrno == 0)
		iserrno = ENOREC;
	return(-1);
}

getcurrent()
{
	if (btcurrent() || !(openp->o_flags & 010))
		goto bad;
	if (lkrecord(treeitem.it_ptr, 3))
		return(-1);
	if (dataread(temprecord, treeitem.it_ptr))
		goto bad;
	return(0);
bad:
	if (iserrno == 0)
		iserrno = ENOCURR;
	return(-1);
}
