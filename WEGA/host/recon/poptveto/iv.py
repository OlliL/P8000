import sys
p=sys.argv[1]; s=open(p).read()
a='''			if (p3->op == JP) {
				if (p1 == p3)
					return p1;'''
assert s.count(a)==1
s=s.replace(a,'''			if (p3->op == JP) {
				if (p1 == p3)
				{
					p2 = p2->back;
					goto ivloop;
				}''')
a='''	/* loop inversion */
	p3 = p1;'''
assert s.count(a)==1
s=s.replace(a,'''	/* loop inversion */
ivloop:
	p3 = p1;'''); open(p,'w').write(s)
