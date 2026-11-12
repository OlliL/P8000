import sys,os
p=sys.argv[1]; s=open(p).read()
cond,n=os.environ['D41C'].split('@')
a='''	n = 24;
	for (;;) {
		if ((p3 = p3->forw) == 0 || p3 == p1 || --n == 0)'''
assert s.count(a)==1
s=s.replace(a,'''	n = %s;
	for (;;) {
		if ((p3 = p3->forw) == 0 || p3 == p1 || ((%s) ? --n == 0 : 0))'''%(n,cond))
open(p,'w').write(s)
