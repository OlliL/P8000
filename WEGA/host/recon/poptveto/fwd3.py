import sys,os
p=sys.argv[1]; s=open(p).read()
old='''	do
		if ((p1 = p1->back) == 0)
			return p;
	while (p1 != p3);
	p1 = p;
	tl = insertl(p1);'''
assert s.count(old)==1
cond=os.environ.get('FWC','p->ref && p->ref->refc >= 3')
s=s.replace(old,'''	do
		if ((p1 = p1->back) == 0) {
			if (fwdinv || !(%s))
				return p;
			fwdinv++;
			break;
		}
	while (p1 != p3);
	p1 = p;
	tl = insertl(p1);'''%cond)
s=s.replace('\tdo {\n\t\tisend = input();','\tdo {\n\t\tfwdinv = 0;\n\t\tisend = input();',1)
s=s.replace('\nint\nmain(argc, argv)','\nint fwdinv;\nint\nmain(argc, argv)',1)
assert 'int fwdinv;' in s and 'fwdinv = 0;' in s
open(p,'w').write(s)
