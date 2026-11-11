import sys,os
p=sys.argv[1]; s=open(p).read()
helper='''backloop(a, b)
register struct node *a, *b;
{
	register struct node *q;

	if (b->ref == 0 || (b->labno >= 10000 && b->labno < 20000))
		return 0;
	for (q = a->ref; q; q = q->back)
		if (q == b->ref)
			break;
	if (q == 0)
		return 0;
	do
		q = q->back;
	while (q && (q->op == LABEL || q->op == KCOMMENT));
	return q && q->op != JP && q->op != RET && q->op != JSW;
}

'''
i=s.index('struct node *\ncodemove(p)')
s=s[:i]+helper+s[i:]
open(p,'w').write(s)
import subprocess
subprocess.run(['python3','/tmp/SCRATCH/refuse.py',p],check=True)
