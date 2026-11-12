import sys,subprocess
d='/tmp/SCRATCH/'
subprocess.run(['python3',d+'vpatchF.py',sys.argv[1]],check=True)
p=sys.argv[1]; s=open(p).read()
# features (from d41c) printed for every move
exec(open(d+'d41c.py').read().split("open(p,'w')")[0].split("old2=")[0].split("p=sys.argv[1]; s=open(p).read()")[1])
old='''				ncmot++;
				nchange++;
				p1->back->forw = p2;'''
s=s.replace(old,'''				printf("/*CMF %d L%d tn %d tc %d jn %d jc %d jl %d pre %d lr %d tend %d*/\\n", vfidx, p1->labno, tn, tc, jn, jc, jl, pre, lr, p3->ref ? p3->labno : -1);
'''+old,1)
a='''			if (!vetoed('F', p->labno))
				return p;'''
assert a in s
s=s.replace(a,'''			printf("/*FWD %d L%d next L%d*/\\n", vfidx, p->labno, p->forw ? p->forw->labno : -1);
			if (!vetoed('F', p->labno))
				return p;''')
open(p,'w').write(s)
