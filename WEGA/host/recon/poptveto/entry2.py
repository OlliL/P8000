import sys,os
p=sys.argv[1]; s=open(p).read()
a='''			comjump();'''
assert s.count(a)==1
s=s.replace(a,'''			gcj++;
			comjump();''')
a='''		niter = 0;
		do {
			refcount();'''
assert s.count(a)==1
s=s.replace(a,'''		niter = 0;
		gcj = 0;
		do {
			refcount();''')
s=s.replace('void iterate();','void iterate();\nint gcj;',1)
a='''	p1 = p;
	if (p1->op != JP)
		return p1;
	p2 = p1->ref;'''
assert s.count(a)==1
s=s.replace(a,'''	p1 = p;
	if (p1->op != JP)
		return p1;
	{
		register struct node *e;
		for (e = first.forw; e && (e->op == KCOMMENT || e->op == DLABEL); e = e->forw)
			;
		if (e == p1 && gcj < %s)
			return p1;
	}
	p2 = p1->ref;'''%os.environ.get('ENTRYN','1'))
open(p,'w').write(s)
