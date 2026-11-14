import sys
p=sys.argv[1]+'/reader3.c'; s=open(p).read()
a="""#ifndef OLD_D1R
			off = freetemp(sz) / 8;
#endif
"""
b="""#ifndef OLD_D1R
#ifdef X_D1RC
			if (p->in.left->in.op != ICON)
#endif
			off = freetemp(sz) / 8;
#endif
"""
assert s.count(a)==1; open(p,'w').write(s.replace(a,b,1))
