import sys,subprocess
d='/tmp/SCRATCH/'
subprocess.run(['python3',d+'vpatchF.py',sys.argv[1]],check=True)
p=sys.argv[1]; s=open(p).read()
def rep(a,b):
    global s
    assert s.count(a)==1,a
    s=s.replace(a,b)
# R: chain retarget in iterate
rep('''			rp = nonlab(p->ref);
			if (rp->op == JP && rp->labno && p->labno != rp->labno
			) {''','''			rp = nonlab(p->ref);
			if (rp->op == JP && rp->labno && p->labno != rp->labno
			    && !vetoed('R', rp->labno)) {''')
# N: jump to the next label
rep('''				if (rp == p->ref) {
					p->back->forw = p->forw;''','''				if (rp == p->ref && !vetoed('N', rp->labno)) {
					p->back->forw = p->forw;''')
# X: xjump step
rep('''		if (!equop(p1, p2) || p1 == p2)
			return;
		p3 = insertl(p2);''','''		if (!equop(p1, p2) || p1 == p2)
			return;
		if (vetoed('X', p2->labno))
			return;
		p3 = insertl(p2);''')
# B: backjmp pair
rep('''				if (p3->op == JP && p3->ref == p2)
					backjmp(p1, p3);''','''				if (p3->op == JP && p3->ref == p2 && !vetoed('B', p2->labno))
					backjmp(p1, p3);''')
open(p,'w').write(s)
