import sys,os,subprocess
p=sys.argv[1]; s=open(p).read()
helper='''turnable(b)
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

'''
i=s.index('struct node *\ncodemove(p)')
s=s[:i]+helper+s[i:]
open(p,'w').write(s)
subprocess.run(['python3','/tmp/SCRATCH/refuse2.py',p],check=True)
