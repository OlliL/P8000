import sys,os
os.environ['FVETO_DIR']='/tmp/SCRATCH/fv'
sys.path.insert(0,'host/recon/poptveto')
import fveto
fveto.setup(sys.argv[1])
want=[(x[0],int(x[1:])) for x in sys.argv[3:]]   # e.g. M109 M105  (first occurrence each, in pass order)
V=[]
while True:
    v,n,ev=fveto.run(sys.argv[2],'jpr',0,V)
    add=None
    for w in want:
        for i,t,l in ev:
            if (t,l)==w and i not in V:
                # only the first occurrence of each wanted event
                if not any((t2,l2)==w and i2 in V for i2,t2,l2 in ev): add=i
                break
        if add is not None: break
    if add is None: break
    V.append(add)
print('V',V,'lines',n)
print(' '.join('%d%s%d'%e for e in ev[:34]))
