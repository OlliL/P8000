"""greedy.py FILE [STEPS]: repeated single changes (leak / allocfree / dropfree) that extend the right prefix the most;
prints each chosen change with its place (the function and the events around it)"""
import sys,collections,os; sys.path.insert(0,os.path.dirname(os.path.abspath(__file__)))
import hyp
from replay import run
f=sys.argv[1]; steps=int(sys.argv[2]) if len(sys.argv)>2 else 8
E=hyp.EXP.get(f,{})
def prefix(ev):
    try: out,_=run(ev)
    except Exception: return -1,''
    order=[]; c=collections.Counter()
    for fn,n,s in out:
        if fn not in order: order.append(fn)
        c[fn]+= 1 if s else 0
    k=0
    for fn in order:
        if c[fn]!=E.get(fn,0): return k,fn
        k+=1
    return k,'-'
ev=list(hyp.EV[f]); tag=0x900000
for st in range(steps):
    base,bfn=prefix(ev)
    if bfn=='-': print('all right'); break
    D=[i for i,e in enumerate(ev) if e[0]=='D']
    hi=[i for i in D if ev[i][1]==bfn][0] if any(ev[i][1]==bfn for i in D) else len(ev)
    lo=([i for i in D if i<hi][-3:] or [0])[0]
    best=None
    for i in range(lo,hi+1):
        for kind in ('leak','allocfree','dropfree'):
            e2=list(ev)
            if kind=='leak': e2.insert(i,('M',26,tag))
            elif kind=='allocfree': e2[i:i]=[('M',26,tag),('f',tag)]
            else:
                if ev[i][0]!='f': continue
                del e2[i]
            p,fn=prefix(e2)
            if best is None or p>best[0]: best=(p,kind,i,e2,fn)
    p,kind,i,e2,fn=best
    if p<=base: print('no single change helps at',bfn); break
    fnn=[ev[j][1] for j in D if j<i][-1:] 
    print('step %d: %s at %d (in %s; events %s) -> prefix %d, next wrong %s'%(st,kind,i,fnn,ev[i-1:i+1],p,fn))
    ev=e2; tag+=2
