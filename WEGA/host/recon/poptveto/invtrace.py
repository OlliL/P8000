import sys
p=sys.argv[1]; s=open(p).read()
a='''	p1 = p;
	tl = insertl(p1);
	p3->subop = revbr[(int)p3->subop];'''
assert s.count(a)==1, 'inv'
s=s.replace(a,'''	p1 = p;
#ifdef TRACE
	if (debug)
		printf("/*INV jp L%d next L%d cjp L%d*/\\n", p1->labno, p1->forw->labno, p3->labno);
#endif
	tl = insertl(p1);
	p3->subop = revbr[(int)p3->subop];''')
open(p,'w').write(s)
