"""extradecl.py [FILE ...]: K extra permanent entries (global declarations the rebuilt source may lack) in front of the
first function (before its #D and its parameters: at the last entry of level 0 before it), K = 0 .. 80, and K entries
fewer (taking out kept global entries); per file the K with the fewest wrong functions"""
import sys,os,collections; sys.path.insert(0,os.path.dirname(os.path.abspath(__file__)))
import hyp
from replay import run
def wrong(f,ev):
    try: out,_=run(ev)
    except Exception: return 99,[]
    c=collections.Counter(); seen=set()
    for fn,n,s in out: seen.add(fn); c[fn]+=1 if s else 0
    E=hyp.EXP.get(f,{})
    bad=[fn for fn in seen|set(E) if c[fn]!=E.get(fn,0)]
    return len(bad),bad
def withk(ev,k):
    D=[i for i,e in enumerate(ev) if e[0]=='D'][0]
    # the place: in front of the first malloc that belongs to the first function (its parameters come before #D)
    j=D
    while j>0 and not (ev[j-1][0]=='XN'): j-=1
    # go back over the entries of the parameters: the run of M/XN/XR right before #D after the last level-0 entry
    p=D
    while p>0 and ev[p-1][0] in ('M','XN','XR'): p-=1
    ins=[('M',26,0xB00000+2*i) for i in range(k)]
    return ev[:p]+ins+ev[p:]
files=sys.argv[1:] or [f for f in hyp.FILES]
for f in files:
    ev=hyp.EV[f]
    if not any(e[0]=='D' for e in ev): continue
    r=[(wrong(f,withk(ev,k))[0],k) for k in range(0,81)]
    base=r[0][0]; best=min(r)
    print('%-15s base %d  best %d at K=%s'%(f,base,best[0],[k for w,k in r if w==best[0]][:8]))
