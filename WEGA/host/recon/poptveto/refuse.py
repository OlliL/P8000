import sys,os
p=sys.argv[1]; s=open(p).read()
exec(open('/tmp/SCRATCH/d41c.py').read().split("old2=")[0].split("p=sys.argv[1]; s=open(p).read()")[1])
# features computed (tn tc jn jc jl pre lr) are now in front of 'ncmot++'; add a refusal right after them
old='''				ncmot++;
				nchange++;
				p1->back->forw = p2;'''
s=s.replace(old,'''				if (%s) {
#ifdef TRACE
					if (debug)
						printf("/*REFUSED*/\\n");
#endif
					return p1;
				}
'''%os.environ['D41C']+old,1)
open(p,'w').write(s)
