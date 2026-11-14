import sys
f=sys.argv[1]; s=open(f).read()
fn = r'''
#ifdef CMFIRST
static void
prepass()
{
	register struct node *p, *rp;

	refcount();
	for (p = first.forw; p; p = p->forw) {
#ifdef CMF_CHAIN
		if ((p->op == JP || p->op == CJP || p->op == JSW) && p->ref) {
			rp = nonlab(p->ref);
			if (rp->op == JP && rp->labno && p->labno != rp->labno) {
				p->labno = rp->labno;
				decref(p->ref);
				++rp->ref->refc;
				p->ref = rp->ref;
			}
		}
#endif
#ifdef CMF_NEXT
		if (p->op == JP && p->ref) {
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
		if (p->op == JP) {
#ifdef CMF_X
			xjump(p);
#endif
			p = codemove(p);
		}
	}
}
#endif

int
main(argc, argv)'''
a="\nint\nmain(argc, argv)"
s=s.replace(a,fn,1)
b="\t\tmovedat();\n"
s=s.replace(b,b+"#ifdef CMFIRST\n\t\tprepass();\n#endif\n",1)
open(f,'w').write(s)
