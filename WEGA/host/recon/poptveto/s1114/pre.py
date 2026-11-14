import sys
f=sys.argv[1]; s=open(f).read()
fn = r'''
#if defined(PRE_TURN) || defined(PRE_NEXT)
static void
prepass()
{
	register struct node *p, *rp, *p1;

	refcount();
	for (p = first.forw; p; p = p->forw) {
#ifdef PRE_TURN
		if (p->op == CJP && p->ref) {
			p1 = p->forw;
			while (p1->op == KCOMMENT)
				p1 = p1->forw;
			if (p1->op == JP && p1->labno) {
				rp = p->ref;
				do
					rp = rp->back;
				while (rp->op == LABEL || rp->op == KCOMMENT);
				if (rp == p1) {
					decref(p->ref);
					p->ref = p1->ref;
					p->labno = p1->labno;
					p1->forw->back = p;
					p->forw = p1->forw;
					p->subop = revbr[(int)p->subop];
				}
			}
		}
#endif
#ifdef PRE_NEXT
		if ((p->op == JP || p->op == CJP) && p->ref) {
			for (rp = p->forw; rp; rp = rp->forw) {
				if (rp->op != LABEL && rp->op != KCOMMENT)
					break;
				if (rp == p->ref) {
					p->back->forw = p->forw;
					p->forw->back = p->back;
					p = p->back;
					decref(rp);
					break;
				}
			}
		}
#endif
	}
}
#endif

int
main(argc, argv)'''
a="\nint\nmain(argc, argv)"
assert a in s
s=s.replace(a,fn,1)
b="\t\tmovedat();\n"
assert b in s
s=s.replace(b,b+"#if defined(PRE_TURN) || defined(PRE_NEXT)\n\t\tprepass();\n#endif\n",1)
open(f,'w').write(s)
