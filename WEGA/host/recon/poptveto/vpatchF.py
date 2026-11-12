import sys,subprocess
subprocess.run(['patch','-s','-p0',sys.argv[1]],stdin=open('/home/olivleh1/git/P8000/WEGA/host/recon/poptveto/z21_veto.patch'),check=True)
p=sys.argv[1]; s=open(p).read()
old='''	do
		if ((p1 = p1->back) == 0)
			return p;
	while (p1 != p3);'''
assert s.count(old)==1
s=s.replace(old,'''	do
		if ((p1 = p1->back) == 0) {
			if (!vetoed('F', p->labno))
				return p;
			break;
		}
	while (p1 != p3);''')
open(p,'w').write(s)
