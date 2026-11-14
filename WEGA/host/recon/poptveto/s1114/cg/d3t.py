import sys,subprocess,os
d=sys.argv[1]
subprocess.run(['python3',os.path.dirname(os.path.abspath(__file__))+'/d1rf.py',d],check=True)
p=d+'/reader3.c'; s=open(p).read()
a="""	case ANDAND:
		lab = false < 0 ? getlab() : false;
		cbranch(p->in.left, -1, lab);
		cbranch(p->in.right, true, false);
		if (false < 0)
			deflab(lab);
		p->in.op = FREE;
#ifndef OLD_D3
#ifdef OLD_D3K
		rfreeall(2);	/* v0: a parameter stays known in its register */
#else
		rfreeall(1);
#endif
#endif
		return;
"""
b="""	case ANDAND:
		lab = false < 0 ? getlab() : false;
#ifdef X_D3T
		d3tdepth++;
#endif
		cbranch(p->in.left, -1, lab);
		cbranch(p->in.right, true, false);
#ifdef X_D3T
		d3tdepth--;
#endif
		if (false < 0)
			deflab(lab);
		p->in.op = FREE;
#ifndef OLD_D3
#ifdef OLD_D3K
		rfreeall(2);	/* v0: a parameter stays known in its register */
#else
#ifdef X_D3T
		if (false < 0 || d3tdepth > 0 X_D3TC)
#endif
		rfreeall(1);
#endif
#endif
		return;
"""
assert s.count(a)==1; s=s.replace(a,b,1)
a="""	case OROR:
		lab = true < 0 ? getlab() : true;
		cbranch(p->in.left, lab, -1);
		cbranch(p->in.right, true, false);
"""
b="""	case OROR:
		lab = true < 0 ? getlab() : true;
#ifdef X_D3T
		d3tdepth++;
#endif
		cbranch(p->in.left, lab, -1);
		cbranch(p->in.right, true, false);
#ifdef X_D3T
		d3tdepth--;
#endif
"""
assert s.count(a)==1; s=s.replace(a,b,1)
s=s.replace("\nlong baseoff = 0;","\n#ifdef X_D3T\nint d3tdepth;\n#ifndef X_D3TC\n#define X_D3TC\n#endif\n#endif\nlong baseoff = 0;",1)
open(p,'w').write(s)
