import sys
f=sys.argv[1]; s=open(f).read()
a="""				if (rp && rp != p1 && (rp->op == JP || rp->op == RET)) {
					int n45 = 0;"""
b="""#ifdef D45N
				if (rp && rp != p1 && (rp->op == JP || rp->op == RET))
					p1 = p;
				else
#endif
				if (rp && rp != p1 && (rp->op == JP || rp->op == RET)) {
					int n45 = 0;"""
assert a in s
open(f,'w').write(s.replace(a,b))
