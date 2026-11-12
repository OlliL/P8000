import sys
p=sys.argv[1]; s=open(p).read()
a='''		if (p->op == JP) {
			xjump(p);
			p = codemove(p);
		}
	}
}'''
assert s.count(a)==1
s=s.replace(a,'''	}
	for (p = first.forw; p; p = p->forw) {
		if (p->op == JP) {
			xjump(p);
			p = codemove(p);
		}
	}
}''')
open(p,'w').write(s)
