import sys
f=sys.argv[1]; s=open(f).read()
helper=r'''
#ifdef RJ_WIN
/* the site lies in the inversion window behind a loop head: a label that is fallen into and named by a jump further on */
static int
rj_inwin(a)
register struct node *a;
{
	register struct node *q, *r, *t;
	register int n;

	n = 0;
	for (q = a->back; q && n < RJ_WIN; q = q->back) {
		n++;
		if (q->op != LABEL || q->refc <= 0)
			continue;
		for (r = q->back; r && (r->op == LABEL || r->op == KCOMMENT); r = r->back)
			;
		if (r == 0 || r->op == JP || r->op == RET || r->op == JSW || r->op == 0 || r->op == OBRACE)
			continue;
		{
		int seen = 0;
		for (t = a->forw; t; t = t->forw) {
			if (t == a->ref)
				seen = 1;
#ifdef RJ_INSIDE
			if (t->op == JP && t->ref == q && !seen)
				break;
#endif
			if (t->op == JP && t->ref == q) {
#ifdef RJ_DBG
				fprintf(stderr, "WIN site L%d head L%d n %d\n", a->labno, q->labno, n);
#endif
				return 1;
			}
		}
		}
	}
	return 0;
}
#endif
'''
a="\nstruct node *\ncodemove(p)\n"
s=s.replace(a,helper+a,1)
old="""				ncmot++;
				nchange++;
				p1->back->forw = p2;"""
new="""#ifdef RJ_WIN
				if (rj_inwin(p1))
					return p1;
#endif
"""+old
assert s.count(old)==1
s=s.replace(old,new,1)
open(f,'w').write(s)
