import sys
p=sys.argv[1]; s=open(p).read(); a='if (rp && rp->op == JP && rp->ref && n45 > 0) {'; assert s.count(a)==1
s=s.replace(a,'if (rp && rp->op == JP && rp->ref && n45 > 0 && !(rp->labno >= 10000 && rp->labno < 20000)) {'); open(p,'w').write(s)
