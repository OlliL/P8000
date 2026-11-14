import sys
f=sys.argv[1]; s=open(f).read()
a="""	p1 = p;
	if (p1->op != JP)
		return p1;
	p2 = p1->ref;
	if (p2 == 0)
		return p1;
"""
b=a+r"""#ifdef REV
	{
		struct node *b, *q, *l0, *prev;
		int ok, nins, ncj;

		/* the target: first label of its run, the node in front must be a jump */
		for (q = p2; q && (q->op == LABEL || q->op == KCOMMENT); q = q->back)
			;
		ok = q && (q->op == JP || q->op == RET) && q != p1;
		l0 = ok ? q->forw : 0;
		/* the block of the jump: back to the jump in front */
		nins = ncj = 0;
		for (b = p1->back; ok && b; b = b->back) {
			if (b == &first || b->op == OBRACE || b->op == DLABEL || b->op == 0) {
				ok = 0;
				break;
			}
			if (b->op == JP || b->op == RET)
				break;
			if (b == p2 || b == l0)
				ok = 0;
			if (b->op != LABEL && b->op != KCOMMENT)
				nins++;
			if (b->op == CJP)
				ncj++;
		}
		if (b == 0)
			ok = 0;
#ifdef REV_NOENTRY
		if (ok && b->labno >= 10000 && b->labno < 20000)
			ok = 0;
#endif
#ifdef REV_ENTRYONLY
		if (ok && !(b->labno >= 10000 && b->labno < 20000))
			ok = 0;
#endif
#ifdef REV_COND
		if (ok && !(REV_COND))
			ok = 0;
#endif
		if (ok && nins > 0) {
			prev = b;
			b = b->forw;		/* first node of the block; p1->back its last */
			q = p1->back;
			/* cut b .. p1 */
			prev->forw = p1->forw;
			p1->forw->back = prev;
			/* put b .. q in front of l0 */
			l0->back->forw = b;
			b->back = l0->back;
			q->forw = l0;
			l0->back = q;
			decref(p1->ref);
			ncmot++;
			nchange++;
			return prev;
		}
	}
#endif
"""
assert a in s
open(f,'w').write(s.replace(a,b,1))
