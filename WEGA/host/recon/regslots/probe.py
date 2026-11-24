"""probe.py FILE: single changes of the event stream of FILE (hyp.EV key, e.g. make/misc) in a window; for each the number
of functions with register locals right from the start (the prefix), best first: what the original needs"""
import sys,collections; sys.path.insert(0,__import__('os').path.dirname(__import__('os').path.abspath(__file__)))
import hyp
from replay import run
f=sys.argv[1]; ev=hyp.EV[f]; E=hyp.EXP.get(f,{})
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
base,bfn=prefix(ev); print('base prefix',base,'first wrong',bfn)
# window: from the start of the function before the first wrong one up to the first wrong one's #D
names=[i for i,e in enumerate(ev) if e[0]=='D']
dpos={ev[i][1]:i for i in names}
hi=dpos.get(bfn,len(ev)); lo=max([i for i in names if i<hi][-6:][:1] or [0])
res=[]
for i in range(lo,hi+1):
    for kind in ('leak','allocfree','dropfree'):
        e2=list(ev)
        if kind=='leak': e2.insert(i,('M',26,0x900000))
        elif kind=='allocfree': e2[i:i]=[('M',26,0x900000),('f',0x900000)]
        elif kind=='dropfree':
            if ev[i][0]!='f': continue
            e2[i]=('M',0,0x900002)  # no-op placeholder
            del e2[i]
        p,fn=prefix(e2); res.append((p,kind,i,fn))
res.sort(reverse=True)
for p,kind,i,fn in res[:12]:
    ctx=[x for x in ev[max(0,i-2):i+2]]
    print(p,kind,i,'next wrong',fn,' at',ctx[1:3])
