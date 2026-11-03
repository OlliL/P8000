#!/usr/bin/env python3
"""castrig/boundary.py DS.json: rule test. The old cas looks at its entry counter only at the end of a function (K) and has a hard limit
inside a function (H). A site is wrong when the rule makes it the other form. Reads dataset.py."""
import json,sys,bisect
D=json.load(open(sys.argv[1]))
def trig(rows,K,H):
    tr=[];base=0;fn=None
    for r in rows:
        if r['a'] is None: continue
        e=r['ent']-base
        if r['fn']!=fn:
            if fn is not None and e>=K:
                tr.append(r['a']);base=r['ent'];e=0
            fn=r['fn']
        if e>=H:
            tr.append(r['a']);base=r['ent']
    return tr
def score(K,H,progs=None,detail=False):
    miss=false=0
    for m in D:
        if progs and m['prog'] not in progs: continue
        T=trig(m['rows'],K,H)
        def sp(a,e):
            i=bisect.bisect_right(T,a)
            return i<len(T) and T[i]<e
        a=sum(1 for x,y in m['long'] if not sp(x,y)); b=sum(1 for x,y in m['short'] if sp(x,y))
        miss+=a;false+=b
        if detail and (a or b or m['long']): print('  %-8s %-9s long %3d short %4d miss %3d false %3d trig %s'%(m['prog'],m['mod'],len(m['long']),len(m['short']),a,b,[hex(t) for t in T]))
    return miss,false
if __name__=='__main__':
    print('no trigger',score(10**6,10**6))
    res=[]
    for H in (240,248,250,252,254,255,256,10**6):
        for K in range(40,300):
            m,f=score(K,H); res.append((m+f,K,H,m,f))
    res.sort()
    print(res[:15])
    t,K,H,m,f=res[0]
    print('best',K,H,m,f); score(K,H,detail=True)
