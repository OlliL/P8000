import sys,os
os.environ['FVETO_DIR']='/tmp/SCRATCH/fv'
sys.path.insert(0,'host/recon/poptveto')
import fveto
from concurrent.futures import ThreadPoolExecutor
sp,raw,depth=sys.argv[1],sys.argv[2],int(sys.argv[3])
fveto.setup(sp)
seen={}; front=[()]
with ThreadPoolExecutor(8) as ex:
    for d in range(depth+1):
        res=list(ex.map(lambda V: fveto.run(raw,'jpr',0,list(V)),front))
        nxt=set()
        for V,n,ev in res:
            seen[V]=n
            if n is None: continue
            if n==0:
                m={k:(t,l) for k,t,l in ev}
                print('ZERO',V,[ '%s%d'%m[k] for k in V if k in m],flush=True)
                continue
            for k,t,l in ev:
                if k not in V and t in 'MISRNX':
                    W=tuple(sorted(V+(k,)))
                    if W not in seen: nxt.add(W)
        front=sorted(nxt)
        print('depth',d,len(res),min((abs(n) for V,n,ev in res if n is not None),default=None),flush=True)
