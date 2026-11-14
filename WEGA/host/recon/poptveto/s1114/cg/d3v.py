import sys,subprocess,os
d=sys.argv[1]
subprocess.run(['python3',os.path.dirname(os.path.abspath(__file__))+'/d3t.py',d],check=True)
p=d+'/reader3.c'; s=open(p).read()
a="""#ifdef X_D3T
		d3tdepth++;
#endif
		cbranch(p->in.left, -1, lab);
		cbranch(p->in.right, true, false);
"""
b="""#ifdef X_D3T
		d3tdepth++;
		d3tcall = d3hascall(p->in.left);
#endif
		cbranch(p->in.left, -1, lab);
		cbranch(p->in.right, true, false);
"""
assert s.count(a)==1; s=s.replace(a,b,1)
a="\n#ifdef X_D3T\nint d3tdepth;\n"
b=r"""
#ifdef X_D3T
int d3tdepth, d3tcall;
static int
d3hascall(q)
register NODE *q;
{
	register int ty;

	if (q->in.op == CALL || q->in.op == UCALL || q->in.op == STCALL || q->in.op == USTCALL)
		return 1;
	ty = optype(q->in.op);
	if (ty == LTYPE)
		return 0;
	if (d3hascall(q->in.left))
		return 1;
	return ty == BITYPE ? d3hascall(q->in.right) : 0;
}
"""
assert a in s; s=s.replace(a,b+"\n",1) if False else s.replace(a,b,1)
s=s.replace("if (false < 0 || d3tdepth > 0 X_D3TC)","if (false < 0 || d3tdepth > 0 || !d3tcall)")
open(p,'w').write(s)
