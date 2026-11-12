# optional events on top of vpatchX: E (entry move held until the first comjump, one toggle per function),
# V (codemove p1 == p3: loop inversion instead of giving up, per occurrence)
import sys
p=sys.argv[1]; s=open(p).read()
def rep(a,b):
    global s
    assert s.count(a)==1,a
    s=s.replace(a,b)
rep('''			comjump();''','''			gcj++;
			comjump();''')
rep('''		niter = 0;
		do {
			refcount();''','''		niter = 0;
		gcj = 0;
		ehold = -1;
		do {
			refcount();''')
s=s.replace('void iterate();','void iterate();\nint gcj, ehold;',1)
rep('''	p1 = p;
	if (p1->op != JP)
		return p1;
	p2 = p1->ref;''','''	p1 = p;
	if (p1->op != JP)
		return p1;
	{
		register struct node *e;
		for (e = first.forw; e && (e->op == KCOMMENT || e->op == DLABEL); e = e->forw)
			;
		if (e == p1) {
			if (ehold < 0)
				ehold = vetoed('E', p1->labno);
			if (ehold && gcj < 1)
				return p1;
		}
	}
	p2 = p1->ref;''')
rep('''			if (p3->op == JP) {
				if (p1 == p3)
					return p1;''','''			if (p3->op == JP) {
				if (p1 == p3) {
					if (vetoed('V', p1->labno)) {
						p2 = p2->back;
						goto ivloop;
					}
					return p1;
				}''')
rep('''	/* loop inversion */
	p3 = p1;''','''	/* loop inversion */
ivloop:
	p3 = p1;''')
open(p,'w').write(s)
