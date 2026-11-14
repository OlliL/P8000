import sys
f=sys.argv[1]; s=open(f).read()
helper=r'''
#if defined(RJ_TURN) || defined(RJ_U1B) || defined(RJ_LJ)
static int
rj_turnable(b)
register struct node *b;
{
	register struct node *q, *r, *t;

	for (q = b->back; q && q->op == KCOMMENT; q = q->back)
		;
	if (q == 0 || q->op != CJP || q->ref == 0)
		return 0;
	t = q->ref;
	r = nonlab(t);
	if (r && r->op == JP && r->ref)
		t = r->ref;
	for (r = b->forw; r && (r->op == KCOMMENT || r->op == LABEL); r = r->forw)
		if (r == t)
			return 1;
	return 0;
}

static int
rj_backloop(a, b)
register struct node *a, *b;
{
	register struct node *q;

	if (b->ref == 0 || (b->labno >= 10000 && b->labno < 20000))
		return 0;
	for (q = a->ref; q; q = q->back)
		if (q == b->ref)
			break;
	if (q == 0)
		return 0;
	do
		q = q->back;
	while (q && (q->op == LABEL || q->op == KCOMMENT));
	return q && q->op != JP && q->op != RET && q->op != JSW;
}

static int
rj_jn(a)
register struct node *a;
{
	register struct node *q;
	register int n;

	n = 0;
	for (q = a->back; q && q->op != LABEL; q = q->back) {
		if (q->op == KCOMMENT)
			continue;
		n++;
		if (q->op == JP || q->op == RET || q->op == JSW)
			break;
	}
	return n;
}
#endif
'''
a="\nstruct node *\ncodemove(p)\n"
assert a in s
s=s.replace(a,helper+a,1)
old="""				ncmot++;
				nchange++;
				p1->back->forw = p2;"""
new="""#ifdef RJ_TURN
				if (rj_turnable(p3))
					return p1;
#endif
#ifdef RJ_LJ
				if (rj_jn(p1) == 0)
					return p1;
#endif
#ifdef RJ_U1B
				if (rj_backloop(p1, p3) && rj_jn(p1) == 1)
					return p1;
#endif
"""+old
assert s.count(old)==1
s=s.replace(old,new,1)
open(f,'w').write(s)
