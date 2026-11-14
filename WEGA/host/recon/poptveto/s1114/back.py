import sys
f=sys.argv[1]; s=open(f).read()
a="""	nchange = 0;
	for (p = first.forw; p; p = p->forw) {
		if ((p->op == JP || p->op == CJP || p->op == JSW) && p->ref) {"""
b="""	nchange = 0;
#ifdef BACKSWEEP
	for (p = first.forw; p->forw; p = p->forw)
		;
	for (; p && p != &first; p = p->back) {
#else
	for (p = first.forw; p; p = p->forw) {
#endif
		if ((p->op == JP || p->op == CJP || p->op == JSW) && p->ref) {"""
assert a in s
open(f,'w').write(s.replace(a,b))
