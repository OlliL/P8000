import sys
p=sys.argv[1]; s=open(p).read()
a='''			if (p3->op == JP) {
				if (p1 == p3)
					return p1;'''
assert s.count(a)==1
s=s.replace(a,'''			if (p3->op == RET)
				return p1;
'''+a); open(p,'w').write(s)
